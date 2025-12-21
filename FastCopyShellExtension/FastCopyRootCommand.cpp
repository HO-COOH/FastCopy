#include "FastCopyRootCommand.h"
#include <Shlwapi.h>
#include "ShellItemArray.h"
#include "CopyOperation.h"
#include <array>
#include <algorithm>
#include "DllIconFormatter.h"
#include "FastCopySubCommand.h"
#include "Recorder.h"
#include "PackageConfig.h"

bool FastCopyRootCommand::hasInvokedCopyOrCut()
{
	return Recorder::HasRecord();
}

void FastCopyRootCommand::init(IShellItemArray* items)
{
	if (m_hasInit)
		return;

	bool const showPasteCommand = hasInvokedCopyOrCut();

	m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Copy, m_site.Get()));
	m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Move, m_site.Get()));
	if (showPasteCommand)
		m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Paste, m_site.Get()));
	if (ShellItemArray{ items }.size() != 0)
		m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Delete, m_site.Get()));
	m_hasInit = true;
}

HRESULT FastCopyRootCommand::SetSite(IUnknown* pUnkSite)
{
    m_site = pUnkSite;
    return S_OK;
}

HRESULT FastCopyRootCommand::GetSite(REFIID riid, void** ppvSite)
{
    return m_site.CopyTo(riid, ppvSite);
}

HRESULT FastCopyRootCommand::GetTitle(IShellItemArray*, PWSTR* name)
{
	return SHStrDup(PackageConfig::PackageName, name);
}

HRESULT FastCopyRootCommand::GetIcon(IShellItemArray*, PWSTR* icon)
{
	return SHStrDup(DllIconFormatter::GetForRootCommand().data(), icon);
}

HRESULT FastCopyRootCommand::GetToolTip(IShellItemArray*, PWSTR* infoTip)
{
	*infoTip = {};
	return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::GetCanonicalName(GUID* guidCommandName)
{
	*guidCommandName = GUID_NULL; 
	return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::GetState(IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState)
{
	init(selection);
	if (m_hasInit && !m_subCommands.empty())
		*cmdState = ECS_ENABLED;
	else
		*cmdState = ECS_DISABLED;
	return S_OK;
}

HRESULT FastCopyRootCommand::GetFlags(EXPCMDFLAGS* flags)
{
	*flags = ECF_HASSUBCOMMANDS;
	return S_OK;
}

HRESULT FastCopyRootCommand::Invoke(IShellItemArray* selection, IBindCtx*)
{
	return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::EnumSubCommands(IEnumExplorerCommand** enumCommands)
{
	m_subCommandIter = m_subCommands.begin();
	AddRef();
	return QueryInterface(IID_PPV_ARGS(enumCommands));
}

HRESULT FastCopyRootCommand::Next(ULONG numItemsToFetch, IExplorerCommand** pUICommand, ULONG* numItemsFetched)
{
	ULONG fetched = 0;

	for (
		ULONG i = 0;
		(i < numItemsToFetch) &&
		(this->m_subCommandIter != this->m_subCommands.cend());
		++i)
	{
		m_subCommandIter->CopyTo(&pUICommand[i]);
		++m_subCommandIter;
		++fetched;
	}

	if (numItemsFetched)
	{
		*numItemsFetched = fetched;
	}

	return (fetched == numItemsToFetch) ? S_OK : S_FALSE;
}

HRESULT FastCopyRootCommand::Skip(ULONG)
{
	return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::Reset()
{
	m_subCommandIter = m_subCommands.begin();
	return S_OK;
}

HRESULT FastCopyRootCommand::Clone(IEnumExplorerCommand** ppenum)
{
	*ppenum = nullptr;
	return E_NOTIMPL;
}
