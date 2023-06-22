#include "pch.h"
#include <SDL_opengl.h>
#include <SDL_video.h>
#include <iostream>
#include <algorithm>
#include "BaseGame.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

float BaseGame::SCALE{ 0.5f };

BaseGame::BaseGame(const Window& window)
    : m_Window{window}
    , m_Viewport{0,0,window.width,window.height}
    , m_Initialized{false}
    , m_pWindow{nullptr}
    , m_pContext{nullptr}
    ,m_MaxElapsedSeconds{ 0.1f }
{
    Initialize();
}

BaseGame::~BaseGame()
{
    Cleanup();
}

void LoopCallback(void* arg)
{
    static_cast<BaseGame*>(arg)->Loop();
}

void BaseGame::Initialize()
{
    // disable console close window button
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    HMENU hmenu = GetSystemMenu(hwnd, FALSE);
    EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
#endif

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_AUDIO*/) < 0)
    {
        std::cerr << "BaseGame::Initialize(), error when calling SDL_Init: " << SDL_GetError() << std::endl;
        return;
    }

    // Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create window
    m_pWindow = SDL_CreateWindow(
        m_Window.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        int(m_Window.width),
        int(m_Window.height),
        SDL_WINDOW_OPENGL);
    if (m_pWindow == nullptr)
    {
        std::cerr << "BaseGame::Initialize(), error when calling SDL_CreateWindow: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetWindowResizable(m_pWindow, SDL_TRUE);

    // Create OpenGL context 
    m_pContext = SDL_GL_CreateContext(m_pWindow);
    if (m_pContext == nullptr)
    {
        std::cerr << "BaseGame::Initialize(), error when calling SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
        return;
    }

#ifndef __EMSCRIPTEN__

    // Set the swap interval for the current OpenGL context,
    // synchronize it with the vertical retrace
    if (m_Window.isVSyncOn)
    {
        if (SDL_GL_SetSwapInterval(1) < 0)
        {
            std::cerr << "BaseGame::Initialize(), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError() << std::endl;
            return;
        }
    }
    else
    {
        SDL_GL_SetSwapInterval(0);
    }
#endif
    
    // Set the Projection matrix to the identity matrix
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();

    // Set up a two-dimensional orthographic viewing region.
    glOrtho(0, m_Window.width, 0, m_Window.height, -10, 10); // y from bottom to top

    // Set the viewport to the client window area
    // The viewport is the rectangular region of the window where the image is drawn.
    glViewport(0, 0, int(m_Window.width), int(m_Window.height));

    // Set the Modelview matrix to the identity matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Enable color blending and use alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "BaseGame::Initialize(), error when calling IMG_Init: " << IMG_GetError() << std::endl;
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "BaseGame::Initialize(), error when calling TTF_Init: " << TTF_GetError() << std::endl;
        return;
    }

    //Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "BaseGame::Initialize(), error when calling Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return;
    }

    m_Initialized = true;
}

void BaseGame::Run()
{
    if (!m_Initialized)
    {
        std::cerr << "BaseGame::Run(), BaseGame not correctly initialized, unable to run the BaseGame\n";
        std::cin.get();
        return;
    }


    // Set start time
    m_t1 = std::chrono::steady_clock::now();

    

    //The event loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(&LoopCallback, this, 0, true);
#else
    while (!m_Quit)
    {
        Loop();
    }
#endif
}

void BaseGame::Loop()
{
    SDL_Event e;
    // Poll next event from queue
    while (SDL_PollEvent(&e) != 0)
    {
        // Handle the polled event
        switch (e.type)
        {
        case SDL_QUIT:
            m_Quit = true;
            break;
        case SDL_KEYDOWN:
            this->ProcessKeyDownEvent(e.key);
            break;
        case SDL_KEYUP:
            this->ProcessKeyUpEvent(e.key);
            break;
        case SDL_MOUSEMOTION:
            e.motion.y = int(m_Window.height) - e.motion.y;
            this->ProcessMouseMotionEvent(e.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
            e.button.y = int(m_Window.height) - e.button.y;
            this->ProcessMouseDownEvent(e.button);
            break;
        case SDL_MOUSEBUTTONUP:
            e.button.y = int(m_Window.height) - e.button.y;
            this->ProcessMouseUpEvent(e.button);
            break;
        case SDL_WINDOWEVENT:
            if(e.window.event != SDL_WINDOWEVENT_RESIZED)
                break;

            int newWindowWidth{ e.window.data1 };
            int newWindowHeight{ e.window.data2 };

            float scaleX = float(newWindowWidth) / m_Window.width;
            float scaleY = float(newWindowHeight) / m_Window.height;

            // letterbox!
            float minScale = std::min(scaleX, scaleY);
            float viewportWidth{ m_Window.width * minScale };
            float viewportHeight{ m_Window.height * minScale };
            
            float viewportX{ 0 };
            if(std::fabs(viewportWidth - newWindowWidth) > 1)
                viewportX = (newWindowWidth - viewportWidth) / 2;

            float viewportY{ 0 };
            if(std::fabs(viewportHeight - newWindowHeight) > 1)
                viewportY = (newWindowHeight - viewportHeight) / 2;


            glViewport(
                viewportX, viewportY,
                viewportWidth, viewportHeight
            );

            break;
        }
    }

    if (!m_Quit)
    {
        // Get current time
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        // Calculate elapsed time
        float elapsedSeconds = std::chrono::duration<float>(t2 - m_t1).count();

        // Update current time
        m_t1 = t2;

        // Prevent jumps in time caused by break points
        elapsedSeconds = std::min(elapsedSeconds, m_MaxElapsedSeconds);

        // Call the BaseGame object 's Update function, using time in seconds (!)
        this->Update(elapsedSeconds);

        // Draw in the back buffer
        this->Draw();

        // Update screen: swap back and front buffer
        SDL_GL_SwapWindow(m_pWindow);
    }
}

void BaseGame::Cleanup()
{
    SDL_GL_DeleteContext(m_pContext);

    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;

// These aren't run to allow asan to detect symbols correctly
#ifndef USING_eASAN
    //Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
#else
    std::cout << "Not unloading SDL and its libraries because we're using AddressSanitizer." << std::endl;
#endif

    // enable console close window button
    #ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    HMENU hmenu = GetSystemMenu(hwnd, FALSE);
    EnableMenuItem(hmenu, SC_CLOSE, MF_ENABLED);
    #endif

}
