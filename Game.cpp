#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <conio.h>

#include "Game.h"
#include "Utils.h"

#define K1 'd'
#define K2 'f'
#define K3 ' '
#define K4 'j'
#define K5 'k'

using namespace std;

Game::Game(){

}

Game::~Game(){
}

void Game::parse(string dataPath){
	music = getMusicFile(dataPath);
    ifstream musicData(dataPath);
	if(!musicData.good()) return;

	string songName;
	float startDelay;
	int bpm;
	getline(musicData, songName);
	musicData >> startDelay >> bpm >> numNotes;

	notes.resize(numNotes);
	noteRow.resize(numNotes);
	noteLength.resize(numNotes);
    time.resize(numNotes);
	pressed.resize(numNotes, false);

	for(int i=0, temp;i<numNotes;i++){
		musicData >> temp >> noteRow.at(i) >> noteLength.at(i);
		if(noteRow.at(i) == 1) notes.at(i) = K1;
    	else if(noteRow.at(i) == 2) notes.at(i) = K2;
    	else if(noteRow.at(i) == 3) notes.at(i) = K3;
    	else if(noteRow.at(i) == 4) notes.at(i) = K4;
    	else if(noteRow.at(i) == 5) notes.at(i) = K5;
	}
    musicData.close();

    time.at(0) = startDelay; //First note is just intro time
    for(int i=1;i<numNotes;i++){
        time.at(i) = time.at(i-1) + noteLength.at(i-1);
    }

    string musicOpen = "open " + music + " alias music";
	mciSendString(musicOpen.c_str(), 0, 0, 0 );
	mciSendString("open hit.wav alias hit", 0, 0, 0);
	mciSendString("open miss.mp3 alias miss", 0, 0, 0);
}

void Game::drawGrid(){
    system("cls");
	system("Color 17");
	setWindowSize(28, 30);

    printColor(0x17, 1, 0,
        "\x00c9\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd"
        "\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00bb");
	printColor(0x17, 1, 25,
        "\x00c8\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd"
        "\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00bc");
	for(int i=0;i<6;i++){
    	for(int j=1;j<25;j++){
            if(i==0 || i==5) printColor(0x17, i*5+1, j, "\x00ba");
            else printColor(0x17, i*5+1, j, "\x00b3");
		}
	}
}

char Game::getInput(char oldinput){
    if(kbhit()){
        char input = getch();
        if(oldinput != input){
            oldinput = input;
            return input;
        }
    }
    return '\0';
}

void Game::drawNote(int x, int y, int row){
    int color = 0x17;
    if(row == 1 || row == 5) color = 0x15; //Magenta
    else if(row == 2 || row == 4) color = 0x13; //Cyan
    else if(row == 3) color = 0x14; //Red
    if(y == 23){
        printColor(color+8, x, y-1, "\x00db\x00db"); //Normal colors
        printColor(color+8, x-1, y, "\x00db\x00db\x00db\x00db"); //Normal colors
        printColor(color+8, x, y+1, "\x00db\x00db"); //Normal colors
    }
    else{
        printColor(color, x, y-1, "\x00b1\x00b1"); //Normal colors
        printColor(color, x-1, y, "\x00b1\x00b1\x00b1\x00b1"); //Normal colors
        printColor(color, x, y+1, "\x00b1\x00b1"); //Normal colors
    }
}

void Game::clearNote(int x, int y){
    printColor(0x17, x, y-3, "  "); //Clear one note higher as well
    printColor(0x17, x-1, y-2, "    ");
    printColor(0x17, x-1, y-1, "    ");
    printColor(0x17, x, y, "  ");
}


void Game::play(){
    drawGrid();

    char input;
    int score = 0;
    int nextNote = 0, nextNoteTemp = 0;

    unsigned long int currentTime = 0; //The current time

    mciSendString("play music", 0, 0, 0 );
    clock_t start = clock();

    while(currentTime < time.back()+2000){
        currentTime = 1000*(clock() - start) / (double)CLOCKS_PER_SEC; //Used for exiting while loop
        input = getInput(input);
        for(int i=nextNote;i<min((int)notes.size(),nextNote+50);i++){ //Go through 50 notes (doesn't have to go through all notes)
            if(pressed.at(i)) continue; //All ready passed, ignore


            if(currentTime <= time.at(i)+range){ //Didn't miss note yet
                if(currentTime >= time.at(i)-range){ //Within time frame to press note
                    if(input && notes.at(i) == input){ //If same key is found, accept note
                        mciSendString("stop hit", 0, 0, 0);
                        mciSendString("play hit from 0", 0, 0, 0); //Play the hit sound
                        score++; //Increase score
                        pressed.at(i) = true; //Hit
                        nextNoteTemp = max(0, i-5);
                    }
                }
                if(currentTime > time.at(i)-1000){
                    int y = 23-max((int)((time.at(i)-currentTime)/50),0);
                    int x = noteRow.at(i)*5-2;
                    clearNote(x, y);
                    //printColor(0x17, noteRow.at(i)*5 - 3, 22, " ");
                    //printColor(0x17, noteRow.at(i)*5, 22, " ");
                    drawNote(x, y, noteRow.at(i));
                }
            }
            else{ //If a note not pressed has passed, note is missed
                mciSendString("stop miss",0,0,0);
                mciSendString("play miss from 0",0,0,0);
                if(score > 0) score--;
                pressed.at(i) = true;

                nextNoteTemp = max(0,i-5);
            }
        }

        setCursor(13,27);
        printCenter(0x1e, 28, "Score: " + to_string(score));
        for(int i=0;i<5;i++){
        	int color = 0x07;
        	if(i == 0 || i == 4) color = 0x1f; //White
        	else if(i == 1 || i == 3) color = 0x17; //Light grey
        	else if(i == 2) color = 0x18; //Dark grey
        	printColor(color, 3+i*5, 22, "\x00b0\x00b0");
        	printColor(color, 2+i*5, 23, "\x00b0\x00b0\x00b0\x00b0");
        	printColor(color, 3+i*5, 24, "\x00b0\x00b0");
        }

        nextNote = nextNoteTemp;
    }

    system("cls");
    system("Color 1f");
    cout << endl;
    printCenter(0x17, 28, "Score: " + to_string(score));
    Sleep(1000);
    if(score == numNotes){
        printCenter(0x1d, 28, "FULL COMBO");
        Sleep(1000);
        printCenter(0x1f, 28, "Rank SS");
    }
    else if(score > numNotes*0.9) printCenter(0x1e, 28, "Rank S");
    else if(score > numNotes*0.8) printCenter(0x1a, 28, "Rank A");
    else if(score > numNotes*0.7) printCenter(0x19, 28, "Rank B");
    else if(score > numNotes*0.6) printCenter(0x15, 28, "Rank C");
    else if(score > numNotes*0.5) printCenter(0x17, 28, "Rank D");
    else if(score > numNotes) printCenter(0x1b, 28, "Rank Hacker");
    else printCenter(0x16, 28, "Failed Song");

    mciSendString("stop music",0,0,0);
    mciSendString("close music",0,0,0);
    mciSendString("close hit",0,0,0);
    mciSendString("close miss",0,0,0);

    getch();
}
