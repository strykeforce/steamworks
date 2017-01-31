#ifndef TERMINAL_ESCAPES_H
#define TERMINAL_ESCAPES_H
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

enum TColor {
  Black = 0x00,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan = 6,
  White = 7,
  Default = 9,
};
struct ScreenSize {
  int width;
  int height;
};
#define Ter_Esc_TOP_LEFT_CORNER "\u250C"
#define Ter_Esc_TOP_RIGHT_CORNER "\u2510"
#define Ter_Esc_TOP_BOTTOM_EDGE "\u2502"
#define Ter_Esc_BOTTOM_LEFT_CORNER "\u2514"
#define Ter_Esc_LEFT_RIGHT_EDGE "\u2500"
#define Ter_Esc_BOTTOM_RIGHT_CORNER "\u2518"

#define Ter_Esc_EVERYTHING_BUT_LEFT "\u251C"
#define Ter_Esc_EVERYTHING_BUT_RIGHT "\u2524"
#define Ter_Esc_EVERYTHING_BUT_UP "\u252C"
#define Ter_Esc_EVERYTHING_BUT_DOWN "\u2534"

void ClearScreen();

void MoveCursorTo(int x, int y);

void ChangeSetting(tcflag_t setting, bool enable);

void SetForgroundColor(TColor c);

void SetBackgroundColor(TColor c);

ScreenSize GetScreenSize();
#endif
