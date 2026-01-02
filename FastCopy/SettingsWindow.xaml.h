#pragma once
#include "SettingsWindow.g.h"
#include <include/PropertyChangeHelper.hpp>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>

namespace winrt::FastCopy::implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow>, MvvmHelper::PropertyChangeHelper<SettingsWindow>
    {
        SettingsWindow();

        winrt::FastCopy::SettingsViewModel ViewModel();
        void ThemeSelectionComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void WindowEx_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void RenameSuffixTextBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);

    private:
        void isRenameSuffixInvalid(bool value);
        //The following revoker does not work because of https://github.com/microsoft/microsoft-ui-xaml/issues/10937
        //winrt::Microsoft::UI::Composition::CompositionScopedBatch::Completed_revoker m_textAnimationCompletedRevoker;

        winrt::event_token m_textAnimationCompletedRevoker{};
        winrt::Microsoft::UI::Composition::CompositionScopedBatch m_textAnimationBatch{ nullptr };
        winrt::Microsoft::UI::Composition::InsetClip m_clip{ nullptr };

        template<bool reveal>
        void playTextRevealAnimation(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& textBlock)
        {
            textBlock.UpdateLayout();
            auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(textBlock);
            auto compositor = visual.Compositor();

            if (!m_clip)
                m_clip = compositor.CreateInsetClip();
            auto const textWidth = static_cast<float>(textBlock.ActualWidth());
            m_clip.RightInset(textWidth);

            visual.Clip(m_clip);

            if (m_textAnimationBatch)
            {
                m_textAnimationBatch.Completed(m_textAnimationCompletedRevoker);
                m_clip.StopAnimation(L"RightInset");
            }

            auto animation = compositor.CreateScalarKeyFrameAnimation();

            if constexpr (reveal)
            {
                animation.InsertKeyFrame(0.0f, textWidth);
                animation.InsertKeyFrame(1.0f, 0.0f);
            }
            else
            {
                animation.InsertKeyFrame(0.0f, 0.0f);
                animation.InsertKeyFrame(1.0f, textWidth);
            }

            animation.Duration(std::chrono::milliseconds{ 500 });

            if constexpr (reveal)
            {
                m_clip.StartAnimation(L"RightInset", animation);
            }
            else
            {

                m_textAnimationBatch = compositor.CreateScopedBatch(winrt::Microsoft::UI::Composition::CompositionBatchTypes::Animation);
                m_textAnimationCompletedRevoker = m_textAnimationBatch.Completed([textBlock](auto&&, auto&&)
                {
                    textBlock.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
                });
                m_clip.StartAnimation(L"RightInset", animation);
                m_textAnimationBatch.End();
            }
        }
    public:
        void ConfirmDeleteToggle_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        constexpr static winrt::Microsoft::UI::Xaml::Visibility NotToVisibility(bool value)
        {
            return value ? winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                : winrt::Microsoft::UI::Xaml::Visibility::Visible;
        }
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow, implementation::SettingsWindow>
    {
    };
}
