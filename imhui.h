#ifndef IMHUI_H_
#define IMHUI_H_

#include <stdlib.h>

#define VERTICES_CAPACITY 69
#define TRIANGLES_CAPACITY 69

#define IMHUI_BUTTON_SIZE vec2(100.0f, 50.0f)
#define IMHUI_BUTTON_COLOR rgba(0.0f, 0.8f, 0.0f, 1.0f)
#define IMHUI_BUTTON_COLOR_HOT rgba(0.0f, 1.0f, 0.0f, 1.0f)
#define IMHUI_BUTTON_COLOR_ACTIVE rgba(1.0f, 0.0f, 0.0f, 1.0f)
#define IMHUI_PADDING 10.0f

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
    BUTTON_LEFT = 1,
} Buttons;

typedef int ImHui_ID;

typedef struct {
    size_t width, height;

    ImHui_ID active;

    Vec2 mouse_pos;
    Buttons mouse_buttons;

    Vertex vertices[VERTICES_CAPACITY];
    size_t vertices_count;

    Triangle triangles[TRIANGLES_CAPACITY];
    size_t triangles_count;

    Vec2 last_widget_position;
} ImHui;

void imhui_mouse_down(ImHui *imhui);
void imhui_mouse_up(ImHui *imhui);
void imhui_mouse_move(ImHui *imhui, float x, float y);

void imhui_begin(ImHui *imhui);
void imhui_text(ImHui *imhui, const char *text);
bool imhui_button(ImHui *imhui, const char *text, ImHui_ID id);
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

void imhui_mouse_down(ImHui *imhui)
{
    imhui->mouse_buttons = imhui->mouse_buttons | BUTTON_LEFT;
}

void imhui_mouse_up(ImHui *imhui)
{
    imhui->mouse_buttons = imhui->mouse_buttons & (~BUTTON_LEFT);
}

void imhui_mouse_move(ImHui *imhui, float x, float y)
{
    imhui->mouse_pos = vec2(x, y);
}

void imhui_begin(ImHui *imhui)
{
    imhui->vertices_count = 0;
    imhui->triangles_count = 0;
    imhui->last_widget_position = vec2(0.0f, 0.0f);
}

void imhui_text(ImHui *imhui, const char *text)
{
    (void) imhui;
    (void) text;
}

bool imhui_button(ImHui *imhui, const char *text, ImHui_ID id)
{
    // TODO: imhui_button does not display its text
    (void) text;
    (void) id;

    const Vec2 p = imhui->last_widget_position;
    const Vec2 s = IMHUI_BUTTON_SIZE;

    imhui->last_widget_position =
        vec2(imhui->last_widget_position.x,
             imhui->last_widget_position.y + IMHUI_BUTTON_SIZE.y + IMHUI_PADDING);

    bool clicked = false;
    RGBA color = IMHUI_BUTTON_COLOR;

    if (imhui->active != id) {
        if (imhui_rect_contains(p, s, imhui->mouse_pos)) {
            if (imhui->mouse_buttons & BUTTON_LEFT) {
                if (imhui->active == 0) {
                    imhui->active = id;
                }
            }
            color = IMHUI_BUTTON_COLOR_HOT;
        }
    } else {
        color = IMHUI_BUTTON_COLOR_ACTIVE;
        if (!(imhui->mouse_buttons & BUTTON_LEFT)) {
            if (imhui_rect_contains(p, s, imhui->mouse_pos)) {
                clicked = true;
            }
            imhui->active = 0;
        }
    }

    imhui_fill_rect(
        imhui,
        p,
        IMHUI_BUTTON_SIZE,
        color);

    return clicked;
}

void imhui_end(ImHui *imhui)
{
    (void) imhui;
}

#endif // IMHUI_IMPLEMENTATION
