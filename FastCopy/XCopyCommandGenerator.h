#pragma once
#include <winrt/Windows.Foundation.Collections.h>
#include "XCopyFlags.g.h"

struct Task
{
	std::string source;
	std::string& destination;
	winrt::FastCopy::ExplorerItem pItem{ nullptr };

	Task(std::string source, std::string& destination, winrt::FastCopy::ExplorerItem item) : source{ std::move(source) }, destination{ destination }, pItem{ item } {}
	Task(winrt::FastCopy::ExplorerItem item, std::string& destination) : source{ winrt::to_string(item.Name()) }, destination{ destination }, pItem{ item } {}
};

class XCopyCommandGenerator
{
public:
	winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> m_flags
	{ 
		winrt::single_threaded_vector<winrt::Windows::Foundation::IInspectable>() 
	};

	std::string makeCommand(Task const& task, winrt::FastCopy::Mode mode);

	static XCopyCommandGenerator& GetInstance();

	XCopyCommandGenerator(XCopyCommandGenerator const&) = delete;
	XCopyCommandGenerator(XCopyCommandGenerator&&) = delete;
	XCopyCommandGenerator& operator=(XCopyCommandGenerator const&) = delete;
	XCopyCommandGenerator& operator=(XCopyCommandGenerator&&) = delete;
private:
	std::string appendCustomCommand();

	XCopyCommandGenerator() = default;
};

