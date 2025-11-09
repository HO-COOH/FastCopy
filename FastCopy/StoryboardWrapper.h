#pragma once
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>

class AnimatedValue;

/**
 * @brief Holding a fire-and-forget storyboard
 */
class StoryboardWrapper
{
	winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard m_storyboard;
	bool m_completed = true;
	winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::Completed_revoker m_revoker;
public:

	/**
	 * @brief Add an instance of `AnimatedValue` to storyboard
	 *
	 * @param animatedValue
	 */
	template<typename... Value>
	StoryboardWrapper& Add(Value&&... animatedValue)
	{
		try
		{
			if (!m_completed)
			{
				m_completed = true;
				m_storyboard.Stop();
				m_storyboard.Children().Clear();
			}

			//This might be called too fast, so animatedValue is already child of an on-going storyboard
			m_storyboard.Children().ReplaceAll({ (animatedValue.m_animation)... });
		}
		catch (...) {}

		return *this;
	}
	void Begin();

	~StoryboardWrapper();
};
