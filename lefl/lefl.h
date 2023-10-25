/*
 * lefl.h
 */

#ifndef LEFL_H_
#define LEFL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _USE_MATH_DEFINES
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "lefl_macro.h"


    /*
     * lefl_menu.c
     */
#define LEFL_MENU_MAX_LENGTH 8

    typedef struct __lefl_menu_t
    {
        const char* *items;
        int16_t selected_index;
        uint16_t len;
        void (*menu_cb)(void* menu);
    } lefl_menu_t;

    void lefl_menu_init(lefl_menu_t* menu, const char* *items,uint8_t len,void (*cb)(void* menu));
    void lefl_menu_index_increase(lefl_menu_t* menu, int8_t delta);
    void lefl_menu_click(lefl_menu_t* menu);
    void fezui_menu_update_selection(lefl_menu_t* menu);
    void lefl_menu_draw(lefl_menu_t* menu);

    /*
     * lefl_keyboard.c
     */
#define LEFL_KEYBOARD_MAX_X 17
#define LEFL_KEYBOARD_MAX_Y 6
    typedef struct __lefl_keyboard_t
    {
        int8_t x;
        int8_t y;
        void (*keyboard_cb)(void* keyboard);
    } lefl_keyboard_t;
    extern const char *hid_usage_id_name[232];

    void lefl_keyboard_x_increase(lefl_keyboard_t* keyboard, int8_t delta);
    void lefl_keyboard_y_increase(lefl_keyboard_t* keyboard, int8_t delta);
    void lefl_keyboard_draw(lefl_keyboard_t* keyboard);

    /*
     * lefl_nav.c
     */
    typedef struct __lefl_page_t
    {
        void (*page_logic_cb)(void* page);
        void (*page_draw_cb)(void* page);
        void (*page_load_cb)(void* page);
        struct __lefl_page_t* forward;
        struct __lefl_page_t* back;
    } lefl_page_t;

    typedef struct __lefl_frame_t
    {
        lefl_page_t* *pages;
        uint8_t index;
        uint8_t len;
        void (*frame_cb)(void* frame);
    } lefl_frame_t;

    void lefl_frame_init(lefl_frame_t* frame, lefl_page_t* *data,uint8_t len);
    void lefl_frame_go_forward(lefl_frame_t* frame);
    void lefl_frame_go_back(lefl_frame_t* frame);
    void lefl_frame_go_home(lefl_frame_t* frame);
    void lefl_frame_navigate(lefl_frame_t* frame, lefl_page_t* page);
    void lefl_frame_logic(lefl_frame_t* frame);
    void lefl_frame_draw(lefl_frame_t* frame);

    typedef struct __lefl_link_frame_t
    {
        lefl_page_t* current_page;
        void (*link_frame_cb)(void* frame);
    } lefl_link_frame_t;

    void lefl_link_frame_go_home(lefl_link_frame_t* frame);
    void lefl_link_frame_go_forward(lefl_link_frame_t* frame);
    void lefl_link_frame_go_back(lefl_link_frame_t* frame);
    void lefl_link_frame_navigate(lefl_link_frame_t* frame, lefl_page_t* page);
    void lefl_link_frame_logic(lefl_link_frame_t* frame);
    void lefl_link_frame_draw(lefl_link_frame_t* frame);

    /*
     * lefl_animation.c
     */

    typedef float lefl_animation_float_t;

    typedef enum
    {
        LEFL_ANIMATION_EASE_IN,
        LEFL_ANIMATION_EASE_OUT,
        LEFL_ANIMATION_EASE_INOUT,
    } lefl_animation_mode_t;

    typedef struct __lefl_animation_base_t
    {
        uint16_t tick;
        uint16_t end;
        lefl_animation_float_t from;
        lefl_animation_float_t to;
        lefl_animation_float_t *target;
        lefl_animation_mode_t mode;
        lefl_animation_float_t parameter1;
        lefl_animation_float_t parameter2;
        lefl_animation_float_t (*easing_func)(lefl_animation_float_t f, ...);
    }lefl_animation_base_t;

    typedef struct __lefl_animation_t
    {
        lefl_animation_base_t *base;
        lefl_animation_float_t from;
        lefl_animation_float_t to;
        lefl_animation_float_t *target;
    }lefl_animation_t;

    void lefl_animation_init(lefl_animation_base_t *a,
            lefl_animation_float_t (*easing_func)(lefl_animation_float_t f, ...),
            lefl_animation_mode_t mode);
    void lefl_animation_bind(lefl_animation_base_t *a,lefl_animation_float_t *f);
    void lefl_animation_begin(lefl_animation_base_t *a);
    lefl_animation_float_t lefl_animation_tick(lefl_animation_base_t *a);
    lefl_animation_float_t lefl_animation_normalize(lefl_animation_base_t *a);

#define LEFL_ANIMATION_SPEED       (1/15.0)
    void lefl_easing_pid(float* f, float target);
    lefl_animation_float_t lefl_animation_sine_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_pow_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_quad_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_cubic_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_quart_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_quint_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_expo_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_circ_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_back_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_elastic_ease(lefl_animation_float_t f, ...);
    lefl_animation_float_t lefl_animation_bounce_ease(lefl_animation_float_t f, ...);


    /*
     * lefl_datastruct.c
     */

    typedef uint16_t lefl_array_elm_t;

    typedef struct __lefl_loop_array_t
    {
        lefl_array_elm_t *data;
        int16_t index;
        int16_t len;
    } lefl_loop_array_t;

    typedef struct __lefl_loop_array_iterator_t
    {
        lefl_loop_array_t* lefl_loop_array_t;
        int16_t index;
    } lefl_loop_array_iterator_t;

    void lefl_loop_array_init(lefl_loop_array_t *arr,lefl_array_elm_t *data,uint16_t len);
    lefl_array_elm_t lefl_loop_array_get(lefl_loop_array_t* arr, int16_t j);
    void lefl_loop_array_push_back(lefl_loop_array_t* arr, lefl_array_elm_t t);
    lefl_array_elm_t lefl_loop_array_max(lefl_loop_array_t* arr);

    typedef size_t lefl_bit_array_unit_t;
#define LEFL_BIT_ARRAY_UNIT_WIDTH (sizeof(lefl_bit_array_unit_t)*8)
    typedef struct __lefl_bit_array_t
    {
        lefl_bit_array_unit_t *data;
        uint16_t len;
    } lefl_bit_array_t;

    void lefl_bit_array_init(lefl_bit_array_t* arr, lefl_bit_array_unit_t *data, uint16_t len);
    void lefl_bit_array_set(lefl_bit_array_t* arr, int16_t n,bool b);
    void lefl_bit_array_set_or(lefl_bit_array_t* arr, int16_t n,bool b);
    bool lefl_bit_array_get(lefl_bit_array_t* arr, int16_t n);
    void lefl_bit_array_shift(lefl_bit_array_t* arr, int16_t n);
    void lefl_bit_array_copy(lefl_bit_array_t* to, int16_t x,lefl_bit_array_t* from, int16_t y, int16_t len);

    typedef uint16_t lefl_stack_elm_t;

    typedef struct __lefl_stack_t
    {
        lefl_stack_elm_t *data;
        int16_t top;
        int16_t len;
    } lefl_stack_t;

    lefl_stack_elm_t lefl_stack_get(lefl_stack_t* stack, lefl_stack_elm_t j);
    void lefl_stack_push(lefl_stack_t* stack, lefl_stack_elm_t t);
    lefl_stack_elm_t lefl_stack_pop(lefl_stack_t* stack, lefl_stack_elm_t *t);


    typedef float lefl_loop_queue_elm_t;

    typedef struct __lefl_loop_queue_t
    {
        lefl_loop_queue_elm_t *data;
        int16_t front;
        int16_t rear;
        int16_t len;
    } lefl_loop_queue_t;

    void lefl_loop_queue_init(lefl_loop_queue_t* q, lefl_loop_queue_elm_t*data, uint16_t len);
    lefl_loop_queue_elm_t lefl_loop_queue_dequeue(lefl_loop_queue_t* q);
    void lefl_loop_queue_enqueue(lefl_loop_queue_t* q, lefl_loop_queue_elm_t t);
#define lefl_loop_queue_foreach(q,i) for(uint16_t (i)=(q)->front;(i)!=(q)->rear;(i)=(i+1)%(q)->len)
    /*
     * lefl_input.c
     */
    typedef enum {
        KEY_EVENT_UP,
        KEY_EVENT_DOWN,
        EVENT_NUM
    } KEY_EVENT;
    typedef void (*lefl_key_cb_t)(void*);
    typedef struct __lefl_key_t
    {
        uint16_t id;
        bool state;
        lefl_key_cb_t key_cb[EVENT_NUM];
    } lefl_key_t;
    void lefl_key_update(lefl_key_t* key, bool state);
    void lefl_key_attach(lefl_key_t* key, KEY_EVENT e,lefl_key_cb_t cb);

    typedef enum
    {
        LEFL_KEY_DIGITAL_MODE,
        LEFL_KEY_ANALOG_NORMAL_MODE,
        LEFL_KEY_ANALOG_RAPID_MODE,
        LEFL_KEY_ANALOG_SPEED_MODE
    } lefl_key_mode_t;

    typedef struct __lefl_advanced_key_t
    {
        lefl_key_t key;
        lefl_key_mode_t mode;
        float value;
        float raw;
        float trigger_distance;
        float release_distance;
        float schmitt_parameter;
        float maximum;
        float minimum;
        float trigger_speed; 
        float release_speed;
        float upper_deadzone;
        float lower_deadzone;
        float range;
        float upper_bound;
        float lower_bound;
    } lefl_advanced_key_t;

    void lefl_advanced_key_init(lefl_advanced_key_t* key);
    void lefl_advanced_key_update(lefl_advanced_key_t* key, float value);
    void lefl_advanced_key_update_raw(lefl_advanced_key_t* key, float value);
    void lefl_advanced_key_update_state(lefl_advanced_key_t* key, bool state);
    float lefl_advanced_key_normalize(lefl_advanced_key_t* key, float value);
    void lefl_advanced_key_set_range(lefl_advanced_key_t* key, float upper, float lower);
    void lefl_advanced_key_set_deadzone(lefl_advanced_key_t* key, float upper, float lower);


    /*
     * lefl_color.c
     */
    typedef struct __lefl_color_rgb_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } lefl_color_rgb_t;

    typedef lefl_color_rgb_t lefl_color_t;

    typedef struct __lefl_color_hsv_t
    {
        uint16_t h;
        uint8_t s;
        uint8_t v;
    } lefl_color_hsv_t;

    void lefl_rgb_to_hsv(lefl_color_hsv_t *hsv, lefl_color_rgb_t *rgb);
    void lefl_hsv_to_rgb(lefl_color_rgb_t *rgb, lefl_color_hsv_t *hsv);
    void lefl_color_get_rgb(lefl_color_t*color, lefl_color_rgb_t*rgb);
    void lefl_color_set_rgb(lefl_color_t*color, lefl_color_rgb_t*rgb);
    void lefl_color_get_hsv(lefl_color_t*color, lefl_color_hsv_t*hsv);
    void lefl_color_set_hsv(lefl_color_t*color, lefl_color_hsv_t*hsv);
#ifdef __cplusplus
}
#endif

#endif /* LEFL_H_ */
