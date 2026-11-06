#define GLFW_INCLUDE_ES3
#include "../../src/app.h"
#include "GLFW/glfw3.h"


static void
send_touch(GLFWwindow* w, App_TouchAction a)
{
    double x, y;
    glfwGetCursorPos(w, &x, &y);
    TouchEvent t = {0};
    t.id         = 0;
    t.x          = (float)x;
    t.y          = (float)y;
    t.action     = a;
    app_on_touch(&t);
}


static void
fb_resize_callback(GLFWwindow* win, int w, int h)
{
    (void)win;
    app_resize(w, h);
}


static void
mouse_button_callback(GLFWwindow* w, int b, int act, int mods)
{
    (void)mods;
    if (b == GLFW_MOUSE_BUTTON_LEFT)
    {
        send_touch(w, act == GLFW_PRESS ? APP_TOUCH_DOWN : APP_TOUCH_UP);
    }
};


static void
cursor_pos_callback(GLFWwindow* w, double whatever, double whenever)
{
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        send_touch(w, APP_TOUCH_MOVE);
    }
}


int
main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    GLFWwindow* win = glfwCreateWindow(1280, 720, "desktop", NULL, NULL);
    if (!win)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(win, fb_resize_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    glfwSetCursorPosCallback(win, cursor_pos_callback);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    app_init(w, h);

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        double now = glfwGetTime();
        app_update((float)(now - last));
        last = now;
        app_render();
        glfwSwapBuffers(win);
    }

    app_shutdown();
    glfwTerminate();
    return 0;
}
