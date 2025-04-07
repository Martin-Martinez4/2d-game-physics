#include "Application.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>

bool Application::IsRunning(){
    return running;
}

void Application::Setup(){
    running = Graphics::OpenWindow();

    // setup objects in the scene
}

void Application::Input(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    running = false;
                }
                break;
        }
    }
}

void Application::Update(){
    return;
}

void Application::Render(){
    Graphics::ClearScreen(0xFF056263);
    Graphics::DrawFillCircle(200, 200, 40, 0xFFFFFFFF);
    Graphics::RenderFrame();
}

void Application::Destroy(){
    // destroy all objects in the scene

    Graphics::CloseWindow();
}


