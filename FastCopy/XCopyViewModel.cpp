#include "pch.h"
#include "XCopyViewModel.h"
#if __has_include("XCopyViewModel.g.cpp")
#include "XCopyViewModel.g.cpp"
#include "XCopyFlags.g.cpp"
#endif

#include <filesystem>
#include "FastCopyCommandParser.h"

namespace winrt::FastCopy::implementation
{
    XCopyViewModel::XCopyViewModel() 
    {
        try
        {
            m_dispatcher = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        }
        catch (...)
        {
        }

        m_sources.VectorChanged(
            [this](auto, auto)
            {
                if(m_destination.empty())
                    return;
                refreshPreview();
                if(m_sources.Size()!=0 && m_dispatcher)
                    raisePropertyChange(L"IsExecutable");
            }
        );
    }

    winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> XCopyViewModel::Flags()
    {
        static bool firstTime = true;
        if (!firstTime)
            return XCopyCommandGenerator::GetInstance().m_flags;

        FastCopyCommandParser parser;
        parser.parseHelp(parser.runHelp(FastCopyCommandParser::Command::xcopy));
        auto const& xcopyCommands = parser.getSwitches();
        for (auto const& xcopyCommand : xcopyCommands)
        {
            XCopyCommandGenerator::GetInstance().m_flags.Append(winrt::FastCopy::XCopyFlags{ winrt::to_hstring(xcopyCommand.first), winrt::to_hstring(xcopyCommand.second) });
        }
        firstTime = true;
        return XCopyCommandGenerator::GetInstance().m_flags;
    }

    void XCopyViewModel::Execute()
    {
        clearAll();
        for (auto item : m_sources)
        {
            m_tasks.push_back(Task{item, m_destinationA});
        }
        execute();
    }
    void XCopyViewModel::Destination(winrt::hstring path)
    {
        m_destination = path;
        m_destinationA = winrt::to_string(m_destination);
        if (m_sources.Size() != 0)
        {
            refreshPreview();
        }
        if (m_dispatcher)
        {
            raisePropertyChange(L"IsExecutable");
            raisePropertyChange(L"Destination");
        }
    }
    void XCopyViewModel::Mode(FastCopy::Mode mode)
    {
        if (mode != m_mode)
        {
            m_mode = mode;
            if(m_dispatcher)
                raisePropertyChange(L"IsCustomEnabled");
        }
    }
    bool XCopyViewModel::IsCustomEnabled()
    {
        return m_mode == Mode::Custom;
    }

    void XCopyViewModel::execute()
    {
        CreateDirectoryW(m_destination.data(), nullptr);
        m_taskIter = m_tasks.begin();
        //push initial tasks
        while (m_taskIter != m_tasks.end() && m_processHandles.size() < ProcessCount())
        {
            m_processHandles.push_back(executeSingleTask());
            ++m_taskIter;
        }

        //push new tasks while there are still tasks left
        while (m_taskIter != m_tasks.end())
        {
            auto retVal = WaitForMultipleObjects(
                m_processHandles.size(),
                m_processHandles.data(),
                false,
                INFINITE
            );

            auto finishedIter = m_processHandles.begin() + (retVal - WAIT_OBJECT_0);
            //CloseHandle(*finishedIter);
            *finishedIter = executeSingleTask();
        }

        //wait for all the tasks
        WaitForMultipleObjects(m_processHandles.size(), m_processHandles.data(), true, INFINITE);
        
        auto taskIter = m_tasks.begin();
        for (auto handle : m_processHandles)
        {
            if (handle == HANDLE{})
            {
                ++taskIter;
                continue;
            }
            DWORD exitCode{};
            auto success = GetExitCodeProcess(handle, &exitCode);
            m_dispatcher.TryEnqueue(
                [this, taskIter, exitCode]() {
                    exitCode == 0 ?
                        taskIter->pItem.IsFinished(true) :
                        taskIter->pItem.IsFailed(true);
                }
            );
            ++taskIter;
            CloseHandle(handle);
        }

        deleteSource();

        //if (m_dispatcher)
        //{
        //    m_dispatcher.TryEnqueue([this]() {
        //        for (auto& task : m_tasks)
        //        task.pItem.IsFinished(true);
        //    });
        //}
    }

    void XCopyViewModel::clearAll()
    {
        m_processHandles.clear();
        m_tasks.clear();
    }

    HANDLE XCopyViewModel::executeSingleTask()
    {
        STARTUPINFOA si{};
        si.cb = sizeof(si);


        PROCESS_INFORMATION pi{};
        
        auto command = XCopyCommandGenerator::GetInstance().makeCommand(*m_taskIter, m_mode);

        OutputDebugString(winrt::to_hstring(command + '\n').data());

        CreateProcessA(
            nullptr,
            command.data(),
            nullptr,
            nullptr,
            false,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi
        );

        CloseHandle(pi.hThread);
        return pi.hProcess;
    }

    void XCopyViewModel::deleteSource()
    {
        if (m_mode != FastCopy::Mode::Move)
            return;

        for (auto const& task : m_tasks)
        {
            std::filesystem::remove_all(task.source);
        }
    }
    void XCopyViewModel::refreshPreview()
    {
        m_preview.Clear();
        for (auto item : m_sources)
        {
            if(m_destination.ends_with(L"\\"))
                m_preview.Append(ExplorerItem{ m_destination + item.FileName(), item.Type() });
            else
                m_preview.Append(ExplorerItem{ m_destination + L"\\" + item.FileName(), item.Type()});
        }
    }
}
