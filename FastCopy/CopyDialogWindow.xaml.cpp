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

        //static bool hasSetSize = false;
        //CopyDialog().Loaded([this](auto, auto)
        //{
        //    auto size = CopyDialog().DesiredSize();
        //    CenterWindow(*this, { static_cast<int>(size.Width), static_cast<int>(size.Height) });
        //    hasSetSize = true;
        //    OutputDebugString(L"Size changed\n");
        //});


        //CopyDialog().MainPanel().SizeChanged([this](auto, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs e)
        //{
        //    if(!hasSetSize)
        //    {
        //        CenterWindow(*this, { static_cast<int>(e.NewSize().Width), static_cast<int>(e.NewSize().Height) });
        //    }
        //    hasSetSize = !hasSetSize;
        //});

        ViewModelLocator::GetInstance().RobocopyViewModel().DuplicateFiles().VectorChanged(
            [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto)
            {
                auto const numElements = original.Size();
                auto const isEmpty = numElements == 0;
                if (isEmpty && m_currentSize != Sizes[0])
                {
                    ResizeWindow(*this, Sizes[0]);
                    m_currentSize = Sizes[0];
                }
                else if (!isEmpty)
                {
                    auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);
                    if (m_currentSize == Sizes[clamped])
                        return;

                    ResizeWindow(*this, Sizes[clamped]);
                    m_currentSize = Sizes[clamped];
                }
            }
        );
    }
}


void winrt::FastCopy::implementation::CopyDialogWindow::CopyDialog_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
