#pragma once
#include <winrt/Microsoft.UI.Xaml.Data.h>

template<typename Self>
class PropertyChangeHelper
{
	winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
public:
	winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
	{
		return m_propertyChanged.add(value);
	}

	void PropertyChanged(winrt::event_token const& token)
	{
		m_propertyChanged.remove(token);
	}

	void raisePropertyChange(wchar_t const* propertyName)
	{
		m_propertyChanged(*(static_cast<Self*>(this)), winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ propertyName });
	}
};