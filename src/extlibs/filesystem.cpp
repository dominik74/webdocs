#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "filesystem.h"
#include "stringExtensions.h"

namespace fs = std::filesystem;

namespace filesystem
{
	std::string readFile(const std::string& path)
	{
		std::ifstream inFile;
		inFile.open(path);

		std::stringstream strStream;
		strStream << inFile.rdbuf();

		return strStream.str();
	}

	void writeFile(const std::string& path, const std::string& contents)
	{
		std::fstream fs;

		fs.open(path, std::fstream::out);
		fs << contents;

		fs.close();
	}

	//works full path and relative path
	//can remove file
	//can remove only EMPTY directory
	void remove(const std::string& path)
	{
		fs::remove(path);
	}

	//works full path and relative path
	//can rename file
	//can rename directory
	void rename(const std::string& oldPath, const std::string& newPath)
	{
		fs::rename(oldPath, newPath);
	}

	//experimental (untested)
	bool copy(const std::string& fromPath, const std::string& toPath)
	{
		fs::copy(fromPath, toPath);
		return true; //temp
	}

	//untested
	bool exists(const std::string& path)
	{
		return fs::exists(path);
	}

	bool isFile(const std::string& path)
	{
		return fs::is_regular_file(path);
	}

	bool isDirectory(const std::string& path)
	{
		return fs::is_directory(path);
	}

	std::string combinePaths(std::string path1, std::string path2)
	{
		//todo: support more than 2 parameters (for now can be worked around by nesting
		//this function)

		stringExtensions::replace(path1, PATH_SEPARATOR_OTHER, PATH_SEPARATOR);
		stringExtensions::replace(path2, PATH_SEPARATOR_OTHER, PATH_SEPARATOR);

		if (stringExtensions::endsWith(path1, PATH_SEPARATOR))
			path1 = path1.substr(0, path1.length() - 1);

		return path1.append(PATH_SEPARATOR).append(path2);
	}

	std::string getCwd()
	{
		return fs::current_path().string();
	}

	bool createDirectory(const std::string& path)
	{
		return fs::create_directory(path);
	}
}