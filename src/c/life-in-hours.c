#include <pebble.h>
#define HOURS_IN_BLOCK 6
#define BLOCKS_IN_DAY 4

static Window *s_window;
static TextLayer *title_text_layer;
TextLayer* hours_in_day[24];
uint16_t selected_row=0; 

typedef struct{
  TextLayer* hours_in_block[HOURS_IN_BLOCK];
}HourBlock;
HourBlock* blocks_of_hours[BLOCKS_IN_DAY];

uint16_t what_hour_is_it(){
  time_t now = time(NULL);
  struct tm *tick_time = localtime(now ? &now : NULL);
  uint16_t hour = tick_time->tm_hour; // 0-23, e.g. 7 for 07:30
  return hour;
}

void hours_passed(uint16_t hours_left){
  for (size_t i = 0; i < BLOCKS_IN_DAY; i++){
    for (size_t j = 0; j < HOURS_IN_BLOCK; j++){
      if(hours_left == 0){
        return;
      }else{
        text_layer_set_text(blocks_of_hours[i]->hours_in_block[j],"x");
        hours_left--;
      }
    }
    
  }
  
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // What hour is it ?
  uint16_t hours_now = what_hour_is_it();
  hours_passed(hours_now);
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(selected_row < 3){
    selected_row++;
  }
  for(int h=0; h < HOURS_IN_BLOCK; h++){
    // text_layer_set_font(blocks[selected_row-1],fonts_get_system_font(FONT_KEY_GOTHIC_24));
    // text_layer_set_font(blocks[selected_row],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  }
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(selected_row > 0){
    selected_row--;
  }
  // text_layer_set_font(row_array[selected_row+1],fonts_get_system_font(FONT_KEY_GOTHIC_24));
  // text_layer_set_font(row_array[selected_row],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // title: Centralized text on Top of the frame
  title_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 40));
  text_layer_set_text_alignment(title_text_layer, GTextAlignmentCenter);
  text_layer_set_text(title_text_layer,"Life in Hours \U0001F603");
  text_layer_set_font(title_text_layer,fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(title_text_layer));
  // Block
  uint16_t y_increments=0;
  uint16_t x_increments=0;
  for(int i =0; i < BLOCKS_IN_DAY; i++){
    // Moves 30 spaces down and resets x to start of the row
    y_increments += 30;
    x_increments = 0;
    HourBlock* block = malloc(sizeof(HourBlock));
    for(int j =0; j < HOURS_IN_BLOCK; j++){
      block->hours_in_block[j] = text_layer_create(GRect(5+x_increments, 10+y_increments, 30, 30)); 
      x_increments += 20;
      text_layer_set_text_alignment(block->hours_in_block[j], GTextAlignmentCenter);
      text_layer_set_text(block->hours_in_block[j],".");
      layer_add_child(window_layer, text_layer_get_layer(block->hours_in_block[j]));
    }
    blocks_of_hours[i] = block;
  }
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(title_text_layer);
  for(int i=0; i < 4 ; i++){
    // text_layer_destroy(row_array[i]);
  }
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
