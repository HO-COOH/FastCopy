#include "pch.h"
#include "CppUnitTest.h"
#include "XCopyViewModel.h"
#include "FastCopyCommandParser.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include "../FastCopyShellExtension/SharedMemoryItemDef.h"
#include "ShellExtensionHelper.h"
#include <ShObjIdl_core.h>
#include <winerror.h>
#include <span>
#include "XCopyCommandGenerator.h"


namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    template<>
    static std::wstring ToString<winrt::FastCopy::Mode>(winrt::FastCopy::Mode const& mode)
    {
        switch (mode)
        {
            case winrt::FastCopy::Mode::Copy: return L"Copy";
            case winrt::FastCopy::Mode::Move: return L"Move";
            default: throw std::exception{};
        }
    }

    template<>
    static std::wstring ToString<Path::Type>(Path::Type const& type)
    {
        switch (type)
        {
            case Path::Type::File: return L"File";
            case Path::Type::Folder: return L"Folder";
            default: throw std::exception{};
        }
    }
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    class Attributes
    {
        LPCTSTR m_fileName;
    public:
        constexpr static inline DWORD Archive = FILE_ATTRIBUTE_ARCHIVE;
        constexpr static inline DWORD Hidden = FILE_ATTRIBUTE_HIDDEN;
        constexpr static inline DWORD Normal = FILE_ATTRIBUTE_NORMAL;
        constexpr static inline DWORD NotContentIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
        constexpr static inline DWORD Offline = FILE_ATTRIBUTE_OFFLINE;
        constexpr static inline DWORD ReadOnly = FILE_ATTRIBUTE_READONLY;
        constexpr static inline DWORD System = FILE_ATTRIBUTE_SYSTEM;
        constexpr static inline DWORD Temporary = FILE_ATTRIBUTE_TEMPORARY;

        

        Attributes(LPCTSTR fileName) :m_fileName{ fileName } {}

        void set(DWORD attribute)
        {
            if (!SetFileAttributes(m_fileName, attribute))
                throw std::exception{};
        }

        auto get()
        {
            return GetFileAttributes(m_fileName);
        }

        bool operator==(Attributes rhs)
        {
            return get() == rhs.get();
        }
    };

    template<typename T>
    concept StdString = 
        std::is_same_v<T, char const*> || 
        std::is_same_v<T, std::string> ||
        std::is_convertible_v<T, std::string>;

    template<typename T>
    concept WideString = 
        std::is_same_v<T, wchar_t const*> || 
        std::is_same_v<T, std::wstring> || 
        std::is_same_v<T, winrt::hstring> ||
        std::is_convertible_v<T, std::wstring> ||
        std::is_convertible_v<T, winrt::hstring>;

    template<typename FileNameType, size_t N>
    void FillSource(winrt::FastCopy::XCopyViewModel& viewModel, std::array<FileNameType, N> const& files, bool isFile = true)
    {
        for (auto const& file : files)
        {
            if constexpr (StdString<FileNameType>)
            {
                viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ winrt::to_hstring(file), isFile? winrt::FastCopy::ExplorerItemType::File : winrt::FastCopy::ExplorerItemType::Folder });
            }
            else if constexpr(WideString<FileNameType>)
            {
                viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ file, isFile ? winrt::FastCopy::ExplorerItemType::File : winrt::FastCopy::ExplorerItemType::Folder });
            }
        }
    }

    TEST_CLASS(XCopyParser)
    {
        static inline FastCopyCommandParser parser;
        
    public:
        TEST_CLASS_INITIALIZE(SetupParser)
        {
            parser.parseHelp(parser.runHelp(FastCopyCommandParser::Command::xcopy));
        }

        TEST_METHOD(ParseHelp)
        {
            auto& result = parser.getSwitches();
            std::array const expectedFlags
            {
                    "/A",
                    "/M",
                    "/D:m-d-y",
                    "/EXCLUDE:file1[+file2][+file3]...",
                    "/P",
                    "/S",
                    "/E",
                    "/V",
                    "/W",
                    "/C",
                    "/I",
                    "/-I",
                    "/Q",
                    "/F",
                    "/L",
                    "/G",
                    "/H",
                    "/R",
                    "/T",
                    "/U",
                    "/K",
                    "/N",
                    "/O",
                    "/X",
                    "/Y",
                    "/-Y",
                    "/Z",
                    "/B",
                    "/J",
                    "/COMPRESS",
                    "/SPARSE"
            };
            Assert::AreEqual(
                result.size(), 
                expectedFlags.size(), 
                L"Parse xcopy commands failed, number of parsed flags does not match");

            for (auto expectedFlag : expectedFlags)
            {
                Assert::IsTrue(
                    result.find(expectedFlag) != result.end(),
                    (winrt::hstring{ L"Parse xcopy commands failed, lacking flag: " } + winrt::to_hstring(expectedFlag)).data()
                );
                Assert::IsFalse(
                    result.find(expectedFlag)->second.empty(), 
                    (winrt::hstring{L"Parse xcopy commands failed, flag: "} + winrt::to_hstring(expectedFlag) + L" does not have description").data()
                );
            }
        }
    };

    TEST_CLASS(FastCopyShellExtensionTest)
    {
        static inline auto LibraryName = LR"(FastCopy\FastCopyShellExtension.dll)";
        using InvokeFunc = void(*)(IShellItemArray*);

        auto static LoadModule()
        {
            return LoadLibrary(LibraryName);
        }

        auto static GetInvokeFunction(HMODULE module)
        {
            return reinterpret_cast<InvokeFunc>(GetProcAddress(module, "InvokeImpl"));
        }

        class MockShellItem : public IShellItem
        {
            std::wstring m_name;
            Path::Type m_type;
        public:
            MockShellItem(std::wstring_view name, Path::Type type) : m_name{ name }, m_type{ type }
            {
            }
            HRESULT STDMETHODCALLTYPE BindToHandler(
                /* [unique][in] */ __RPC__in_opt IBindCtx* pbc,
                /* [in] */ __RPC__in REFGUID bhid,
                /* [in] */ __RPC__in REFIID riid,
                /* [iid_is][out] */ __RPC__deref_out_opt void** ppv)override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetParent(
                /* [out] */ __RPC__deref_out_opt IShellItem** ppsi) override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetDisplayName(
                /* [in] */ SIGDN sigdnName,
                /* [annotation][string][out] */
                _Outptr_result_nullonfailure_  LPWSTR* ppszName) override
            {
                *ppszName = m_name.data();
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE GetAttributes(
                /* [in] */ SFGAOF sfgaoMask,
                /* [out] */ __RPC__out SFGAOF* psfgaoAttribs) override
            {
                if (sfgaoMask & SFGAO_FOLDER)
                {
                    *psfgaoAttribs = (m_type == Path::Type::Folder ? SFGAO_FOLDER : 0);
                    return S_OK;
                }

                throw std::logic_error{ "Dead code" };
            }

            HRESULT STDMETHODCALLTYPE Compare(
                /* [in] */ __RPC__in_opt IShellItem* psi,
                /* [in] */ SICHINTF hint,
                /* [out] */ __RPC__out int* piOrder) override
            {
                return E_NOTIMPL;
            }

            #pragma region IUnknown
            HRESULT QueryInterface(REFIID, void**) override
            {
                return E_NOTIMPL;
            }

            ULONG AddRef() override { return {}; }
            ULONG Release() override { return {}; }
            #pragma endregion IUnknown
        };

        template<size_t N>
        class MockShellItemArray : public IShellItemArray
        {
            std::span<MockShellItem, N> m_items;
        public:
            MockShellItemArray(std::span<MockShellItem, N> items) : m_items{ items } {}
            HRESULT STDMETHODCALLTYPE BindToHandler(
                /* [unique][in] */ __RPC__in_opt IBindCtx* pbc,
                /* [in] */ __RPC__in REFGUID bhid,
                /* [in] */ __RPC__in REFIID riid,
                /* [iid_is][out] */ __RPC__deref_out_opt void** ppvOut) override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetPropertyStore(
                /* [in] */ GETPROPERTYSTOREFLAGS flags,
                /* [in] */ __RPC__in REFIID riid,
                /* [iid_is][out] */ __RPC__deref_out_opt void** ppv) override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetPropertyDescriptionList(
                /* [in] */ __RPC__in REFPROPERTYKEY keyType,
                /* [in] */ __RPC__in REFIID riid,
                /* [iid_is][out] */ __RPC__deref_out_opt void** ppv) override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetAttributes(
                /* [in] */ SIATTRIBFLAGS AttribFlags,
                /* [in] */ SFGAOF sfgaoMask,
                /* [out] */ __RPC__out SFGAOF* psfgaoAttribs) override
            {
                return E_NOTIMPL;
            }

            HRESULT STDMETHODCALLTYPE GetCount(
                /* [out] */ __RPC__out DWORD* pdwNumItems)
            {
                *pdwNumItems = m_items.size();
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE GetItemAt(
                /* [in] */ DWORD dwIndex,
                /* [out] */ __RPC__deref_out_opt IShellItem** ppsi) override
            {
                *ppsi = &m_items[dwIndex];
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE EnumItems(
                /* [out] */ __RPC__deref_out_opt IEnumShellItems** ppenumShellItems) override
            {
                return E_NOTIMPL;
            }

            #pragma region IUnknown
            HRESULT QueryInterface(REFIID, void**) override
            {
                return E_NOTIMPL;
            }

            ULONG AddRef() override { return {}; }
            ULONG Release() override { return {}; }
            #pragma endregion IUnknown
        };

    public:
        TEST_METHOD(UIEntry)
        {

        }

        TEST_METHOD(LoadModuleNotNull)
        {
            auto module = LoadModule();
            Assert::IsTrue(module, L"Load FastCopyShellExtension.dll failed");
            Assert::IsTrue(GetInvokeFunction(module), L"Load FastCopyShellExtension.dll cannot find function: InvokeImpl()");
            Assert::IsTrue(FreeLibrary(module), L"Free FastCopyShellExtension.dll failed");
        }

        TEST_METHOD(SetSharedMemory)
        {
            using Func = void(IShellItem* item, std::vector<Path, boost::interprocess::allocator<Path, boost::interprocess::managed_shared_memory::segment_manager>>* v);
            
            constexpr std::array testPaths
            {
                LR"(D:\file1)",
                LR"(D:\folder1)"
            };

            std::array testItem
            {
                MockShellItem{ testPaths[0], Path::Type::File},
                MockShellItem{ testPaths[1], Path::Type::Folder}
            };

            MockShellItemArray mockItems{ std::span{testItem} };
            
            auto module = LoadModule();
            auto func = GetInvokeFunction(module);
            func(&mockItems);
            FreeLibrary(module);

            ShellExtensionHelper helper;
            auto& result = helper.get("MyVector");

            Assert::AreEqual(result.size(), 2ull, L"Shared Memory size wrong");
            Assert::AreEqual(std::wstring_view{ result[0].content }, std::wstring_view{ testPaths[0] });
            Assert::AreEqual(std::wstring_view{ result[1].content }, std::wstring_view{ testPaths[1] });
            Assert::AreEqual(result[0].type, Path::Type::File);
            Assert::AreEqual(result[1].type, Path::Type::Folder);
        }
    };

    TEST_CLASS(XCopyTest)
    {
        static inline std::string SourceFolder = R"(E:\src has space\)";
        static inline std::wstring SourceFolderW = LR"(E\src has space\)";
        static inline std::string DestFolder = R"(D:\dest has space\)";

        static void MakeTestFiles(int count = 2)
        {
            for (int i = 0; i < count; ++i)
            {
                auto const fileName = i == 0 ? 
                    std::format("{}testFile.txt", SourceFolder) : 
                    std::format("{}testFile{}.txt", SourceFolder, i);

                std::ofstream{ fileName };
            }
        }

        static void MakeTestFolders(int count = 2)
        {
            for (int i = 0; i < count; ++i)
            {
                std::filesystem::create_directories(i == 0 ?
                    std::format("{}testFolder", SourceFolder) :
                    std::format("{}testFolder{}", SourceFolder, i)
                );
            }
        }
    public:
        TEST_METHOD_INITIALIZE(CreateFile)
        {
            //create file in D:
            MakeTestFiles(2);
            MakeTestFolders(2);
        }

        TEST_METHOD_CLEANUP(CleanUp)
        {
            //delete copied files
            std::filesystem::remove_all(DestFolder);
        }

        TEST_METHOD(DefaultMode)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            Assert::AreEqual(viewModel.Mode(), winrt::FastCopy::Mode::Copy);
            Assert::AreEqual(viewModel.ProcessCount(), static_cast<int>(std::thread::hardware_concurrency()));
        }
        
        TEST_METHOD(CopyFile)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            FillSource(viewModel,
            std::array{
                R"(E:\src has space\testFile.txt)",
                R"(E:\src has space\testFile1.txt)"
            });
            viewModel.Destination(winrt::to_hstring(DestFolder));
            viewModel.Execute();
            
            //check result
            Assert::IsTrue(std::filesystem::exists(R"(D:\dest has space\testFile.txt)"));
            Assert::IsTrue(std::filesystem::exists(R"(D:\dest has space\testFile1.txt)"));
            Assert::IsTrue(std::filesystem::exists(SourceFolder + "testFile.txt"));
            Assert::IsTrue(std::filesystem::exists(SourceFolder + "testFile1.txt"));
            //check no additional files are copied
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFolder)"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFolder1)"));
        }

        TEST_METHOD(CopyFolder)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            FillSource(
                viewModel,
                std::array{
                    SourceFolder + "testFolder",
                    SourceFolder + "testFolder1"
                },
                false
            );
            viewModel.Destination(winrt::to_hstring(DestFolder));
            viewModel.Execute();
            
            //check result
            Assert::IsTrue(std::filesystem::exists(DestFolder + "testFolder"));
            Assert::IsTrue(std::filesystem::exists(DestFolder + "testFolder1"));
            Assert::IsTrue(std::filesystem::exists(SourceFolder + "testFolder"));
            Assert::IsTrue(std::filesystem::exists(SourceFolder + "testFolder1"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFile.txt)"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFile1.txt)"));
        }

        TEST_METHOD(MoveFile)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            FillSource(viewModel,
                std::array{
                    R"(E:\src has space\testFile.txt)",
                    R"(E:\src has space\testFile1.txt)"
                });
            viewModel.Destination(winrt::to_hstring(DestFolder));
            viewModel.Mode(winrt::FastCopy::Mode::Move);
            viewModel.Execute();

            //check result
            Assert::IsTrue(std::filesystem::exists(R"(D:\dest has space\testFile.txt)"));
            Assert::IsTrue(std::filesystem::exists(R"(D:\dest has space\testFile1.txt)"));
            Assert::IsFalse(std::filesystem::exists(SourceFolder + "testFile.txt"));
            Assert::IsFalse(std::filesystem::exists(SourceFolder + "testFile1.txt"));
            //check no additional files are copied
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFolder)"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFolder1)"));
        }

        TEST_METHOD(MoveFolder)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            MakeTestFolders();
            FillSource(
                viewModel,
                std::array{
                    SourceFolder + "testFolder",
                    SourceFolder + "testFolder1"
                },
                false
            );
            viewModel.Destination(winrt::to_hstring(DestFolder));
            viewModel.Mode(winrt::FastCopy::Mode::Move);
            viewModel.Execute();

            //check result
            Assert::IsTrue(std::filesystem::exists(DestFolder + "testFolder"));
            Assert::IsTrue(std::filesystem::exists(DestFolder + "testFolder1"));
            Assert::IsFalse(std::filesystem::exists(SourceFolder + "testFolder"));
            Assert::IsFalse(std::filesystem::exists(SourceFolder + "testFolder1"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFile.txt)"));
            Assert::IsFalse(std::filesystem::exists(R"(D:\dest has space\testFile1.txt)"));
        }

        TEST_METHOD(LongFileName_ExpectFail)
        {
            constexpr static wchar_t LongFileName[] = L"Looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooog.txt";
            constexpr static std::wstring_view Path = LR"(E:\src has space\looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong folder\)";
            constexpr static auto LongFileNameLength = sizeof(LongFileName) / sizeof(*(std::decay_t<decltype(LongFileName)>{}));
            constexpr static auto LongFolderLength = Path.size();
            constexpr static auto LongFilePathLength = LongFileNameLength + LongFolderLength;
            
            static_assert(LongFileNameLength <= 255);
            static_assert(LongFolderLength <= 255);
            static_assert(LongFileNameLength + LongFolderLength >= MAX_PATH);

            auto filePath = std::wstring{ LR"(\\?\)" } + std::wstring{ Path } + LongFileName;

            DeleteFileW(filePath.data());
            RemoveDirectoryW(Path.data());

            Assert::IsTrue(CreateDirectory(Path.data(), nullptr));

            auto hFile = ::CreateFileW(
                filePath.c_str(), 
                GENERIC_READ | GENERIC_WRITE, 
                0, 
                nullptr,
                CREATE_NEW, 
                FILE_ATTRIBUTE_NORMAL, 
                NULL);

            Assert::AreNotEqual(hFile, INVALID_HANDLE_VALUE);
            CloseHandle(hFile);

            winrt::FastCopy::XCopyViewModel viewModel;
            viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ std::wstring{Path} + LongFileName, winrt::FastCopy::ExplorerItemType::File});
            viewModel.Destination(winrt::to_hstring(DestFolder));
            viewModel.Execute();

            Assert::AreEqual(
                INVALID_HANDLE_VALUE,
                ::CreateFileW(
                    (winrt::to_hstring(DestFolder) + LongFileName).data(),
                    0,
                    0,
                    nullptr, 
                    OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL, 
                    NULL
                )
            );
        }

        TEST_METHOD(EmptySource_ExpectNotExecutable)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            viewModel.Destination(winrt::to_hstring(DestFolder));
            Assert::IsFalse(viewModel.IsExecutable(), L"Empty source is not executable");
            
            viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ L"", winrt::FastCopy::ExplorerItemType::File });
            Assert::IsTrue(viewModel.IsExecutable());

        }

        TEST_METHOD(EmptyDestination_ExpectNotExecutable)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            Assert::IsFalse(viewModel.IsExecutable());
            viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ L"", winrt::FastCopy::ExplorerItemType::File });
            Assert::IsFalse(viewModel.IsExecutable());

            viewModel.Destination(LR"(E:\)");
            Assert::IsTrue(viewModel.IsExecutable());
        }
    };

    TEST_CLASS(XCopyPreview)
    {
        
    public:
        TEST_METHOD(AddItem)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            viewModel.Destination(LR"(E:\dest)");

            Assert::AreEqual(viewModel.Preview().Size(), 0u, L"Empty sources but preview is not empty");
            
            viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ LR"(D:\src\f.txt)", winrt::FastCopy::ExplorerItemType::File });
            Assert::AreEqual(viewModel.Preview().Size(), 1u);
            Assert::AreEqual((*viewModel.Preview().begin()).Name().data(), LR"(E:\dest\f.txt)");
        }

        TEST_METHOD(ChangeDestination)
        {
            winrt::FastCopy::XCopyViewModel viewModel;
            viewModel.Destination(LR"(E:\dest)");
            viewModel.Sources().Append(winrt::FastCopy::ExplorerItem{ LR"(D:\src\f.txt)", winrt::FastCopy::ExplorerItemType::File });
            Assert::AreEqual((*viewModel.Preview().begin()).Name().data(), LR"(E:\dest\f.txt)");

            //change destination
            viewModel.Destination(LR"(D:\dest)");
            Assert::AreEqual((*viewModel.Preview().begin()).Name().data(), LR"(D:\dest\f.txt)");
        }
    };

    TEST_CLASS(XCopyCommandGeneratorTest)
    {
        TEST_METHOD(CustomMode)
        {
            auto& instance = XCopyCommandGenerator::GetInstance();
            std::string dest = R"(E:\dest)";
            
            winrt::FastCopy::XCopyViewModel viewModel;
            viewModel.Flags(); //let it parse the command once
           

            (*instance.m_flags.begin()).as<winrt::FastCopy::XCopyFlags>().Enabled(true);

            Assert::AreEqual(
                instance.makeCommand(
                    Task{ winrt::FastCopy::ExplorerItem{LR"(E:\src\file.txt)", winrt::FastCopy::ExplorerItemType::File}, dest},
                    winrt::FastCopy::Mode::Custom
                ).data(),
                R"(C:\Windows\system32\xcopy.exe "E:\src\file.txt" "E:\dest\" /A )"
            );
        }
    };
}
