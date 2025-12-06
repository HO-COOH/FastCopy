#pragma once

#include <windows.h>
#include <chrono>

namespace DebugHelper
{
    [[nodiscard]] constexpr bool is_debug_build() noexcept
    {
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
        return true;
#else
        return false;
#endif
    }

    // Returns true if a debugger is currently attached.
    [[nodiscard]] inline bool is_debugger_attached() noexcept
    {
        return ::IsDebuggerPresent() != FALSE;
    }

    // Triggers a breakpoint.
    inline void debug_break() noexcept
    {
#if defined(_MSC_VER)
        __debugbreak();
#else
        ::DebugBreak();
#endif
    }

    // Waits for a debugger to attach and breaks; returns false on timeout.
    inline bool DebugBreakWaitForAttach(
        std::chrono::milliseconds timeout = std::chrono::milliseconds{ -1 },
        std::chrono::milliseconds poll_interval = std::chrono::milliseconds{ 100 }
    ) noexcept
    {
        const bool infinite_wait = (timeout.count() < 0);
        const auto start = std::chrono::steady_clock::now();

        long long poll_ms_ll = poll_interval.count();
        if (poll_ms_ll <= 0)
            poll_ms_ll = 1;
        const DWORD poll_ms = static_cast<DWORD>(poll_ms_ll);

        while (!is_debugger_attached())
        {
            if (!infinite_wait)
            {
                const auto now = std::chrono::steady_clock::now();
                const auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

                if (elapsed >= timeout)
                {
                    // Timeout: no debugger attached, continue execution.
                    return false;
                }
            }

            ::Sleep(poll_ms);
        }

        // Debugger has attached; break here.
        debug_break();
        return true;
    }

    // In debug builds: break only if a debugger is attached.
    inline void DebugBreakIfAttached() noexcept
    {
        if constexpr (is_debug_build())
        {
            if (is_debugger_attached())
                debug_break();
        }
    }

    // In debug builds: wait for debugger and break; in release, no-op.
    inline bool SmartDebugBreakWaitForAttach(
        std::chrono::milliseconds timeout = std::chrono::milliseconds{ -1 },
        std::chrono::milliseconds poll_interval = std::chrono::milliseconds{ 100 }
    ) noexcept
    {
        if constexpr (is_debug_build())
        {
            return DebugBreakWaitForAttach(timeout, poll_interval);
        }
        else
        {
            (void)timeout;
            (void)poll_interval;
            return false;
        }
    }

} // namespace debug

// Break on this line in debug build if a debugger is attached.
#define DEBUG_BREAK_HERE() \
    do { ::DebugHelper::DebugBreakIfAttached(); } while (false)

// In debug build: wait up to timeout_ms for a debugger, then break here.
#define DEBUG_WAIT_FOR_DEBUGGER(timeout_ms)                                      \
    do {                                                                         \
        ::DebugHelper::SmartDebugBreakWaitForAttach(                           \
            std::chrono::milliseconds(static_cast<long long>(timeout_ms)));      \
    } while (false)
