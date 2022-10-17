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
void buildSinglePage();
void loadConfig();
std::string configGetLineValue(const std::vector<std::string>& lines, int index);
void clearBuildDir();
void generateNavCode();
void generateNavCodeWithSectionLinks();
std::string parseTextToHtml(const std::filesystem::path& filePath);
std::string parseMarkdownToHtml(const std::filesystem::path& filePath);
std::string parseImageToHtml(const std::filesystem::path& filePath);
void buildPage(const std::string& fileName, const std::string& contents);

std::string tabText;
std::string navTitle;
bool autoInsertHeadingForTxtFiles;
bool singlePageMode;

std::string navCode;
std::string style;
std::string pageTemplate;

std::string sourceDirectory;
std::string buildDirectory;

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
		loadConfig();

		if (singlePageMode)
			buildSinglePage();
		else
			build();
		return 0;
	}

	if (std::string(argv[1]) == "--use-custom-html")
	{
		fs::writeFile("config.html", PAGE_TEMPLATE);
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
	fs::writeFile("config.css", CSS_TEMPLATE);
}

void build()
{
	clearBuildDir();
	generateNavCode();

	for (const auto& file : std::filesystem::directory_iterator(sourceDirectory))
	{
		if (file.path().extension() == ".txt")
		{
			std::string htmlOutput = parseTextToHtml(file.path());
			buildPage(file.path().stem().string() + ".html", htmlOutput);
		}
		else if (file.path().extension() == ".md")
		{
			std::string htmlOutput = parseMarkdownToHtml(file.path());
			buildPage(file.path().stem().string() + ".html", htmlOutput);
		}
		else if (file.path().extension() == ".png" || file.path().extension() == ".jpg")
		{
			std::string htmlImage = parseImageToHtml(file.path());
			buildPage(file.path().filename().string() + ".html", htmlImage);

			fs::copy(fs::combinePaths(sourceDirectory, file.path().filename().string()),
				fs::combinePaths(buildDirectory, file.path().filename().string()));
		}
	}

	fs::writeFile(fs::combinePaths(buildDirectory, "style.css"), style);
	buildPage("index.html", "");
}

void buildSinglePage()
{
	clearBuildDir();
	generateNavCodeWithSectionLinks();

	std::string pageContent;

	for (const auto& file : std::filesystem::directory_iterator(sourceDirectory))
	{
		if (file.path().extension() == ".txt")
		{
			std::string htmlOutput = parseTextToHtml(file.path());
			pageContent += "<h1 id=\"" + file.path().stem().string() + "\">" + file.path().stem().string() + "</h1>" + htmlOutput;
		}
		else if (file.path().extension() == ".md")
		{
			std::string htmlOutput = parseMarkdownToHtml(file.path());
			pageContent += "<h1 id=\"" + file.path().stem().string() + "\">" + file.path().stem().string() + "</h1>" + htmlOutput;
		}
		else if (file.path().extension() == ".png" || file.path().extension() == ".jpg")
		{
			pageContent += parseImageToHtml(file.path());

			fs::copy(fs::combinePaths(sourceDirectory, file.path().filename().string()),
				fs::combinePaths(buildDirectory, file.path().filename().string()));
		}
	}

	fs::writeFile(fs::combinePaths(buildDirectory, "style.css"), style);
	buildPage("index.html", pageContent);
}

void loadConfig()
{
	std::string config = fs::readFile("config.txt");
	std::vector<std::string> lines = strex::split(config, '\n');

	tabText = configGetLineValue(lines, 0);
	navTitle = configGetLineValue(lines, 1);
	autoInsertHeadingForTxtFiles = configGetLineValue(lines, 2) == "true";
	singlePageMode = configGetLineValue(lines, 3) == "true";
	sourceDirectory = configGetLineValue(lines, 4);
	buildDirectory = configGetLineValue(lines, 5);

	style = fs::readFile("config.css");

	if (fs::exists("config.html"))
		pageTemplate = fs::readFile("config.html");
	else
		pageTemplate = PAGE_TEMPLATE;
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
	for (const auto& path : std::filesystem::directory_iterator(buildDirectory))
	{
		std::filesystem::remove_all(path);
	}
}

void generateNavCode()
{
	navCode = "<table>\n";

	for (const auto& file : std::filesystem::directory_iterator(sourceDirectory))
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

void generateNavCodeWithSectionLinks()
{
	navCode = "<table>\n";

	for (const auto& file : std::filesystem::directory_iterator(sourceDirectory))
	{
		std::string row;

		std::string displayName = file.path().filename().string();
		strex::replace(displayName, ".txt", "");
		strex::replace(displayName, ".md", "");

		row = "<tr><td><a href=\"#" + displayName + "\">" + displayName
			+ "</a></td></tr>\n";

		navCode += row;
	}

	navCode += "</table>";
}

std::string parseTextToHtml(const std::filesystem::path& filePath)
{
	std::string contents = fs::readFile(filePath.string());
	strex::replace(contents, "\n", "<br>");

	if (autoInsertHeadingForTxtFiles)
		contents = "<h1>" + filePath.stem().string() + "</h1>" + contents;

	return contents;
}

std::string parseMarkdownToHtml(const std::filesystem::path& filePath)
{
	std::stringstream markdownInput(fs::readFile(filePath.string()));
	std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>();
	std::string htmlOutput = parser->Parse(markdownInput);

	strex::replace(htmlOutput, ".md", ".html");

	return htmlOutput;
}

std::string parseImageToHtml(const std::filesystem::path& filePath)
{
	return "<img src=\"" + filePath.filename().string() + "\">";
}

void buildPage(const std::string& fileName, const std::string& contents)
{
	std::string navCode = ::navCode;

	std::string displayName = fileName;
	strex::replace(displayName, ".html", "");

	strex::replace(navCode, "<tr><td><a href=\"" + fileName + "\">" + displayName + "</a></td></tr>",
		"<tr id=\"selected\"><td><a href=\"" + fileName + "\">" + displayName + "</a></td></tr>");

	std::string resultContent = pageTemplate;
	strex::replace(resultContent, "[tab text]", tabText);
	strex::replace(resultContent, "[nav title]", navTitle);
	strex::replace(resultContent, "[nav content]", navCode);
	strex::replace(resultContent, "[main content]", contents);

	fs::writeFile(fs::combinePaths(buildDirectory, fileName), resultContent);
}