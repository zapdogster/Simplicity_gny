// Simplicity_gny
// Pebble example; modified by gny

#include "pebble.h"

Window 		*g_Window;
TextLayer 	*g_Text_date_layer;
TextLayer 	*g_Text_time_layer;
Layer 		*g_Line_layer;

// ================================================================
// Line layer update callback function
void line_layer_update_callback(Layer *Layer, GContext* Ctx) {
  // Set line colour
  graphics_context_set_fill_color (Ctx, GColorWhite);
  // draw a rectangle
  graphics_fill_rect (Ctx, layer_get_bounds(Layer), 0, GCornerNone);
}

// ================================================================
// Minute event handler
// This does all the work

void handle_minute_tick(struct tm *Tick_time, TimeUnits Units_changed) {
  // Need to be static because they're used by the system later.
  static char Time_text[] = "00:00";
  static char Date_text[] = "Xxxxxxxxx 00";

  char *Time_format;


  // TODO: Only update the date when it's changed.
  // Format the date string, and update the date text layer
  strftime (Date_text, sizeof(Date_text), "%B %e", Tick_time);
  text_layer_set_text (g_Text_date_layer, Date_text);


  if (clock_is_24h_style()) {
    Time_format = "%R";
  } else {
    Time_format = "%I:%M";
  }

  strftime(Time_text, sizeof(Time_text), Time_format, Tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (Time_text[0] == '0')) {
    memmove (Time_text, &Time_text[1], sizeof(Time_text) - 1);
  }

  text_layer_set_text (g_Text_time_layer, Time_text);
}



// ================================================================
void handle_init(void) {
  // Create a g_Window
  g_Window = window_create();
  window_stack_push (g_Window, true /* Animated */);		// push it to hte screen
  window_set_background_color (g_Window, GColorBlack);

  Layer *Window_layer = window_get_root_layer(g_Window);

  // Create a text layer for the date, and set it's colours, font type and size
  g_Text_date_layer = text_layer_create (GRect(8, 68, 144-8, 168-68));
  text_layer_set_text_color (g_Text_date_layer, GColorWhite);
  text_layer_set_background_color( g_Text_date_layer, GColorClear);
  text_layer_set_font (g_Text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  
  // Make the date text layer a child of the window layer
  layer_add_child (Window_layer, text_layer_get_layer(g_Text_date_layer));

  // Create a text layer for the time, and set it's colours, font type and size
  g_Text_time_layer = text_layer_create (GRect(7, 92, 144-7, 168-92)) ;
  text_layer_set_text_color (g_Text_time_layer, GColorWhite) ;
  text_layer_set_background_color (g_Text_time_layer, GColorClear) ;
  text_layer_set_font (g_Text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49))) ;

  // Make the time text layer a child of the window layer
  layer_add_child (Window_layer, text_layer_get_layer(g_Text_time_layer)) ;

  GRect Line_frame = GRect(8, 97, 139, 2) ;

  // Create a text layer for drawing a line
  g_Line_layer = layer_create(Line_frame);
  // register the function to update the line layer
  layer_set_update_proc (g_Line_layer, line_layer_update_callback) ;

  // Make the line layer a child of the window layer
  layer_add_child (Window_layer, g_Line_layer) ;

  // register the timer minute event handler function
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  // TODO: Update display here to avoid blank display on launch?
}

// ================================================================
void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  
  // according to the style guide, should be destroying all things that were created
  // these were missing in the original....
  text_layer_destroy (g_Text_date_layer) ; 
  text_layer_destroy (g_Text_time_layer) ;
  layer_destroy (g_Line_layer) ;
}

// ================================================================
// ================================================================  
int main(void) {
  handle_init();

  app_event_loop();
  
  handle_deinit();
}

// EOF
