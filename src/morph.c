#include "pebble.h"

static Window *window;
static Layer *layer;

#define NUM_POINTS 19
#define NUM_CHAR 10

static GPoint NUMBERS[][NUM_POINTS] ={
    {{ 0, 10}, {10,  0}, {30,  0}, {40, 10}, {40, 40}, {30, 50}, {10, 50}, { 0, 40}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}, { 0, 10}}, // Zero
    {{ 0, 10}, {20,  0}, {20, 50}, { 0, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}}, // One
    {{ 0, 10}, {20,  0}, {40, 10}, {40, 20}, {10, 30}, { 0, 50}, {10, 50}, {20, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}}, // Two
    {{ 0, 10}, {20,  0}, {40, 10}, {20, 20}, {40, 30}, {40, 40}, {20, 50}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}, { 0, 40}}, // Three
    {{20,  0}, {10, 10}, { 0, 30}, {40, 30}, {40,  0}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}, {40, 50}}, // Four
    {{40,  0}, {10,  0}, { 0, 20}, {30, 20}, {40, 30}, {30, 50}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}, {10, 40}}, // Five
    {{30,  0}, {10,  0}, { 0, 20}, { 0, 30}, {10, 50}, {30, 50}, {40, 30}, {30, 20}, {10, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}, { 0, 20}}, // Six
    {{ 0,  0}, {40,  0}, {20, 20}, {10, 20}, {20, 20}, {30, 20}, {20, 20}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}, {20, 50}}, // Seven
    {{10,  0}, {30,  0}, {40, 10}, {40, 20}, {30, 25}, {20, 25}, {30, 25}, {40 ,30}, {40, 40}, {40, 50}, {10, 50}, { 0, 40}, { 0, 30}, {10, 25}, {20, 25}, {10, 25}, { 0, 20}, { 0, 10}, {10,  0}},  // Eight
    {{40, 20}, {30, 30}, {10, 30}, { 0, 20}, {10,  0}, {30,  0}, {40, 20}, {40 ,30}, {30, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}, {10, 50}},  // Nine
};

#define DELAY 10
#define NUM_STEPS 20.0

static GPoint currentPoints[NUM_POINTS] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}};
static float step = 1;

static int fromChar = 0;
static int toChar = 1;

static void layer_update_callback(Layer *me, GContext *ctx) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    GPoint t;
    GPoint s;

    for (unsigned int i = 0; i < NUM_POINTS; i++){
        s = NUMBERS[fromChar][i];
        t = NUMBERS[toChar][i];

        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Loop index now %d",  (int)(100 * (step/NUM_STEPS)));
        currentPoints[i].x = (s.x - ((s.x - t.x) * (step / NUM_STEPS))) * 2 + 30;
        currentPoints[i].y = (s.y - ((s.y - t.y) * (step / NUM_STEPS))) * 2 + 10;

        if (i >0){
            graphics_draw_line(ctx, currentPoints[i-1], currentPoints[i]);
        }
    }

    step++;
    if (step > NUM_STEPS){
        fromChar = toChar;
        toChar++; 
        if (toChar>NUM_CHAR-1) toChar=0;
        step = 1;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "From char: %d To char %d", fromChar, toChar);
    }
}

static void refresh(){
    layer_mark_dirty(layer);
    app_timer_register(DELAY, refresh, 0);
}

static void init() {
    window = window_create();
    window_stack_push(window, true /* Animated */);
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    layer = layer_create(bounds);
    layer_add_child(window_layer, layer);

    app_timer_register(DELAY, refresh, 0);
    layer_set_update_proc(layer, layer_update_callback);
}

static void deinit() {
    layer_destroy(layer);
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
