#include <iostream>
#include <vector>

void getWindowSize(int& width, int& height);
void setWindowSize(int width, int height);
void setCursor(int x, int y);
void setColor(int color);
void printColor(int color, int x, int y, std::string text);
void printCenter(int color, int x, std::string text);

std::string ExePath();
std::string getMusicFile(std::string fullPath);

bool fileExists(const char *filename);
int SearchDirectory(std::vector<std::string>& refvecFiles,
									const std::string & fefcstrRootDirectory,
									const std::string & refcstrExtension,
									bool bSearchSubdirectories);
