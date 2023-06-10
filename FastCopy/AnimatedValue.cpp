#include "pch.h"
#include "AnimatedValue.h"


void AnimatedValue::create()
{
	if (m_animation)
		return;
	m_value = {};
	m_animation = {};
	m_animation.EnableDependentAnimation(true);
	winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(m_animation, L"Value");
	winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(m_animation, m_value);
}

AnimatedValue& AnimatedValue::From(double value)
{
	create();
	m_animation.From(value);
	return *this;
}

AnimatedValue& AnimatedValue::To(double value)
{
	create();
	m_animation.To(value);
	return *this;
}

AnimatedValue& AnimatedValue::EasingFunction(winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase const& easing)
{
	create();
	m_animation.EasingFunction(easing);
	return *this;
}

StoryboardWrapper& StoryboardWrapper::operator<<(AnimatedValue const& animatedValue)
{
	try
	{
		//This might be called too fast, so animatedValue is already child of an on-going storyboard
		m_storyboard.Children().Append(animatedValue.m_animation);
	}
	catch (...) {}

	return *this;
}

void StoryboardWrapper::Begin()
{
	//m_storyboard.Duration({ std::chrono::milliseconds{5000} });
	m_storyboard.Begin();
	m_storyboard.Completed([now = std::chrono::steady_clock::now()](auto...)
		{
			OutputDebugString(std::to_wstring(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count()).data());
		});
}
