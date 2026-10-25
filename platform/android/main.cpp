#include "../../src/app.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <chrono>

#define LOG_TAG   "CppAndroidTemplate"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct AndroidEngine
{
    struct android_app* app = nullptr;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    EGLConfig  config  = nullptr;

    int32_t width  = 0;
    int32_t height = 0;

    bool initialized_app = false;
    bool animating       = false;
    bool has_focus       = true;
    bool is_paused       = false;

    std::chrono::steady_clock::time_point last_frame_time;
};

static int
init_display(AndroidEngine* engine)
{
    if (engine->display == EGL_NO_DISPLAY)
    {
        engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (engine->display == EGL_NO_DISPLAY)
        {
            LOGE("eglGetDisplay failed");
            return -1;
        }

        if (!eglInitialize(engine->display, nullptr, nullptr))
        {
            LOGE("eglInitialize failed");
            return -1;
        }

        const EGLint attribs[] = {
            EGL_SURFACE_TYPE,
            EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES3_BIT,
            EGL_BLUE_SIZE,
            8,
            EGL_GREEN_SIZE,
            8,
            EGL_RED_SIZE,
            8,
            EGL_ALPHA_SIZE,
            8,
            EGL_DEPTH_SIZE,
            24,
            EGL_NONE,
        };

        EGLint numConfigs = 0;
        if (!eglChooseConfig(engine->display, attribs, &engine->config, 1,
                             &numConfigs)
            || numConfigs <= 0)
        {
            LOGE("eglChooseConfig failed");
            return -1;
        }

        const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3,
                                         EGL_NONE};

        engine->context = eglCreateContext(engine->display, engine->config,
                                           EGL_NO_CONTEXT, contextAttribs);
        if (engine->context == EGL_NO_CONTEXT)
        {
            LOGE("eglCreateContext failed");
            return -1;
        }
    }

    EGLint format;
    eglGetConfigAttrib(engine->display, engine->config, EGL_NATIVE_VISUAL_ID,
                       &format);
    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    engine->surface = eglCreateWindowSurface(engine->display, engine->config,
                                             engine->app->window, nullptr);
    if (engine->surface == EGL_NO_SURFACE)
    {
        LOGE("eglCreateWindowSurface failed");
        return -1;
    }

    if (!eglMakeCurrent(engine->display, engine->surface, engine->surface,
                        engine->context))
    {
        LOGE("eglMakeCurrent failed");
        return -1;
    }

    eglQuerySurface(engine->display, engine->surface, EGL_WIDTH,
                    &engine->width);
    eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT,
                    &engine->height);

    if (!engine->initialized_app)
    {
        app::init(engine->width, engine->height);
        engine->initialized_app = true;
    }
    else
    {
        app::resize(engine->width, engine->height);
    }

    engine->last_frame_time = std::chrono::steady_clock::now();
    engine->animating       = !engine->is_paused && engine->has_focus;

    return 0;
}

static void
term_display(AndroidEngine* engine)
{
    if (engine->display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        if (engine->surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(engine->display, engine->surface);
            engine->surface = EGL_NO_SURFACE;
        }
    }
    engine->animating = false;
}

static void
full_cleanup(AndroidEngine* engine)
{
    term_display(engine);
    if (engine->display != EGL_NO_DISPLAY)
    {
        if (engine->context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(engine->display, engine->context);
            engine->context = EGL_NO_CONTEXT;
        }
        eglTerminate(engine->display);
        engine->display = EGL_NO_DISPLAY;
    }
    if (engine->initialized_app)
    {
        app::shutdown();
        engine->initialized_app = false;
    }
}

static void
handle_cmd(struct android_app* app, int32_t cmd)
{
    auto* engine = static_cast<AndroidEngine*>(app->userData);
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        if (app->window != nullptr)
        {
            init_display(engine);
        }
        break;
    case APP_CMD_TERM_WINDOW:
        term_display(engine);
        break;
    case APP_CMD_WINDOW_RESIZED:
        if (engine->surface != EGL_NO_SURFACE)
        {
            eglQuerySurface(engine->display, engine->surface, EGL_WIDTH,
                            &engine->width);
            eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT,
                            &engine->height);
            app::resize(engine->width, engine->height);
        }
        break;
    case APP_CMD_GAINED_FOCUS:
        engine->has_focus = true;
        engine->animating = !engine->is_paused
                         && (engine->surface != EGL_NO_SURFACE);
        break;
    case APP_CMD_LOST_FOCUS:
        engine->has_focus = false;
        engine->animating = false;
        break;
    case APP_CMD_PAUSE:
        engine->is_paused = true;
        engine->animating = false;
        break;
    case APP_CMD_RESUME:
        engine->is_paused       = false;
        engine->animating       = engine->has_focus
                               && (engine->surface != EGL_NO_SURFACE);
        engine->last_frame_time = std::chrono::steady_clock::now();
        break;
    case APP_CMD_DESTROY:
        full_cleanup(engine);
        break;
    default:
        break;
    }
}

static int32_t
handle_input(struct android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        int32_t action_raw    = AMotionEvent_getAction(event);
        int32_t action_masked = action_raw & AMOTION_EVENT_ACTION_MASK;
        size_t  pointer_index = (action_raw
                                 & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                             >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        app::TouchEvent touch{};
        touch.id = AMotionEvent_getPointerId(event, pointer_index);
        touch.x  = AMotionEvent_getX(event, pointer_index);
        touch.y  = AMotionEvent_getY(event, pointer_index);

        switch (action_masked)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            touch.action = app::TouchAction::Down;
            break;
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            touch.action = app::TouchAction::Up;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            touch.action = app::TouchAction::Move;
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            touch.action = app::TouchAction::Cancel;
            break;
        default:
            return 0;
        }

        app::on_touch(touch);
        return 1;
    }
    return 0;
}

void
android_main(struct android_app* state)
{
    LOGI("Hello from C++");

    AndroidEngine engine{};
    engine.app          = state;
    state->userData     = &engine;
    state->onAppCmd     = handle_cmd;
    state->onInputEvent = handle_input;

    while (true)
    {
        int                         ident;
        int                         events;
        struct android_poll_source* source = nullptr;

        while ((ident = ALooper_pollOnce(engine.animating ? 0 : -1, nullptr,
                                         &events, (void**)&source))
               >= 0)
        {
            if (source != nullptr)
            {
                source->process(state, source);
            }

            if (state->destroyRequested != 0)
            {
                full_cleanup(&engine);
                return;
            }
        }

        if (engine.animating && engine.surface != EGL_NO_SURFACE)
        {
            auto  current_time = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(current_time
                                                    - engine.last_frame_time)
                           .count();
            engine.last_frame_time = current_time;

            app::update(dt);
            app::render();

            if (!eglSwapBuffers(engine.display, engine.surface))
            {
                LOGE("eglSwapBuffers failed: 0x%x", eglGetError());
            }
        }
    }
}
