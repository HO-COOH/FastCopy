#include "pch.h"
#include "StoryboardWrapper.h"
#include "AnimatedValue.h"

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

StoryboardWrapper::~StoryboardWrapper()
{
	m_storyboard.Stop();
	m_storyboard.Children().Clear();
}
