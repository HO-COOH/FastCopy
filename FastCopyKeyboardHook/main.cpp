#include "KeyboardHookApp.h"
#include "../Public/KeyboardHookSettings.h"
#include <wil/resource.h>
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    try
    {
        // Both gates are checked before constructing: the constructor already
        // creates the message window and installs the low-level keyboard hook.
        if (!KeyboardHookSettings::IsEnabled())
        {
            return 0;
        }

        // Held for the lifetime of the app, so a second instance never installs
        // a second system-wide hook.
        wil::unique_mutex singleton{ CreateMutexW(nullptr, FALSE, KeyboardHookSettings::SingletonName) };
        if (!singleton || GetLastError() == ERROR_ALREADY_EXISTS)
        {
            return 0;
        }

        KeyboardHookApp app{ instance };
        return app.Run();
    }
    catch (wil::ResultException const& e)
    {
        KeyboardHookApp::ShowError(e.what());
        return 1;
    }
    catch (std::exception const& e)
    {
        KeyboardHookApp::ShowError(e.what());
        return 1;
    }
}
