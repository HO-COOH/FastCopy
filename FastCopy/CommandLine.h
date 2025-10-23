#pragma once
#include <string_view>
#include <winrt/base.h>
#include <boost/container/small_vector.hpp>
class Command
{
	boost::container::small_vector<std::wstring, 3> m_args;
	constexpr static std::wstring_view protocol = L"fastcopy://";
public:
	void Set(PWSTR cmd);
	static Command& Get();

	winrt::hstring GetDestination();
	std::wstring RecordFile();
};
