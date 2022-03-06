#include <iostream>
#include <filesystem>
#include <sstream>
#include "extlibs/filesystem.h"
#include "extlibs/stringExtensions.h"
#include "extlibs/NEWLINE_DEF.h"
#include "templates.h"
#include "extlibs/maddy/parser.h"

namespace fs = filesystem;
namespace strex = stringExtensions;

void generateProjectFiles();
void build();
void loadConfig();
std::string configGetLineValue(const std::vector<std::string>& lines, int index);
void clearBuildDir();
void generateNavCode();
void buildPage(const std::string& fileName, const std::string& contents);

std::string tabText;
std::string navTitle;
std::string navCode;

//args 1: command
int main(int argc, char* argv[])
{
	if (std::string(argv[1]) == "init")
	{
		generateProjectFiles();
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

void generateProjectFiles()
{
	fs::createDirectory("src");
	fs::createDirectory("build");
	fs::writeFile("config.txt", CONFIG_TEMPLATE);
}

void build()
{
	loadConfig();
	clearBuildDir();
	generateNavCode();

	for (const auto& file : std::filesystem::directory_iterator("src"))
	{
		if (file.path().extension() == ".txt")
		{
			std::string contents = fs::readFile(file.path().string());
			strex::replace(contents, "\n", "<br>");
			buildPage(file.path().stem().string() + ".html", contents);
		}
		else if (file.path().extension() == ".md")
		{
			std::stringstream markdownInput(fs::readFile(file.path().string()));
			std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>();
			std::string htmlOutput = parser->Parse(markdownInput);
			buildPage(file.path().stem().string() + ".html", htmlOutput);
		}
		else if (file.path().extension() == ".png" || file.path().extension() == ".jpg")
		{
			std::string htmlImage = "<img src=\"" + file.path().filename().string() + "\">";
			buildPage(file.path().filename().string() + ".html", htmlImage);

			fs::copy(fs::combinePaths("src", file.path().filename().string()),
				fs::combinePaths("build", file.path().filename().string()));
		}
	}

	fs::writeFile(fs::combinePaths("build", "style.css"), CSS_TEMPLATE);
	buildPage("index.html", "");
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
	for (const auto& path : std::filesystem::directory_iterator("build"))
	{
		std::filesystem::remove_all(path);
	}
}

void generateNavCode()
{
	navCode = "<table>\n";

	for (const auto& file : std::filesystem::directory_iterator("src"))
	{
		std::string row;

		std::string displayName = file.path().filename().string();
		strex::replace(displayName, ".txt", "");
		strex::replace(displayName, ".md", "");

		std::string name = displayName + ".html";

		row = "<tr><td><a href=\"" + name + "\">" + displayName
			+ "</a></td></tr>\n";

		navCode += row;
	}

	navCode += "</table>";
}

void buildPage(const std::string& fileName, const std::string& contents)
{
	std::string navCode = ::navCode;

	std::string displayName = fileName;
	strex::replace(displayName, ".html", "");

	strex::replace(navCode, "<tr><td><a href=\"" + fileName + "\">" + displayName + "</a></td></tr>",
		"<tr id=\"selected\"><td><a href=\"" + fileName + "\">" + displayName + "</a></td></tr>");

	std::string resultContent = PAGE_TEMPLATE;
	strex::replace(resultContent, "[tab text]", tabText);
	strex::replace(resultContent, "[nav title]", navTitle);
	strex::replace(resultContent, "[nav content]", navCode);
	strex::replace(resultContent, "[main content]", contents);

	fs::writeFile(fs::combinePaths("build", fileName), resultContent);
}