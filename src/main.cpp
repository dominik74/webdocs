#include <iostream>
#include <filesystem>
#include "extlibs/filesystem.h"
#include "extlibs/stringExtensions.h"
#include "extlibs/NEWLINE_DEF.h"
#include "templates.h"

namespace fs = filesystem;
namespace strex = stringExtensions;

void generateProjectFolders();
void build();
void clearBuildDir();
std::string generateNavCode();
std::string buildPage(const std::string& contents);

std::string navCode;

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

	navCode = generateNavCode();

	std::string srcDir = fs::combinePaths(fs::getCwd(), "src");
	std::string buildDir = fs::combinePaths(fs::getCwd(), "build");

	for (const auto& file : std::filesystem::directory_iterator(srcDir))
	{
		if (file.path().extension() == ".txt")
		{
			std::string contents = fs::readFile(file.path().string());
			strex::replace(contents, "\n", "<br>");
			contents = buildPage(contents);

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

	fs::writeFile(fs::combinePaths(buildDir, "style.css"), CSS_TEMPLATE);
}

void clearBuildDir()
{
	std::string buildDir = fs::combinePaths(fs::getCwd(), "build");

	for (const auto& path : std::filesystem::directory_iterator(buildDir))
	{
		std::filesystem::remove_all(path);
	}
}

std::string generateNavCode()
{
	std::string result = "<table>\n";

	std::string srcDir = fs::combinePaths(fs::getCwd(), "src");

	for (const auto& file : std::filesystem::directory_iterator(srcDir))
	{
		std::string row;
		std::string newName;

		if (file.path().extension() == ".txt" || file.path().extension() == ".md")
			newName = file.path().stem().string() + ".html";
		else
			newName = file.path().filename().string();

		row = "<tr><td><a href=\"" + newName + "\">" + file.path().stem().string()
			+ "</a></td></tr>\n";

		result += row;
	}

	result += "</table>";

	return result;
}

std::string buildPage(const std::string& contents)
{
	std::string result = PAGE_TEMPLATE;
	strex::replace(result, "[website name]", "web docs"); //<- need a config file for this
	strex::replace(result, "[nav content]", navCode);
	strex::replace(result, "[main content]", contents);
	return result;
}