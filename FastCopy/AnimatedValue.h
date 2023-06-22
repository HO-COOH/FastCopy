#pragma once
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <DependentValue.g.h>

/**
 * @brief Wrapper for animated `DependencyValue`
 */
class AnimatedValue
{
	winrt::FastCopy::DependentValue m_value{nullptr};
	winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation m_animation{nullptr};

	void create();
public:
	AnimatedValue() = default;

	template<typename T>
	AnimatedValue(double from, double to, T duration, winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase* easing = nullptr)
	{
		create();
		m_animation.From(from);
		m_animation.To(to);
		m_animation.Duration(duration);
		if (easing)
			m_animation.EasingFunction(*easing);
	}

	AnimatedValue& From(double value);
	AnimatedValue& To(double value);
	AnimatedValue& EasingFunction(winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase const& easing);
	
	template<typename T>
	AnimatedValue& Duration(T value)
	{
		create();
		m_animation.Duration({ value, winrt::Microsoft::UI::Xaml::DurationType::TimeSpan });
		return *this;
	}

	template<typename Func>
	void OnValueChange(Func&& onChange)
	{
		create();
		m_value.ValueChanged(onChange);
	}

	template<typename T>
	operator T()
	{
		return static_cast<T>(m_value.Value());
	}

	friend class StoryboardWrapper;
};

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

