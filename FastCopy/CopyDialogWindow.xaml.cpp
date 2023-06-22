// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "CopyDialogWindow.xaml.h"
#if __has_include("CopyDialogWindow.g.cpp")
#include "CopyDialogWindow.g.cpp"
#endif

#include "WindowHelper.h"
#include "Global.h"
#include "Taskbar.h"
#include <winrt/Windows.Graphics.h>
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include "ViewModelLocator.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    CopyDialogWindow::CopyDialogWindow()
    {
        InitializeComponent();

        CenterWindow(*this, m_currentSize);
        auto appWindow = GetAppWindow(*this);
        auto presenter = appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();
        presenter.IsResizable(false);
        presenter.IsMaximizable(false);
        presenter.IsMinimizable(true);

        auto titleBar = appWindow.TitleBar();
        titleBar.ExtendsContentIntoTitleBar(true);
        titleBar.ButtonBackgroundColor(winrt::Windows::UI::Colors::Transparent());
        titleBar.ButtonInactiveBackgroundColor(winrt::Windows::UI::Colors::Transparent());
        Global::UIThread = DispatcherQueue();
        Taskbar::SetProgressState(Global::MainHwnd, Taskbar::ProgressState::Indeterminate);


        ViewModelLocator::GetInstance().RobocopyViewModel().DuplicateFiles().VectorChanged(
            [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto)
            {
                auto const numElements = original.Size();
                auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);

                if (m_currentSize == Sizes[clamped])
                    return;

                playWindowAnimation(Sizes[clamped]);

                /*set size cache*/
                m_currentSize = Sizes[clamped];
            }
        );
        Global::copyWindow = *this;
    }
    void CopyDialogWindow::Resize(winrt::Windows::Graphics::SizeInt32 size)
    {
        playWindowAnimation(size);
        m_currentSize = size;
    }
    void CopyDialogWindow::playWindowAnimation(winrt::Windows::Graphics::SizeInt32 targetSize)
    {
        winrt::Microsoft::UI::Xaml::Media::Animation::CircleEase f;
        f.EasingMode(winrt::Microsoft::UI::Xaml::Media::Animation::EasingMode::EaseInOut);
        m_heightValue
            .From(m_currentSize.Height)
            .To(targetSize.Height)
            .EasingFunction(f)
            .Duration(WindowAnimationDuration);
        m_widthValue
            .From(m_currentSize.Width)
            .To(targetSize.Width)
            .EasingFunction(f)
            .Duration(WindowAnimationDuration);

        (m_storyboardWrapper << m_heightValue << m_widthValue).Begin();

        m_heightValue.OnValueChange([this](auto, double value) {
            auto const widthValue = (int)m_widthValue;
            if (widthValue != 0 && value != 0)
                ResizeWindow(*this, { .Width = widthValue, .Height = (int)value });
        });
    }
}

