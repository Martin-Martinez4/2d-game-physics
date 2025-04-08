#include "Application.h"
#include "Graphics.h"
#include "Vec2.h"
#include "Constants.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>

bool Application::IsRunning(){
    return running;
}

void Application::Setup(){
    running = Graphics::OpenWindow();

    Particle* smallBall = new Particle(50, 100, 1.0f);
    smallBall->radius = 4;
    particles.push_back(smallBall);

    Particle* bigBall = new Particle(200, 100, 3.0);
    bigBall->radius = 12;
    particles.push_back(bigBall);

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
                if(event.key.keysym.sym == SDLK_UP){
                    pushForce.y = -50 * PIXELS_PER_METER;
                }
                if(event.key.keysym.sym == SDLK_RIGHT){
                    pushForce.x = 50 * PIXELS_PER_METER;
                }
                if(event.key.keysym.sym == SDLK_DOWN){
                    pushForce.y = 50 * PIXELS_PER_METER;
                }
                if(event.key.keysym.sym == SDLK_LEFT){
                    pushForce.x = -50 * PIXELS_PER_METER;
                }
                break;
            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_UP){
                    pushForce.y =0;
                }
                if(event.key.keysym.sym == SDLK_RIGHT){
                    pushForce.x = 0;
                }
                if(event.key.keysym.sym == SDLK_DOWN){
                    pushForce.y = 0;
                }
                if(event.key.keysym.sym == SDLK_LEFT){
                    pushForce.x =0;
                }
                break;
        }
    }
}

void Application::Update(){

    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if(timeToWait > 0){
        SDL_Delay(timeToWait);
    }

    float deltaTime = (SDL_GetTicks() - timePreviousFrame)/1000.0f;
    if(deltaTime > 0.016){
        deltaTime = 0.016;
    }

    timePreviousFrame = SDL_GetTicks();

    Vec2 wind = Vec2(0.2 * PIXELS_PER_METER, 0.0);
    Vec2 weight = Vec2(0.0f, 9.8 * PIXELS_PER_METER);

    for(auto particle: particles){
        particle->AddForce(wind);
        particle->AddForce(weight * particle->mass);
        particle->AddForce(pushForce);
        particle->Integrate(deltaTime);

         if (particle->position.x - particle->radius <= 0) {
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9;
        } else if (particle->position.x + particle->radius >= Graphics::Width()) {
            particle->position.x = Graphics::Width() - particle->radius;
            particle->velocity.x *= -0.9;
        }
        if (particle->position.y - particle->radius <= 0) {
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        } else if (particle->position.y + particle->radius >= Graphics::Height()) {
            particle->position.y = Graphics::Height() - particle->radius;
            particle->velocity.y *= -0.9;
        }
    }


    
}

void Application::Render(){
    Graphics::ClearScreen(0xFF056263);

    for(auto particle: particles){

        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    }
    // Graphics::DrawFillRect(400, 400, 100, 200, 0x99999999);
    // Graphics::DrawRect(700, 700, 100, 200, 0xFFFFFFFF);
    // Graphics::DrawPolygon(500, 500, std::vector<Vec2>{{300, 500},{440, 540},{460, 500}, {300, 500}}, 0x99999999);
    Graphics::RenderFrame();
}

void Application::Destroy(){
    // destroy all objects in the scene
    for(auto particle: particles){
        delete particle;
    }

    Graphics::CloseWindow();
}


