#include "app.h"
#include "math.h"
#include <GLES3/gl3.h>


int   g_width        = 0;
int   g_height       = 0;
float g_elapsed_time = 0.0f;
float g_touch_tint   = 0.0f;


void
app_init(int width, int height)
{
    g_width        = width;
    g_height       = height;
    g_elapsed_time = 0.0f;
    g_touch_tint   = 0.0f;
    glViewport(0, 0, width, height);
}

void
app_resize(int width, int height)
{
    g_width  = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

void
app_update(float dt)
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
app_render()
{
    // Smoothly pulsating background color
    float r = 0.2f + 0.15f * sin(g_elapsed_time * 1.5f) + g_touch_tint;
    float g = 0.3f + 0.20f * sin(g_elapsed_time * 1.2f + 1.0f) + g_touch_tint;
    float b = 0.5f + 0.25f * sin(g_elapsed_time * 0.9f + 2.0f) + g_touch_tint;

    if (r > 1.0f) r = 1.0f;
    if (g > 1.0f) g = 1.0f;
    if (b > 1.0f) b = 1.0f;

    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
app_shutdown()
{
    // Release any portable graphics or simulation resources
}

void
app_on_touch(TouchEvent* event)
{
    if (event->action == APP_TOUCH_DOWN || event->action == APP_TOUCH_MOVE)
    {
        // Flash screen slightly brighter on touch to provide visual input
        // feedback
        g_touch_tint = 0.25f;
    }
}
