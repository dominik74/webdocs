#pragma once
#include <string>
#include <vector>

namespace stringExtensions
{
	void replace(std::string& str, const std::string& from, const std::string& to);

	bool contains(const std::string& str, const std::string& str2);
	bool startsWith(const std::string& str, const std::string& starting);
	bool endsWith(std::string const& str, std::string const& ending);

	std::vector<std::string> split(const std::string& str, const char& separator);
}