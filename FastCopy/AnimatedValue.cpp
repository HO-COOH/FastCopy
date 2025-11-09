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

double AnimatedValue::To() const
{
	return m_animation.To().GetDouble();
}

AnimatedValue& AnimatedValue::EasingFunction(winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase const& easing)
{
	create();
	m_animation.EasingFunction(easing);
	return *this;
}
