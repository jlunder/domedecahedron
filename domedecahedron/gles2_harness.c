//========================================================================
// Simple GLFW example
// Copyright (c) Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "gles2_harness.h"

#include <GLFW/glfw3.h>

#include <time.h>

///MACOS
#include <errno.h>
#include <fcntl.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
///MACOS

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "glus.h"


static GLuint g_program;
static GLuint g_vertShader;
static GLuint g_fragShader;

static GLint g_viewProjectionMatrixLocation;
static GLint g_modelMatrixLocation;
static GLint g_colorLocation;

static GLint g_vertexLocation;

static GLuint g_verticesVBO;

static GLfloat g_aspectRatio;

static float const gles2_harness_delta_v = (float)M_PI;
static float const gles2_harness_max_v = (float)M_PI / 2.0f;

bool gles2_harness_input_left = false;
bool gles2_harness_input_right = false;
bool gles2_harness_input_up = false;
bool gles2_harness_input_down = false;

float gles2_harness_horizontal_v = 0.f;
float gles2_harness_horizontal_pos = 0.f;
float gles2_harness_vertical_v = 0.f;
float gles2_harness_vertical_pos = 0.f;
float gles2_harness_dist = 5.f;

#define GLES2_HARNESS_LINE_BUF_EMPTY    1
#define GLES2_HARNESS_LINE_BUF_FULL     2
#define GLES2_HARNESS_LINE_BUF_OVERRUN  3
#define GLES2_HARNESS_LINE_BUF_FLUSHING 4

#define GLES2_HARNESS_LINE_NOT_READY 1
#define GLES2_HARNESS_LINE_READY     2
#define GLES2_HARNESS_LINE_LOST      3

size_t gles2_harness_serial_count;
int gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_EMPTY;
char gles2_harness_serial_buf[1024];
char gles2_harness_line_buf[1024];
int gles2_harness_serial_fd = -1;

float gles2_harness_fake_input_time = 0.0f;


bool gles2_harness_init(char const * dev);
int gles2_harness_serial_set_interface_attribs(int fd, int speed, int parity);
void gles2_harness_serial_set_blocking(int fd, int should_block);
void gles2_harness_init_serial(char const * dev);
void gles2_harness_process_serial(void);
int gles2_harness_read_serial(void);
void gles2_harness_reshape(int width, int height);
void gles2_harness_update(float time);
void gles2_harness_generate_motion_input(float time);
void gles2_harness_process_lookaround_input(float time);
void gles2_harness_draw_lights(float time);
void gles2_harness_terminate(void);


static void error_callback(int error, const char* description)
{
    (void)error;
    
    fputs(description, stderr);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;
    
    if(action == GLFW_PRESS) {
        switch(key) {
        case 'Q': ddh_mode = 0; break;
        case 'W': ddh_mode = 1; break;
        case 'E': ddh_mode = 2; break;
        case 'R': ddh_mode = 3; break;
        case 'T': ddh_mode = 4; break;
        case 'Y': ddh_mode = 5; break;
        case 'U': ddh_mode = 6; break;
        case 'I': ddh_mode = 7; break;
        case 'O': ddh_mode = 8; break;
        case 'P': ddh_mode = 9; break;
        
        case 'A': ddh_submode = 0; break;
        case 'S': ddh_submode = 1; break;
        case 'D': ddh_submode = 2; break;
        case 'F': ddh_submode = 3; break;
        case 'G': ddh_submode = 4; break;
        case 'H': ddh_submode = 5; break;
        case 'J': ddh_submode = 6; break;
        case 'K': ddh_submode = 7; break;
        case 'L': ddh_submode = 8; break;
        case ';': ddh_submode = 9; break;
        
        case '[': ddh_button_a = true; break;
        case ']': ddh_button_b = true; break;
        
        case GLFW_KEY_LEFT: gles2_harness_input_left = true; break;
        case GLFW_KEY_RIGHT: gles2_harness_input_right = true; break;
        case GLFW_KEY_UP: gles2_harness_input_up = true; break;
        case GLFW_KEY_DOWN: gles2_harness_input_down = true; break;
        
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }
    }
    else if(action == GLFW_RELEASE) {
        switch(key) {
        case '[': ddh_button_a = false; break;
        case ']': ddh_button_b = false; break;
        
        case GLFW_KEY_LEFT: gles2_harness_input_left = false;
        case GLFW_KEY_RIGHT: gles2_harness_input_right = false;
        case GLFW_KEY_UP: gles2_harness_input_up = false;
        case GLFW_KEY_DOWN: gles2_harness_input_down = false;
        }
    }
}


void gles2_harness_main(int argc, char * argv[])
{
    GLFWwindow* window;
    //LINUX
    //struct timespec startts;
    //clock_gettime(CLOCK_MONOTONIC, &startts);
    //LINUX
    
    //MACOS
    uint64_t lasttime = mach_absolute_time();
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    //MACOS

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(640, 480, "Domedecahedron", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    
    gles2_harness_init(argc > 1 ? argv[1] : NULL);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //LINUX
        //struct timespec ts;
        //LINUX
        uint64_t time;
        uint64_t time_ns;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        gles2_harness_reshape(width, height);
        
        //MACOS
        time = mach_absolute_time();
        time_ns = ((time - lasttime) * timebase.numer) / timebase.denom;
        lasttime = time;
        /*LINUX
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_nsec -= startts.tv_nsec;
        if(ts.tv_nsec < 0) {
            ts.tv_nsec += 1000000000;
        }
        assert(ts.tv_nsec >= 0 && ts.tv_nsec < 1000000000);
        */
        
        gles2_harness_update((float)time_ns * 1.0e-9f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    gles2_harness_terminate();

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


static char const * light_frag =
    //"precision lowp float;\n"
    "\n"
    "uniform vec4 u_color;\n"
    "\n"
    "varying vec4 v_texCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	 gl_FragColor = vec4(u_color.xyz, 1);\n"
    "}\n";

static char const * light_vert =
    "uniform mat4 u_viewProjectionMatrix;\n"
    "uniform mat4 u_modelMatrix;\n"
    "\n"
    "attribute vec4 a_vertex;\n"
    "\n"
    "varying vec4 v_color;\n"
    "varying vec4 v_texCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	gl_Position = u_viewProjectionMatrix * u_modelMatrix * a_vertex;\n"
    "	v_texCoord = a_vertex;\n"
    "}\n";


static void show_info_log(
    GLuint object,
    void (* glGet__iv)(GLuint, GLenum, GLsizei *),
    void (* glGet__InfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)
)
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

bool gles2_harness_init(char const * dev)
{
    GLchar const * vert_source = light_vert;
    GLint vert_length = strlen(light_vert);
    GLchar const * frag_source = light_frag;
    GLint frag_length = strlen(light_frag);
    GLint result;
    
    g_vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(g_vertShader, 1, &vert_source, &vert_length);
    glCompileShader(g_vertShader);
    glGetShaderiv(g_vertShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        fprintf(stderr, "Failed to compile vertex shader\n");
        show_info_log(g_vertShader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(g_vertShader);
        exit(2);
    }
    
    g_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_fragShader, 1, &frag_source, &frag_length);
    glCompileShader(g_fragShader);
    glGetShaderiv(g_fragShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        fprintf(stderr, "Failed to compile fragment shader\n");
        show_info_log(g_fragShader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(g_fragShader);
        exit(2);
    }
    
    g_program = glCreateProgram();
    glAttachShader(g_program, g_vertShader);
    glAttachShader(g_program, g_fragShader);
    glLinkProgram(g_program);
    glGetProgramiv(g_program, GL_LINK_STATUS, &result);
    if (!result) {
        fprintf(stderr, "Failed to compile link shader program\n");
        show_info_log(g_program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(g_program);
        exit(2);
    }
    
    g_viewProjectionMatrixLocation = glGetUniformLocation(g_program, "u_viewProjectionMatrix");
    g_modelMatrixLocation = glGetUniformLocation(g_program, "u_modelMatrix");
    g_colorLocation = glGetUniformLocation(g_program, "u_color");
    
    g_vertexLocation = glGetAttribLocation(g_program, "a_vertex");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // premultiplied alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);
    
    GLfloat lightVertices[] = {
        // cube
        -1.0f,  1.0f, -1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,
        
        -1.0f, -1.0f, -1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,
        
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,
         
        -1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,
        
        
        -1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,
        
        -1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,
        
         1.0f,  1.0f, -1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,
         
         1.0f, -1.0f, -1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        
        
        -1.0f, -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,
        
         1.0f, -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,
        
        -1.0f,  1.0f, -1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f,
         
        -1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,
    };
    
    glGenBuffers(1, &g_verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (lightVertices), lightVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if(dev != NULL) {
        gles2_harness_init_serial(dev);
    }
    
    eu_initialize_random(time(NULL));
    ddh_initialize();
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        size_t g = ddh_light_group[i];
        size_t d = ddh_light_dodecahedron[i];
        size_t v = ddh_light_vertex[i];
        assert(ddh_group_dodecahedron_vertex_offsets[g][d][v] == i);
    }
    
    for(size_t g = 0; g < DDH_TOTAL_GROUPS; ++g) {
        for(size_t d = 0; d < DDH_DODECAHEDRONS_PER_GROUP; ++d) {
            for(size_t v = 0; v < DDH_VERTICES_PER_DODECAHEDRON; ++v) {
                size_t i = ddh_group_dodecahedron_vertex_offsets[g][d][v];
                assert(ddh_light_group[i] == g);
                assert(ddh_light_dodecahedron[i] == d);
                assert(ddh_light_vertex[i] == v);
            }
        }
    }
    
    return true;
}


int gles2_harness_serial_set_interface_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0) {
        perror("error from tcgetattr");
        return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
        perror("error from tcsetattr");
        return -1;
    }
    return 0;
}

void gles2_harness_serial_set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0) {
        perror("error from tcgetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 0;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
        perror("error setting term attributes");
    }
}


void gles2_harness_init_serial(char const * dev)
{
    gles2_harness_serial_fd = open(dev, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    
    if(gles2_harness_serial_fd < 0) {
        perror("error opening tty");
        return;
    }
    
    // set speed to 115,200 bps, 8n1 (no parity)
    gles2_harness_serial_set_interface_attribs(gles2_harness_serial_fd,
        B115200, 0);
    
    // set no blocking
    gles2_harness_serial_set_blocking(gles2_harness_serial_fd, 0);
}


void gles2_harness_process_serial(void)
{
    int n;
    
    if(gles2_harness_serial_fd < 0) {
        return;
    }
    
    if(sizeof gles2_harness_serial_buf - gles2_harness_serial_count == 0) {
        return;
    }
    
    n = read(gles2_harness_serial_fd,
        gles2_harness_serial_buf + gles2_harness_serial_count,
        sizeof gles2_harness_serial_buf - gles2_harness_serial_count);
    if(n <= 0) {
        return;
    }
    
    gles2_harness_serial_count += n;
    assert(gles2_harness_serial_count <= sizeof gles2_harness_serial_buf);
    
    if(gles2_harness_line_buf_state == GLES2_HARNESS_LINE_BUF_EMPTY) {
        size_t eol;
        
        for(eol = 0; eol < gles2_harness_serial_count; ++eol) {
            if(gles2_harness_serial_buf[eol] == '\n') {
                break;
            }
        }
        
        if(eol < gles2_harness_serial_count) {
            assert(gles2_harness_serial_buf[eol] == '\n');
            assert(gles2_harness_serial_count >= eol + 1);
            assert(eol + 1 <= sizeof gles2_harness_line_buf);
            // copy the full line into the line buf
            memcpy(gles2_harness_line_buf, gles2_harness_serial_buf, eol);
            gles2_harness_line_buf[eol] = '\0';
            // pack the read buffer down -- +1 to skip \n
            memmove(gles2_harness_serial_buf,
                gles2_harness_serial_buf + eol + 1,
                gles2_harness_serial_count - (eol + 1));
            gles2_harness_serial_count -= eol + 1;
            gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_FULL;
        }
        else if(eol >= (sizeof gles2_harness_line_buf - 1)) {
            gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_OVERRUN;
            gles2_harness_line_buf[0] = '\0';
        }
    }
    else if(gles2_harness_line_buf_state == GLES2_HARNESS_LINE_BUF_FLUSHING) {
        size_t eol;
        
        for(eol = 0; eol < gles2_harness_serial_count; ++eol) {
            if(gles2_harness_serial_buf[eol] == '\n') {
                break;
            }
        }
        if(eol < gles2_harness_serial_count) {
            // found eol, need to preserve characters after that
            assert(gles2_harness_serial_buf[eol] == '\n');
            assert(gles2_harness_serial_count >= eol + 1);
            // pack the read buffer down -- +1 to skip \n
            memmove(gles2_harness_serial_buf,
                gles2_harness_serial_buf + eol + 1,
                gles2_harness_serial_count - (eol + 1));
            gles2_harness_serial_count -= eol + 1;
            gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_EMPTY;
        }
        else {
            // no EOL, just clear the buffer
            gles2_harness_serial_count = 0;
        }
        
        // recurse and try to fill that line buf again
        gles2_harness_process_serial();
    }
}


int gles2_harness_read_serial(void)
{
    if(gles2_harness_serial_fd < 0) {
        return GLES2_HARNESS_LINE_NOT_READY;
    }
    
    gles2_harness_process_serial();
    
    switch(gles2_harness_line_buf_state) {
    case GLES2_HARNESS_LINE_BUF_EMPTY:
    case GLES2_HARNESS_LINE_BUF_FLUSHING:
        return GLES2_HARNESS_LINE_NOT_READY;
    case GLES2_HARNESS_LINE_BUF_FULL:
        gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_EMPTY;
        return GLES2_HARNESS_LINE_READY;
    default:
    case GLES2_HARNESS_LINE_BUF_OVERRUN:
        gles2_harness_line_buf_state = GLES2_HARNESS_LINE_BUF_FLUSHING;
        return GLES2_HARNESS_LINE_LOST;
    }
}


void gles2_harness_reshape(int width, int height)
{
    // Set the viewport depending on the width and height of the window.
    glViewport(0, 0, width, height);
    
    g_aspectRatio = (GLfloat)width / (GLfloat)height;
}


void gles2_harness_update(float time)
{
    int64_t frame_nsec = (int64_t)round(time * 1.0e9);
    
    gles2_harness_generate_motion_input(time);
    
    ddh_process(frame_nsec);
    
    gles2_harness_process_lookaround_input(time);
    gles2_harness_draw_lights(time);
}


void gles2_harness_generate_motion_input(float time)
{
    if(gles2_harness_serial_fd < 0) {
        // Make up some fake input
        float x = 0.0f, y = 0.0f, z = -1.0f;
        
        gles2_harness_fake_input_time =
            fmodf(gles2_harness_fake_input_time + time / 5.0f, 10.0f);
        if(gles2_harness_fake_input_time < 1.0f) {
            float t = (gles2_harness_fake_input_time - 0.0f) *
                TWO_PI_F * 5.0f;
            x = cosf(t);
            y = sinf(t);
            //ddh_log("Generating fake input: rotate ccw (%g, %g)\n", x, y);
        }
        else if(gles2_harness_fake_input_time < 2.0f) {
            float t = (gles2_harness_fake_input_time - 1.0f) *
                TWO_PI_F * 5.0f;
            x = cosf(-t);
            y = sinf(-t);
            //ddh_log("Generating fake input: rotate cw (%g, %g)\n", x, y);
        }
        else if(gles2_harness_fake_input_time < 3.0f) {
            float t = (gles2_harness_fake_input_time - 2.0f) * 6.0f;
            x = floorf(t / 2.0f) - 1.0f;
            y = fmodf(t, 1.0f) * 2.0f - 1.0f;
            //ddh_log("Generating fake input: linear forward (%g, %g)\n", x, y);
        }
        else if(gles2_harness_fake_input_time < 4.0f) {
            float t = (gles2_harness_fake_input_time - 3.0f) * 6.0f;
            x = floorf(t / 2.0f) - 1.0f;
            y = -(fmodf(t, 1.0f) * 2.0f - 1.0f);
            //ddh_log("Generating fake input: linear back (%g, %g)\n", x, y);
        }
        else if(gles2_harness_fake_input_time < 5.0f) {
            float t = (gles2_harness_fake_input_time - 4.0f) * 3.0f;
            z = -(fmodf(t, 2.0f) - 1.0f);
            //ddh_log("Generating fake input: linear down (%g)\n", z);
        }
        else if(gles2_harness_fake_input_time < 6.0f) {
            z = 1.0f;
        }
        else if(gles2_harness_fake_input_time < 7.0f) {
            float t = (gles2_harness_fake_input_time - 6.0f) * 6.0f;
            z = fmodf(t, 2.0f) - 1.0f;
            //ddh_log("Generating fake input: linear up (%g)\n", z);
        }
        else if(gles2_harness_fake_input_time < 8.0f) {
            z = 1.0f;
        }
        else {
            z = 1.0f;
        }
        
        for(size_t i = 0; i < 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                float px = j * 2.0f / 3.0f - 1.0f;
                float py = -(i * 2.0f / 3.0f - 1.0f);
                float distsq = (px - x * 1.25) * (px - x * 1.25) +
                    (py - y * 1.25) * (py - y * 1.25);
                distsq *= 1.5f * 1.5f;
                if(distsq < 1.0f) {
                    distsq = 1.0f;
                }
                ddh_dais_proximity[i][j] = (int)((1.0f / distsq) *
                    (z * -150.0f + 650.0f));
                //ddh_log("%d,%d=%3d; ", i, j, ddh_dais_proximity[i][j]);
            }
            //ddh_log("\n");
        }
    }
    else {
        while(gles2_harness_read_serial() == GLES2_HARNESS_LINE_READY) {
            int n;
        
            n = sscanf(gles2_harness_line_buf,
                "dr:%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,",
                &ddh_dais_proximity[0][0], &ddh_dais_proximity[0][1],
                &ddh_dais_proximity[0][2], &ddh_dais_proximity[0][3],
                &ddh_dais_proximity[1][0], &ddh_dais_proximity[1][1],
                &ddh_dais_proximity[1][2], &ddh_dais_proximity[1][3],
                &ddh_dais_proximity[2][0], &ddh_dais_proximity[2][1],
                &ddh_dais_proximity[2][2], &ddh_dais_proximity[2][3],
                &ddh_dais_proximity[3][0], &ddh_dais_proximity[3][1],
                &ddh_dais_proximity[3][2], &ddh_dais_proximity[3][3]);
            // if(n == 16) ddh_log("yay! %d\n", c++);
        }
    }
}


void gles2_harness_process_lookaround_input(float time)
{
    float horizontal_a = 0.0f;
    if(gles2_harness_input_left && !gles2_harness_input_right) {
        horizontal_a = gles2_harness_delta_v;
        gles2_harness_horizontal_v += horizontal_a * time;
    }
    if(!gles2_harness_input_left && gles2_harness_input_right) {
        horizontal_a = -gles2_harness_delta_v;
        gles2_harness_horizontal_v += horizontal_a * time;
    }
    if(!gles2_harness_input_left && !gles2_harness_input_right) {
        float delta_v;
        
        if(gles2_harness_horizontal_v > 0) {
            horizontal_a = -gles2_harness_delta_v;
        }
        else {
            horizontal_a = gles2_harness_delta_v;
        }
        
        delta_v = horizontal_a * time;
        
        if(fabsf(delta_v) > fabsf(gles2_harness_horizontal_v)) {
            horizontal_a = 0.f;
            gles2_harness_horizontal_v = 0.f;
        }
        else {
            gles2_harness_horizontal_v += delta_v;
        }
    }
    if(gles2_harness_horizontal_v > gles2_harness_max_v) {
        gles2_harness_horizontal_v = gles2_harness_max_v;
        horizontal_a = 0.f;
    }
    if(gles2_harness_horizontal_v < -gles2_harness_max_v) {
        gles2_harness_horizontal_v = -gles2_harness_max_v;
        horizontal_a = 0.f;
    }
    
    float vertical_a = 0.0f;
    if(gles2_harness_input_up && !gles2_harness_input_down) {
        vertical_a = gles2_harness_delta_v * gles2_harness_dist * 0.5f;
        gles2_harness_vertical_v += vertical_a * time;
    }
    if(!gles2_harness_input_up && gles2_harness_input_down) {
        vertical_a = -gles2_harness_delta_v * gles2_harness_dist * 0.5f;
        gles2_harness_vertical_v += vertical_a * time;
    }
    if(!gles2_harness_input_up && !gles2_harness_input_down) {
        float delta_v;
        
        if(gles2_harness_vertical_v > 0) {
            vertical_a = -gles2_harness_delta_v;
        }
        else {
            vertical_a = gles2_harness_delta_v;
        }
        
        delta_v = vertical_a * time;
        
        if(fabsf(delta_v) > fabsf(gles2_harness_vertical_v)) {
            vertical_a = 0.f;
            gles2_harness_vertical_v = 0.f;
        }
        else {
            gles2_harness_vertical_v += delta_v;
        }
    }
    if(gles2_harness_vertical_v > gles2_harness_max_v) {
        gles2_harness_vertical_v = gles2_harness_max_v;
        vertical_a = 0.f;
    }
    if(gles2_harness_vertical_v < -gles2_harness_max_v) {
        gles2_harness_vertical_v = -gles2_harness_max_v;
        vertical_a = 0.f;
    }
    
    
    gles2_harness_horizontal_pos = fmodf(
        gles2_harness_horizontal_pos +
        gles2_harness_horizontal_v * time +
        0.5f * horizontal_a * time * time,
        (float)M_PI * 2.f);
    gles2_harness_vertical_pos +=
        gles2_harness_vertical_v * time +
        0.5f * vertical_a * time * time;
    if(gles2_harness_vertical_pos > 2.f) {
        gles2_harness_vertical_pos = 2.f;
        if(gles2_harness_vertical_v > 0.f) {
            gles2_harness_vertical_v = 0.f;
        }
    }
    if(gles2_harness_vertical_pos < -2.f) {
        gles2_harness_vertical_pos = -2.f;
        if(gles2_harness_vertical_v < 0.f) {
            gles2_harness_vertical_v = 0.f;
        }
    }
}


void gles2_harness_draw_lights(float time)
{
    GLfloat viewMatrix[16];
    GLfloat viewProjectionMatrix[16];
    GLfloat modelMatrix[16];
    
    GLfloat lightSize = 0.02f;
    
    UNUSED(time);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glusLookAtf(viewMatrix,
        gles2_harness_dist * cosf(gles2_harness_horizontal_pos),
            gles2_harness_dist * -sinf(gles2_harness_horizontal_pos),
            gles2_harness_vertical_pos + 2.f,
        0.0f, 0.0f, -2.0f,
        0.0f, 0.0f, 1.0f);
    glusPerspectivef(viewProjectionMatrix, 45.0f, g_aspectRatio, 0.1f, 1000.0f);
    glusMatrix4x4Multiplyf(viewProjectionMatrix, viewProjectionMatrix, viewMatrix);
    
    
    glUseProgram(g_program);
    
    
    glUniformMatrix4fv(g_viewProjectionMatrixLocation, 1, GL_FALSE, viewProjectionMatrix);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
    glVertexAttribPointer(g_vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(g_vertexLocation);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        vertex_t const * vertex = &ddh_vertex_coords[i];
        
        if((((i / 120) == 4) && ((i / 20 % 6) == 5)) ||
                (((i / 120) == 5) && ((i / 20 % 6) == 5))) {
            continue;
        }
        
        /////////
        glusMatrix4x4Identityf(modelMatrix);
        glusMatrix4x4Translatef(modelMatrix, vertex->x, vertex->y, vertex->z);
        glusMatrix4x4Scalef(modelMatrix, lightSize, lightSize, lightSize);
        glUniformMatrix4fv(g_modelMatrixLocation, 1, GL_FALSE, modelMatrix);

        glUniform4f(g_colorLocation,
            ddh_frame_buffer[i].r * (1.0f / 255.0f),
            ddh_frame_buffer[i].g * (1.0f / 255.0f),
            ddh_frame_buffer[i].b * (1.0f / 255.0f),
            0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        ////////
    }
    
    glDisableVertexAttribArray(g_vertexLocation);
    
    glUseProgram(0);
}


void gles2_harness_terminate(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDeleteBuffers(1, &g_verticesVBO);
    
    glUseProgram(0);
    
    glDeleteProgram(g_program);
    glDeleteShader(g_vertShader);
    glDeleteShader(g_fragShader);
}


void ddh_log(char const * format, ...)
{
    va_list args;
    
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

