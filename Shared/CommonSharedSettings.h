#pragma once

#include <Windows.h>

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <thread>

namespace FastCopy::Settings
{
    class CommonSharedSettings
    {
    public:
        CommonSharedSettings(const CommonSharedSettings&) = delete;
        CommonSharedSettings& operator=(const CommonSharedSettings&) = delete;
        CommonSharedSettings(CommonSharedSettings&&) = delete;
        CommonSharedSettings& operator=(CommonSharedSettings&&) = delete;

        ~CommonSharedSettings()
        {
            // In non-DLL scenarios (regular EXE), ensure that background
            // monitoring threads are properly stopped before the process exits.
            // In DLL scenarios, if Shutdown is not explicitly called,
            // the destructor might never run 
            // (for example，Host process may call TerminateProcess),
            // so it also needs to cooperate with the DLL's cleanup callback.
            Shutdown();
        }

        static CommonSharedSettings& Instance() noexcept;

        // Shutdown the background monitor thread and close the change notification handle.
        //
        // - EXE: the destructor already calls Shutdown(), so it is normally optional.
        //        You may still call it explicitly if you want deterministic cleanup.
        // - DLL (e.g. shell extensions in dllhost.exe): the destructor might never run
        //        on the TerminateProcess path, so the host must call Shutdown() from an
        //        explicit cleanup point (e.g. FastCopyCleanupCallback in DllMain).
        void Shutdown();

        // Local data directory: %LocalAppData%\Packages\<family>\LocalCache\Local
        [[nodiscard]]
        std::optional<std::filesystem::path> const& GetLocalDataFolder() const noexcept
        {
            return m_localDataFolder;
        }

        // Settings Directory: %LocalAppData%\Packages\<family>\Settings
        [[nodiscard]]
        std::optional<std::filesystem::path> const& GetSettingsFolder() const noexcept
        {
            return m_settingsFolder;
        }

        // settings.ini full path：%LocalAppData%\Packages\<family>\Settings\settings.ini
        [[nodiscard]]
        std::optional<std::filesystem::path> const& GetSettingsIniPath() const noexcept
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

    private:
        CommonSharedSettings();

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
        std::atomic<bool> m_stopMonitor{ false };
        static std::atomic<bool> s_manualShutdown;
        std::thread m_monitorThread;

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
