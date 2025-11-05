#pragma once
#include <gdiplus.h>

class GdiplusInitializer
{
public:
	GdiplusInitializer()
	{
		Gdiplus::GdiplusStartup(&gdiplusToken, &input, nullptr);
	}

	~GdiplusInitializer()
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
private:
	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR gdiplusToken;
};