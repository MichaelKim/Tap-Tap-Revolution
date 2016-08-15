#include <iostream>
#include <vector>
#include <fstream>
#include <conio.h>
#include <windows.h>

#include "Game.h"
#include "Utils.h"
using namespace std;

void intro()
{
    SetConsoleOutputCP(65001);
    setWindowSize(20, 10);
    system("Color 1f");
    printColor(0x1f, 0, 4, "     Welcome to     \n");
    for(int i=4; i>=0; i--)
    {
        printColor(0x1f, i, 4, "~");
        printColor(0x1f, 19-i, 4, "~");
        Sleep(500);
    }

    system("cls");
    setWindowSize(80, 6);
    system("Color fc");
    std::cout << "  ______           ______            ___                __     __  _         \n";
    std::cout << " /_  __/__ ____   /_  __/__ ____    / _ \\___ _  _____  / /_ __/ /_(_)__  ___ \n";
    std::cout << "  / /  / _ `/ _ \\  / / / _ `/ _ \\  / , _/ -_) |/ / _ \\/ / // / __/ / _ \\/ _ \\\n";
    std::cout << " /_/   \\_,_/ .__/ /_/  \\_,_/ .__/ /_/|_|\\__/|___/\\___/_/\\_,_/\\__/_/\\___/_//_/\n";
    std::cout << "          /_/             /_/                                                \n";
    for(int i=0; i<10; i++)
    {
        Sleep(100);
        system("Color f2");
        Sleep(100);
        system("Color fd");
    }
}

void displayMenu()
{
    system("cls");
    system("Color 1c");
    setWindowSize(45, 10);
    cout << " ___       ___        __                       \n";
    cout << "  | _  _    | _  _   |__)_   _ |   |_. _  _    \n";
    cout << "  |(_||_)   |(_||_)  | \\(-\\/(_)||_||_|(_)| ) \n";
    cout << "      |         |                              \n";
    printCenter(0x1b, 45, "Main Menu");
    printCenter(0x1d, 45, "1. Choose a song");
    printCenter(0x1d, 45, "2. Credits");
    printCenter(0x1d, 45, "0. Exit");
}

void songsMenu()
{
    system("Color 2f");
    printCenter(0x2f, 45, "List of Songs");

    vector<string> files = getSongList();
    for(int i=0; i<files.size(); i++)
    {
        ifstream musicData(files.at(i));
        if(musicData.good())
        {
            string songName;
            getline(musicData, songName);

            string musicList = "Song " + to_string(i+1) + ": " + songName;
            printCenter(0x2e, 45, musicList);
        }
        else
        {
            printError("Could not open data file: " + files.at(i));
        }
        musicData.close();
    }
    printCenter(0x2f, 45, "0: Back");

    int songChoice = 0;
    char menu;
    printColor(0x2f, 3, songChoice+1, "->");
    do
    {
        menu = getch();

        printColor(0x2f, 3, songChoice+1, "  ");
        if(menu == 's')
        {
            if(songChoice < files.size()) songChoice++;
            else songChoice = 0;
        }
        else if(menu == 'w')
        {
            if(songChoice > 0) songChoice--;
            else songChoice = files.size();
        }
        printColor(0x2f, 3, songChoice+1, "->");
    }
    while(menu != '\r' && menu != '0' && menu - '1' >= files.size());

    if(menu >= '0' && menu <= '9')
    {
        songChoice = menu - '1';
    }

    if(songChoice >= 0 && songChoice < files.size())
    {
        Game game;
        game.parse(files.at(songChoice));
        game.play();
    }
}

int main()
{
    intro();

    char menu;
    do
    {
        displayMenu();
        menu = getch();
        system("cls");

        if(menu == '1')
        {
            songsMenu();
        }
        else if(menu == '2')
        {
            system("Color 3a");
            cout << endl;
            printCenter(0x3d, 45, "By [LenKagamine]");
            printCenter(0x3a, 45, "For TEJ Summative");
            getch();
        }
    }
    while(menu != '0');
    return 0;
}
