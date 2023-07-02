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
		if (!m_completed)
			return *this;

		//This might be called too fast, so animatedValue is already child of an on-going storyboard
		m_storyboard.Children().Append(animatedValue.m_animation);
	}
	catch (...) {}

	return *this;
}

void StoryboardWrapper::Begin()
{
	if (!m_completed)
		return;

	m_revoker = m_storyboard.Completed(winrt::auto_revoke, [now = std::chrono::steady_clock::now(), this](auto...)
	{
		m_completed = true;
		m_storyboard.Stop();
		m_storyboard.Children().Clear();
	});
	m_completed = false;
	m_storyboard.Begin();
}
