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
	DWORD const count = ShellItemArray{ items }.size();
	if (!m_hasInit)
	{
		if (count == 0 && hasInvokedCopyOrCut())
		{
			auto pasteCommand = Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Paste);
			if (m_subCommands.empty())
			{
				m_subCommands.push_back(pasteCommand);
			}
			else
			{
				m_subCommands.insert(m_subCommands.begin() + 2, pasteCommand);
			}
			m_hasInit = true;
		}
	}



	if (count != 0)
	{
		m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Copy));
		m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Move));
		m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Delete));
		m_hasInit = true;
	}
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
	if (!okToBeSlow)
		return E_PENDING;

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
		m_subCommandIter->CopyTo(&pUICommand[0]);
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
