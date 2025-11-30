// CopyFilesByRules.cpp
// Match files under SourceDir using a rule configuration file and copy them to TargetDir
// Our rule supports '*' and '?' for filename matching (NOT DIRECTORY PATH!!!)
// , and is case-insensitive.

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <cwctype>

namespace fs = std::filesystem;

struct Rule
{
    std::wstring patternLower;
    bool softOnTargetLocked = false;
};

struct FileToCopy
{
    fs::path src;
    bool softOnTargetLocked = false;
};

static std::wstring ToLower(std::wstring s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](wchar_t ch) {
            return static_cast<wchar_t>(
                std::towlower(static_cast<wint_t>(ch))
                );
        });
    return s;
}

static bool MatchPattern(const std::wstring& pattern, const std::wstring& text)
{
    size_t p = 0;
    size_t t = 0;
    size_t starPos = std::wstring::npos;
    size_t matchPos = 0;

    while (t < text.size())
    {
        if (p < pattern.size() &&
            (pattern[p] == text[t] || pattern[p] == L'?'))
        {
            ++p;
            ++t;
        }
        else if (p < pattern.size() && pattern[p] == L'*')
        {
            starPos = p++;
            matchPos = t;
        }
        else if (starPos != std::wstring::npos)
        {
            p = starPos + 1;
            ++matchPos;
            t = matchPos;
        }
        else
        {
            return false;
        }
    }

    while (p < pattern.size() && pattern[p] == L'*')
        ++p;

    return p == pattern.size();
}

static void Trim(std::wstring& s)
{
    const wchar_t* ws = L" \t\r\n";
    auto start = s.find_first_not_of(ws);
    if (start == std::wstring::npos)
    {
        s.clear();
        return;
    }
    auto end = s.find_last_not_of(ws);
    s = s.substr(start, end - start + 1);
}

static std::wstring NormalizePathArg(const wchar_t* arg)
{
    if (!arg)
        return {};

    std::wstring s(arg);
    Trim(s);

    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
    {
        s = s.substr(1, s.size() - 2);
        Trim(s);
    }

    return s;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 4)
    {
        std::wcerr << L"ArgsNumberError: Expected 4 args, got "
            << argc << L"\n";
        std::wcerr << L"[CopyFilesByRules] Usage:\n"
            << L"  CopyFilesByRules.exe <SourceDir> <TargetDir> <RulesFile>\n";
        return 1;
    }

    std::wstring sourceArg = NormalizePathArg(argv[1]);
    std::wstring targetArg = NormalizePathArg(argv[2]);
    std::wstring rulesArg = NormalizePathArg(argv[3]);

    std::wcout << L"[CopyFilesByRules] SourceDir: " << sourceArg << L"\n";
    std::wcout << L"[CopyFilesByRules] TargetDir: " << targetArg << L"\n";
    std::wcout << L"[CopyFilesByRules] RulesFile: " << rulesArg << L"\n";

    fs::path sourceDir = sourceArg;
    fs::path targetDir = targetArg;
    fs::path rulesFile = rulesArg;

    try
    {
        if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir))
        {
            std::wcerr << L"[CopyFilesByRules][ERROR] SourceDir is not a directory: "
                << sourceDir << L"\n";
            return 1;
        }

        if (!fs::exists(rulesFile))
        {
            std::wcerr << L"[CopyFilesByRules][ERROR] RulesFile not found: "
                << rulesFile << L"\n";
            return 1;
        }

        std::wifstream fin(rulesFile);
        if (!fin)
        {
            std::wcerr << L"[CopyFilesByRules][ERROR] Failed to open RulesFile: "
                << rulesFile << L"\n";
            return 1;
        }

        fin.imbue(std::locale(""));

        std::vector<Rule> rules;
        std::wstring line;

        // Global soft switch (optional, @SoftOnTargetLocked)
        bool globalSoftOnTargetLocked = false;

        while (std::getline(fin, line))
        {
            if (!line.empty() && line[0] == 0xFEFF)
                line.erase(line.begin());

            Trim(line);
            if (line.empty())
                continue;

            if (line[0] == L'#' || line[0] == L';')
                continue;

            if (line[0] == L'@')
            {
                std::wstring lower = ToLower(line);
                if (lower == L"@softontargetlocked" || lower == L"@soft_on_target_locked")
                {
                    globalSoftOnTargetLocked = true;
                    std::wcout << L"[CopyFilesByRules] Option: Global SoftOnTargetLocked = true\n";
                }
                else
                {
                    std::wcout << L"[CopyFilesByRules] Unknown option line ignored: "
                        << line << L"\n";
                }
                continue;
            }

            // Lined rule format: pattern [| flags...]
            Rule rule;

            std::wstring patternPart = line;
            std::wstring flagsPart;

            auto barPos = line.find(L'|');
            if (barPos != std::wstring::npos)
            {
                patternPart = line.substr(0, barPos);
                flagsPart = line.substr(barPos + 1);
                Trim(patternPart);
                Trim(flagsPart);
            }

            if (patternPart.empty())
                continue;

            rule.patternLower = ToLower(patternPart);
            rule.softOnTargetLocked = false;

            if (!flagsPart.empty())
            {
                std::wstring flagsLower = ToLower(flagsPart);
                // As long as 'flags' contains 'soft', the pattern is considered a soft rule in file copying.
                if (flagsLower.find(L"soft") != std::wstring::npos)
                {
                    rule.softOnTargetLocked = true;
                }
            }

            rules.push_back(rule);
        }

        if (rules.empty())
        {
            std::wcout << L"[CopyFilesByRules] No rules found in file: "
                << rulesFile << L"\n";
            return 0;
        }

        std::vector<FileToCopy> filesToCopy;
        std::unordered_set<std::wstring> seen;

        for (const auto& entry : fs::directory_iterator(sourceDir))
        {
            if (!entry.is_regular_file())
                continue;

            fs::path srcPath = entry.path();
            std::wstring filename = srcPath.filename().wstring();
            std::wstring filenameLower = ToLower(filename);

            bool matched = false;
            bool softOnTargetLocked = false;

            for (const auto& rule : rules)
            {
                if (MatchPattern(rule.patternLower, filenameLower))
                {
                    matched = true;
                    softOnTargetLocked = rule.softOnTargetLocked || globalSoftOnTargetLocked;
                    break;
                }
            }

            if (!matched)
                continue;

            std::wstring key = ToLower(srcPath.wstring());
            if (seen.insert(key).second)
            {
                FileToCopy item;
                item.src = srcPath;
                item.softOnTargetLocked = softOnTargetLocked;
                filesToCopy.push_back(std::move(item));
            }
        }

        if (filesToCopy.empty())
        {
            std::wcout << L"[CopyFilesByRules] No files matched any rule under: "
                << sourceDir << L"\n";
            return 0;
        }

        std::error_code ec;
        fs::create_directories(targetDir, ec);
        if (ec)
        {
            std::wcerr << L"[CopyFilesByRules][ERROR] Failed to create TargetDir: "
                << targetDir << L" (error: " << ec.message().c_str() << L")\n";
            return 1;
        }

        bool anyError = false;

        for (const auto& item : filesToCopy)
        {
            const fs::path& src = item.src;
            bool ruleSoftOnTargetLocked = item.softOnTargetLocked;

            fs::path dst = targetDir / src.filename();

            bool dstExistedBefore = fs::exists(dst);

            try
            {
                fs::copy_file(src, dst,
                    fs::copy_options::overwrite_existing);
                std::wcout << L"[CopyFilesByRules] Copy \""
                    << src.wstring() << L"\" -> \""
                    << dst.wstring() << L"\"\n";
            }
            catch (const fs::filesystem_error& ex)
            {
                // Soft error determination:
                // 1) This pattern has enabled soft On Target Locked (by rule or globally), and
                // 2) The target file already exists before copying (i.e., an error occurs when overwriting)
                if (ruleSoftOnTargetLocked && dstExistedBefore)
                {
                    std::wcerr << L"[CopyFilesByRules][WARN] Failed to overwrite existing file \""
                        << dst.wstring() << L"\".\n"
                        << L"  Source:  " << src.wstring() << L"\n"
                        << L"  Reason:  " << ex.what() << L"\n"
                        << L"  (SoftOnTargetLocked is enabled for this file, "
                        << L"so this is treated as non-fatal.)\n";
                    continue;
                }

                // All other cases are considered as fatal errors
                std::wcerr << L"[CopyFilesByRules][ERROR] Failed to copy \""
                    << src.wstring() << L"\" -> \""
                    << dst.wstring() << L"\"\n"
                    << L"  Reason: " << ex.what() << L"\n";
                anyError = true;
            }
        }

        if (anyError)
        {
            std::wcerr << L"[CopyFilesByRules] Some files failed to copy.\n";
            return 1;
        }

        std::wcout << L"[CopyFilesByRules] Done.\n";
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::wcerr << L"[CopyFilesByRules][ERROR] Exception: " << ex.what() << L"\n";
        return 1;
    }
}