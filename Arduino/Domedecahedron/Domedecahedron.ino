#include "domedecahedron.h"

static uint32_t last_micros;
static uint32_t last_frame_end_micros;
static uint32_t last_frame;

static uint32_t debug_mode_debounce_micros;
static uint32_t debug_submode_debounce_micros;
static uint32_t debug_button_a_debounce_micros;
static uint32_t debug_button_b_debounce_micros;

static uint8_t debug_mode;
static uint8_t debounced_debug_mode;
static uint8_t debug_submode;
static uint8_t debounced_debug_submode;
static bool debug_button_a;
static bool debounced_debug_button_a;
static bool debug_button_b;
static bool debounced_debug_button_b;


// On the Due, pins 33-41 map to PORTC bits 1-9. This is the largest contiguous
// ordered range on the Due, so poach it.
static int const group_data_pins[DDH_TOTAL_GROUPS] = {33, 34, 35, 36, 37, 38};
static int const clock_pin = 40;

static int const debug_pins[4] = {23, 25, 27, 29};
static int const debug_mode_pins[4] = {2, 3, 4, 5};
static int const debug_submode_pins[4] = {6, 7, 8, 9};
static int const debug_button_a_pin = 10;
static int const debug_button_b_pin = 11;

static int const debounce_micros = 500;

static uint8_t intensity_map[256];

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) {
    // wait
  }
  /**/
  
  for(size_t i = 0; i < DDH_TOTAL_GROUPS; ++i) {
    pinMode(group_data_pins[i], OUTPUT);
  }
  pinMode(clock_pin, OUTPUT);
  
  for(size_t i = 0; i < 4; ++i) {
    pinMode(debug_pins[i], OUTPUT);
  }
  
  for(size_t i = 0; i < 3; ++i) {
    pinMode(debug_mode_pins[i], INPUT_PULLUP);
    pinMode(debug_submode_pins[i], INPUT_PULLUP);
  }
  
  pinMode(debug_button_a_pin, INPUT_PULLUP);
  pinMode(debug_button_b_pin, INPUT_PULLUP);
  
  /*
  Serial.print("OWSR: ");
  Serial.print(REG_PIOC_OWSR, HEX);
  Serial.print("\n");
  Serial.print("ODSR: ");
  Serial.print(REG_PIOC_ODSR, HEX);
  Serial.print("\n");
  */
  
  for(size_t i = 0; i < 256; ++i) {
    intensity_map[i] = i;
  }
  
  ddh_initialize();
  last_micros = micros();
  last_frame = ddh_total_frames;
}

void loop()
{
  uint32_t this_micros;
  uint32_t process_delta_us;
  uint32_t frame_this_micros;
  uint64_t delta_us;
  
  digitalWrite(debug_pins[0], 1);
  
  this_micros = micros();
  delta_us = (this_micros - last_micros) & 0xFFFFFFFF;
  last_micros = this_micros;
  last_frame = ddh_total_frames;
  
  digitalWrite(debug_pins[1], 1);
  
  debug_mode =
    !!digitalRead(debug_mode_pins[0]) |
    (!!digitalRead(debug_mode_pins[1]) << 1) |
    (!!digitalRead(debug_mode_pins[2]) << 2) |
    (!!digitalRead(debug_mode_pins[3]) << 2);
  debug_submode =
    !!digitalRead(debug_submode_pins[0]) |
    (!!digitalRead(debug_submode_pins[1]) << 1) |
    (!!digitalRead(debug_submode_pins[2]) << 2) |
    (!!digitalRead(debug_submode_pins[3]) << 3);
  debug_button_a = digitalRead(debug_button_a_pin);
  debug_button_b = digitalRead(debug_button_b_pin);
  
  if(debug_mode != debounced_debug_mode) {
    debug_mode_debounce_micros = debounce_micros;
    debounced_debug_mode = debug_mode;
  }
  if(debug_mode_debounce_micros < delta_us) {
    debug_mode_debounce_micros = 0;
    ddh_debug_mode = debug_mode;
  }
  else {
    debug_mode_debounce_micros -= delta_us;
  }
  
  if(debug_submode != debounced_debug_submode) {
    debug_submode_debounce_micros = debounce_micros;
    debounced_debug_submode = debug_submode;
  }
  if(debug_submode_debounce_micros < delta_us) {
    debug_submode_debounce_micros = 0;
    ddh_debug_submode = debug_submode;
  }
  else {
    debug_submode_debounce_micros -= delta_us;
  }
  
  if(debug_button_a != debounced_debug_button_a) {
    debug_button_a_debounce_micros = debounce_micros;
    debounced_debug_button_a = debug_button_a;
  }
  if(debug_button_a_debounce_micros < delta_us) {
    debug_button_a_debounce_micros = 0;
    ddh_debug_button_a = debug_button_a;
  }
  else {
    debug_button_a_debounce_micros -= delta_us;
  }
  
  if(debug_button_b != debounced_debug_button_b) {
    debug_button_b_debounce_micros = debounce_micros;
    debounced_debug_button_b = debug_button_b;
  }
  if(debug_button_b_debounce_micros < delta_us) {
    debug_button_b_debounce_micros = 0;
    ddh_debug_button_b = debug_button_b;
  }
  else {
    debug_button_b_debounce_micros -= delta_us;
  }
  
  ddh_process(delta_us * 1000ULL);
  
  digitalWrite(debug_pins[1], 0);
  
  process_delta_us = micros() - this_micros;
  
  if(last_frame == ddh_total_frames) {
    digitalWrite(debug_pins[0], 0);
    delay(1);
    return;
  }
  
  digitalWrite(debug_pins[2], 1);
  // make sure at least 500us have passed since we finished our last data emit!
  do {
    frame_this_micros = micros();
    delta_us = (this_micros - last_frame_end_micros) & 0xFFFFFFFF;
  } while(delta_us < 500);
  digitalWrite(debug_pins[2], 0);
  
  digitalWrite(debug_pins[3], 1);
  emit_frame();
  digitalWrite(debug_pins[3], 0);
  
  last_frame_end_micros = micros();
  
  digitalWrite(debug_pins[0], 0);
  
  Serial.print(ddh_frames_since(last_frame), DEC);
  Serial.print(" frames; process ");
  Serial.print(process_delta_us, DEC);
  Serial.print("us, emit ");
  Serial.print(last_frame_end_micros - frame_this_micros, DEC);
  Serial.print("us; mode=");
  Serial.print(ddh_debug_mode, DEC);
  Serial.print(", submode=");
  Serial.print(ddh_debug_submode, DEC);
  Serial.print(", a=");
  Serial.print(ddh_debug_button_a, DEC);
  Serial.print(", b=");
  Serial.print(ddh_debug_button_b, DEC);
  Serial.print("\n");
}

void emit_frame()
{
  REG_PIOC_OWDR = ~0x1FEUL;
  
  /*
  Serial.print("OWSR: ");
  Serial.print(REG_PIOC_OWSR, HEX);
  Serial.print("\n");
  Serial.print("ODSR: ");
  Serial.print(REG_PIOC_ODSR, HEX);
  Serial.print("\n");
  */
  
  color_t * vertex_base_ptr = ddh_frame_buffer;
  
  for(size_t i = 0; i < DDH_DODECAHEDRONS_PER_GROUP * DDH_VERTICES_PER_DODECAHEDRON; ++i) {
    static size_t const group_stride = DDH_DODECAHEDRONS_PER_GROUP * DDH_VERTICES_PER_DODECAHEDRON;
    
    uint32_t temp_group_data_0;
    uint32_t temp_group_data_1;
    uint32_t temp_group_data_2;
    uint32_t temp_group_data_3;
    uint32_t temp_group_data_4;
    uint32_t temp_group_data_5;
    
 #define GET_GROUP_DATA(x) \
      temp_group_data_##x = vertex_base_ptr[group_stride * x].color; \
      temp_group_data_##x = \
        (intensity_map[(temp_group_data_##x >> 0) & 0xFF] << 8) | \
        (intensity_map[(temp_group_data_##x >> 8) & 0xFF] << 16) | \
        (intensity_map[(temp_group_data_##x >> 16) & 0xFF] << 0);
    
    GET_GROUP_DATA(0);
    GET_GROUP_DATA(1);
    GET_GROUP_DATA(2);
    GET_GROUP_DATA(3);
    GET_GROUP_DATA(4);
    GET_GROUP_DATA(5);
    /*
    temp_group_data_0 = vertex_base_ptr[group_stride * 0].color;
    temp_group_data_0 = (temp_group_data_0 << 8) | ((temp_group_data_0 >> 16) & 0xFF);
    temp_group_data_1 = vertex_base_ptr[group_stride * 1].color;
    temp_group_data_1 = (temp_group_data_1 << 8) | ((temp_group_data_1 >> 16) & 0xFF);
    temp_group_data_2 = vertex_base_ptr[group_stride * 2].color;
    temp_group_data_2 = (temp_group_data_2 << 8) | ((temp_group_data_2 >> 16) & 0xFF);
    temp_group_data_3 = vertex_base_ptr[group_stride * 3].color;
    temp_group_data_3 = (temp_group_data_3 << 8) | ((temp_group_data_3 >> 16) & 0xFF);
    temp_group_data_4 = vertex_base_ptr[group_stride * 4].color;
    temp_group_data_4 = (temp_group_data_4 << 8) | ((temp_group_data_4 >> 16) & 0xFF);
    temp_group_data_5 = vertex_base_ptr[group_stride * 5].color;
    temp_group_data_5 = (temp_group_data_5 << 8) | ((temp_group_data_5 >> 16) & 0xFF);
    */
    for(size_t j = 0; j < 24; ++j) {
      uint32_t data =
        ((temp_group_data_0 & 1) << (0 + 1)) |
        ((temp_group_data_1 & 1) << (1 + 1)) |
        ((temp_group_data_2 & 1) << (2 + 1)) |
        ((temp_group_data_3 & 1) << (3 + 1)) |
        ((temp_group_data_4 & 1) << (4 + 1)) |
        ((temp_group_data_5 & 1) << (5 + 1));
      
      // PORTC bits 1-9 map to our data + clock pins
      REG_PIOC_SODR = data;
      REG_PIOC_CODR = ~data;
      
      temp_group_data_0 = temp_group_data_0 >> 1;
      temp_group_data_1 = temp_group_data_1 >> 1;
      temp_group_data_2 = temp_group_data_2 >> 1;
      temp_group_data_3 = temp_group_data_3 >> 1;
      temp_group_data_4 = temp_group_data_4 >> 1;
      temp_group_data_5 = temp_group_data_5 >> 1;
      
      delay_precise(0);
      
      REG_PIOC_SODR = data | (0x80 << 1);
    }
    
    ++vertex_base_ptr;
  }
  digitalWrite(clock_pin, 0);
  REG_PIOC_OWER = 0x7FFFFFFF;
}

uint32_t volatile delay_precise_counter;

void delay_precise(uint32_t count)
{
   delay_precise_counter = count;
   while(delay_precise_counter > 0) {
     --delay_precise_counter;
   }
}

