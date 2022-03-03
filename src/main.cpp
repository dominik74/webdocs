#include <iostream>
#include <filesystem>
#include "extlibs/filesystem.h"
#include "extlibs/stringExtensions.h"
#include "extlibs/NEWLINE_DEF.h"

namespace fs = filesystem;
namespace strex = stringExtensions;

void generateProjectFolders();
void build();
void clearBuildDir();

//args 1: command
int main(int argc, char* argv[])
{
	if (std::string(argv[1]) == "init")
	{
		generateProjectFolders();
		return 0;
	}

	if (std::string(argv[1]) == "build")
	{
		build();
		return 0;
	}

	std::cout << "error: unsupported argument '" << argv[1] << "' entered" << std::endl;
	return 1;
}

void generateProjectFolders()
{
	fs::createDirectory("src");
	fs::createDirectory("build");
}

void build()
{
	clearBuildDir();

	std::string srcDir = fs::combinePaths(fs::getCwd(), "src");
	std::string buildDir = fs::combinePaths(fs::getCwd(), "build");

	for (const auto& file : std::filesystem::directory_iterator(srcDir))
	{
		if (file.path().extension() == ".txt")
		{
			std::string contents = fs::readFile(file.path().string());
			strex::replace(contents, "\n", "<br>");
			fs::writeFile(fs::combinePaths(buildDir, file.path().stem().string() + ".html"), contents);
		}
		else if (file.path().extension() == ".md")
		{
			continue; //markdown files currently not supported
		}
		else
		{
			fs::copy(file.path().string(), fs::combinePaths(buildDir, file.path().stem().string() + file.path().extension().string()));
		}
	}
}

void clearBuildDir()
{
	std::string buildDir = fs::combinePaths(fs::getCwd(), "build");

	for (const auto& path : std::filesystem::directory_iterator(buildDir))
	{
		std::filesystem::remove_all(path);
	}
}