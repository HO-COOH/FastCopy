#pragma once
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
struct LayoutUpdateAwaiter
{
    winrt::Microsoft::UI::Xaml::FrameworkElement m_target;
    winrt::Microsoft::UI::Xaml::FrameworkElement::LayoutUpdated_revoker m_revoker;
    bool m_completed = false;

    LayoutUpdateAwaiter(winrt::Microsoft::UI::Xaml::FrameworkElement const& target) : m_target(target)
    {
    }

    bool await_ready()
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
        m_revoker = m_target.LayoutUpdated(winrt::auto_revoke, [this, handle](auto&&...)
            {
                if (!m_completed)
                {
                    m_completed = true;
                    m_revoker.revoke();
                    handle.resume();
                }
            });
        m_target.UpdateLayout();
    }

    void await_resume()
    {
    }
};
class TextBlockClipAnimation
{
    //The following revoker does not work because of https://github.com/microsoft/microsoft-ui-xaml/issues/10937
    //winrt::Microsoft::UI::Composition::CompositionScopedBatch::Completed_revoker m_textAnimationCompletedRevoker;

    winrt::event_token m_textAnimationCompletedRevoker{};
    winrt::Microsoft::UI::Composition::CompositionScopedBatch m_textAnimationBatch{ nullptr };
    winrt::Microsoft::UI::Composition::InsetClip m_clip{ nullptr };
public:
    template<bool reveal>
    winrt::fire_and_forget PlayTextRevealAnimation(auto const& element)
    {
        if (element.ActualWidth() == 0)
        {
            element.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            co_await LayoutUpdateAwaiter{ element };
        }

        auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
        auto compositor = visual.Compositor();

        if (!m_clip)
        {
            m_clip = compositor.CreateInsetClip();
            visual.Clip(m_clip);
        }

        auto const width = static_cast<float>(element.ActualWidth());
        m_clip.RightInset(width);

        if (m_textAnimationBatch)
        {
            m_textAnimationBatch.Completed(m_textAnimationCompletedRevoker);
            m_clip.StopAnimation(L"RightInset");
        }

        auto animation = compositor.CreateScalarKeyFrameAnimation();

        if constexpr (reveal)
        {
            element.Opacity(0.8);
            animation.InsertKeyFrame(0.0f, width);
            animation.InsertKeyFrame(1.0f, 0.0f);
        }
        else
        {
            animation.InsertKeyFrame(0.0f, 0.0f);
            animation.InsertKeyFrame(1.0f, width);
        }

        animation.Duration(std::chrono::milliseconds{ 500 });

        if constexpr (reveal)
        {
            m_clip.StartAnimation(L"RightInset", animation);
        }
        else
        {

            m_textAnimationBatch = compositor.CreateScopedBatch(winrt::Microsoft::UI::Composition::CompositionBatchTypes::Animation);
            m_textAnimationCompletedRevoker = m_textAnimationBatch.Completed([weakElement = winrt::make_weak(element)](auto&&...)
            {
                if (auto strongElement = weakElement.get())
                    strongElement.Opacity(0);
            });
            m_clip.StartAnimation(L"RightInset", animation);
            m_textAnimationBatch.End();
        }
    }
};