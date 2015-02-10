#include "pebble.h"

static Window *window;
static Layer *layer;

static GPathInfo ONE = {
    5,
    (GPoint []) {{0,10}, {10,0}, {10, 40}, {0, 40}, {20, 40}, {20,40}, {20,40}}
};

static GPoint TWO[] = {{{0,10}, {10,0}, {20, 10}, {0, 40}, {20, 40}, {20, 40}, {20, 40}}
};

static GPathInfo THREE = {
    5,
    (GPoint []) {{0,10}, {10,0}, {20, 10}, {10, 20}, {20, 30}, {10,40}, {0,30}}
};

#define NUM_POINTS 7
#define NUM_STEPS  100

static GPoint points[] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}};

static GPathInfo *target_path = &TWO;
static GPathInfo *source_path = &ONE;
static float step = NUM_STEPS;
static int shape = 0;

static void layer_update_callback(Layer *me, GContext *ctx) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    GPoint t;
    GPoint s;

    for (unsigned int i = 0; i < NUM_POINTS; i++){
        t = (*target_path).points[i];
        s = (*source_path).points[i];

        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Loop index now %d %d %d", (int)step, c.y, t.y);
        points[i].x = s.x + ((t.x - s.x) / step);
        points[i].y = s.y + ((t.y - s.y) / step);
        //c.x = t.x;
        //c.y = t.y;
    }
    for (unsigned int i = 0; i < NUM_POINTS-1; i++){
        graphics_draw_line(ctx, points[i+1], points[i]);
    }
    //APP_LOG(APP_LOG_LEVEL_DEBUG, 
    //    "Loop index now %d %d %d",
    //   (int)step, points[3].x, points[1].y);
    //graphics_draw_line(ctx, (GPoint){0,0}, (GPoint){100,100});
    step -= 1;
    if (step == 0){
        step = 100;
        source_path = target_path;
        if (shape == 0){
            shape = 1;
            target_path = &TWO;
        }if(shape == 1){
            shape = 2;
            target_path = &THREE;
        }
        else{
            shape = 0;
            target_path = &ONE;
        }
    }
}

static void refresh(){
    layer_mark_dirty(layer);
    app_timer_register(10, refresh, 0);
}

static void init() {
    window = window_create();
    window_stack_push(window, true /* Animated */);
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    layer = layer_create(bounds);
    layer_add_child(window_layer, layer);
    
    app_timer_register(10, refresh, 0);
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
