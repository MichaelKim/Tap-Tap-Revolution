#ifndef GAME_H
#define GAME_H

class Game{
    public:
        Game();
        ~Game();
        void parse(std::string dataPath);
        void play();    //Gap of time during which note can be accepted

        const int range = 150;

    private:
        void drawGrid();
        char getInput(char oldinput);
        void drawNote(int x, int y, int row);
        void clearNote(int x, int y);

        int numNotes;                   //Total number of notes
        std::string music;              //Music file filename
        std::vector<char>  notes;       //Type of note
        std::vector<int>   noteRow;     //Row number of note (1-5)
        std::vector<float> noteLength;  //Individual time for each note, for input
        std::vector<float> time;        //Total time for a note
        std::vector<bool>  pressed;     //Pressed/missed or not pressed

};

#endif // GAME_H
