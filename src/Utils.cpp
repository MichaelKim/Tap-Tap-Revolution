#include <iostream>
#include <vector>
#include <windows.h>
#include <dirent.h>

#include "Utils.h"

void getWindowSize(int& width, int& height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi))
    {
        width  = csbi.srWindow.Right  -csbi.srWindow.Left +1;
        height = csbi.srWindow.Bottom -csbi.srWindow.Top  +1;
    }
    else width = height = 0;
}

void setWindowSize(int width, int height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi))
    {
        // Make sure the new size isn't too big
        if(height > csbi.dwSize.Y) height = csbi.dwSize.Y;
        if(width  > csbi.dwSize.X) width  = csbi.dwSize.X;

        // Adjust window origin if necessary
        if((csbi.srWindow.Top  +height) > csbi.dwSize.Y) csbi.srWindow.Top  = csbi.dwSize.Y -height -1;
        if((csbi.srWindow.Left +width)  > csbi.dwSize.Y) csbi.srWindow.Left = csbi.dwSize.X -width  -1;

        // Calculate new size
        csbi.srWindow.Bottom = csbi.srWindow.Top  +height -1;
        csbi.srWindow.Right  = csbi.srWindow.Left +width  -1;

        SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &csbi.srWindow);
    }
}

void setCursor(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle( STD_OUTPUT_HANDLE ), coord);
}

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printColor(int color, int x, int y, std::string text)
{
    setCursor(x, y);
    setColor(color);
    std::cout << text;
    setColor(7);
}

void printCenter(int color, int width, std::string text)
{
    int len = text.length();
    CONSOLE_SCREEN_BUFFER_INFO SBInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
    printColor(color, (width-len)/2, SBInfo.dwCursorPosition.Y, text+"\n");
}

void printError(std::string text)
{
    int width = 0, height = 0;
    getWindowSize(width, height);

    text = "ERROR: " + text;
    int y = text.length() / width + 1;
    for(int i=y; i>=1; i--)
    {
        printColor(0x0f, 0, height-i, text.substr((y-i)*width, width));
    }
}

std::vector<std::string> getSongList()
{
    std::vector<std::string> songList = searchDir(currentDir() + "\\songs", "txt");
    for(std::string &song: songList)
    {
        song = "songs\\" + song;
    }
    return songList;
}

std::string currentDir()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\");
    return std::string(buffer).substr(0, pos);
}

std::string getMusicFile(std::string fullPath)
{
    std::string music = fullPath.substr(0, fullPath.rfind('.')) + ".mp3";
    return music;
}

std::vector<std::string> searchDir(std::string rootDir, std::string extension)
{
    std::vector<std::string> files;
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(rootDir.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while((ent = readdir(dir)) != NULL)
        {
            std::string fileName = ent->d_name;
            std::string extName = fileName.substr(fileName.rfind('.') + 1);
            if(extName == extension) files.push_back(ent->d_name);
        }
        closedir(dir);
    }
    else
    {
        printError("Could not search for songs");
    }
    return files;
}

