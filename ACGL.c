#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_FILL ' '

typedef struct{
    int width;
    int height;
    char *data;
}Screen;

Screen* screen_create(int width, int height);
void screen_set(Screen *s,int row,int col, char c);
void screen_clear(Screen *s, char fill);
void render_screen(Screen *s);
void screen_write(Screen *s,int row,int col,char *text);
void screen_drawRect(Screen *s,int row,int col,int width,int height,bool fill,char c);
void screen_drawLine(Screen *s,int x1,int y1,int x2,int y2,char c);

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

    return s;
}

/*
Write to a given location of a Screen buffer
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
c should be a valid ASCII char

To write to a screen named X_screen :
screen_set(X_screen,row,col,utf-8 char);
*/
void screen_set(Screen *s,int row,int col, char c){
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
    if (row < 0 || row >= s->height || col < 0 || col >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }
    s->data[row * s->width + col] = c;
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
render_screen(X_screen);
*/
void render_screen(Screen *s){
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
}

/*
Store a string at a given position inside a Screen instance
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
text should be a array of ASCII character

to check is a text fit inside a Screen instance you can check by doing : (the number of character of the text array + the starting x point) 
if this value exceed the Screen instance width, then the text will get cutoff 

To write a text to a screen named X_screen :
screen_write(X_screen,starting row,starting col,'your text');
*/
void screen_write(Screen *s,int row,int col,char *text){
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

    if (row < 0 || row >= s->height || col < 0 || col >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }
    int len = strlen(text);

    for(int i = 0; i < len; i++){
        if(col + i >= s->width) break;  // stop at edge
        screen_set(s, row, col + i, text[i]);
    }
}

/*
Draw a rectangle starting in the upper left
row and col should be superior to 0 and smaller or egal the the maximun size of the corresponding screen axis
height and width should be

To draw a rect in a screen called X_screen :
screen_drawRect(X_screen,row,col,width,height,[0 OR 1]);
*/
void screen_drawRect(Screen *s,int row,int col,int width,int height,bool fill,char c){
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

    if (row < 0 || row >= s->height || col < 0 || col >= s->width){ 
        fprintf(stderr, "Error: invalid set location\n");
        return;
    }

    for(int r = 0; r < height; r++){
        for(int c = 0; c < width; c++){

            int screen_row = row + r;
            int screen_col = col + c;

            if(screen_row >= s->height || screen_col >= s->width) continue;

            if(fill){
                screen_set(s, screen_row, screen_col, c);
            } else {
                if(r == 0 || r == height-1 || c == 0 || c == width-1){
                    screen_set(s, screen_row, screen_col, c);
                }
            }
        }
    }
}

void screen_drawLine(Screen *s,int x1,int y1,int x2,int y2,char c){

}

int main(){
    Screen *s = screen_create(5, 3);
    screen_clear(s, '.');

    screen_set(s, 0, 0, 'A'); // top-left
    screen_set(s, 1, 2, 'B'); // middle row, 3rd column

    render_screen(s);

    free(s->data);
    free(s);
    return 0;
}