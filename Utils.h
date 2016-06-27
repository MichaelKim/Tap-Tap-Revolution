#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>

void getWindowSize(int& width, int& height);
void setWindowSize(int width, int height);
void setCursor(int x, int y);
void setColor(int color);
void printColor(int color, int x, int y, std::string text);
void printCenter(int color, int x, std::string text);

std::string currentDir();
std::string getMusicFile(std::string fullPath);

std::vector<std::string> searchDir(std::string rootDir, std::string extension);

#endif // UTILS_H
