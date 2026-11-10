#pragma once
#include <winrt/Microsoft.UI.Composition.h>

class TextBlockClipAnimation
{
    //The following revoker does not work because of https://github.com/microsoft/microsoft-ui-xaml/issues/10937
    //winrt::Microsoft::UI::Composition::CompositionScopedBatch::Completed_revoker m_textAnimationCompletedRevoker;

    winrt::Microsoft::UI::Composition::Compositor m_compositor{ nullptr };
    winrt::Microsoft::UI::Composition::ExpressionAnimation m_bindExpression{ nullptr };
    winrt::Microsoft::UI::Composition::ScalarKeyFrameAnimation m_clipRightInsetRevealAnimation{ nullptr };
    winrt::Microsoft::UI::Composition::ScalarKeyFrameAnimation m_clipRightInsetHideAnimation{ nullptr };

    void createResourcesIfNeeded(winrt::Microsoft::UI::Composition::Visual const& elementVisual);
public:
    TextBlockClipAnimation& SetImplicitAnimations(winrt::Microsoft::UI::Xaml::UIElement const& element);
};