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
#include "ViewModelLocator.h"
#include <include/HwndHelper.hpp>

namespace winrt::FastCopy::implementation
{
    CopyDialogWindow::CopyDialogWindow()
    {
        CenterWindow(Window(), NormalSize);
        Global::UIThread = DispatcherQueue();

        //The view model outlives this window, so its handlers hold it weakly. A strong ref would keep it alive past XAML shutdown
        ViewModel().DuplicateFiles().VectorChanged(
            { get_weak(), [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto&&)
            {
                auto const numElements = original.Size();
                auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);

                //set size cache and play window animation
                //playWindowAnimation(m_currentSize = Sizes[clamped]);
            } }
        );
        ViewModel().Finished(
            [weak = get_weak()](winrt::Windows::Foundation::IInspectable const&, winrt::FastCopy::FinishState state)
            {
                if (state != winrt::FastCopy::FinishState::Failed)
                    return;

                //Some items failed: hide the progress dialog and show the (bound) error text instead
                //This is raised off the UI thread, and resolving there could run our destructor there, so wait until we're on it
                Global::UIThread.TryEnqueue([weak]
                {
                    auto self = weak.get();
                    if (!self)
                        return;

                    self->CopyDialog().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
                    self->ErrorScrollViewer().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
                    //self->playWindowAnimation(Sizes[std::size(Sizes) - 1]);
                });
            }
        );
    }

    winrt::FastCopy::RobocopyViewModel CopyDialogWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().RobocopyViewModel();
    }


    void CopyDialogWindow::WindowEx_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args)
    {
        ViewModelLocator::GetInstance().RobocopyViewModel().Cancel();
    }
}
