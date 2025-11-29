#include "DebugPrint.h"

#include "CommonSharedSettings.h"

namespace
{
    FastCopyLogger::Verbosity GetVerbosityFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            auto v = s.GetInt(L"Logger", L"Verbosity").value_or(2); // default: Warn(2)
            if (v < 0) v = 0;
            if (v > 5) v = 5;
            return static_cast<FastCopyLogger::Verbosity>(v);
        }
        catch (...) {
            return FastCopyLogger::Verbosity::Warn;
        }
    }

    bool GetEnabledFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            return s.GetBool(L"Logger", L"Enabled").value_or(true);
        }
        catch (...) {
            return true;
        }
    }

    bool GetDebugBreakEnabledFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            return s.GetBool(L"Logger", L"DebugBreakEnabled").value_or(false);
        }
        catch (...) {
            return false;
        }
    }

    FastCopyLogger::Verbosity GetDebugBreakVerbosityFromConfig() noexcept {
        try {
            auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
            auto v = s.GetInt(L"Logger", L"DebugBreakMinVerbosity").value_or(4); // default: Debug(4)
            if (v < 0) v = 0;
            if (v > 5) v = 5;
            return static_cast<FastCopyLogger::Verbosity>(v);
        }
        catch (...) {
            return FastCopyLogger::Verbosity::Warn;
        }
    }
} // namespace

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
    static FastCopyLogger s;
    return s;
}