#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <conio.h>
#include <sstream>

#include "Game.h"
#include "Utils.h"

#define K1 'd'
#define K2 'f'
#define K3 ' '
#define K4 'j'
#define K5 'k'

using namespace std;

void game(string dataPath){
	system("cls");
	system("Color 17");
	setWindowSize(28, 30);

	string music = getMusicFile(dataPath);
    ifstream musicData(dataPath.c_str());
	if(!musicData) return;

	string songName;
	float count;
	int numNotes, filler, bpm; //Total number of notes, filler for line number, bpm
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

    printColor(0x17, 1, 0,
        "\x00c9\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00d1\x00cd\x00cd\x00cd\x00cd\x00bb");
	printColor(0x17, 1, 25,
        "\x00c8\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00cf\x00cd\x00cd\x00cd\x00cd\x00bc");
	for(int i=0;i<6;i++){
    	for(int j=1;j<25;j++){
            if(i==0 || i==5) printColor(0x17, i*5+1, j, "\x00ba");
            else printColor(0x17, i*5+1, j, "\x00b3");
		}
	}

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

							//setCursor(0,15);                                      //right frame for the note
                            //cout << i+1 << "th note accepted. Time: " << currentTime; //Accept the note
                            mciSendString("stop hit",0,0,0);
                            mciSendString("play hit from 0",0,0,0); //Play the hit sound
                            score++; //Increase score
                            pressed.at(i) = true; //Hit

                            int y = 23-max((int)((time.at(i)-currentTime)/50),0);
			                int x = literalNotes.at(i)*5-2;
			                printColor(0x17, x, y-3, "  "); //Clear one note higher as well
			                printColor(0x17, x-1, y-2, "    ");
			                printColor(0x17, x-1, y-1, "    ");
			                printColor(0x17, x-1, y, "    ");
							for(int j=0;j<5;j++){
                                printColor(0x17, 2+j*5, 22, " ");
                                printColor(0x17, 5+j*5, 22, " ");
                            }
                            nextNoteTemp = max(0,i-5);
                        }
                    }
                }
                nextNote = nextNoteTemp;
            }
        }
        else input = '\0'; //Reset the input
        setCursor(13,27);
        stringstream scoreStream;
        scoreStream << "Score: " << score;
        printCenter(0x1e, 28, scoreStream.str());
        for(int i=0;i<5;i++){
        	int color = 0x07;
        	if(i == 0 || i == 4) color = 0x1f; //White
        	else if(i == 1 || i == 3) color = 0x17; //Light grey
        	else if(i == 2) color = 0x18; //Dark grey
        	printColor(color, 3+i*5, 22, "\x00b0\x00b0");
        	printColor(color, 2+i*5, 23, "\x00b0\x00b0\x00b0\x00b0");
        	printColor(color, 3+i*5, 24, "\x00b0\x00b0");
        }
        for(int i=nextNote;i<min((int)notes.size(),nextNote+50);i++){ //Go through 50 notes (doesn't have to go through all notes)
            if(pressed.at(i)) continue; //All ready passed, ignore

            currentTime = 1000*(clock() - start) / (double)CLOCKS_PER_SEC; //Get the time
            if(!pressed.at(i) && currentTime > time.at(i)+range){ //If a note not pressed has passed
                //setCursor(0,15);
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
                printColor(0x17, x, y-3, "  "); //Clear one note higher as well
                printColor(0x17, x-1, y-2, "    ");
                printColor(0x17, x-1, y-1, "    ");
                printColor(0x17, x, y, "  ");

                int color = 0x17;
                if(literalNotes.at(i) == 1 || literalNotes.at(i) == 5) color = 0x15; //Magenta
                else if(literalNotes.at(i) == 2 || literalNotes.at(i) == 4) color = 0x13; //Cyan
                else if(literalNotes.at(i) == 3) color = 0x14; //Red
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
        }
        nextNote = nextNoteTemp;
    }
    system("cls");
    system("Color 1f");
    cout << endl;
    stringstream scoreStream;
    scoreStream << "Score: " << score;
    printCenter(0x17, 28, scoreStream.str());
    Sleep(1000);
    if(score == numNotes){
        printCenter(0x1d, 28, "FULL COMBO");
        Sleep(1000);
        printCenter(0x1f, 28, "Rank SS");
        Sleep(1000);
    }
    else if(score > numNotes*0.9) printCenter(0x1e, 28, "Rank S");
    else if(score > numNotes*0.8) printCenter(0x1a, 28, "Rank A");
    else if(score > numNotes*0.7) printCenter(0x19, 28, "Rank B");
    else if(score > numNotes*0.6) printCenter(0x15, 28, "Rank C");
    else if(score > numNotes*0.5) printCenter(0x17, 28, "Rank D");
    else if(score > numNotes) printCenter(0x1b, 28, "Rank Hacker");
    else printCenter(0x16, 28, "Failed Song");
    Sleep(1000);
    getch();
    mciSendString("stop music",0,0,0);
    mciSendString("close music",0,0,0);
    mciSendString("close hit",0,0,0);
    mciSendString("close miss",0,0,0);
	return;
}
