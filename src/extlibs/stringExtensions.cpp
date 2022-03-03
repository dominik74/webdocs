#include <string>
#include <vector>
#include <sstream>

namespace stringExtensions
{
	void replace(std::string& str, const std::string& from, const std::string& to)
	{
		size_t pos = str.find(from);

		while (pos != std::string::npos)
		{
			str.replace(pos, from.length(), to);
			pos = str.find(from, pos + to.length());
		}
	}

	bool contains(const std::string& str, const std::string& str2)
	{
		if (str.find(str2) != std::string::npos)
			return true;
		else
			return false;
	}

	bool startsWith(const std::string& str, const std::string& starting)
	{
		return str.find(starting) == 0;
	}

	bool endsWith(std::string const& str, std::string const& ending)
	{
		if (str.length() >= ending.length())
			return (0 == str.compare(str.length() - ending.length(), ending.length(), ending));
		else
			return false;
	}

	std::vector<std::string> split(const std::string& str, const char& separator)
	{
		std::stringstream strStream(str);
		std::string segment;
		std::vector<std::string> segList;

		while (std::getline(strStream, segment, separator))
		{
			segList.push_back(segment);
		}

		return segList;
	}
}