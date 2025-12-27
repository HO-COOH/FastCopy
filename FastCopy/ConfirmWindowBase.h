#pragma once
#include "RecycleBinIcon.h"
#include <include/HwndHelper.hpp>
#include "WindowHelper.h"
#include "Settings.h"

namespace winrt
{
	namespace Windows::Foundation
	{
		struct IInspectable;
	}

	namespace Microsoft::UI::Xaml
	{
		struct RoutedEventArgs;
	}
}

template<typename Derived>
struct ConfirmWindowBase
{
	bool Result{};

	ConfirmWindowBase()
	{
		auto self = static_cast<Derived*>(this);
		self->InitializeComponent();
		self->RecycleBinIcon().Source(::RecycleBinIcon{});
		auto const hwnd = GetHwnd(*self);
		SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) /*& ~WS_THICKFRAME*/ & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_DLGMODALFRAME);
		CenterWindow(hwnd, Derived::Size);
		Settings{}.Set(Settings::IsFirstConfirm, false);
	}

	void YesButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		Result = true;
		static_cast<Derived*>(this)->Close();
	}

	void NoButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		Result = false;
		static_cast<Derived*>(this)->Close();
	}
};
