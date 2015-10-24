#include <fstream>
#include <vector>
#include <windows.h>

#include "Utils.h"

void getWindowSize(int& width, int& height){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi)){
		width  = csbi.srWindow.Right  -csbi.srWindow.Left +1;
		height = csbi.srWindow.Bottom -csbi.srWindow.Top  +1;
	}
	else width = height = 0;
}

void setWindowSize(int width, int height){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi)){
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

void setCursor(int x, int y){
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle( STD_OUTPUT_HANDLE ), coord);
}

void setColor(int color){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printColor(int color, int x, int y, std::string text){
    setCursor(x, y);
    setColor(color);
    std::cout << text;
    setColor(7);
}

void printCenter(int color, int width, std::string text){
	int len = text.length();
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
	printColor(color, (width-len)/2, SBInfo.dwCursorPosition.Y, text+"\n");
}

std::string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

std::string getMusicFile(std::string fullPath){
    std::string music = fullPath;
    music.erase(0,ExePath().length()+1);
    music.erase(music.end()-3,music.end());
    music.append("mp3");
    return music;
}

bool fileExists(const char *filename){
    std::ifstream ifile(filename);
    return ifile;
}

int SearchDirectory(std::vector<std::string>& refvecFiles,
									const std::string & fefcstrRootDirectory,
									const std::string & refcstrExtension,
									bool bSearchSubdirectories){
    std::string	 strFilePath;			 // Filepath
    std::string	 strPattern;			  // Pattern
    std::string	 strExtension;			// Extension
    HANDLE		  hFile;				   // Handle to file
    WIN32_FIND_DATA FileInformation;		 // File information

    strPattern = fefcstrRootDirectory + "\\*.*";

    hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
    if(hFile != INVALID_HANDLE_VALUE){
	    do{
	        if(FileInformation.cFileName[0] != '.'){
                strFilePath.erase();
		        strFilePath = fefcstrRootDirectory + "\\" + FileInformation.cFileName;

          		if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                    //True if you want to search subdirectories
		            if(bSearchSubdirectories){
					    // Search subdirectory
					    int iRC = SearchDirectory(refvecFiles,
											    strFilePath,
											    refcstrExtension,
											    bSearchSubdirectories);
					    if(iRC)	return iRC;
                    }
		        }
		        else{
                    // Check extension
		            strExtension = FileInformation.cFileName;
		            strExtension = strExtension.substr(strExtension.rfind(".") + 1);
		            if(strExtension == refcstrExtension){
 	                    // Save filename
 	                    if(fileExists((getMusicFile(strFilePath)).c_str()))
                            refvecFiles.push_back(strFilePath);
		            }
		        }
            }
	    }while(::FindNextFile(hFile, &FileInformation) == TRUE);
	    // Close handle
	    ::FindClose(hFile);
	    DWORD dwError = ::GetLastError();
	    if(dwError != ERROR_NO_MORE_FILES)
        return dwError;
    }
    return 0;
}
