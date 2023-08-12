#define NOMINMAX
#include "TUI.h"
#include "TestFactory.h"
#include "TestCases.h"
#include "AllImplementations.h"
#include "Config.h"


#include <Windows.h>
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector
#include <format>
#include <filesystem>
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border
#pragma once

using namespace ftxui;
using namespace ftxui::Container;


static auto PrintTime(std::chrono::steady_clock::duration duration)
{
    auto const min = std::chrono::duration_cast<std::chrono::minutes>(duration);
    auto const sec = std::chrono::duration_cast<std::chrono::seconds>(duration - min).count();
    return std::format("{}:{} ", min.count(), sec);
}

static std::string PrintTime(Config::TabData::Implementations::ImplementationResult const& implementationResult)
{
    if (!implementationResult.started)
    {
        return "xx: xx";
    }

    auto const endTime = implementationResult.endTime == std::chrono::steady_clock::time_point{} ? std::chrono::steady_clock::now() : implementationResult.endTime;
    return PrintTime(endTime - implementationResult.startTime);
}

auto makeTestResult(auto name, Config::TabData::Implementations::ImplementationResult& implementationResult)
{
    auto checkbox = Checkbox(std::string{name}, & implementationResult.selected);
    return Renderer(
        checkbox,
        [=, &implementationResult] {
            return hbox({ checkbox->Render(),
                filler() | flex,
                text(PrintTime(implementationResult))
            });
        }
    );
}

auto makeSection(auto name, Config::TabData::Implementations& implementation)
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

auto makeAdjustableSection(auto name, Config::TabData::Implementations& implementation, int& sliderValue)
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
        [=, valuePtr = &sliderValue] {
            return hbox({
                vbox({sectionCheckbox->Render() | center, text(std::format("{} MB", *valuePtr))}),
                separator(),
                checkboxes->Render() | xflex
            });
        }
    );
}

auto makeTab(Config::TabData& tabData, auto startButtonPress)
{
    return
        Vertical({
            makeSection("4K files", tabData.fourK) | border,
            makeAdjustableSection("Big files", tabData.bigFile, tabData.bigFileSizeMB) | border,
            Button("Start", startButtonPress)
        });
}

static void SetImplementationStart(Config::TabData::Implementations::ImplementationResult& implementation)
{
    implementation.started = true;
    implementation.startTime = std::chrono::steady_clock::now();
}

static void SetImplementationFinished(Config::TabData::Implementations::ImplementationResult& implementation)
{
    implementation.endTime = std::chrono::steady_clock::now();
}

template<typename Implementation, typename TestCaseBase>
static void AddImplementationToTestCase(TestCaseBase& testCase, Config::TabData::Implementations::ImplementationResult& implementationResult)
{
    if (!implementationResult.selected)
        return;

    auto test = std::make_unique<Implementation>();
    test->started = [&implementationResult] { SetImplementationStart(implementationResult); };
    test->finished = [&implementationResult] {SetImplementationFinished(implementationResult); };
    testCase << std::move(test);
}


void TUI::runCopyTab()
{
    std::thread{[this] {
        Config::GetInstance().sourceFolder = m_tempSourceDir;
        Config::GetInstance().destinationFolder = m_tempDestDir;

        Random4KFiles randomTestCase{ /*1024ull * 1024ull * 128*/ 1024ull * 1024ull * 32 };
        AddImplementationToTestCase<COMApiTest>(randomTestCase, Config::GetInstance().copy_tab.fourK.COM_selected);
        AddImplementationToTestCase<Win32ApiTest>(randomTestCase, Config::GetInstance().copy_tab.fourK.win32_selected);
        AddImplementationToTestCase<RobocopyTest>(randomTestCase, Config::GetInstance().copy_tab.fourK.robocopy_selected);
        AddImplementationToTestCase<XCopyTest>(randomTestCase, Config::GetInstance().copy_tab.fourK.xcopy_selected);
        AddImplementationToTestCase<FilesystemApiTest>(randomTestCase, Config::GetInstance().copy_tab.fourK.std_selected);

        if (randomTestCase)
            TestFactory{} << randomTestCase;
    }}.detach();
}

void TUI::runImpl()
{
    std::vector<std::string> tab_values
    {
        "Copy",
        "Move",
        "Delete",
    };
    int tab_selected = 0;
    auto tab_toggle = Toggle(&tab_values, &tab_selected);


    auto sourceDirInput = Input(&m_tempSourceDir, "SoureDir");
    auto destDirInput = Input(&m_tempDestDir, "DestDir");

    auto tab_container = Tab(
        {
            makeTab(Config::GetInstance().copy_tab, [this] {runCopyTab(); }),
            makeTab(Config::GetInstance().move_tab, [] {OutputDebugString(L"Move"); }),
            makeTab(Config::GetInstance().delete_tab, [] {OutputDebugString(L"Delete"); })
        }, 
        &tab_selected
    );

    auto container = Vertical({
        sourceDirInput,
        destDirInput,
        tab_toggle,
        tab_container
    });
    

    screen.Loop(Renderer(
        container,
        [&]
        {
            auto const isSourceCorrect = std::filesystem::is_directory(m_tempSourceDir);
            auto const isDestCorrect = std::filesystem::is_directory(m_tempDestDir);
            return vbox
            ({
                isSourceCorrect ? sourceDirInput->Render() | xflex : color(Color::Red, sourceDirInput->Render() | xflex),
                isDestCorrect? destDirInput->Render() | xflex : color(Color::Red, destDirInput->Render() | xflex),
                separator(),
                tab_toggle->Render(),
                separator(),
                tab_container->Render(),
            }) | border;
        })
    );
}

void TUI::redraw()
{
    screen.PostEvent(ftxui::Event::Custom);
}

void TUI::Run()
{
    TUI tui{};
    std::thread t{
        [&tui] { tui.runImpl(); }
    };

    t.detach();

    while (true)
    {
        tui.redraw();
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}
