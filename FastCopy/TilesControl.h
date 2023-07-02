#pragma once

#include "TilesControl.g.h"

namespace winrt::FastCopy::implementation
{
    struct TilesControl : TilesControlT<TilesControl>
    {
        TilesControl();
    private:
        winrt::Microsoft::UI::Xaml::Controls::ItemsControl control{ nullptr };
        Loaded_revoker m_loadedRevoker;
        std::vector<double> m_renderOffsets;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct TilesControl : TilesControlT<TilesControl, implementation::TilesControl>
    {
    };
}
