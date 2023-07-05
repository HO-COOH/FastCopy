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
        using Type = winrt::Microsoft::UI::Xaml::FrameworkElement;

        /**
         * @brief Make sliding animation
         * 
         * @param currentItemIndex
         * @param nextItemIndex
         */
        void makeAnimation(int currentItemIndex, int nextItemIndex);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct TilesControl : TilesControlT<TilesControl, implementation::TilesControl>
    {
    };
}
