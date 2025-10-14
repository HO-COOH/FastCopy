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
	StoryboardWrapper& operator<<(AnimatedValue const& animatedValue);
	void Begin();
};
