#pragma once
#include "UIThreadWrapper.h"
#include <optional>
#include <variant>
#include "AcrylicHelper.h"
#include "CopyDialogWindow.g.h"
struct Global
{
	static inline UIThreadWrapper UIThread;
	static inline HWND MainHwnd;
	static inline WindowEffectHelper windowEffectHelper;
	static inline winrt::FastCopy::CopyDialogWindow copyWindow{nullptr};
};

