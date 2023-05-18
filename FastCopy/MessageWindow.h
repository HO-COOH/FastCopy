#pragma once

#include <functional>
class MessageWindow
{
public:
	static MessageWindow& GetInstance();

	template<typename Func>
	void OnSettingsChanged(Func&& f);

private:
};

