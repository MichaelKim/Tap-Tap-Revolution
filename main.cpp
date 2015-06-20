//Standard stuff
#include <cstdlib>
#include <cstdio>
#include <iostream>
//File I/O
#include <fstream>
//For kbhit, getch
#include <conio.h>
//System methods
#include <windows.h>
//Clock
#include <ctime>
//Vectors
#include <vector>
//Max/Min
#include <algorithm>
//Stringstream
#include <sstream>
using namespace std;

#define K1 'd'
#define K2 'f'
#define K3 ' '
#define K4 'j'
#define K5 'k'

void gotoxy(int column, int line){
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(GetStdHandle( STD_OUTPUT_HANDLE ), coord);
}
void setColor(int color){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
void coutc(int color, string output){ //color: 0xab, a = text; b = background
	setColor(color);
    cout << output;
    setColor(7); //Default = light grey text with black background
}
void get_window_size(int& lines, int& columns){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi)){
		lines   = csbi.srWindow.Bottom -csbi.srWindow.Top  +1;
		columns = csbi.srWindow.Right  -csbi.srWindow.Left +1;
	}
	else lines = columns = 0;
}

void set_window_size(int lines, int columns){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi)){
		// Make sure the new size isn't too big
		if(lines   > csbi.dwSize.Y) lines   = csbi.dwSize.Y;
		if(columns > csbi.dwSize.X) columns = csbi.dwSize.X;

		// Adjust window origin if necessary
		if((csbi.srWindow.Top  +lines)   > csbi.dwSize.Y) csbi.srWindow.Top  = csbi.dwSize.Y -lines   -1;
		if((csbi.srWindow.Left +columns) > csbi.dwSize.Y) csbi.srWindow.Left = csbi.dwSize.X -columns -1;

		// Calculate new size
		csbi.srWindow.Bottom = csbi.srWindow.Top  +lines   -1;
		csbi.srWindow.Right  = csbi.srWindow.Left +columns -1;

		SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &csbi.srWindow);
	}
}
void centerText(string text, int width, int color){
	int len = text.length();
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);
    gotoxy((width-len)/2,SBInfo.dwCursorPosition.Y);
	coutc(color, text+"\n");
}
string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}
string getMusicFile(string fullPath){
    string music = fullPath;
    music.erase(0,ExePath().length()+1);
    music.erase(music.end()-3,music.end());
    music.append("mp3");
    return music;
}
bool fileExists(const char *filename){
    ifstream ifile(filename);
    return ifile;
}
int SearchDirectory(vector<string>& refvecFiles,
									const string & fefcstrRootDirectory,
									const string & refcstrExtension,
									bool bSearchSubdirectories){
    string	 strFilePath;			 // Filepath
    string	 strPattern;			  // Pattern
    string	 strExtension;			// Extension
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
void game(string dataPath){
	system("cls");
	system("Color 17");
    set_window_size(30,28);
	string music = getMusicFile(dataPath);

    ifstream musicData(dataPath.c_str());
	if(!musicData) return;
	string songName;
	float count;
	int numNotes, inputNote, filler, bpm; //Total number of notes, input for note type, filler for line number, bpm
	getline(musicData,songName);
	musicData >> count >> bpm >> numNotes;
	vector<char> notes(numNotes); //Type of note
	vector<int> literalNotes(numNotes); //Actual number (1-5)
	vector<float> timesIndiv(numNotes); //Individual time for each note, for input
	vector<float> time(numNotes); //Total time for a note
	vector<bool> pressed(numNotes,false); //Pressed/missed or not pressed
	for(int i=0;i<numNotes;i++){
		musicData >> filler >> literalNotes.at(i) >> timesIndiv.at(i);
		if(literalNotes.at(i) == 1) notes.at(i) = K1;
    	else if(literalNotes.at(i) == 2) notes.at(i) = K2;
    	else if(literalNotes.at(i) == 3) notes.at(i) = K3;
    	else if(literalNotes.at(i) == 4) notes.at(i) = K4;
    	else if(literalNotes.at(i) == 5) notes.at(i) = K5;
	}
    musicData.close();
    time.at(0) = count; //First note is just intro time
    for(int i=1;i<numNotes;i++){
    	float temp = count;
        for(int j=1;j<=i;j++) temp += timesIndiv.at(j-1);
        time.at(i) = temp;
        temp = count;
    }
    char input, tempinput;
    int range = 150; //Gap of time during which note can be accepted
    int score = 0;
    int nextNote = 0, nextNoteTemp = 0;

    unsigned long int currentTime = 0; //The current time

	gotoxy(1,0);
    coutc(0x17,"\x00c9\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00bb");
    gotoxy(1,25);
    coutc(0x17,"\x00c8\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00bc");
    for(int i=0;i<6;i++){
    	for(int j=1;j<25;j++){
    		gotoxy(i*5+1,j);
    		if(i==0 || i==5) coutc(0x17,"\x00ba");
    		else coutc(0x17,"\x00b3");
		}
	}

    float whole = (60.0/bpm)*4000; //These 5 lines have no real use here
    float half = whole/2;
    float quarter = whole/4;
    float eighth = whole/8;
    float sixteenth = whole/16;

    string musicOpen = "open " + music + " alias music";
	mciSendString(musicOpen.c_str(), 0, 0, 0 );
	mciSendString("open hit.wav alias hit",0,0,0);
	mciSendString("open miss.mp3 alias miss",0,0,0);
    clock_t start;
    mciSendString("play music", 0, 0, 0 );
    start = clock();
    while(currentTime < time.back()+2000){
        currentTime = 1000*(clock() - start) / (double)CLOCKS_PER_SEC; //Used for exiting while loop
        if(kbhit()){ //If pressed something
            tempinput = getch(); //Get the input
            if(input != tempinput){ //If the input is different
                input = tempinput; //Store that new input
                for(int i=nextNote;i<min((int)notes.size(),nextNote+50);i++){ //Go through 50 notes (doesn't have to go through all notes)
                    if(pressed.at(i)) continue;

                    if(notes.at(i) == input){ //If same key is found
                        currentTime = 1000*(clock() - start) / (double)CLOCKS_PER_SEC; //Get the time
                        if(!pressed.at(i) && currentTime >= time.at(i)-range && currentTime <= time.at(i)+range){ //If the time is in the

							//gotoxy(0,15);                                      //right frame for the note
                            //cout << i+1 << "th note accepted. Time: " << currentTime; //Accept the note
                            mciSendString("stop hit",0,0,0);
                            mciSendString("play hit from 0",0,0,0); //Play the hit sound
                            score++; //Increase score
                            pressed.at(i) = true; //Hit

                            int y = 23-max((int)((time.at(i)-currentTime)/50),0);
			                int x = literalNotes.at(i)*5-2;
			                gotoxy(x,y-3); //Clear one note higher as well
							coutc(0x17,"  ");
			                gotoxy(x-1,y-2);
							coutc(0x17,"    ");
							gotoxy(x-1,y-1);
							coutc(0x17,"    ");
			            	gotoxy(x-1,y);
							coutc(0x17,"    ");
							for(int j=0;j<5;j++){
							    gotoxy(2+j*5,22);
							    coutc(0x17," ");
							    gotoxy(5+j*5,22);
							    coutc(0x17," ");
                            }
                            nextNoteTemp = max(0,i-5);
                        }
                    }
                }
                nextNote = nextNoteTemp;
            }
        }
        else input = '\0'; //Reset the input
        gotoxy(13,27);
        stringstream scoreStream;
        scoreStream << "Score: " << score;
        centerText(scoreStream.str(),28,0x1e);
        for(int i=0;i<5;i++){
        	int color = 0x07;
        	if(i == 0 || i == 4) color = 0x1f; //White
        	else if(i == 1 || i == 3) color = 0x17; //Light grey
        	else if(i == 2) color = 0x18; //Dark grey
        	gotoxy(3+i*5,22);
        	coutc(color,"\x00b0\x00b0");
        	gotoxy(2+i*5,23);
        	coutc(color,"\x00b0\x00b0\x00b0\x00b0");
        	gotoxy(3+i*5,24);
        	coutc(color,"\x00b0\x00b0");
        }
        for(int i=nextNote;i<min((int)notes.size(),nextNote+50);i++){ //Go through 50 notes (doesn't have to go through all notes)
            if(pressed.at(i)) continue; //All ready passed, ignore

            currentTime = 1000*(clock() - start) / (double)CLOCKS_PER_SEC; //Get the time
            if(!pressed.at(i) && currentTime > time.at(i)+range){ //If a note not pressed has passed
                //gotoxy(0,15);
                //cout << i+1 << "th note missed. Time: " << currentTime; //Note is missed
                mciSendString("stop miss",0,0,0);
                mciSendString("play miss from 0",0,0,0);
                score -= (score == 0)?0:1;
                pressed.at(i) = true;

                nextNoteTemp = max(0,i-5);
            }
            if(currentTime < time.at(i)+range && currentTime > time.at(i)-1000){
                int y = 23-max((int)((time.at(i)-currentTime)/50),0);
                int x = literalNotes.at(i)*5-2;
                gotoxy(x,y-3); //Clear one note higher as well
				coutc(0x17,"  ");
                gotoxy(x-1,y-2);
				coutc(0x17,"    ");
				gotoxy(x-1,y-1);
				coutc(0x17,"    ");
            	gotoxy(x,y);
				coutc(0x17,"  ");

                int color = 0x17;
                if(literalNotes.at(i) == 1 || literalNotes.at(i) == 5) color = 0x15; //Magenta
                else if(literalNotes.at(i) == 2 || literalNotes.at(i) == 4) color = 0x13; //Cyan
                else if(literalNotes.at(i) == 3) color = 0x14; //Red
                if(y == 23){
					gotoxy(x,y-1);
					coutc(color+8,"\x00db\x00db"); //Normal colors
					gotoxy(x-1,y);
					coutc(color+8,"\x00db\x00db\x00db\x00db"); //Normal colors
                	gotoxy(x,y+1);
					coutc(color+8,"\x00db\x00db"); //Normal colors
				}
                else{
                	gotoxy(x,y-1);
					coutc(color,"\x00b1\x00b1"); //Normal colors
					gotoxy(x-1,y);
					coutc(color,"\x00b1\x00b1\x00b1\x00b1"); //Normal colors
                	gotoxy(x,y+1);
					coutc(color,"\x00b1\x00b1"); //Normal colors
				}
            }
        }
        nextNote = nextNoteTemp;
    }
    system("cls");
    system("Color 1f");
    cout << endl;
    stringstream scoreStream;
    scoreStream << "Score: " << score;
    centerText(scoreStream.str(),28,0x17);
    Sleep(1000);
    if(score == numNotes){
        centerText("FULL COMBO",28,0x1d);
        Sleep(1000);
        centerText("Rank SS",28,0x1f);
        Sleep(1000);
    }
    else if(score > numNotes*0.9) centerText("Rank S",28,0x1e);
    else if(score > numNotes*0.8) centerText("Rank A",28,0x1a);
    else if(score > numNotes*0.7) centerText("Rank B",28,0x19);
    else if(score > numNotes*0.6) centerText("Rank C",28,0x15);
    else if(score > numNotes*0.5) centerText("Rank D",28,0x17);
    else if(score > numNotes) centerText("Rank Hacker",28,0x1b);
    else centerText("Failed Song",28,0x16);
    Sleep(1000);
    getch();
    mciSendString("stop music",0,0,0);
    mciSendString("close music",0,0,0);
    mciSendString("close hit",0,0,0);
    mciSendString("close miss",0,0,0);
	return;
}
int main(){
    int height,width;
	get_window_size(height,width);
	set_window_size(10,20);
	system("Color 1f");
	gotoxy(0,4);
	coutc(0x1f,"     Welcome to     \n");
	for(int i=4;i>=0;i--){
		gotoxy(i,4);
		coutc(0x1f,"~");
		gotoxy(19-i,4);
		coutc(0x1f,"~");
		Sleep(500);
	}
	system("cls");
	set_window_size(6,80);
	system("Color fc");
	cout << "  ______           ______            ___                __     __  _         \n";
	cout << " /_  __/__ ____   /_  __/__ ____    / _ \\___ _  _____  / /_ __/ /_(_)__  ___ \n";
	cout << "  / /  / _ `/ _ \\  / / / _ `/ _ \\  / , _/ -_) |/ / _ \\/ / // / __/ / _ \\/ _ \\\n";
	cout << " /_/   \\_,_/ .__/ /_/  \\_,_/ .__/ /_/|_|\\__/|___/\\___/_/\\_,_/\\__/_/\\___/_//_/\n";
	cout << "          /_/             /_/                                                \n";
	for(int i=0;i<10;i++){
		Sleep(100);
		system("Color f2");
		Sleep(100);
		system("Color fd");
	}
	while(1){
	system("cls");
	system("Color 1c");
	set_window_size(10,45);
	cout << " ___       ___        __                     \n";
	cout << "  | _  _    | _  _   |__)_   _ |   |_. _  _  \n";
	cout << "  |(_||_)   |(_||_)  | \\(-\\/(_)||_||_|(_)| ) \n";
	cout << "      |         |                            \n";
	centerText("Main Menu",45,0x1b);
	centerText("1. Choose a song",45,0x1d);
	centerText("2. Credits",45,0x1d);
	centerText("3. Exit",45,0x1d);
	char menu = getch();
        if(menu == '1'){
            system("cls");
            system("Color 2f");
            centerText("List of Songs",45,0x2f);
            vector<string> files(0);
            SearchDirectory(files,ExePath(),"txt",false);
            for(int i=0;i<files.size();i++){
            	ifstream musicData((files.at(i)).c_str());
            	if(!musicData) continue;
            	string songName;
            	getline(musicData,songName);
            	musicData.close();
            	stringstream line;
            	line << "Song " << i+1 << ": " << songName;
            	centerText(line.str(),45,0x2e);
            }
            centerText("0: Back",45,0x2f);
            char choice;
            int songChoice = 0;
            gotoxy(3,songChoice+1);
            coutc(0x2f,"->");
            while(choice = getch()){
                gotoxy(3,songChoice+1);
                coutc(0x2f,"  ");
                if(choice == 's' && songChoice < files.size()) songChoice++;
                else if(choice == 'w' && songChoice > 0) songChoice--;
                else if(choice == '\r') break;
                gotoxy(3,songChoice+1);
                coutc(0x2f,"->");
            }
            if(songChoice != files.size()) game(files.at(songChoice));
        }
        else if(menu == '2'){
             system("cls");
             system("Color 3a");
             cout << endl;
             centerText("By [LenKagamine]",45,0x3d);
             centerText("For TEJ Summative",45,0x3a);
             getch();
        }
        else if(menu == '3') break;
        else menu = getch();
    }
    return 0;
}
