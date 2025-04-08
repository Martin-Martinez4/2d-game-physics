#include "Application.h"
#include "Graphics.h"
#include "Vec2.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>

bool Application::IsRunning(){
    return running;
}

void Application::Setup(){
    running = Graphics::OpenWindow();

    // setup objects in the scene
    particle = new Particle(50, 100, 1.0);
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
    particle->velocity = Vec2(2.0, 0.0);

    particle->position += particle->velocity;
}

void Application::Render(){
    Graphics::ClearScreen(0xFF056263);
    Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->mass * 4, 0xFFFFFFFF);
    // Graphics::DrawFillRect(400, 400, 100, 200, 0x99999999);
    // Graphics::DrawRect(700, 700, 100, 200, 0xFFFFFFFF);
    // Graphics::DrawPolygon(500, 500, std::vector<Vec2>{{300, 500},{440, 540},{460, 500}, {300, 500}}, 0x99999999);
    Graphics::RenderFrame();
}

void Application::Destroy(){
    // destroy all objects in the scene

    Graphics::CloseWindow();
}


