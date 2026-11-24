#pragma once
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

class RobocopyJobFile
{
	std::filesystem::path m_path;
public:
	explicit RobocopyJobFile(std::span<std::wstring const> includeFiles);
	~RobocopyJobFile();

	RobocopyJobFile(RobocopyJobFile const&) = delete;
	RobocopyJobFile(RobocopyJobFile&&) noexcept = delete;
	RobocopyJobFile& operator=(RobocopyJobFile const&) = delete;
	RobocopyJobFile& operator=(RobocopyJobFile&&) noexcept = delete;

	[[nodiscard]] std::wstring_view PathView() const noexcept;
	[[nodiscard]] static bool CanUseInJobFile(std::wstring_view fileName) noexcept;
};
