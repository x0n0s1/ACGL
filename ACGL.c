#include "acgl.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

//Constants and Checks
#define DEFAULT_FILL ' '
static bool acgl_initialized = false;

//Internal Helpers
static inline bool screen_valid(Screen *s) {
    return s && s->data && s->width > 0 && s->height > 0;
}

static inline void screen_put(Screen *s, int x, int y, char c) {
    if (x < 0 || y < 0 || x >= s->width || y >= s->height) return;
    s->data[y * s->width + x] = c;
}

//Init

void ACGL_init(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
#endif

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\x1b[?25l"); // hide cursor
}

//Screen Lifecycle

Screen* screen_create(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;
    if (width > INT_MAX / height) return NULL;

    Screen *s = malloc(sizeof(Screen));
    if (!s) return NULL;

    s->width = width;
    s->height = height;

    s->data = malloc(width * height);
    if (!s->data) {
        free(s);
        return NULL;
    }

    if (!acgl_initialized) {
        ACGL_init();
        acgl_initialized = true;
    }

    return s;
}

void screen_destroy(Screen *s) {
    if (!s) return;
    free(s->data);
    free(s);
}

//Core Operations

void screen_set(Screen *s, int x, int y, char c) {
    if (!screen_valid(s)) return;
    screen_put(s, x, y, c);
}

void screen_clear(Screen *s, char fill) {
    if (!screen_valid(s)) return;

    int total = s->width * s->height;
    for (int i = 0; i < total; i++) {
        s->data[i] = fill;
    }
}

void screen_render(Screen *s) {
    if (!screen_valid(s)) return;

    for (int i = 0; i < s->width * s->height; i++) {
        putchar(s->data[i]);
        if (i % s->width == s->width - 1)
            putchar('\n');
    }

    putchar('\n');
    fflush(stdout);
}

void screen_write(Screen *s, int x, int y, const char *text) {
    if (!screen_valid(s) || !text) return;

    int len = strlen(text);

    for (int i = 0; i < len && x + i < s->width; i++) {
        screen_put(s, x + i, y, text[i]);
    }
}

//Shapes

void screen_drawRect(Screen *s, int x, int y, int w, int h, bool fill, char c) {
    if (!screen_valid(s)) return;

    for (int ry = 0; ry < h; ry++) {
        for (int cx = 0; cx < w; cx++) {

            int sx = x + cx;
            int sy = y + ry;

            if (fill) {
                screen_put(s, sx, sy, c);
            } else {
                if (ry == 0 || ry == h - 1 || cx == 0 || cx == w - 1) {
                    screen_put(s, sx, sy, c);
                }
            }
        }
    }
}

void screen_drawLine(Screen *s, int x1, int y1, int x2, int y2, char c) {
    if (!screen_valid(s)) return;

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int error = dx - dy;

    while (1) {
        screen_put(s, x1, y1, c);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * error;

        if (e2 > -dy) {
            error -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            error += dx;
            y1 += sy;
        }
    }
}

void screen_drawCircle(Screen *s, int cx, int cy, int r, bool fill, char c) {
    if (!screen_valid(s) || r < 0) return;

    int x = r;
    int y = 0;
    int t1 = r / 16;

    while (x >= y) {

        if (fill) {
            for (int i = cx - x; i <= cx + x; i++) {
                screen_put(s, i, cy + y, c);
                screen_put(s, i, cy - y, c);
            }

            for (int i = cx - y; i <= cx + y; i++) {
                screen_put(s, i, cy + x, c);
                screen_put(s, i, cy - x, c);
            }
        } else {
            screen_put(s, cx + x, cy + y, c);
            screen_put(s, cx - x, cy + y, c);
            screen_put(s, cx + x, cy - y, c);
            screen_put(s, cx - x, cy - y, c);

            screen_put(s, cx + y, cy + x, c);
            screen_put(s, cx - y, cy + x, c);
            screen_put(s, cx + y, cy - x, c);
            screen_put(s, cx - y, cy - x, c);
        }

        y++;
        t1 += y;

        if (t1 - x >= 0) {
            t1 -= x;
            x--;
        }
    }
}

//Terminal utility

void screen_terminalReset(void) {
    printf("\x1b[H");
}

void screen_refreshRate(int fps) {
    if (fps <= 0) return;
    usleep(1000000 / fps);
}
