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
void loadConfig();
std::string configGetLineValue(const std::vector<std::string>& lines, int index);
void clearBuildDir();
std::string generateNavCode();
std::string buildPage(const std::string& contents);
void generateIndexFile();

std::string tabText;
std::string navTitle;
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
	fs::writeFile("config.txt", CONFIG_TEMPLATE);
}

void build()
{
	loadConfig();
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
	generateIndexFile();
}

void loadConfig()
{
	std::string config = fs::readFile("config.txt");
	std::vector<std::string> lines = strex::split(config, '\n');

	tabText = configGetLineValue(lines, 0);
	navTitle = configGetLineValue(lines, 1);
}

std::string configGetLineValue(const std::vector<std::string>& lines, int index)
{
	std::string::size_type startPos = 0;
	std::string::size_type endPos = 0;

	startPos = lines[index].find("\"");

	if (startPos != std::string::npos)
	{
		startPos++;
		endPos = lines[index].find("\"", startPos);

		if (endPos != std::string::npos)
		{
			return lines[index].substr(startPos, endPos - startPos);
		}
	}

	return "";
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
	strex::replace(result, "[tab text]", tabText);
	strex::replace(result, "[nav title]", navTitle);
	strex::replace(result, "[nav content]", navCode);
	strex::replace(result, "[main content]", contents);
	return result;
}

void generateIndexFile()
{
	std::string buildDir = fs::combinePaths(fs::getCwd(), "build");
	fs::writeFile(fs::combinePaths(buildDir, "index.html"), buildPage(""));
}