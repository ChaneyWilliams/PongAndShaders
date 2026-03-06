#include "window.h"
#include "opengl.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

i32 InitWindow(AppContext* _appContext)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_SetHint(SDL_HINT_RENDER_GPU_LOW_POWER, "0"); // prefer high perforance GPU
    SDL_SetHint(SDL_HINT_OPENGL_FORCE_SRGB_CAPABLE, "0");

    _appContext->window = (void*)SDL_CreateWindow("CPup", _appContext->windowWidth, _appContext->windowHeight, SDL_WINDOW_OPENGL);

    if (_appContext->window == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        return 1;
    }

    _appContext->glContext = (void*)SDL_GL_CreateContext((SDL_Window*)_appContext->window);

    if (_appContext->glContext == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create GL context: %s", SDL_GetError());
        return 1;
    }

    glewExperimental = true;
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GLEW Failed: %s", glewGetErrorString(error));
        return 1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA);
    glDepthFunc(GL_LESS);

    return 0;
}

void FreeWindow(AppContext* _appContext)
{
    if (_appContext->glContext)
        SDL_GL_DestroyContext((SDL_GLContext)_appContext->glContext);
    if (_appContext->window)
        SDL_DestroyWindow((SDL_Window*)_appContext->window);
    
    _appContext->glContext = NULL;
    _appContext->window = NULL;
}

void SwapWindow(AppContext* _appContext)
{
    SDL_GL_SwapWindow((SDL_Window*)_appContext->window);
}

void SetWindowTitle(AppContext* _appContext, const char* _name)
{
    SDL_SetWindowTitle((SDL_Window*)_appContext->window, _name);
}

void ClearWindow()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}