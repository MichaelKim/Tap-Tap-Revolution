# Tap-Tap-Revolution
C++ Console Rhythm Game

A very crude game made for my Grade 10 TEJ Summative. This is what happens when you try to show GUI in console.

This was originally coded to be played using a simple controller I built connected via the parallel port. I later ported it to be playable with the keyboard. It was patched together in a couple of days, so the code is very messy (system calls everywhere).

Controls: 5 rows from left to right: d f (space) j k

Run with `-lwinmm` to use `mciSendString()`.
