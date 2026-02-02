#include "pch.h"
#include "TextBlockClipAnimation.h"

void TextBlockClipAnimation::createResourcesIfNeeded(winrt::Microsoft::UI::Composition::Visual const& elementVisual)
{
    if (m_compositor)
        return;

    m_compositor = elementVisual.Compositor();

    m_clipRightInsetRevealAnimation = m_compositor.CreateScalarKeyFrameAnimation();
    m_clipRightInsetRevealAnimation.InsertKeyFrame(0.f, 1.f);
    m_clipRightInsetRevealAnimation.InsertKeyFrame(1.f, 0.f);
    m_clipRightInsetRevealAnimation.Target(L"RotationAxis.X");
    //We use RotationAxis because the implicit animation can ONLY target intrinsic property on a Visual
    //Therefore we need an intrinsic property as a "proxy" and bind the clip to that "proxy"

    m_clipRightInsetHideAnimation = m_compositor.CreateScalarKeyFrameAnimation();
    m_clipRightInsetHideAnimation.InsertKeyFrame(0.f, 0.f);
    m_clipRightInsetHideAnimation.InsertKeyFrame(1.f, 1.f);
    m_clipRightInsetHideAnimation.Target(L"RotationAxis.X");

    m_bindExpression = m_compositor.CreateExpressionAnimation(L"host.RotationAxis.X * host.Size.X");
}