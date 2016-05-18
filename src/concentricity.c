#include <pebble.h>
#include "ui.h"

// Define struct to store colors for each time unit
typedef struct Palette {
  GColor seconds;
  GColor minutes;
  GColor hours;
} Palette;

static Window *s_window;
static Layer *s_layer;
static Palette *s_palette;

static uint8_t s_hour;
static uint8_t s_minute;
static uint8_t s_second;

// Set the color for drawing routines
static void set_color(GContext *ctx, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_color(ctx, color);
}

// Update the watchface display
static void update_display(Layer *layer, GContext *ctx) {
  bool refresh_display = true;

#if defined(PBL_HEALTH)
  // Get an activities mask
  HealthActivityMask activities = health_service_peek_current_activities();

  // Determine which bits are set, and hence which activity is active
  if(activities & HealthActivitySleep) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is sleeping.");
    refresh_display = false;
  } else if(activities & HealthActivityRestfulSleep) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is sleeping peacefully.");
    refresh_display = false;
  } 

  // Use the step count metric
  HealthMetric metric = HealthMetricStepCount;
  // Create timestamps for midnight (the start time) and now (the end time)
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  // Check step data is available
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
  bool any_data_available = mask & HealthServiceAccessibilityMaskAvailable;
  // draw arc
  int steps = (int)health_service_sum_today(metric);
  #else
  // Health data is not available here
  bool any_data_available = false;
#endif

  
  if (refresh_display) {
    set_color(ctx, s_palette->seconds);
    draw_seconds(ctx, steps, "St" ,layer);

    set_color(ctx, s_palette->minutes);
    draw_minutes(ctx, s_minute, "M" , layer);

    set_color(ctx, s_palette->hours);
    draw_hours(ctx, s_hour % 12, "H", layer);

    static char s_battery_buffer[16];
    BatteryChargeState charge_state = battery_state_service_peek();
    if (charge_state.is_charging) {
      set_color(ctx, GColorGreen);
      draw_battery(ctx, 0, layer);  
    } else {
      if (charge_state.charge_percent <= 20 ) {
        set_color(ctx, GColorRed);
      } else {
        set_color(ctx, GColorWhite);
      }
      draw_battery(ctx, (int)charge_state.charge_percent, layer);  
    }
} else {
    // Draw one arc only
    set_color(ctx, GColorElectricBlue);
    draw_seconds(ctx, 0 ,"" , layer);
    
    set_color(ctx, GColorBlack);
    draw_minutes(ctx, 0, "" ,layer);
    draw_hours(ctx, 0, "", layer);
    draw_battery(ctx, 0, layer);  
  }
}

// Update the current time values for the watchface
static void update_time(struct tm *tick_time) {
  s_hour = tick_time->tm_hour;
  s_minute = tick_time->tm_min;
  s_second = tick_time->tm_sec;
  layer_mark_dirty(s_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void window_load(Window *window) {
  s_palette = malloc(sizeof(Palette));
  *s_palette = (Palette) {
      COLOR_FALLBACK(GColorDarkCandyAppleRed, GColorWhite),
      COLOR_FALLBACK(GColorJaegerGreen, GColorWhite),
      COLOR_FALLBACK(GColorBlueMoon, GColorWhite)
  };

  s_layer = layer_create(layer_get_bounds(window_get_root_layer(s_window)));
  layer_add_child(window_get_root_layer(s_window), s_layer);
  layer_set_update_proc(s_layer, update_display);
}

static void window_unload(Window *window) {
  free(s_palette);
  layer_destroy(s_layer);
}

static void init(void) {
  s_window = window_create();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_set_background_color(s_window, GColorBlack);
  window_stack_push(s_window, true);

  time_t start = time(NULL);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time(localtime(&start));
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
