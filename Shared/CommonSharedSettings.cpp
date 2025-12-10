#include "CommonSharedSettings.h"
#include "var_init_once.h"
#include "DebugHelper.hpp"

#include <array>
#include <algorithm>

#include <Windows.h>
#include <appmodel.h>
#include <ShlObj_core.h>

#include <wil/com.h>
#include <wil/win32_helpers.h>

namespace FastCopy::Settings
{
    using namespace std::literals;

    /* static */
    std::atomic<bool> CommonSharedSettings::s_manualShutdown{ false };

    CommonSharedSettings& CommonSharedSettings::Instance() noexcept
    {
        STATIC_INIT_ONCE(CommonSharedSettings, s);
        return *s;
    }

    void CommonSharedSettings::Shutdown()
    {
        bool expected = false;
        if (!s_manualShutdown.compare_exchange_strong(expected, 
            true, std::memory_order_relaxed))
            return;

        {
            std::lock_guard lock(m_listenersMutex);
            m_listeners.clear();
        }

        m_stopMonitor.store(true, std::memory_order_relaxed);

        HANDLE h = m_hChange;
        if (h && h != INVALID_HANDLE_VALUE)
        {
            // Close the notification handle and
            // wake up the WaitForSingleObject in the MonitorLoop
            FindCloseChangeNotification(h);
            m_hChange = nullptr;
        }

        if (m_monitorThread.joinable())
        {
            m_monitorThread.join();
        }

        // only work in debug mode
        DEBUG_BREAK_HERE();    // make a breakpoint here to check if the destructor is called
    }

    [[nodiscard]]
    std::optional<std::filesystem::path>
        CommonSharedSettings::BuildPackageSubPath(std::wstring_view subDir)
    {
        wil::unique_cotaskmem_string localAppData;
        HRESULT const hr = SHGetKnownFolderPath(
            FOLDERID_LocalAppData,
            0,
            nullptr,
            localAppData.put());
        if (FAILED(hr) || !localAppData)
        {
            return std::nullopt;
        }

        std::array<wchar_t, PACKAGE_FAMILY_NAME_MAX_LENGTH + 1> familyName{};
        UINT32 len = static_cast<UINT32>(familyName.size());
        LONG const rc = GetCurrentPackageFamilyName(&len, familyName.data());
        if (rc != ERROR_SUCCESS)
        {
            return std::nullopt;
        }

        size_t const nameLen =
            wcsnlen_s(familyName.data(), familyName.size());

        std::filesystem::path result{ localAppData.get() };
        result /= L"Packages";
        result /= std::wstring_view{ familyName.data(), nameLen };
        result /= subDir;

        return result;
    }

    CommonSharedSettings::CommonSharedSettings()
        : m_localDataFolder(
            BuildPackageSubPath(L"LocalCache\\Local")),
        m_settingsFolder(
            BuildPackageSubPath(L"Settings"))
    {
        if (m_settingsFolder && !m_settingsFolder->empty())
        {
            std::filesystem::path ini = *m_settingsFolder;
            ini /= L"settings.ini";
            m_settingsIniPath = std::move(ini);

            StartMonitorThread();
        }
    }

    void CommonSharedSettings::StartMonitorThread()
    {
        if (!m_settingsIniPath || m_settingsIniPath->empty())
            return;

        auto dir = m_settingsIniPath->parent_path();
        if (dir.empty())
            return;

        m_hChange = FindFirstChangeNotificationW(
            dir.c_str(),
            FALSE, // Non-recursive
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_FILE_NAME);

        if (!m_hChange || m_hChange == INVALID_HANDLE_VALUE)
        {
            m_hChange = nullptr;
            return;
        }

        m_monitorStarted.store(true, std::memory_order_relaxed);

        m_monitorThread = std::thread {
            [this]
            {
                MonitorLoop();
            }
        };
    }

    void CommonSharedSettings::MonitorLoop()
    {
        if (!m_hChange || m_hChange == INVALID_HANDLE_VALUE)
            return;

        FILETIME lastWrite{};
        if (m_settingsIniPath && !m_settingsIniPath->empty())
        {
            WIN32_FILE_ATTRIBUTE_DATA fad{};
            if (GetFileAttributesExW(
                m_settingsIniPath->c_str(),
                GetFileExInfoStandard,
                &fad))
            {
                lastWrite = fad.ftLastWriteTime;
            }
        }

        for (;;)
        {
            if (m_stopMonitor.load(std::memory_order_relaxed))
                break;

            DWORD dwWait = WaitForSingleObject(m_hChange, INFINITE);
            if (dwWait != WAIT_OBJECT_0)
            {
                break; // Handle error or thread terminated
            }

            if (m_settingsIniPath && !m_settingsIniPath->empty())
            {
                WIN32_FILE_ATTRIBUTE_DATA cur{};
                if (GetFileAttributesExW(
                    m_settingsIniPath->c_str(),
                    GetFileExInfoStandard,
                    &cur))
                {
                    if (CompareFileTime(&cur.ftLastWriteTime, &lastWrite) != 0)
                    {
                        lastWrite = cur.ftLastWriteTime;
                        NotifyConfigChanged();
                    }
                }
            }

            if (!FindNextChangeNotification(m_hChange))
            {
                break;
            }
        }

        m_monitorStarted.store(false, std::memory_order_relaxed);
    }

    void CommonSharedSettings::NotifyConfigChanged()
    {
        if (s_manualShutdown.load(std::memory_order_relaxed))
            return;

        std::vector<Listener> listenersCopy;
        {
            std::lock_guard lock(m_listenersMutex);
            listenersCopy = m_listeners;
        }

        for (auto const& l : listenersCopy)
        {
            if (l.cb)
            {
                try
                {
                    l.cb(l.ctx);
                }
                catch (...)
                {
                    // ignore
                }
            }
        }
    }

    void CommonSharedSettings::RegisterChangeListener(ChangeCallback cb, void* context)
    {
        if (s_manualShutdown.load(std::memory_order_relaxed))
            return;

        if (!cb)
            return;

        std::lock_guard lock(m_listenersMutex);
        m_listeners.push_back(Listener{ cb, context });
    }

    void CommonSharedSettings::UnregisterChangeListener(ChangeCallback cb, void* context)
    {
        if (s_manualShutdown.load(std::memory_order_relaxed))
            return;

        std::lock_guard lock(m_listenersMutex);

        auto it = std::remove_if(
            m_listeners.begin(),
            m_listeners.end(),
            [cb, context](Listener const& l)
            {
                return l.cb == cb && l.ctx == context;
            });

        m_listeners.erase(it, m_listeners.end());
    }

    std::optional<std::wstring> CommonSharedSettings::GetString(
        std::wstring_view section,
        std::wstring_view key) const
    {
        if (!m_settingsIniPath || m_settingsIniPath->empty())
            return std::nullopt;

        std::lock_guard guard(m_ioMutex);

        wchar_t buf[256] = {};
        SetLastError(0);

        DWORD len = GetPrivateProfileStringW(
            std::wstring(section).c_str(),
            std::wstring(key).c_str(),
            nullptr,
            buf,
            static_cast<DWORD>(std::size(buf)),
            m_settingsIniPath->c_str());

        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
            return std::nullopt;

        if (len == 0)
            return std::nullopt;

        return std::wstring(buf, len);
    }

    void CommonSharedSettings::SetString(
        std::wstring_view section,
        std::wstring_view key,
        std::wstring_view value)
    {
        if (!m_settingsIniPath || m_settingsIniPath->empty())
            return;

        std::lock_guard guard(m_ioMutex);

        SetLastError(0);
        WritePrivateProfileStringW(
            std::wstring(section).c_str(),
            std::wstring(key).c_str(),
            std::wstring(value).c_str(),
            m_settingsIniPath->c_str());
    }

    std::optional<int> CommonSharedSettings::GetInt(
        std::wstring_view section,
        std::wstring_view key) const
    {
        auto s = GetString(section, key);
        if (!s)
            return std::nullopt;

        try
        {
            return std::stoi(*s);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    void CommonSharedSettings::SetInt(
        std::wstring_view section,
        std::wstring_view key,
        int value)
    {
        SetString(section, key, std::to_wstring(value));
    }

    std::optional<bool> CommonSharedSettings::GetBool(
        std::wstring_view section,
        std::wstring_view key) const
    {
        auto v = GetInt(section, key);
        if (!v)
            return std::nullopt;
        return *v != 0;
    }

    void CommonSharedSettings::SetBool(
        std::wstring_view section,
        std::wstring_view key,
        bool value)
    {
        SetInt(section, key, value ? 1 : 0);
    }
}
