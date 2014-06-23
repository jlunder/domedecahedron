#include "domedecahedron.h"

#include <assert.h>

#include "dais_input.h"
#include "effect.h"


vector3_t ddh_vertex_coords_fix[DDH_TOTAL_VERTICES];

color_t ddh_frame_buffer[DDH_TOTAL_VERTICES];

uint64_t ddh_total_ns = 0;
uint32_t ddh_last_frames = 0;
uint32_t ddh_total_frames = 0;
uint64_t ddh_frame_fraction = 0;

uint64_t ddh_frame_convert_offset = 0;

uint8_t ddh_mode;
uint8_t ddh_submode;
bool ddh_last_button_a;
bool ddh_button_a;
bool ddh_button_a_edge;
bool ddh_last_button_b;
bool ddh_button_b;
bool ddh_button_b_edge;

uint32_t ddh_dais_proximity[4][4];

uint32_t ddh_debug_start_frame;
uint32_t ddh_debug_cursor;
uint8_t ddh_debug_last_submode;
uint32_t ddh_debug_edge_frame;

void ddh_initialize_mode_run(void);
void ddh_process_mode_run(void);
void ddh_initialize_mode_configure(void);
void ddh_process_mode_configure(void);
void ddh_initialize_mode_debug_locate(void);
void ddh_process_mode_debug_locate(void);
void ddh_initialize_mode_debug_sweep(void);
void ddh_process_mode_debug_sweep(void);
void ddh_initialize_mode_debug_color(void);
void ddh_process_mode_debug_color(void);
void ddh_initialize_mode_debug_log(void);
void ddh_process_mode_debug_log(void);

void ddh_initialize_debug_cursor(void);
void ddh_process_debug_cursor(uint32_t cursor_wrap);

uint8_t ddh_last_debug_mode = 255;

void ddh_initialize(void)
{
    di_initialize();
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        ddh_vertex_coords_fix[i].x = fix16_from_float(ddh_vertex_coords[i].x);
        ddh_vertex_coords_fix[i].y = fix16_from_float(ddh_vertex_coords[i].y);
        ddh_vertex_coords_fix[i].z = fix16_from_float(ddh_vertex_coords[i].z);
    }
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        ddh_frame_buffer[i] = color_make(255, 255, 255);
    }
}

void ddh_process(uint64_t delta_ns)
{
    static const uint64_t billion = 1000000000ULL;
    static const uint64_t tenbillion = 10000000000ULL;
    
    ddh_total_ns += delta_ns;
    
    // 30 FPS
    ddh_frame_fraction += delta_ns * DDH_FPS;
    
    if(ddh_frame_fraction > tenbillion) {
        uint32_t delta_frames = (uint32_t)(ddh_frame_fraction / billion);
        ddh_total_frames += delta_frames;
        ddh_frame_fraction -= delta_frames * billion;
    }
    else {
        while(ddh_frame_fraction > billion) {
            ++ddh_total_frames;
            ddh_frame_fraction -= billion;
        }
    }
    
    if((ddh_total_frames - ddh_last_frames) == 0) { // mod 2^32 arithmetic
        return;
    }
    
    ddh_last_frames = ddh_total_frames;
    
    if(ddh_mode != ddh_last_debug_mode) {
        ddh_last_debug_mode = ddh_mode;
        switch(ddh_mode) {
        default:
        case DDH_MODE_RUN:
            ddh_initialize_mode_run();
            break;
        case DDH_MODE_CONFIGURE:
            ddh_initialize_mode_configure();
            break;
        case DDH_MODE_DEBUG_LOCATE:
            ddh_initialize_mode_debug_locate();
            break;
        case DDH_MODE_DEBUG_SWEEP:
            ddh_initialize_mode_debug_sweep();
            break;
        case DDH_MODE_DEBUG_COLOR:
            ddh_initialize_mode_debug_color();
            break;
        case DDH_MODE_DEBUG_LOG:
            ddh_initialize_mode_debug_log();
            break;
        }
    }
    
    ddh_button_a_edge = (ddh_button_a != ddh_last_button_a);
    ddh_button_b_edge = (ddh_button_b != ddh_last_button_b);
    ddh_last_button_a = ddh_button_a;
    ddh_last_button_b = ddh_button_b;
    
    di_process();
    
    switch(ddh_mode) {
    default:
    case DDH_MODE_RUN:
        ddh_process_mode_run();
        break;
    case DDH_MODE_CONFIGURE:
        ddh_process_mode_configure();
        break;
    case DDH_MODE_DEBUG_LOCATE:
        ddh_process_mode_debug_locate();
        break;
    case DDH_MODE_DEBUG_SWEEP:
        ddh_process_mode_debug_sweep();
        break;
    case DDH_MODE_DEBUG_COLOR:
        ddh_process_mode_debug_color();
        break;
    case DDH_MODE_DEBUG_LOG:
        ddh_process_mode_debug_log();
        break;
    }
}

uint64_t ddh_ns_since(uint64_t total_ns)
{
    return ddh_total_ns - total_ns;
}

uint32_t ddh_ms_since(uint64_t total_ns)
{
    return ddh_ns_since(total_ns) / 1000000ULL;
}

uint32_t ddh_frames_since(uint32_t total_frames)
{
    return ddh_total_frames - total_frames;
}

void ddh_initialize_mode_run(void)
{
}

void ddh_process_mode_run(void)
{
    effect_process_plasma_0(ddh_frame_buffer);
}

void ddh_initialize_mode_configure(void)
{
}

void ddh_process_mode_configure(void)
{
}

void ddh_initialize_mode_debug_locate(void)
{
    ddh_debug_start_frame = ddh_total_frames;
    ddh_debug_cursor = 0;
    ddh_debug_last_submode = 255;
}

void ddh_process_mode_debug_locate(void)
{
    color_t ddh_colors[10] = {
        {{ 31,  31,  31, 0}},
        color_brown,
        color_red,
        color_orange,
        color_yellow,
        color_green,
        color_blue,
        color_purple,
        color_grey,
        color_white,
    };

    static uint32_t const cycle_frames = DDH_FPS;
    
    uint32_t frame;
    bool cursor_on = false;
    color_t cursor_color = color_make(0, 0, 0);
    uint32_t cursor_wrap;
    
    while(true) {
        frame = ddh_frames_since(ddh_debug_start_frame);
        if(frame > cycle_frames) {
            ddh_debug_start_frame += cycle_frames;
        }
        else {
            break;
        }
    }
    
    if(frame < cycle_frames * 1 / 2) {
        cursor_on = true;
        cursor_color = color_make(255, 255, 255);
    }
    
    if(ddh_submode != ddh_debug_last_submode) {
        ddh_debug_cursor = 0;
        ddh_debug_last_submode = ddh_submode;
    }
    
    switch(ddh_submode) {
    case DDH_SUBMODE_DEBUG_LOCATE_VERTICES:
        cursor_wrap = DDH_VERTICES_PER_DODECAHEDRON;
        break;
    case DDH_SUBMODE_DEBUG_LOCATE_DODECAHEDRONS:
        cursor_wrap = DDH_DODECAHEDRONS_PER_GROUP;
        break;
    case DDH_SUBMODE_DEBUG_LOCATE_FACES:
        cursor_wrap = DDH_FACES_PER_DODECAHEDRON;
        break;
    case DDH_SUBMODE_DEBUG_LOCATE_GROUPS:
        cursor_wrap = DDH_TOTAL_GROUPS;
        break;
    }
    
    if(ddh_button_a && ddh_button_a_edge) {
        ++ddh_debug_cursor;
    }
    if(ddh_button_b && ddh_button_b_edge) {
        ddh_debug_cursor += cursor_wrap - 1;
    }
    ddh_debug_cursor %= cursor_wrap;
    
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        switch(ddh_submode) {
        case DDH_SUBMODE_DEBUG_LOCATE_VERTICES:
            {
                if((ddh_light_vertex[i] == ddh_debug_cursor) && cursor_on) {
                    ddh_frame_buffer[i] = cursor_color;
                }
                else {
                    color_t c = ddh_colors[ddh_light_dodecahedron[i]];
                    ddh_frame_buffer[i] = color_make(c.r / 4, c.g / 4, c.b / 4);
                }
            }
            break;
        case DDH_SUBMODE_DEBUG_LOCATE_DODECAHEDRONS:
            {
                if((ddh_light_dodecahedron[i] == ddh_debug_cursor) && cursor_on) {
                    ddh_frame_buffer[i] = cursor_color;
                }
                else {
                    color_t c = ddh_colors[ddh_light_group[i]];
                    ddh_frame_buffer[i] = color_make(c.r / 4, c.g / 4, c.b / 4);
                }
            }
            break;
        case DDH_SUBMODE_DEBUG_LOCATE_FACES:
            {
                bool is_face = false;
                for(size_t j = 0; j < 3; ++j) {
                    if(ddh_light_faces[i][j] == ddh_debug_cursor) {
                        is_face = true;
                    }
                }
                if(is_face && cursor_on) {
                    ddh_frame_buffer[i] = cursor_color;
                }
                else {
                    color_t c = ddh_colors[ddh_light_dodecahedron[i]];
                    ddh_frame_buffer[i] = color_make(c.r / 4, c.g / 4, c.b / 4);
                }
            }
            break;
        case DDH_SUBMODE_DEBUG_LOCATE_GROUPS:
            {
                if((ddh_light_group[i] == ddh_debug_cursor) && cursor_on) {
                    ddh_frame_buffer[i] = cursor_color;
                }
                else {
                    color_t c = ddh_colors[ddh_light_group[i]];
                    ddh_frame_buffer[i] = color_make(c.r / 4, c.g / 4, c.b / 4);
                }
            }
            break;
        }
    }
}

void ddh_initialize_mode_debug_sweep(void)
{
    ddh_debug_start_frame = ddh_total_frames;
    ddh_initialize_debug_cursor();
}

void ddh_process_mode_debug_sweep(void)
{
    static uint32_t const cursor_wrap = 200;
    static uint32_t const cycle_frames = DDH_FPS * 10;
    static float const scale = 5.5f;
    uint32_t frame;
    float xmin = 0, xmax = 0;
    float ymin = 0, ymax = 0;
    float zmin = 0, zmax = 0;
    
    while(true) {
        frame = ddh_frames_since(ddh_debug_start_frame);
        if(frame > cycle_frames) {
            ddh_debug_start_frame += cycle_frames;
        }
        else {
            break;
        }
    }
    
    ddh_process_debug_cursor(cursor_wrap);
    
    switch(ddh_submode) {
    case DDH_SUBMODE_DEBUG_SWEEP_ALL:
        {
            if(frame < cycle_frames * 1 / 3) {
                float t = (float)(frame - cycle_frames * 0 / 3) / (float)(cycle_frames * 1 / 3 - cycle_frames * 0 / 3 - 1);
                xmin = t * scale - scale * 0.5f - 0.5f;
                xmax = t * scale - scale * 0.5f + 0.5f;
            }
            else if(frame < cycle_frames * 2 / 3) {
                float t = (float)(frame - cycle_frames * 1 / 3) / (float)(cycle_frames * 2 / 3 - cycle_frames * 1 / 3 - 1);
                ymin = t * scale - scale * 0.5f - 0.5f;
                ymax = t * scale - scale * 0.5f + 0.5f;
            }
            else {
                float t = (float)(frame - cycle_frames * 2 / 3) / (float)(cycle_frames * 3 / 3 - cycle_frames * 2 / 3 - 1);
                zmin = t * scale - scale * 0.5f - 1.0f - 0.5f;
                zmax = t * scale - scale * 0.5f - 1.0f + 0.5f;
            }
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_X:
        {
            float t = (float)frame / (float)(cycle_frames - 1);
            xmin = t * scale - scale * 0.5f - 0.5f;
            xmax = t * scale - scale * 0.5f + 0.5f;
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_Y:
        {
            float t = (float)frame / (float)(cycle_frames - 1);
            ymin = t * scale - scale * 0.5f - 0.5f;
            ymax = t * scale - scale * 0.5f + 0.5f;
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_Z:
        {
            float t = (float)frame / (float)(cycle_frames - 1);
            zmin = t * scale - scale * 0.5f - 1.0f - 0.5f;
            zmax = t * scale - scale * 0.5f - 1.0f + 0.5f;
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_STEP_X:
        {
            float t = (float)ddh_debug_cursor / (float)(cursor_wrap - 1);
            xmin = t * scale - scale * 0.5f - 0.5f;
            xmax = t * scale - scale * 0.5f + 0.5f;
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_STEP_Y:
        {
            float t = (float)ddh_debug_cursor / (float)(cursor_wrap - 1);
            ymin = t * scale - scale * 0.5f - 0.5f;
            ymax = t * scale - scale * 0.5f + 0.5f;
        }
        break;
    case DDH_SUBMODE_DEBUG_SWEEP_STEP_Z:
        {
            float t = (float)ddh_debug_cursor / (float)(cursor_wrap - 1);
            zmin = t * scale - scale * 0.5f - 1.0f - 0.5f;
            zmax = t * scale - scale * 0.5f - 1.0f + 0.5f;
        }
        break;
    }
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        vertex_t vertex = ddh_vertex_coords[i];
        ddh_frame_buffer[i] = color_make(
            vertex.x >= xmin && vertex.x < xmax ? 255 : 0,
            vertex.y >= ymin && vertex.y < ymax ? 255 : 0,
            vertex.z >= zmin && vertex.z < zmax ? 255 : 0);
    }
}

void ddh_initialize_mode_debug_color(void)
{
    ddh_initialize_debug_cursor();
}

void ddh_process_mode_debug_color(void)
{
    static uint32_t const cursor_wrap = 256;
    
    ddh_process_debug_cursor(cursor_wrap);
    
    if(ddh_submode == DDH_SUBMODE_DEBUG_COLOR_GRADIENT) {
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            ddh_frame_buffer[i] = color_make(
                255 * ddh_light_dodecahedron[i] /
                    (DDH_DODECAHEDRONS_PER_GROUP - 1),
                255 * ddh_light_vertex[i] /
                    (DDH_VERTICES_PER_DODECAHEDRON - 1),
                255 * ddh_light_group[i] / (DDH_TOTAL_GROUPS - 1));
        }
    }
    else {
        color_t color = color_make(0xFF, 0x00, 0xFF);
        
        switch(ddh_submode) {
        case DDH_SUBMODE_DEBUG_COLOR_WHITE:
            color = color_make(0xFF, 0xFF, 0xFF);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_RED:
            color = color_make(0xFF, 0x00, 0x00);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_GREEN:
            color = color_make(0x00, 0xFF, 0x00);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_BLUE:
            color = color_make(0x00, 0x00, 0xFF);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_STEP_WHITE:
            color = color_make(ddh_debug_cursor, ddh_debug_cursor, ddh_debug_cursor);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_STEP_RED:
            color = color_make(ddh_debug_cursor, 0x00, 0x00);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_STEP_GREEN:
            color = color_make(0x00, ddh_debug_cursor, 0x00);
            break;
        case DDH_SUBMODE_DEBUG_COLOR_STEP_BLUE:
            color = color_make(0x00, 0x00, ddh_debug_cursor);
            break;
        }
        
        for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
            ddh_frame_buffer[i] = color;
        }
    }
}

void ddh_initialize_mode_debug_log(void)
{
}

void ddh_process_mode_debug_log(void)
{
    for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
        ddh_frame_buffer[i] = color_make(0x00, 0xFF, 0xFF);
    }
}

void ddh_initialize_debug_cursor(void)
{
    ddh_debug_cursor = 0;
    ddh_debug_last_submode = 255;
    ddh_debug_edge_frame = ddh_total_frames;
}

void ddh_process_debug_cursor(uint32_t cursor_wrap)
{
    if(ddh_submode != ddh_debug_last_submode) {
        ddh_debug_cursor = 0;
        ddh_debug_last_submode = ddh_submode;
    }
    
    if(ddh_button_a && !ddh_button_b) {
        if(ddh_button_a_edge) {
            ddh_debug_edge_frame = ddh_total_frames;
        }
        if(ddh_button_a_edge ||
                ((ddh_frames_since(ddh_debug_edge_frame) >
                (DDH_FPS * 1 / 2)) &&
                (ddh_frames_since(ddh_debug_edge_frame) % 2 == 0))) {
            ++ddh_debug_cursor;
        }
    }
    if(!ddh_button_a && ddh_button_b) {
        if(ddh_button_b_edge) {
            ddh_debug_edge_frame = ddh_total_frames;
        }
        if(ddh_button_b_edge ||
                ((ddh_frames_since(ddh_debug_edge_frame) >
                (DDH_FPS * 1 / 2)) &&
                (ddh_frames_since(ddh_debug_edge_frame) % 2 == 0))) {
            ddh_debug_cursor += cursor_wrap - 1;
        }
    }
    ddh_debug_cursor %= cursor_wrap;
}


