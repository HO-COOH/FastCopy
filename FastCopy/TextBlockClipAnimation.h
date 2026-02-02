#pragma once
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>

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
    TextBlockClipAnimation& SetImplicitAnimations(auto const& element)
    {
        auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
        createResourcesIfNeeded(visual);

        auto clip = m_compositor.CreateInsetClip();
        visual.Clip(clip);
        m_bindExpression.SetReferenceParameter(L"host", visual);
        clip.StartAnimation(L"RightInset", m_bindExpression);

        winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetImplicitShowAnimation(element, m_clipRightInsetRevealAnimation);
        winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetImplicitHideAnimation(element, m_clipRightInsetHideAnimation);
        
        return *this;
    }
};