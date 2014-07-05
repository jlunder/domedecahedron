#include "domedecahedron.h"

#include "dais_input.h"


static uint32_t last_micros;
static uint32_t last_frame_end_micros;
static uint32_t last_frame;

static uint32_t mode_debounce_micros;
static uint32_t submode_debounce_micros;
static uint32_t button_a_debounce_micros;
static uint32_t button_b_debounce_micros;

static uint8_t mode;
static uint8_t debounced_mode;
static uint8_t submode;
static uint8_t debounced_submode;
static bool button_a;
static bool debounced_button_a;
static bool button_b;
static bool debounced_button_b;


// On the Due, pins 33-41 map to PORTC bits 1-9. This is the largest contiguous
// ordered range on the Due, so poach it.
static int const data_pin = 33;
static int const clock_pin = 40;

static int const debug_pins[4] = {23, 25, 27, 29};
static int const mode_pins[4] = {2, 3, 4, 5};
static int const submode_pins[4] = {6, 7, 8, 9};
static int const button_a_pin = 10;
static int const button_b_pin = 11;

static int const analog_pins[3] = {A3, A4, A5};

static int const debounce_micros = 500;

static uint8_t const intensity_map[256]  = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,
    4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,
    7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11,
   11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16,
   16, 17, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22,
   23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30,
   30, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39,
   40, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50,
   50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 62,
   62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75,
   76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
   92, 93, 94, 95, 96, 97, 98, 99,100,101,102,104,105,106,107,108,
  109,110,111,113,114,115,116,117,118,120,121,122,123,125,126,127
};

extern uint32_t ddh_debug_cursor;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) {
    // wait
  }
  /**/
  
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);
  
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  
  for(size_t i = 0; i < 4; ++i) {
    pinMode(debug_pins[i], OUTPUT);
  }
  
  for(size_t i = 0; i < 3; ++i) {
    pinMode(mode_pins[i], INPUT_PULLUP);
    pinMode(submode_pins[i], INPUT_PULLUP);
  }
  
  pinMode(button_a_pin, INPUT_PULLUP);
  pinMode(button_b_pin, INPUT_PULLUP);
  
  ddh_initialize();
  last_micros = micros();
  last_frame = ddh_total_frames;
}

void loop()
{
  uint32_t this_micros;
  uint32_t process_delta_us;
  uint32_t frame_this_micros;
  
  uint32_t analog_read_start_micros;
  uint32_t analog_read_us;
  
  uint64_t delta_us;
  
  digitalWrite(debug_pins[0], 1);
  
  this_micros = micros();
  delta_us = (this_micros - last_micros) & 0xFFFFFFFF;
  last_micros = this_micros;
  last_frame = ddh_total_frames;
  
  digitalWrite(debug_pins[1], 1);
  
  mode =
    !!digitalRead(mode_pins[0]) |
    (!!digitalRead(mode_pins[1]) << 1) |
    (!!digitalRead(mode_pins[2]) << 2) |
    (!!digitalRead(mode_pins[3]) << 3);
  submode =
    !!digitalRead(submode_pins[0]) |
    (!!digitalRead(submode_pins[1]) << 1) |
    (!!digitalRead(submode_pins[2]) << 2) |
    (!!digitalRead(submode_pins[3]) << 3);
  button_a = digitalRead(button_a_pin);
  button_b = digitalRead(button_b_pin);
  
  if(mode != debounced_mode) {
    mode_debounce_micros = debounce_micros;
    debounced_mode = mode;
  }
  if(mode_debounce_micros < delta_us) {
    mode_debounce_micros = 0;
    ddh_mode = mode;
  }
  else {
    mode_debounce_micros -= delta_us;
  }
  
  if(submode != debounced_submode) {
    submode_debounce_micros = debounce_micros;
    debounced_submode = submode;
  }
  if(submode_debounce_micros < delta_us) {
    submode_debounce_micros = 0;
    ddh_submode = submode;
  }
  else {
    submode_debounce_micros -= delta_us;
  }
  
  if(button_a != debounced_button_a) {
    button_a_debounce_micros = debounce_micros;
    debounced_button_a = button_a;
  }
  if(button_a_debounce_micros < delta_us) {
    button_a_debounce_micros = 0;
    ddh_button_a = button_a;
  }
  else {
    button_a_debounce_micros -= delta_us;
  }
  
  if(button_b != debounced_button_b) {
    button_b_debounce_micros = debounce_micros;
    debounced_button_b = button_b;
  }
  if(button_b_debounce_micros < delta_us) {
    button_b_debounce_micros = 0;
    ddh_button_b = button_b;
  }
  else {
    button_b_debounce_micros -= delta_us;
  }
  
  analog_read_start_micros = micros();
  for(size_t i = 0; i < 3; ++i) {
    ddh_dais_proximity[i] = analogRead(analog_pins[i]);
  }
  analog_read_us = micros() - analog_read_start_micros;
  
  ddh_process(delta_us * 1000ULL);
  
  digitalWrite(debug_pins[1], 0);
  
  process_delta_us = micros() - analog_read_us - analog_read_start_micros;
  
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
  Serial.print("f/p");
  Serial.print(process_delta_us, DEC);
  Serial.print("/e");
  Serial.print(last_frame_end_micros - frame_this_micros, DEC);
  
  Serial.print("\n");
}

void emit_frame()
{
  REG_PIOC_OWDR = ~0x1FEUL;
  
  color_t * vertex_base_ptr = ddh_frame_buffer;
  
  for(size_t i = 0; i < DDH_TOTAL_VERTICES; ++i) {
    uint32_t temp_data = ddh_frame_buffer[i].color;
    temp_data = \
      (intensity_map[((temp_data >> 0) & 0xFF)] << 8) |
      (intensity_map[((temp_data >> 8) & 0xFF)] << 0) |
      (intensity_map[((temp_data >> 16) & 0xFF)] << 16);
    
    for(size_t j = 0; j < 24; ++j) {
      uint32_t data = (temp_data & 0x800000) >> (22 - 0);
      
      // PORTC bits 1-9 map to our data + clock pins
      REG_PIOC_SODR = data;
      REG_PIOC_CODR = ~data;
      
      delay_precise(0);
      
      REG_PIOC_SODR = data | (0x80 << 1);
      temp_data = temp_data << 1;
    }
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

