#pragma once

#include <Windows.h>

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

namespace FastCopy::Settings
{
    class CommonSharedSettings
    {
    public:
        static CommonSharedSettings& Instance() noexcept;

        // Local data directory: %LocalAppData%\Packages\<family>\LocalCache\Local
        [[nodiscard]]
        const std::optional<std::filesystem::path>& GetLocalDataFolder() const noexcept
        {
            return m_localDataFolder;
        }

        // Settings Directory: %LocalAppData%\Packages\<family>\Settings
        [[nodiscard]]
        const std::optional<std::filesystem::path>& GetSettingsFolder() const noexcept
        {
            return m_settingsFolder;
        }

        // settings.ini full path：%LocalAppData%\Packages\<family>\Settings\settings.ini
        [[nodiscard]]
        const std::optional<std::filesystem::path>& GetSettingsIniPath() const noexcept
        {
            return m_settingsIniPath;
        }

        std::optional<std::wstring> GetString(
            std::wstring_view section,
            std::wstring_view key) const;

        void SetString(
            std::wstring_view section,
            std::wstring_view key,
            std::wstring_view value);

        std::optional<int> GetInt(
            std::wstring_view section,
            std::wstring_view key) const;

        void SetInt(
            std::wstring_view section,
            std::wstring_view key,
            int value);

        std::optional<bool> GetBool(
            std::wstring_view section,
            std::wstring_view key) const;

        void SetBool(
            std::wstring_view section,
            std::wstring_view key,
            bool value);

        // --- Register Change Notification ---
        //
        // When settings.ini is modified (write time changes),
        // all callbacks will be called sequentially in the background monitoring thread.
        //
        // Note: Callbacks are executed in a "background thread", 
        // should not throw exceptions, and should not perform UI operations.

        using ChangeCallback = void(*)(void* context);

        void RegisterChangeListener(ChangeCallback cb, void* context);
        void UnregisterChangeListener(ChangeCallback cb, void* context);

        CommonSharedSettings();
        CommonSharedSettings(const CommonSharedSettings&) = delete;
        CommonSharedSettings(CommonSharedSettings&&);
        CommonSharedSettings& operator=(const CommonSharedSettings&) = delete;
        CommonSharedSettings& operator=(CommonSharedSettings&&) = delete;
        ~CommonSharedSettings() = default;

    private:
        // MakePath: %LocalAppData%\Packages\<family>\ + subDir
        [[nodiscard]]
        static std::optional<std::filesystem::path>
            BuildPackageSubPath(std::wstring_view subDir);

        void StartMonitorThread();
        void MonitorLoop();
        void NotifyConfigChanged();

    private:
        std::optional<std::filesystem::path> m_localDataFolder;
        std::optional<std::filesystem::path> m_settingsFolder;
        std::optional<std::filesystem::path> m_settingsIniPath;

        // File Watch Handle
        HANDLE            m_hChange = nullptr;
        std::atomic<bool> m_monitorStarted{ false };

        // this is I/O mutex
        mutable std::mutex m_ioMutex;

        // Change Callback
        struct Listener
        {
            ChangeCallback cb{};
            void* ctx{};
        };

        mutable std::mutex           m_listenersMutex;
        std::vector<Listener>        m_listeners;
    };

    // Get the current settings.ini path
    inline std::filesystem::path GetFastCopyIniPath()
    {
        auto& instance = CommonSharedSettings::Instance();
        const auto& opt = instance.GetSettingsIniPath();
        return opt ? *opt : std::filesystem::path{};
    }
}
