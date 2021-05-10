#ifndef IMHUI_H_
#define IMHUI_H_

#include <stdlib.h>

#define VERTICES_CAPACITY 69
#define TRIANGLES_CAPACITY 69
#define EVENT_CAPACITY 69

#define IMHUI_BUTTON_SIZE vec2(100.0f, 50.0f)
#define IMHUI_BUTTON_COLOR rgba(0.0f, 1.0f, 0.0f, 1.0f)
#define IMHUI_BUTTON_COLOR_PRESSED rgba(1.0f, 0.0f, 0.0f, 1.0f)

#define VEC2_COUNT 2

typedef struct {
    float x, y;
} Vec2;

Vec2 vec2(float x, float y)
{
    return (Vec2) {
        x, y
    };
}

#define RGBA_COUNT 4

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
    Vec2 position;
    RGBA color;
} Vertex;

Vertex vertex(Vec2 position, RGBA color)
{
    return (Vertex) {
        .position = position,
        .color = color,
    };
}

#define TRIANGLE_COUNT 3

typedef struct {
    unsigned int a, b, c;
} Triangle;

Triangle triangle(unsigned int a, unsigned int b, unsigned int c)
{
    return (Triangle) {
        .a = a,
        .b = b,
        .c = c,
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

    Vertex vertices[VERTICES_CAPACITY];
    size_t vertices_count;

    Triangle triangles[TRIANGLES_CAPACITY];
    size_t triangles_count;
} ImHui;

void imhui_mouse_down(ImHui *imhui, size_t x, size_t y);
void imhui_mouse_up(ImHui *imhui, size_t x, size_t y);

void imhui_begin(ImHui *imhui);
void imhui_text(ImHui *imhui, const char *text);
void imhui_end(ImHui *imhui);

#endif // IMHUI_H_

#ifdef IMHUI_IMPLEMENTATION

static unsigned int imhui_append_vertex(ImHui *imhui, Vertex v)
{
    assert(imhui->vertices_count < VERTICES_CAPACITY);
    unsigned int result = imhui->vertices_count;
    imhui->vertices[imhui->vertices_count++] = v;
    return result;
}

static void imhui_append_triangle(ImHui *imhui, Triangle t)
{
    assert(imhui->triangles_count < TRIANGLES_CAPACITY);
    imhui->triangles[imhui->triangles_count++] = t;
}

static void imhui_fill_rect(ImHui *imhui, Vec2 p, Vec2 s, RGBA c)
{
    const unsigned int p0 = imhui_append_vertex(imhui, vertex(p, c));
    const unsigned int p1 = imhui_append_vertex(imhui, vertex(vec2(p.x + s.x, p.y), c));
    const unsigned int p2 = imhui_append_vertex(imhui, vertex(vec2(p.x, p.y + s.y), c));
    const unsigned int p3 = imhui_append_vertex(imhui, vertex(vec2(p.x + s.x, p.y + s.y), c));

    imhui_append_triangle(imhui, triangle(p0, p1, p2));
    imhui_append_triangle(imhui, triangle(p1, p2, p3));
}

static bool imhui_rect_contains(Vec2 p, Vec2 s, Vec2 t)
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
    imhui->vertices_count = 0;
    imhui->triangles_count = 0;
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

    const Vec2 p = vec2(0.0, 0.0);
    const Vec2 s = IMHUI_BUTTON_SIZE;

    for (size_t i = 0; i < imhui->events_count; ++i) {
        switch (imhui->events[i].type) {
        case IMHUI_EVENT_MOUSE_DOWN: {
            const ImHui_Event_Mouse *mouse = &imhui->events[i].mouse;
            if (imhui_rect_contains(p, s, vec2((float) mouse->x, (float) mouse->y))) {
                imhui_fill_rect(
                    imhui,
                    vec2(0.0, 0.0),
                    IMHUI_BUTTON_SIZE,
                    IMHUI_BUTTON_COLOR_PRESSED);
                return false;
            }
        }
        break;
        case IMHUI_EVENT_MOUSE_UP: {
            const ImHui_Event_Mouse *mouse = &imhui->events[i].mouse;
            if (imhui_rect_contains(p, s, vec2((float) mouse->x, (float) mouse->y))) {
                imhui_fill_rect(
                    imhui,
                    vec2(0.0, 0.0),
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
        vec2(0.0, 0.0),
        IMHUI_BUTTON_SIZE,
        IMHUI_BUTTON_COLOR);

    return false;
}

void imhui_end(ImHui *imhui)
{
    imhui->events_count = 0;
}

#endif // IMHUI_IMPLEMENTATION
