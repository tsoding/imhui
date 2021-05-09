#ifndef IMHUI_H_
#define IMHUI_H_

#include <stdlib.h>

#define CAPACITY 69

#define VERT_A 0
#define VERT_B 1
#define VERT_C 2

typedef struct {
    float x, y;
} Vec2f;

Vec2f vec2f(float x, float y)
{
    return (Vec2f) {
        x, y
    };
}

typedef struct {
    float r, g, b, a;
} RGBA;

RGBA rgba(float r, float g, float b, float a)
{
    return (RGBA) {
        r, g, b, a
    };
}

typedef struct {
    size_t width, height;

    Vec2f positions[CAPACITY][3];
    RGBA colors[CAPACITY][3];
    size_t count;
} ImHui;

void imhui_begin(ImHui *imhui);
void imhui_text(ImHui *imhui, const char *text);
void imhui_end(ImHui *imhui);

#endif // IMHUI_H_

#ifdef IMHUI_IMPLEMENTATION

static void imhui_append_tri(ImHui *imhui,
                             Vec2f a, Vec2f b, Vec2f c,
                             RGBA ac, RGBA bc, RGBA cc)
{
    if (imhui->count < CAPACITY) {
        imhui->positions[imhui->count][VERT_A] = a;
        imhui->positions[imhui->count][VERT_B] = b;
        imhui->positions[imhui->count][VERT_C] = c;
        imhui->colors[imhui->count][VERT_A] = ac;
        imhui->colors[imhui->count][VERT_B] = bc;
        imhui->colors[imhui->count][VERT_C] = cc;
        imhui->count += 1;
    }
}

void imhui_begin(ImHui *imhui)
{
    imhui->count = 0;
}

void imhui_text(ImHui *imhui, const char *text)
{
    (void) imhui;
    (void) text;
}

void imhui_end(ImHui *imhui)
{
    const float qw = (float) imhui->width / 4.0f;
    const float qh = (float) imhui->height / 4.0f;
    imhui_append_tri(
        imhui,
        vec2f(2.0f * qw, qh),
        vec2f(qw, 3.0f * qh),
        vec2f(3.0f * qw, 3.0f * qh),
        rgba(1.0f, 0.0f, 0.0f, 1.0f),
        rgba(0.0f, 1.0f, 0.0f, 1.0f),
        rgba(0.0f, 0.0f, 1.0f, 1.0f));
}

#endif // IMHUI_IMPLEMENTATION
