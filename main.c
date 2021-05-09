#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define IMHUI_IMPLEMENTATION
#include "imhui.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

const char *const vert_shader_source =
    "#version 330 core\n"
    "\n"
    "uniform vec2 resolution;\n"
    "\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "\n"
    "out vec4 output_color;\n"
    "\n"
    "vec2 flip(vec2 p) {\n"
    "    return vec2(p.x, resolution.y - p.y);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4((flip(position) - resolution * 0.5) / (resolution * 0.5), 0.0, 1.0);\n"
    "    output_color = color;\n"
    "}\n"
    "\n";

const char *const frag_shader_source =
    "#version 330 core\n"
    "\n"
    "in vec4 output_color;\n"
    "out vec4 final_color;\n"
    "\n"
    "void main() {\n"
    "    final_color = output_color;\n"
    "}\n"
    "\n";

const char *shader_type_as_cstr(GLuint shader)
{
    switch (shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

bool compile_shader_source(const GLchar *source, GLenum shader_type, GLuint *shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile %s\n", shader_type_as_cstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint *program)
{
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    }

    return program;
}

typedef enum {
    IMHUI_POSITION_ATTRIB = 0,
    IMHUI_COLOR_ATTRIB,
    COUNT_IMHUI_ATTRIBS
} ImHui_Attribs;

typedef struct {
    GLuint vao;
    GLuint vbo_ids[COUNT_IMHUI_ATTRIBS];
} ImHui_GL;

void imhui_gl_begin(ImHui_GL *imhui_gl, const ImHui *imhui)
{
    glGenVertexArrays(1, &imhui_gl->vao);
    glBindVertexArray(imhui_gl->vao);

    glGenBuffers(COUNT_IMHUI_ATTRIBS, imhui_gl->vbo_ids);

    // Position
    {
        const ImHui_Attribs attrib = IMHUI_POSITION_ATTRIB;
        glBindBuffer(GL_ARRAY_BUFFER, imhui_gl->vbo_ids[attrib]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(imhui->positions),
                     NULL,
                     GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(
            attrib,             // index
            2,                  // numComponents
            GL_FLOAT,           // type
            0,                  // normalized
            0,                  // stride
            0                   // offset
        );
    }

    // Color
    {
        const ImHui_Attribs attrib = IMHUI_COLOR_ATTRIB;
        glBindBuffer(GL_ARRAY_BUFFER, imhui_gl->vbo_ids[attrib]);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(imhui->colors),
                     NULL,
                     GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(
            attrib,             // index
            4,                  // numComponents
            GL_FLOAT,           // type
            0,                  // normalized
            0,                  // stride
            0                   // offset
        );
    }
}

void imhui_gl_render(ImHui_GL *imhui_gl, const ImHui *imhui)
{
    glBindVertexArray(imhui_gl->vao);

    glBindBuffer(GL_ARRAY_BUFFER, imhui_gl->vbo_ids[IMHUI_POSITION_ATTRIB]);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        imhui->count * sizeof(imhui->positions[0]),
        imhui->positions);

    glBindBuffer(GL_ARRAY_BUFFER, imhui_gl->vbo_ids[IMHUI_COLOR_ATTRIB]);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        imhui->count * sizeof(imhui->colors[0]),
        imhui->colors);

    glDrawArrays(GL_TRIANGLES, 0, imhui->count * 3);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window;
    glViewport(
        width / 2 - DISPLAY_WIDTH / 2,
        height / 2 - DISPLAY_HEIGHT / 2,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT);
}

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * const window =
        glfwCreateWindow(DISPLAY_WIDTH,
                         DISPLAY_HEIGHT,
                         "ImHui",
                         NULL,
                         NULL);
    if (window == NULL) {
        fprintf(stderr, "ERROR: could not create a window.\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        fprintf(stderr, "ERROR: Could not initialize GLEW!\n");
        exit(1);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glfwSetFramebufferSizeCallback(window, window_size_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vert_shader = 0;
    if (!compile_shader_source(vert_shader_source, GL_VERTEX_SHADER, &vert_shader)) {
        exit(1);
    }

    GLuint frag_shader = 0;
    if (!compile_shader_source(frag_shader_source, GL_FRAGMENT_SHADER, &frag_shader)) {
        exit(1);
    }

    GLuint program = 0;
    if (!link_program(vert_shader, frag_shader, &program)) {
        exit(1);
    }
    glUseProgram(program);

    GLuint resolutionUniform = glGetUniformLocation(program, "resolution");
    glUniform2f(resolutionUniform,
                (float) DISPLAY_WIDTH,
                (float) DISPLAY_HEIGHT);

    ImHui imhui = {
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
    };
    ImHui_GL imhui_gl = {0};

    imhui_gl_begin(&imhui_gl, &imhui);

    while (!glfwWindowShouldClose(window)) {
        imhui_begin(&imhui);
        imhui_text(&imhui, "Hello, World");
        imhui_end(&imhui);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        imhui_gl_render(&imhui_gl, &imhui);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
