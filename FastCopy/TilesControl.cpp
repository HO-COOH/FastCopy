#include "pch.h"
#include "TilesControl.h"
#if __has_include("TilesControl.g.cpp")
#include "TilesControl.g.cpp"
#endif
#include <wil/cppwinrt.h>
#include <wil/cppwinrt_helpers.h>

namespace winrt::FastCopy::implementation
{
	TilesControl::TilesControl()
	{
		m_loadedRevoker = Loaded(winrt::auto_revoke, [this](auto...)->winrt::Windows::Foundation::IAsyncAction
		{
			auto dispatcher = DispatcherQueue();
			auto thisPtr = this;
			using Type = winrt::Microsoft::UI::Xaml::FrameworkElement;
			Width(Items().GetAt(0).as<Type>().ActualWidth());
			Height(Items().GetAt(0).as<Type>().ActualHeight());
			
			auto const size = Items().Size();
			m_renderOffsets.resize(size);
			m_renderOffsets[0] = 0;

			{
				Items().GetAt(0).as<Type>().RenderTransform(winrt::Microsoft::UI::Xaml::Media::TranslateTransform{});
			}

			for (int i = 1; i < size; ++i)
			{
				winrt::Microsoft::UI::Xaml::Media::TranslateTransform translate;
				translate.Y(10000);
				Items().GetAt(i).as<Type>().RenderTransform(translate);
				m_renderOffsets[i] = m_renderOffsets[i - 1] + Items().GetAt(i - 1).as<Type>().ActualHeight();
			}
			co_await std::chrono::seconds{3};
			while (true)
			{
				for (int i = 0; i < size; ++i)
				{
					co_await wil::resume_foreground(dispatcher);
					auto previous = thisPtr->Items().GetAt(i == 0? size - 1 : i - 1).as<Type>();
					previous.RenderTransform().as<winrt::Microsoft::UI::Xaml::Media::TranslateTransform>().Y(-10000);

					auto current = thisPtr->Items().GetAt(i).as<Type>();
					current.RenderTransform().as<winrt::Microsoft::UI::Xaml::Media::TranslateTransform>().Y(-thisPtr->m_renderOffsets[i]);
					OutputDebugString(std::format(L"{}: {}\n", i, thisPtr->m_renderOffsets[i]).data());
					co_await std::chrono::seconds{3};
				}
			}
		});
	}
}
