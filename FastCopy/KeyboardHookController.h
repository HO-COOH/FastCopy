#pragma once

namespace KeyboardHookController
{
    [[nodiscard]] bool IsEnabled();
    void SetEnabled(bool enabled);
    void Start();
    void Stop();
}
