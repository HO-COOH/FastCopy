#include "DebugPrint.h"

#include "CommonSharedSettings.h"
#include "var_init_once.h"

namespace
{
    // Default verbosity levels
    constexpr FastCopyLogger::Verbosity DefaultLogVerbosity = FastCopyLogger::Verbosity::Warn;
    constexpr FastCopyLogger::Verbosity DefaultDebugBreakVerbosity = FastCopyLogger::Verbosity::Debug;

    // Get the verbosity level from the config and validate it
    FastCopyLogger::Verbosity GetVerbosityFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();

            // Read the verbosity as an int from the configuration
            auto v = s.GetInt(L"Logger", L"Verbosity")
                .value_or(static_cast<int>(DefaultLogVerbosity)); // Default to Warn (2)

            // Ensure the verbosity is within the allowed range
            if (v < static_cast<int>(FastCopyLogger::Verbosity::MinVerbosity)) {
                v = static_cast<int>(FastCopyLogger::Verbosity::MinVerbosity);
            }
            if (v > static_cast<int>(FastCopyLogger::Verbosity::MaxVerbosity)) {
                v = static_cast<int>(FastCopyLogger::Verbosity::MaxVerbosity);
            }

            // Return the enum value by casting back from int to Verbosity
            return static_cast<FastCopyLogger::Verbosity>(v);
        }
        catch (...) {
            return DefaultLogVerbosity;
        }
    }

    // Get the 'Enabled' setting from the config
    bool GetEnabledFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            return s.GetBool(L"Logger", L"Enabled").value_or(true);
        }
        catch (...) {
            return true; // Default to enabled
        }
    }

    // Get the 'DebugBreakEnabled' setting from the config
    bool GetDebugBreakEnabledFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            return s.GetBool(L"Logger", L"DebugBreakEnabled").value_or(false);
        }
        catch (...) {
            return false; // Default to disabled
        }
    }

    // Get the debug break verbosity level from the config
    FastCopyLogger::Verbosity GetDebugBreakVerbosityFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();

            // Read the debug break verbosity as an int
            auto v = s.GetInt(L"Logger", L"DebugBreakMinVerbosity")
                .value_or(static_cast<int>(DefaultDebugBreakVerbosity)); // Default to Debug (4)

            // Ensure the verbosity is within the valid range
            if (v < static_cast<int>(FastCopyLogger::Verbosity::Error)) {
                v = static_cast<int>(FastCopyLogger::Verbosity::Error);
            }
            if (v > static_cast<int>(FastCopyLogger::Verbosity::MaxVerbosity)) {
                v = static_cast<int>(FastCopyLogger::Verbosity::MaxVerbosity);
            }

            // Return the enum value by casting from int to Verbosity
            return static_cast<FastCopyLogger::Verbosity>(v);
        }
        catch (...) {
            return DefaultDebugBreakVerbosity;
        }
    }
}  // namespace

FastCopyLogger::FastCopyLogger() noexcept
    : FastCopyLogger(GetVerbosityFromConfig())
{
    SetEnabled(GetEnabledFromConfig());

    // Register settings.ini change notification. 
    // This will refresh settings from configuration whenever there is a change
    FastCopy::Settings::CommonSharedSettings::Instance().RegisterChangeListener(
        [](void* /*context*/) noexcept {
            auto& logger = FastCopyLogger::Instance();
            logger.OnConfigChanged(
                GetEnabledFromConfig(), 
                GetVerbosityFromConfig(), 
                GetDebugBreakEnabledFromConfig(), 
                GetDebugBreakVerbosityFromConfig() // notice: Min verbosity filter level for debug break
            );
        },
        nullptr);
}

FastCopyLogger& FastCopyLogger::Instance() noexcept
{
    STATIC_INIT_ONCE(FastCopyLogger, s);
    return *s;
}