#ifndef IMHUI_H_
#define IMHUI_H_

#include <stdlib.h>

#define CAPACITY 69
#define EVENT_CAPACITY 69

#define VERT_A 0
#define VERT_B 1
#define VERT_C 2

#define IMHUI_BUTTON_SIZE vec2f(100.0f, 50.0f)
#define IMHUI_BUTTON_COLOR rgba(0.0f, 1.0f, 0.0f, 1.0f)
#define IMHUI_BUTTON_COLOR_PRESSED rgba(1.0f, 0.0f, 0.0f, 1.0f)

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

typedef enum {
    IMHUI_EVENT_MOUSE_DOWN = 0,
    IMHUI_EVENT_MOUSE_UP,
} ImHui_Event_Type;

typedef struct {
    ImHui_Event_Type type;
    size_t x, y;
} ImHui_Event_Mouse;

typedef union {
    ImHui_Event_Type type;
    ImHui_Event_Mouse mouse;
} ImHui_Event;

typedef struct {
    size_t width, height;

    ImHui_Event events[EVENT_CAPACITY];
    size_t events_count;

    Vec2f positions[CAPACITY][3];
    RGBA colors[CAPACITY][3];
    size_t count;
} ImHui;

void imhui_mouse_down(ImHui *imhui, size_t x, size_t y);
void imhui_mouse_up(ImHui *imhui, size_t x, size_t y);

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

static void imhui_fill_rect(ImHui *imhui, Vec2f p, Vec2f s, RGBA c)
{
    imhui_append_tri(
        imhui,
        p,
        vec2f(p.x + s.x, p.y),
        vec2f(p.x + s.x, p.y + s.y),
        c, c, c);
    imhui_append_tri(
        imhui,
        p,
        vec2f(p.x + s.x, p.y + s.y),
        vec2f(p.x, p.y + s.y),
        c, c, c);
}

static bool imhui_rect_contains(Vec2f p, Vec2f s, Vec2f t)
{
    return p.x <= t.x && t.x < p.x + s.x &&
           p.y <= t.y && t.y < p.y + s.y;
}

void imhui_mouse_down(ImHui *imhui, size_t x, size_t y)
{
    if (imhui->events_count < EVENT_CAPACITY) {
        imhui->events[imhui->events_count].type = IMHUI_EVENT_MOUSE_DOWN;
        imhui->events[imhui->events_count].mouse.x = x;
        imhui->events[imhui->events_count].mouse.y = y;
        imhui->events_count += 1;
    }
}

void imhui_mouse_up(ImHui *imhui, size_t x, size_t y)
{
    if (imhui->events_count < EVENT_CAPACITY) {
        imhui->events[imhui->events_count].type = IMHUI_EVENT_MOUSE_UP;
        imhui->events[imhui->events_count].mouse.x = x;
        imhui->events[imhui->events_count].mouse.y = y;
        imhui->events_count += 1;
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

bool imhui_button(ImHui *imhui, const char *text)
{
    // TODO: imhui_button does not display its text
    (void) text;

    const Vec2f p = vec2f(0.0, 0.0);
    const Vec2f s = IMHUI_BUTTON_SIZE;

    for (size_t i = 0; i < imhui->events_count; ++i) {
        switch (imhui->events[i].type) {
        case IMHUI_EVENT_MOUSE_DOWN: {
            const ImHui_Event_Mouse *mouse = &imhui->events[i].mouse;
            if (imhui_rect_contains(p, s, vec2f((float) mouse->x, (float) mouse->y))) {
                imhui_fill_rect(
                    imhui,
                    vec2f(0.0, 0.0),
                    IMHUI_BUTTON_SIZE,
                    IMHUI_BUTTON_COLOR_PRESSED);
                return false;
            }
        }
        break;
        case IMHUI_EVENT_MOUSE_UP: {
            const ImHui_Event_Mouse *mouse = &imhui->events[i].mouse;
            if (imhui_rect_contains(p, s, vec2f((float) mouse->x, (float) mouse->y))) {
                imhui_fill_rect(
                    imhui,
                    vec2f(0.0, 0.0),
                    IMHUI_BUTTON_SIZE,
                    IMHUI_BUTTON_COLOR);
                return true;
            }
        }
        break;
        }
    }

    imhui_fill_rect(
        imhui,
        vec2f(0.0, 0.0),
        IMHUI_BUTTON_SIZE,
        IMHUI_BUTTON_COLOR);

    return false;
}

void imhui_end(ImHui *imhui)
{
    imhui->events_count = 0;
}

#endif // IMHUI_IMPLEMENTATION
