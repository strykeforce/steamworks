#include "TerminalEscapes.h"

#include <sys/ioctl.h>

void ClearScreen()
{
	printf("\033[H\033[J");
}
void MoveCursorTo(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}
void SetForgroundColor(TColor c)
{
	printf("\033[3%cm", '0' + c);
}
void SetBackgroundColor(TColor c)
{
	printf("\033[4%cm", '0' + c);
}
/*
 *	ICANON - Makes characters feed in line by line, disable for char by char
 *	ECHO   - Makes characters print out as typed
 *
 */
void ChangeSetting(tcflag_t setting, bool enable)
{
	struct termios term;                     
	tcgetattr(STDIN_FILENO, &term);    
	if(enable)
	{         
		term.c_lflag |= setting;                  
	}
	else
	{      
		term.c_lflag &= ~setting;                 
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &term); 
}
ScreenSize GetScreenSize()
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	ScreenSize returned;
	returned.width = w.ws_col;
	returned.height = w.ws_row;
	return returned;
}
