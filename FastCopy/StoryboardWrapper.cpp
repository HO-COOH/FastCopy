#include "pch.h"
#include "StoryboardWrapper.h"
#include "AnimatedValue.h"

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

	m_revoker = m_storyboard.Completed(winrt::auto_revoke, [this](auto...)
		{
			m_completed = true;
			m_storyboard.Stop();
			m_storyboard.Children().Clear();
		});
	m_completed = false;
	m_storyboard.Begin();
}
