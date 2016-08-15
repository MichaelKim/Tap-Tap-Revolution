# Tap-Tap-Revolution

A C++ rhythm game using the console window for my Grade 10 TEJ course.

This was my summative project for the Grade 10 Computer Engineering (TEJ3M) course. This was originally coded to be played using a simple controller I built connected via the parallel port. I later ported it to be playable with the keyboard.

To compile: Tap Tap Revolution makes a lot of Windows API calls to manipulate the console size, change colour, etc. It plays music using the MCI command `mciSendString()`, which requires `-lwinmm`.

### Gameplay
Controls:
- w/s and enter to navigate menus
- 5 columns from left to right: `d` `f` `space` `j` `k`

After selecting a song, notes will start falling from five columns. The notes will follow the rhythm of the song playing in the background. When a note falls to the bottom of a column, press the corresponding key to hit the note (+1 to score). If the note isn't hit in time, it will be missed (-1 to score).
