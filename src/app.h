#ifndef APP_H
#define APP_H


typedef enum {
    APP_TOUCH_DOWN,
    APP_TOUCH_UP,
    APP_TOUCH_MOVE,
    APP_TOUCH_CANCEL
} App_TouchAction;


typedef struct {
    int id;
    float x;
    float y;
    App_TouchAction action;
} TouchEvent;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Lifecycle callbacks
void app_init(int width, int height);
void app_resize(int width, int height);
void app_update(float dt);
void app_render();
void app_shutdown();


// Input callbacks
void app_on_touch(TouchEvent* event);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // APP_H
