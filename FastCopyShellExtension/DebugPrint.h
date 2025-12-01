#pragma once

#include <Windows.h>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <format>


namespace {
    std::wstring Win32FormatGuid(REFGUID guid) noexcept
    {
        wchar_t buf[64] = {};
        if(!StringFromGUID2(guid, buf, ARRAYSIZE(buf))) return {};
        return buf;
    }

    std::wstring utf8_to_wstring(char const* utf8_str) noexcept {
        if (!utf8_str) return {};

        // Get required buffer size (including null terminator)  
        int size_needed = MultiByteToWideChar(
            CP_UTF8,          // Code page: UTF-8  
            0,                // Flags: no special handling  
            utf8_str,         // Input narrow string  
            -1,               // Length: -1 = auto-detect null terminator  
            nullptr,          // Output buffer: none (get size)  
            0                 // Output buffer size: 0 (get size)  
        );

        if (size_needed == 0) {
            return {};
        }

        // Allocate buffer and convert  
        std::wstring wstr(size_needed - 1, L'\0');  // -1 to exclude null terminator  
        int converted = MultiByteToWideChar(
            CP_UTF8,
            0,
            utf8_str,
            -1,
            &wstr[0],        // Output buffer  
            size_needed      // Buffer size  
        );

        if (converted == 0) {
            return {};
        }

        return wstr;
    }
} // namespace

class FastCopyLogger
{
public:
    enum class Verbosity : int
    {
        MinVerbosity = 0,
        Off = 0,
        Error = 1,
        Warn = 2,
        Info = 3,
        Debug = 4,
        Trace = 5,
        MaxVerbosity = 5,
    };

    static FastCopyLogger& Instance() noexcept;

    void SetEnabled(bool enabled) noexcept
    {
        m_enabled.store(enabled, std::memory_order_relaxed);
    }

    bool IsEnabled() const noexcept
    {
        return m_enabled.load(std::memory_order_relaxed);
    }

    void SetGlobalVerbosity(Verbosity v) noexcept
    {
        std::lock_guard guard(m_mutex);
        m_initialVerbosity.store(v, std::memory_order_relaxed);
        if (m_threadVerbosityCount == 0)
        {
            m_currentVerbosity.store(v, std::memory_order_relaxed);
        }
    }

    Verbosity GetGlobalVerbosity() const noexcept
    {
        return m_currentVerbosity.load(std::memory_order_relaxed);
    }

    void OnConfigChanged(bool enabled, Verbosity verbosity, 
        bool BreakOnLog, Verbosity BreakMinLevel) noexcept
    {
        SetEnabled(enabled);
        SetGlobalVerbosity(verbosity);
        SetBreakOnLog(BreakOnLog, BreakMinLevel);
    }
    
    bool SetThreadVerbosity(Verbosity verbosity) noexcept
    {
        auto& tv = GetThreadVerbosityRef();
        if (tv) // already in use
            return false;

        tv = verbosity;

        std::lock_guard guard(m_mutex);
        ++m_threadVerbosityCount;

        auto cur = m_currentVerbosity.load(std::memory_order_relaxed);
        if (static_cast<int>(cur) < static_cast<int>(verbosity))
        {
            m_currentVerbosity.store(verbosity, std::memory_order_relaxed);
        }

        return true;
    }

    void ResetThreadVerbosity() noexcept
    {
        auto& tv = GetThreadVerbosityRef();
        if (!tv)
            return;

        tv.reset();

        std::lock_guard guard(m_mutex);
        if (--m_threadVerbosityCount == 0)
        {
            m_currentVerbosity.store(m_initialVerbosity.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
        }
    }

    class ScopedThreadVerbosity
    {
    public:
        explicit ScopedThreadVerbosity(Verbosity v) noexcept
        {
            m_inUse = FastCopyLogger::Instance().SetThreadVerbosity(v);
        }

        ~ScopedThreadVerbosity()
        {
            if (m_inUse)
                FastCopyLogger::Instance().ResetThreadVerbosity();
        }

        ScopedThreadVerbosity(const ScopedThreadVerbosity&) = delete;
        ScopedThreadVerbosity& operator=(const ScopedThreadVerbosity&) = delete;

    private:
        bool m_inUse{ false };
    };

    bool ShouldLog(Verbosity msgLevel) const noexcept
    {
        if (!m_enabled.load(std::memory_order_relaxed))
            return false;

        auto const& tv = GetThreadVerbosityRef();
        if (tv)
        {
            return static_cast<int>(*tv) >= static_cast<int>(msgLevel);
        }

        auto cur = m_currentVerbosity.load(std::memory_order_relaxed);
        return static_cast<int>(cur) >= static_cast<int>(msgLevel);
    }

    // L"xxx {}", arg
    template<typename... Args>
    void LogFmt(Verbosity lvl,
        wchar_t const* func,
        int line,
        std::wformat_string<Args...> fmt,
        Args&&... args) noexcept
    {
        if (!ShouldLog(lvl))
            return;

        try
        {
            std::wstring body = std::format(fmt, std::forward<Args>(args)...);
            std::wstring msg;
            msg.reserve(body.size() + 128);

            std::wstring_view levelTag = LevelToTag(lvl);
            std::format_to(std::back_inserter(msg),
                L"[FastCopy][{}][PID:{}][TID:{}][{}:{}] {}",
                levelTag,
                static_cast<unsigned long>(GetCurrentProcessId()),
                static_cast<unsigned long>(GetCurrentThreadId()),
                func ? func : L"(null)",
                line,
                body);

            msg.push_back(L'\n');
            OutputDebugStringW(msg.c_str());
#ifdef _DEBUG
            if (m_breakOnLog.load(std::memory_order_relaxed) &&
                static_cast<int>(lvl) <=
                static_cast<int>(m_breakMinLevel.load(std::memory_order_relaxed)) &&
                ::IsDebuggerPresent() &&
                IsDebugBreakKeyHeld())
            {
                ::DebugBreak();
            }
#endif
        }
        catch (...) {
#ifdef _DEBUG
            OutputDebugStringW(L"[FastCopy] LogFmt: failed to format log message.\n");
#endif
        }
    }

    template<typename... Args>
    void LogFmtA(Verbosity lvl,
        char const* func,
        int line,
        std::format_string<Args...> fmt,
        Args&&... args) noexcept
    {
        if (!ShouldLog(lvl))
            return;

        try
        {
            std::string bodyA = std::format(fmt, std::forward<Args>(args)...);
            std::wstring body = utf8_to_wstring(bodyA.c_str());

            std::wstring funcW = func ? utf8_to_wstring(func) : L"(null)";

            std::wstring msg;
            msg.reserve(body.size() + 128);

            std::wstring_view levelTag = LevelToTag(lvl);
            std::format_to(std::back_inserter(msg),
                L"[FastCopy][{}][PID:{}][TID:{}][{}:{}] {}",
                levelTag,
                static_cast<unsigned long>(GetCurrentProcessId()),
                static_cast<unsigned long>(GetCurrentThreadId()),
                funcW,
                line,
                body);

            msg.push_back(L'\n');
            OutputDebugStringW(msg.c_str());
#ifdef _DEBUG
            if (m_breakOnLog.load(std::memory_order_relaxed) &&
                static_cast<int>(lvl) <=
                static_cast<int>(m_breakMinLevel.load(std::memory_order_relaxed)) &&
                ::IsDebuggerPresent() &&
                IsDebugBreakKeyHeld())
            {
                ::DebugBreak();
            }
#endif
        }
        catch (...) {
#ifdef _DEBUG
            OutputDebugStringW(L"[FastCopy] LogFmt: failed to format log message.\n");
#endif
        }
    }

    void LogProcessInfo(wchar_t const* tag = L"DllMain") noexcept
    {
        wchar_t exePath[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));

        wchar_t const* exeName = wcsrchr(exePath, L'\\');
        exeName = exeName ? exeName + 1 : exePath;

        LogFmt(Verbosity::Info,
            L"ProcessInfo",
            __LINE__,
            L"[{}] Host process '{}' (PID={}), path='{}'",
            tag,
            exeName,
            static_cast<unsigned long>(GetCurrentProcessId()),
            exePath);
    }

    void LogDllPath(HMODULE hModule,
        wchar_t const* tag = L"DllMain") noexcept
    {
        wchar_t dllPath[MAX_PATH] = {};
        if (hModule)
            GetModuleFileNameW(hModule, dllPath, ARRAYSIZE(dllPath));

        LogFmt(Verbosity::Info,
            L"DllInfo",
            __LINE__,
            L"[{}] DllModule={}, path='{}'",
            tag,
            static_cast<void const*>(hModule),
            dllPath[0] ? dllPath : L"(unknown)");
    }

    void SetBreakOnLog(bool enable,
        Verbosity minLevel = Verbosity::Error) noexcept
    {
        m_breakOnLog.store(enable, std::memory_order_relaxed);
        m_breakMinLevel.store(minLevel, std::memory_order_relaxed);
    }

    bool IsBreakOnLogEnabled() const noexcept
    {
        return m_breakOnLog.load(std::memory_order_relaxed);
    }

    Verbosity GetBreakOnLogMinLevel() const noexcept
    {
        return m_breakMinLevel.load(std::memory_order_relaxed);
    }

    FastCopyLogger() noexcept;
    ~FastCopyLogger() = default;

    explicit FastCopyLogger(Verbosity initial) noexcept
        : m_initialVerbosity(initial),
        m_currentVerbosity(initial)
    {
    }
private:

    static std::optional<Verbosity>& GetThreadVerbosityRef() noexcept
    {
        static thread_local std::optional<Verbosity> s_threadVerbosity;
        return s_threadVerbosity;
    }

    static std::wstring_view LevelToTag(Verbosity lvl) noexcept
    {
        switch (lvl)
        {
        case Verbosity::Off:   return L"OFF";
        case Verbosity::Error: return L"ERR";
        case Verbosity::Warn:  return L"WRN";
        case Verbosity::Info:  return L"INF";
        case Verbosity::Debug: return L"DBG";
        case Verbosity::Trace: return L"TRC";
        }
        return L"UNK";
    }

    static bool IsDebugBreakKeyHeld() noexcept
    {
        // Debug break key is Ctrl + Shift + F1
        SHORT const ctrl = ::GetAsyncKeyState(VK_CONTROL);
        SHORT const shift = ::GetAsyncKeyState(VK_SHIFT);
        SHORT const f1 = ::GetAsyncKeyState(VK_F1);

        return (ctrl & 0x8000) &&
            (shift & 0x8000) &&
            (f1 & 0x8000);
    }
private:
    std::atomic<bool>      m_enabled{ true };
    std::atomic<Verbosity> m_initialVerbosity{ Verbosity::Warn };
    std::atomic<Verbosity> m_currentVerbosity{ Verbosity::Warn };
    std::atomic<bool>      m_breakOnLog{ false };
    std::atomic<Verbosity> m_breakMinLevel{ Verbosity::Error };

    mutable std::mutex m_mutex;
    int                m_threadVerbosityCount{ 0 };
};

#ifndef FC_LOG_DISABLE

#define FC_LOG_AT(level, ...) \
    FastCopyLogger::Instance().LogFmt( \
        level, \
        __FUNCTIONW__, \
        __LINE__, \
        __VA_ARGS__)

#define FC_LOG_ERROR(...) \
    FC_LOG_AT(FastCopyLogger::Verbosity::Error, __VA_ARGS__)

#define FC_LOG_WARN(...) \
    FC_LOG_AT(FastCopyLogger::Verbosity::Warn, __VA_ARGS__)

#define FC_LOG_INFO(...) \
    FC_LOG_AT(FastCopyLogger::Verbosity::Info, __VA_ARGS__)

#define FC_LOG_DEBUG(...) \
    FC_LOG_AT(FastCopyLogger::Verbosity::Debug, __VA_ARGS__)

#define FC_LOG_TRACE(...) \
    FC_LOG_AT(FastCopyLogger::Verbosity::Trace, __VA_ARGS__)

#define FC_LOGA_AT(level, ...) \
    FastCopyLogger::Instance().LogFmtA( \
        level, \
        __FUNCTION__, \
        __LINE__, \
        __VA_ARGS__)

#define FC_LOGA_ERROR(...) \
    FC_LOGA_AT(FastCopyLogger::Verbosity::Error, __VA_ARGS__)

#define FC_LOGA_WARN(...) \
    FC_LOGA_AT(FastCopyLogger::Verbosity::Warn, __VA_ARGS__)

#define FC_LOGA_INFO(...) \
    FC_LOGA_AT(FastCopyLogger::Verbosity::Info, __VA_ARGS__)

#define FC_LOGA_DEBUG(...) \
    FC_LOGA_AT(FastCopyLogger::Verbosity::Debug, __VA_ARGS__)

#define FC_LOGA_TRACE(...) \
    FC_LOGA_AT(FastCopyLogger::Verbosity::Trace, __VA_ARGS__)

#else

#define FC_LOG_AT(level, ...)   ((void)0)
#define FC_LOG_ERROR(...)       ((void)0)
#define FC_LOG_WARN(...)        ((void)0)
#define FC_LOG_INFO(...)        ((void)0)
#define FC_LOG_DEBUG(...)       ((void)0)
#define FC_LOG_TRACE(...)       ((void)0)

#define FC_LOGA_AT(level, ...)  ((void)0)
#define FC_LOGA_ERROR(...)      ((void)0)
#define FC_LOGA_WARN(...)       ((void)0)
#define FC_LOGA_INFO(...)       ((void)0)
#define FC_LOGA_DEBUG(...)      ((void)0)
#define FC_LOGA_TRACE(...)      ((void)0)

#endif