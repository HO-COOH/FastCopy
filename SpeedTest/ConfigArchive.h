#pragma once
#include <fstream>
#include <cereal/archives/json.hpp>
class ConfigArchive
{
public:
	constexpr ConfigArchive(char const* const fileName) : m_fileName{ fileName } {}

	template<typename T>
	void Save(T const& objectToSave) const
	{
		std::ofstream fs{m_fileName};
		if (!fs.is_open())
			return;

		cereal::JSONOutputArchive{fs}(objectToSave);
	}

	template<typename T>
	void Load(T& objectToLoad) const
	{
		std::ifstream fs{m_fileName};
		if (!fs.is_open())
			return;

		cereal::JSONInputArchive{fs}(objectToLoad);
	}
private:
	char const* const m_fileName;
};

