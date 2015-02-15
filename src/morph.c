#include "pebble.h"

static Window *window;
static Layer *layer;

#define NUM_POINTS 13
#define NUM_CHAR 10

static GPoint NUMBERS[][NUM_POINTS] = {
    {{ 0,  2}, { 2,  0}, { 6,  0}, { 8,  2}, { 8,  8}, { 6, 10}, { 2, 10}, { 0,  8}, { 0,  2}, { 0,  2}, { 0,  2}, { 0,  2}, { 0,  2}}, // Zero
    {{ 0,  2}, { 4,  0}, { 4, 10}, { 0, 10}, { 2, 10}, { 4, 10}, { 6, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}}, // One
    {{ 0,  2}, { 4,  0}, { 8,  2}, { 8,  4}, { 2,  6}, { 0, 10}, { 2, 10}, { 4, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}}, // Two
    {{ 0,  2}, { 4,  0}, { 8,  2}, { 4,  4}, { 8,  6}, { 8,  8}, { 4, 10}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}}, // Three
    {{ 4,  0}, { 2,  2}, { 0,  6}, { 8,  6}, { 8,  0}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}, { 8, 10}}, // Four
    {{ 6,  0}, { 1,  0}, { 0,  4}, { 6,  4}, { 8,  6}, { 6, 10}, { 2, 10}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}, { 0,  8}}, // Five
    {{ 6,  0}, { 2,  0}, { 0,  4}, { 0,  6}, { 2, 10}, { 6, 10}, { 8,  6}, { 6,  4}, { 2,  4}, { 0,  6}, { 0,  6}, { 0,  6}, { 0,  6}}, // Six
    {{ 0,  0}, { 8,  0}, { 4,  4}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}, { 4, 10}}, // Seven
    {{ 2,  0}, { 6,  0}, { 7,  3}, { 6,  4}, { 2,  4}, { 0,  7}, { 2, 10}, { 6, 10}, { 8,  7}, { 6,  4}, { 2,  4}, { 1,  3}, { 2,  0}}, // Eight
    {{ 8,  4}, { 6,  6}, { 2,  6}, { 0,  4}, { 2,  0}, { 6,  0}, { 8,  4}, { 8,  6}, { 6, 10}, { 2, 10}, { 2, 10}, { 2, 10}, { 2, 10}}, // Nine
};

#define DELAY 16
#define NUM_STEPS 32
#define DIGITS 6

static int targetDigits[] = {1,2,3,4,5,6};

static GPoint digits[DIGITS][NUM_POINTS];
static float step = 0;

static void update_digit(int digit, int targetDigit, float scale, int step, int xoffset, int yoffset){

    GPoint t;
    GPoint s;

    for (unsigned int i = 0; i < NUM_POINTS; i++){
        s = digits[digit][i];
        t = NUMBERS[targetDigit][i];

        int divisor = (NUM_STEPS - step);
        int xdelta = ((t.x * scale + xoffset) - s.x) / divisor;
        int ydelta = ((t.y * scale + yoffset) - s.y) / divisor;

        digits[digit][i].x = s.x + xdelta; 
        digits[digit][i].y = s.y + ydelta;

    }
}


static void anim_frame(){
    float scale = 5;

    update_digit( 0, targetDigits[0], scale, step, 30, 10);
    update_digit( 1, targetDigits[1], scale, step, 80, 10);

    update_digit( 2, targetDigits[2], scale, step, 30, 70);
    update_digit( 3, targetDigits[3], scale, step, 80, 70);

    update_digit( 4, targetDigits[4], scale/2, step, 50, 130);
    update_digit( 5, targetDigits[5], scale/2, step, 80, 130);
    step++;

    if (step > NUM_STEPS){
        step = 0;
    }
    else{
        layer_mark_dirty(layer);
        app_timer_register(DELAY, anim_frame, 0);
    }
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    // Update things here
    targetDigits[0] = (*tick_time).tm_hour /10;
    targetDigits[1] = (*tick_time).tm_hour %10;

    targetDigits[2] = (*tick_time).tm_min / 10;
    targetDigits[3] = (*tick_time).tm_min % 10;

    targetDigits[4] = (*tick_time).tm_sec / 10;
    targetDigits[5] = (*tick_time).tm_sec % 10;

    anim_frame();

} 


static void draw_digits(Layer *me, GContext *ctx){
    graphics_context_set_stroke_color(ctx, GColorWhite);
    for(unsigned int d= 0; d<DIGITS; d++){   
        for (unsigned int i= 1; i<NUM_POINTS; i++){
            graphics_draw_line(ctx, digits[d][i-1], digits[d][i]);
        }
    }
}


static void init() {
    window = window_create();
    window_stack_push(window, true /* Animated */);
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    layer = layer_create(bounds);
    layer_add_child(window_layer, layer);

    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

    layer_set_update_proc(layer, draw_digits);
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
