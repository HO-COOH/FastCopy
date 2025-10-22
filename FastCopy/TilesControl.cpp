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
			
			Width(Items().GetAt(0).as<ChildItem>().ActualWidth());
			Height(Items().GetAt(0).as<ChildItem>().ActualHeight());
			
			auto const size = Items().Size();
			m_renderOffsets.resize(size);
			m_renderOffsets[0] = 0;

			Items().GetAt(0).as<ChildItem>().RenderTransform(winrt::Microsoft::UI::Xaml::Media::TranslateTransform{});

			for (int i = 1; i < size; ++i)
			{
				winrt::Microsoft::UI::Xaml::Media::TranslateTransform translate;
				translate.Y(10000);
				Items().GetAt(i).as<ChildItem>().RenderTransform(translate);
				m_renderOffsets[i] = m_renderOffsets[i - 1] + Items().GetAt(i - 1).as<ChildItem>().ActualHeight();
			}
			co_await std::chrono::seconds{3};

			bool isFirst = true;
			while (true)
			{
				for (int i = 0; i < size; ++i)
				{
					co_await wil::resume_foreground(dispatcher);
					if (!isFirst)
					{
						thisPtr->makeAnimation(i == 0 ? size - 1 : i - 1, i);
					}
					isFirst = false;
					co_await std::chrono::seconds{3};
				}
			}
		});
	}

	void TilesControl::makeAnimation(int previousItemIndex, int currentItemIndex)
	{
		winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard itemAnimations;
		winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation previousItemAnimation, currentItemAnimation;

		previousItemAnimation.From(-m_renderOffsets[previousItemIndex]);
		previousItemAnimation.To(-(m_renderOffsets[previousItemIndex] + ActualHeight()));
		previousItemAnimation.Duration({ std::chrono::milliseconds{300}, winrt::Microsoft::UI::Xaml::DurationType::TimeSpan });


		currentItemAnimation.From(-(m_renderOffsets[currentItemIndex] - ActualHeight()));
		currentItemAnimation.To(-m_renderOffsets[currentItemIndex]);
		currentItemAnimation.Duration({ std::chrono::milliseconds{300}, winrt::Microsoft::UI::Xaml::DurationType::TimeSpan });

		winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(previousItemAnimation, L"Y");
		winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(previousItemAnimation, Items().GetAt(previousItemIndex).as<ChildItem>().RenderTransform());
		winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(currentItemAnimation, L"Y");
		winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(currentItemAnimation, Items().GetAt(currentItemIndex).as<ChildItem>().RenderTransform());

		itemAnimations.Children().ReplaceAll({ previousItemAnimation, currentItemAnimation });
		itemAnimations.Begin();
	}
}
