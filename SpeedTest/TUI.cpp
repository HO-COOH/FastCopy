#define NOMINMAX
#include "TUI.h"
#include "TestFactory.h"
#include "TestCases.h"
#include "COMApiTest.h"
#include "FilesystemApiTest.h"
#include "RobocopyTest.h"


#include <Windows.h>
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector
#include <format>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border
#pragma once

using namespace ftxui;
using namespace ftxui::Container;



namespace std
{
    template<>
    struct formatter<std::reference_wrapper<int>> : std::formatter<int>
    {
        auto format(std::reference_wrapper<int> intRef, auto& formatContext) const noexcept
        {
            return std::formatter<int>{}.format(intRef.get(), formatContext);
        }
    };
}

static auto PrintTime(std::chrono::steady_clock::duration duration)
{
    auto const min = std::chrono::duration_cast<std::chrono::minutes>(duration);
    auto const sec = std::chrono::duration_cast<std::chrono::seconds>(duration - min).count();
    return std::format("{}:{} ", min.count(), sec);
}

auto makeTestResult(auto name, TestData::TabData::Implementations::ImplementationResult& implementationResult)
{
    auto checkbox = Checkbox(std::string{name}, & implementationResult.selected);
    return Renderer(
        checkbox,
        [=, &implementationResult] {
            return hbox({ checkbox->Render(),
                filler() | flex,
                text(implementationResult.started? PrintTime(implementationResult.endTime == 0? std::chrono::steady_clock::now() - implementationResult.startTime : implementationResult.endTime - implementationResult.startTime) : "xx:xx "),
            });
        }
    );
}

auto makeSection(auto name, TestData::TabData::Implementations& implementation)
{
    auto checkboxes = Vertical({
        makeTestResult("COM", implementation.COM_selected),
        makeTestResult("Win32", implementation.win32_selected),
        makeTestResult("std::filesystem", implementation.std_selected),
        makeTestResult("xcopy", implementation.xcopy_selected),
        makeTestResult("robocopy", implementation.robocopy_selected),
        });
    auto sectionCheckbox = Checkbox(std::string{name}, & implementation.generate);

    auto section = Horizontal({ sectionCheckbox , checkboxes });
    return Renderer(
        section,
        [=]() {
            return hbox({
                sectionCheckbox->Render() | center,
                separator(),
                checkboxes->Render() | xflex
                });
        }
    );
}

auto makeAdjustableSection(auto name, TestData::TabData::Implementations& implementation, int& sliderValue)
{
    auto checkboxes = Vertical({
        Slider("Size: ", &sliderValue, 256, 4096),
        makeTestResult("COM", implementation.COM_selected),
        makeTestResult("Win32", implementation.win32_selected),
        makeTestResult("std::filesystem", implementation.std_selected),
        makeTestResult("xcopy", implementation.xcopy_selected),
        makeTestResult("robocopy", implementation.robocopy_selected),
        });
    auto sectionCheckbox = Checkbox(std::string{name}, & implementation.generate);

    auto section = Horizontal({ sectionCheckbox , checkboxes });
    return Renderer(
        section,
        [=] {
            return hbox({
                sectionCheckbox->Render() | center,
                separator(),
                checkboxes->Render() | xflex
                });
        }
    );
}

auto makeTab(TestData::TabData& tabData, auto startButtonPress)
{
    return
        Vertical({
            makeSection("4K files", tabData.fourK) | border,
            makeAdjustableSection("Big files", tabData.fourK, tabData.bigFileSizeMB) | border,
            Button("Start", startButtonPress)
        });
}

void TUI::runCopyTab()
{
    if (data.copy_tab.fourK.COM_selected.selected)
        TestFactory::Register(std::make_unique<COMApiTest>());

    TestFactory::BeforeRunImplementation([](TestData::TabData::Implementations::ImplementationResult& result)
    {
        result.started = true;
        result.startTime = std::chrono::steady_clock::now();
    });

    TestFactory::AfterRunImplementation([](TestData::TabData::Implementations::ImplementationResult& result)
    {
        result.endTime = std::chrono::steady_clock::now();
    });
    TestFactory::RunAllTest();
    
}

void TUI::runImpl()
{
//	//ExplorerGuard guard;
//#ifdef _DEBUG
//		//manually add debugging test implementation here...
//		//In release build, all tests registered with AutoRegister<Self> runs
//
//		//TestFactory::Register(std::make_unique<COMApiTest>());
//	TestFactory::Register(std::make_unique<COMApiTest>());
//	TestFactory::Register(std::make_unique<FilesystemApiTest>());
//	TestFactory::Register(std::make_unique<RobocopyTest>());
//#endif
//	TestFactory{}
//		//<< Random4KFiles{}
//		//<< BigFile{};
//	<< MoveFileSamePartition{};
//	TestFactory::RunAllTest();
//	TestFactory::PrintResult();
//	//TestFactory::Clear();

    std::vector<std::string> tab_values
    {
        "Copy",
        "Move",
        "Delete",
    };
    int tab_selected = 0;
    auto tab_toggle = Toggle(&tab_values, &tab_selected);


    auto sourceDirInput = Input(&data.sourceDir, "SoureDir");
    auto destDirInput = Input(&data.destDir, "DestDir");

    auto tab_container = Tab(
        {
            makeTab(data.copy_tab, [this] {runCopyTab(); }),
            makeTab(data.move_tab, [] {OutputDebugString(L"Move"); }),
            makeTab(data.delete_tab, [] {OutputDebugString(L"Delete"); })
        }, &tab_selected
    );

    auto container = Vertical({
        sourceDirInput,
        destDirInput,
        tab_toggle,
        tab_container
    });


    ScreenInteractive::TerminalOutput().Loop(Renderer(
        container,
        [&]
        {
            return vbox
            ({
                sourceDirInput->Render() | xflex,
                destDirInput->Render() | xflex,
                separator(),
                tab_toggle->Render(),
                separator(),
                tab_container->Render(),
                }) | border;
        })
    );
}

void TUI::Run()
{
    std::thread t{
        [] { TUI{}.runImpl(); }
    };

    t.detach();

    while (true)
    {
        ftxui::ScreenInteractive::TerminalOutput().PostEvent(ftxui::Event::Custom);
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
    //t.join();
}
