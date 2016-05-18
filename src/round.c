#include <pebble.h>
#include "ui.h"

#define TIME_ANGLE(time) time * (TRIG_MAX_ANGLE / 60)
#define HOUR_ANGLE(hour) hour * (TRIG_MAX_ANGLE / 12)
#define STEPS_ANGLE(steps) steps * (TRIG_MAX_ANGLE / DAILY_STEPS)
#define BATTERY_ANGLE(battery) battery * (TRIG_MAX_ANGLE / 100)

// Draw an arc with given inner/outer radii
static void draw_arc(GContext *ctx, GRect rect, uint16_t thickness, uint32_t end_angle) {
  if (end_angle == 0) {
    graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle, thickness, 0, TRIG_MAX_ANGLE);
  } else {
    graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle, thickness, 0, end_angle);
  }
}

// Draw circle
static void draw_circle(GContext *ctx, GPoint point, uint16_t thickness, char *text) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Point is %d %d", point.x, point.y);
  //graphics_context_set_stroke_color(ctx, graphic_context);
  //graphics_draw_circle(ctx, point, thickness);
  graphics_fill_circle(ctx, point, thickness);
  
  // Draw text
  graphics_context_set_stroke_color(ctx, GColorWhite);
  GRect rect;
  rect.origin.x = point.x-thickness-1;
  rect.origin.y = point.y-thickness-1;
  rect.size.h = 20;
  rect.size.w = 20;
  graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), rect, GTextOverflowModeWordWrap, GTextAlignmentCenter,0);
}

static GRect calculate_rect(Layer *layer, uint8_t arc_id) {
  // Space between arcs
  uint8_t padding = 6;
  return grect_inset(layer_get_bounds(layer), GEdgeInsets(padding*(3*arc_id)));
}

static GPoint calculate_point(Layer *layer, uint8_t arc_id) {
  // Space between arcs
  uint8_t padding = 6;
  //GRect rect = layer_get_bounds(layer);
  GPoint point;
  
  point.x = layer_get_bounds(layer).size.w/2;
  point.y = grect_inset(layer_get_bounds(layer), GEdgeInsets(padding*(3*arc_id))).origin.y+padding*3;
  
  //APP_LOG(APP_LOG_LEVEL_INFO, "Rect coordinate is %d %d", rect.origin.x, rect.origin.y);
  //APP_LOG(APP_LOG_LEVEL_INFO, "Rect size is %d %d", rect.size.w, rect.size.h);
  
  return point;
}

// Handle representation for battery
void draw_battery(GContext *ctx, uint8_t battery, Layer *layer) {
  GRect rect;
  rect.origin.x = 4;
  rect.origin.y = layer_get_bounds(layer).size.h-24;
  rect.size.h = 20;
  rect.size.w = 20;
    
  draw_arc(ctx, rect, 15, BATTERY_ANGLE(battery));
//  draw_circle(ctx, calculate_point(layer,0), 7, text);
}


// Handle representation for seconds
void draw_seconds(GContext *ctx, uint16_t steps, char *text, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 0), 15, STEPS_ANGLE(steps));
  draw_circle(ctx, calculate_point(layer,0), 7, text);
}

// Handle representation for minutes
void draw_minutes(GContext *ctx, uint8_t minutes, char *text, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 1), 15, TIME_ANGLE(minutes));
  draw_circle(ctx, calculate_point(layer,1), 7, text);
}

// Handle representation for hours
void draw_hours(GContext *ctx, uint8_t hours, char *text, Layer *layer) {
  draw_arc(ctx, calculate_rect(layer, 2), 15, HOUR_ANGLE(hours));
  draw_circle(ctx, calculate_point(layer,2), 7, text);
}

//#endif
