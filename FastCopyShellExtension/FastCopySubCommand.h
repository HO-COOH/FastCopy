#pragma once
#include <wrl/implements.h>
#include <ShlObj_core.h>
#include "CopyOperation.h"
#include <string_view>

class FastCopySubCommand final :
    public Microsoft::WRL::RuntimeClass
    <
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        IExplorerCommand
    >
{
    CopyOperation m_op;
    IShellItem* m_parent{};

    void recordFilesImpl(IShellItemArray* selection);
    void callMainProgramImpl(std::wstring_view arg);
public:
    FastCopySubCommand(CopyOperation op);

    void SetParentForPasteForWindows10(IShellItem* parent)
    {
        m_parent = parent;
    }

    // IExplorerCommand
    HRESULT GetTitle(IShellItemArray* items, PWSTR* name) override;
    HRESULT GetIcon(IShellItemArray* items, PWSTR* icon) override;
    HRESULT GetToolTip(IShellItemArray* items, PWSTR* infoTip) override;
    HRESULT GetCanonicalName(GUID* guidCommandName) override;
    HRESULT GetState(IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState) override;
    HRESULT Invoke(IShellItemArray* selection, IBindCtx* ctx) override;
    HRESULT GetFlags(EXPCMDFLAGS* flags) override;
    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) override;
};
