#pragma once


namespace app {


enum class TouchAction {
    Down,
    Up,
    Move,
    Cancel
};

struct TouchEvent {
    int id;
    float x;
    float y;
    TouchAction action;
};

// Lifecycle callbacks
void init(int width, int height);
void resize(int width, int height);
void update(float dt);
void render();
void shutdown();

// Input callbacks
void on_touch(const TouchEvent& event);


} // namespace app
