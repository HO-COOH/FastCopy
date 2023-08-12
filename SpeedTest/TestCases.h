#pragma once
#include "TestOperation.h"
#include <vector>
#include <memory>
#include "ICopyBase.h"
#include <filesystem>
#include <cassert>

template<typename Derived>
class ITestCaseBase
{
public:
	Derived& operator<<(std::unique_ptr<IImplementationBase>&& implementation)
	{
		m_implementations.push_back(std::move(implementation));
		return *getSelf();
	}

	void operator()()
	{
		getSelf()->beforeRun();
		
		for (auto& implementation : m_implementations)
		{
			if(implementation->started)
				implementation->started();
			implementation->Run(test);
			if (implementation->finished)
				implementation->finished();
			clearDestination();
		}


		getSelf()->afterRun();
	}

	operator bool() const
	{
		return !m_implementations.empty();
	}
protected:
	std::vector<TestOperation> test;

	static void removeFolderContent(std::filesystem::path const& path)
	{
		assert(std::filesystem::is_directory(path));
		std::filesystem::remove_all(path);
		std::filesystem::create_directory(path);
	}

	void clearSource()
	{
		for (auto const& op : test)
		{
			removeFolderContent(op.source);
		}
	}

	void clearDestination()
	{
		for (auto const& op : test)
		{
			removeFolderContent(op.destination);
		}
	}
private:
	std::vector<std::unique_ptr<IImplementationBase>> m_implementations;

	Derived* getSelf()
	{
		return static_cast<Derived*>(this);
	}
};


//---------------------------------Test------------------------------------------


class Random4KFiles : public ITestCaseBase<Random4KFiles>
{
public:
	Random4KFiles(uint64_t totalBytes);
private:
	uint64_t m_totalBytes{};

	constexpr static auto RandomFileSize = 4 * 1024; //4K
	friend class ITestCaseBase<Random4KFiles>;

	void beforeRun();
	void afterRun() {}
};

class BigFile : public ITestCaseBase<BigFile>
{
public:
	BigFile(uint64_t bigFileSize);
private:
	uint64_t m_bigFileSize{};

	friend class ITestCaseBase<BigFile>;
	void beforeRun();
	void afterRun() {}
};

class MoveFileSamePartition : public ITestCaseBase<MoveFileSamePartition>
{
public:
	MoveFileSamePartition(uint64_t bigFileSize);
private:
	uint64_t m_bigFileSize{};
	friend class ITestCaseBase<MoveFileSamePartition>;
	void beforeRun();
	void afterRun() {}
};