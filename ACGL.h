#ifndef ACGL_H
#define ACGL_H

#include <stdbool.h>

//Constants
#define DEFAULT_FILL ' '

//Data Structures
typedef struct {
    int width;
    int height;
    char *data;
} Screen;

//Initialization

/*
Used to initialize the library
If the user doesn't initialize the library, it will be done at the creation of a Screen instance

To use :
ACGL_init();
*/
void ACGL_init(void);

// Screen Management

/*
Initialize a instance of Screen
width and height should be superior to 0

To create a screen called X_screen : 
Screen X_screen = create_screen(width,height);
*/
Screen* screen_create(int width, int height);

/*
Delete a instance of Screen

To delete a screen called X_screen :
screen_destroy(X_screen);
*/
void screen_destroy(Screen *s);

//Basic Operations

/*
Write to a given location of a Screen buffer
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
c should be a valid ASCII char

To write to a screen named X_screen :
screen_set(X_screen,x,y,'char');
*/
void screen_set(Screen *s, int x, int y, char c);

/*
Fill the whole screen with a specified character
fill should be a valid ASCII character

To fill a screen named X_screen :
screen_clear(X_screen,'char');

OR

To fill with empty space :
screen_clear(X_screen,DEFAULT_FILL);
*/
void screen_clear(Screen *s, char fill);

/*
Print the screen in the terminal

To print a screen named X_screen :
screen_render(X_screen);
*/
void screen_render(Screen *s);

/*
Store a string at a given position inside a Screen instance

to check is a text fit inside a Screen instance you can check by doing : (the number of character of the text array + the starting x point) 
if this value exceed the Screen instance width, then the text will get cutoff 

To write a text to a screen named X_screen :
screen_write(X_screen,x,y,'your text');
*/
void screen_write(Screen *s, int x, int y, const char *text);

//Drawing Functions

/*
Draw a rectangle starting in the upper left

To draw a rect in a screen called X_screen :
screen_drawRect(X_screen,x,y,width,height,[true OR false],'char');
*/
void screen_drawRect(Screen *s, int x, int y, int width, int height, bool fill, char c);

/*
Draw a line between two point (x1,y1) to (x2,y2)
Use the Bresenham's line algorithm

To draw a line in a screen called X_screen :
screen_drawLine(X_screen,x1,y1,x2,y2,'char')
*/
void screen_drawLine(Screen *s, int x1, int y1, int x2, int y2, char c);

/*
Draw a circle with (cx,cy) as center of a given radius
Use the Midpoint circle algorithm (Jesko's method)

To draw a circle in a screen called X_screen :
screen_drawCricle(X_screen,cx,cy,radius,[true OR false],'char');
*/
void screen_drawCircle(Screen *s, int x, int y, int radius, bool fill, char c);

//Terminal Utilities

/*
Clear the Terminal

To use :
screen_terminalReset();
*/
void screen_terminalReset(void);

/*
put a delay given in fps

To use put it a the end of your loop
*/
void screen_refreshRate(int fps);

#endif