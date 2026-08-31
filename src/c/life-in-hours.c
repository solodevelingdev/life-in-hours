#include <pebble.h>



static Window *s_window;
static TextLayer *title_text_layer;
static TextLayer *row1_text_layer;
static TextLayer *row2_text_layer;
static TextLayer *row3_text_layer;
static TextLayer *row4_text_layer;
TextLayer* row_array[4];
uint16_t selected_row=0; 

uint16_t what_hour_is_it(){
  time_t now = time(NULL);
  struct tm *tick_time = localtime(now ? &now : NULL);
  uint16_t hour = tick_time->tm_hour; // 0-23, e.g. 7 for 07:30
  return hour;
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  char* hours_taken = "x x x x x x";
  // What hour is it ?
  uint16_t hours_now = what_hour_is_it();
  // if hour > 6 row1 = x
  if(hours_now > 6){
    text_layer_set_text(row_array[0],hours_taken);
  }
  // if hour > 12 row1 && row2 = x
  if(hours_now > 12){
    text_layer_set_text(row_array[1],hours_taken);
  }
  // if hour > 18 row1 && row2 && row3 = x
  if(hours_now > 18){
    text_layer_set_text(row_array[2],hours_taken);
  }
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(selected_row < 3){
    selected_row++;
  }
  text_layer_set_font(row_array[selected_row-1],fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(row_array[selected_row],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(selected_row > 0){
    selected_row--;
  }
  text_layer_set_font(row_array[selected_row+1],fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(row_array[selected_row],fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
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
  // 
  row1_text_layer = text_layer_create(GRect(0, 40, bounds.size.w, 30));
  row2_text_layer = text_layer_create(GRect(0, 70, bounds.size.w, 30));
  row3_text_layer = text_layer_create(GRect(0, 100, bounds.size.w, 30));
  row4_text_layer = text_layer_create(GRect(0, 130, bounds.size.w, 30));
  // 
  text_layer_set_text_alignment(row1_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(row2_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(row3_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(row4_text_layer, GTextAlignmentCenter);
  //
  char* hours_not_taken = ". . . . . .";
  text_layer_set_text(row1_text_layer,hours_not_taken);
  text_layer_set_text(row2_text_layer,hours_not_taken);
  text_layer_set_text(row3_text_layer,hours_not_taken);
  text_layer_set_text(row4_text_layer,hours_not_taken);
  //
  layer_add_child(window_layer, text_layer_get_layer(row1_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(row2_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(row3_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(row4_text_layer));

  row_array[0] = row1_text_layer;
  row_array[1]= row2_text_layer;
  row_array[2]= row3_text_layer;
  row_array[3]= row4_text_layer;
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(title_text_layer);
  for(int i=0; i < 4 ; i++){
    text_layer_destroy(row_array[i]);
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
