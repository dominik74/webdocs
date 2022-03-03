#pragma once
#include <string>

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#define PATH_SEPARATOR_OTHER "/"
#else
#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_OTHER "\\"
#endif

namespace filesystem
{
	std::string readFile(const std::string& path);
	void writeFile(const std::string& path, const std::string& contents);
	void remove(const std::string& path);
	void rename(const std::string& oldPath, const std::string& newPath);
	bool copy(const std::string& fromPath, const std::string& toPath);
	bool exists(const std::string& path);

	/// <summary>
	/// returns true if the specified path represents a file, otherwise returns false.
	/// if the file does not exist, returns false
	/// </summary>
	/// <param name="path">- the full path to the file or directory</param>
	/// <returns></returns>
	bool isFile(const std::string& path);

	/// <summary>
	/// returns true if the specified path represents a directory, otherwise returns false.
	/// if the directory does not exist, returns false
	/// </summary>
	/// <param name="path">- the full path to the file or directory</param>
	/// <returns></returns>
	bool isDirectory(const std::string& path);

	std::string combinePaths(std::string path1, std::string path2);

	std::string getCwd();

	bool createDirectory(const std::string& path);
}