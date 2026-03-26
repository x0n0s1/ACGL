#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 

#ifdef _WIN32
#include <windows.h>
#endif

#define DEFAULT_FILL ' '

typedef struct{
    int width;
    int height;
    char *data;
}Screen;

Screen* screen_create(int width, int height);
void screen_set(Screen *s,int x,int y, char c);
void screen_clear(Screen *s, char fill);
void screen_render(Screen *s);
void screen_write(Screen *s,int x,int y,char *text);
void screen_drawRect(Screen *s,int x,int y,int width,int height,bool fill,char c);
void screen_destroy(Screen *s);
void screen_terminalReset(void);
void ACGL_init (void);

static bool acgl_initialized = 0;

/*
Initialize a instance of Screen
width and height should be superior to 0

To create a screen called X_screen : 
Screen X_screen = create_screen(width,height);
*/
Screen* screen_create(int width, int height) {
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Error: invalid screen size\n");
        return NULL;
    }

    if (width > INT_MAX / height) {
        fprintf(stderr, "Error: screen size too large (overflow)\n");
        return NULL;
    }

    Screen *s = malloc(sizeof(Screen));
    if (!s) {
        fprintf(stderr, "Error: struct allocation failed\n");
        return NULL;
    }

    s->width = width;
    s->height = height;

    s->data = malloc(width * height);
    if (!s->data) {
        fprintf(stderr, "Error: buffer allocation failed\n");
        free(s);
        return NULL;
    }

    if (!acgl_initialized) {
        ACGL_init();
        acgl_initialized = true;
    }

    return s;
}

/*
Write to a given location of a Screen buffer
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
c should be a valid ASCII char

To write to a screen named X_screen :
screen_set(X_screen,x,y,ASCII char);
*/
void screen_set(Screen *s,int x,int y, char c){
    if (!s){
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if(!s->data){
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    
    if (s->width <= 0 || s->height <= 0){
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }
    if (y < 0 || y >= s->height || x < 0 || x >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }
    s->data[y * s->width + x] = c;
}

/*
Fill the whole screen with a specified character
fill should be a valid ASCII character

To fill a screen named X_screen :
screen_clear(X_screen,"char");

OR

To fill with empty space :
screen_clear(X_screen,DEFAULT_FILL);
*/
void screen_clear(Screen *s, char fill){
    if (!s) {
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if (!s->data) {
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    if (s->width <= 0 || s->height <= 0) {
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }

    int total = s->width * s->height;
    for (int i = 0; i < total; i++) {
        s->data[i] = fill;
    }
}

/*
Print the screen in the terminal

To print a screen named X_screen :
screen_render(X_screen);
*/
void screen_render(Screen *s){
     if (!s){
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if(!s->data){
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    
    if (s->width <= 0 || s->height <= 0){
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }

    for(int i=0;i<(s->width*s->height);i++){
        printf("%c",*(s->data+i));
        if(i%s->width==s->width-1) printf("\n");
    }

    printf("\n");
    fflush(stdout);
}

/*
Store a string at a given position inside a Screen instance
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
text should be a array of ASCII character

to check is a text fit inside a Screen instance you can check by doing : (the number of character of the text array + the starting x point) 
if this value exceed the Screen instance width, then the text will get cutoff 

To write a text to a screen named X_screen :
screen_write(X_screen,x,y,'your text');
*/
void screen_write(Screen *s,int x,int y,char *text){
    if (!s){
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if(!s->data){
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    
    if (s->width <= 0 || s->height <= 0){
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }

    if (y < 0 || y >= s->height || x < 0 || x >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }
    int len = strlen(text);

    for(int i = 0; i < len; i++){
        if(x + i >= s->width) break;  // stop at edge
        screen_set(s, x+i, y , text[i]);
    }
}

/*
Draw a rectangle starting in the upper left
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
height and width should be

To draw a rect in a screen called X_screen :
screen_drawRect(X_screen,x,y,width,height,[0 OR 1]);
*/
void screen_drawRect(Screen *s,int x,int y,int width,int height,bool fill,char c){
    if (!s){
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if(!s->data){
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    
    if (s->width <= 0 || s->height <= 0){
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }

    if (y < 0 || y >= s->height || x < 0 || x >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }

    for(int ry = 0; ry < height; ry++){
        for(int cx = 0; cx < width; cx++){

            int screen_y = y + ry;
            int screen_x = x + cx;

            if(screen_y >= s->height || screen_x >= s->width) continue;

            if(fill){
                screen_set(s, screen_x, screen_y, c);
            } else {
                if(ry == 0 || ry == height-1 || cx == 0 || cx == width-1){
                    screen_set(s, screen_x, screen_y, c);
                }
            }
        }
    }
}

/*
Delete a instance of Screen

To delete a screen called X_screen :
screen_destroy(X_screen);
*/
void screen_destroy(Screen *s){
        if (!s){
        fprintf(stderr, "Error: non-initialized Screen struct\n");
        return;
    }
    if(!s->data){
        fprintf(stderr, "Error: buffer allocation invalid\n");
        return;
    }
    
    if (s->width <= 0 || s->height <= 0){
        fprintf(stderr, "Error: invalid screen size\n");
        return;
    }

    free(s->data);
    free(s);
}

/*
Clear the Terminal

To use :
screen_terminalReset();
*/
void screen_terminalReset(void){
    printf("\x1b[H");
}

/*
Used to initialize the library
If the user doesn't initialize the library, it will be done at the creation of a Screen instance

To use :
ACGL_init();
*/
void ACGL_init (void){
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
    #endif

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\x1b[?25l");
}

int main(){

    //demo of boucing ball inside a box
    Screen *s = screen_create(30, 10);

    int x = 1;
    int y = 1;
    int dx = 1;
    int dy = 1;

    while(1){
        screen_terminalReset();          // clear terminal
        screen_clear(s, ' ');            // clear buffer

        // draw border
        screen_drawRect(s, 0, 0, s->width, s->height, false, '#');

        // draw moving point
        screen_set(s, x, y, 'O');

        // render
        screen_render(s);

        // update position
        x += dx;
        y += dy;

        // bounce on walls
        if(x <= 1 || x >= s->width - 2) dx = -dx;
        if(y <= 1 || y >= s->height - 2) dy = -dy;

        usleep(50000); // ~50ms delay (20 FPS)
    }

    screen_destroy(s);
    return 0;
}
