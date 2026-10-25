#define GLFW_INCLUDE_ES3
#include "../../src/app.h"
#include "GLFW/glfw3.h"


static void
send_touch(GLFWwindow* w, app::TouchAction a)
{
    double x, y;
    glfwGetCursorPos(w, &x, &y);
    app::TouchEvent t{};
    t.id     = 0;
    t.x      = (float)x;
    t.y      = (float)y;
    t.action = a;
    app::on_touch(t);
}


int
main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    GLFWwindow* win = glfwCreateWindow(1280, 720, "desktop", nullptr, nullptr);
    if (!win)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(win, [](GLFWwindow*, int w, int h)
                                   { app::resize(w, h); });
    glfwSetMouseButtonCallback(
        win,
        [](GLFWwindow* w, int b, int act, int)
        {
            if (b == GLFW_MOUSE_BUTTON_LEFT)
                send_touch(w, act == GLFW_PRESS ? app::TouchAction::Down
                                                : app::TouchAction::Up);
        });
    glfwSetCursorPosCallback(
        win,
        [](GLFWwindow* w, double, double) // lambda; callback unnanmed function
        {
            if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                send_touch(w, app::TouchAction::Move);
        });

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    app::init(w, h);

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        double now = glfwGetTime();
        app::update((float)(now - last));
        last = now;
        app::render();
        glfwSwapBuffers(win);
    }

    app::shutdown();
    glfwTerminate();
    return 0;
}
