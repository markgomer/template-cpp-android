#include "app.h"
#include <GLES3/gl3.h>
#include <cmath>

namespace
{
int   g_width        = 0;
int   g_height       = 0;
float g_elapsed_time = 0.0f;
float g_touch_tint   = 0.0f;
} // namespace

namespace app
{

void
init(int width, int height)
{
    g_width        = width;
    g_height       = height;
    g_elapsed_time = 0.0f;
    g_touch_tint   = 0.0f;
    glViewport(0, 0, width, height);
}

void
resize(int width, int height)
{
    g_width  = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

void
update(float dt)
{
    g_elapsed_time += dt;

    // Decay touch flash back to zero
    if (g_touch_tint > 0.0f)
    {
        g_touch_tint -= dt * 2.0f;
        if (g_touch_tint < 0.0f)
        {
            g_touch_tint = 0.0f;
        }
    }
}

void
render()
{
    // Smoothly pulsating background color
    float r = 0.2f + 0.15f * std::sin(g_elapsed_time * 1.5f) + g_touch_tint;
    float g =
        0.3f + 0.20f * std::sin(g_elapsed_time * 1.2f + 1.0f) + g_touch_tint;
    float b =
        0.5f + 0.25f * std::sin(g_elapsed_time * 0.9f + 2.0f) + g_touch_tint;

    if (r > 1.0f)
        r = 1.0f;
    if (g > 1.0f)
        g = 1.0f;
    if (b > 1.0f)
        b = 1.0f;

    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
shutdown()
{
    // Release any portable graphics or simulation resources
}

void
on_touch(const TouchEvent& event)
{
    if (event.action == TouchAction::Down || event.action == TouchAction::Move)
    {
        // Flash screen slightly brighter on touch to provide visual input
        // feedback
        g_touch_tint = 0.25f;
    }
}

} // namespace app
