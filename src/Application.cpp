#include "Application.h"
#include "Constraint.h"
#include "Graphics.h"
#include "Vec2.h"
#include "Constants.h"
#include "Body.h"
#include "World.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <vector>
#include <iostream>

bool Application::IsRunning(){
    return running;
}

void Application::Setup() {
   running = Graphics::OpenWindow();
   
   SDL_DisplayMode display_mode;
   SDL_GetCurrentDisplayMode(0, &display_mode);

   debug = true;

    // Create a physics world with gravity of -9.8 m/s2
    world = new World(-9.8, 50);

    // Add a static circle in the middle of the screen
    // Body* bigBall = new Body(CircleShape(64), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    // bigBall->SetTexture("./assets/bowlingball.png");
    // world->AddBody(bigBall);

    // // Add a floor and walls to contain objects objects
    Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2.0, Graphics::Height() - 50, 0.0);
    Body* leftWall = new Body(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0);
    Body* rightWall = new Body(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0);
    floor->restitution = 0.7;
    leftWall->restitution = 0.2;
    rightWall->restitution = 0.2;
    world->AddBody(floor);
    world->AddBody(leftWall);
    world->AddBody(rightWall);
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
                if(event.key.keysym.sym == SDLK_d){
                    debug = !debug;
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
            case SDL_MOUSEBUTTONDOWN:

                    int x = event.button.x;
                    int y = event.button.y;
                   
                    
                    
                    if (event.button.button == SDL_BUTTON_LEFT) {
                    
                    Body* ball = new Body(CircleShape(64), x, y, 2.0);
                    ball->SetTexture("./assets/basketball.png");
                    ball->restitution = 0.9;
                    world->AddBody(ball);
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    
                    Body* box = new Body(BoxShape(140, 140), x, y, 4.0);
                    box->SetTexture("./assets/crate.png");
                    box->restitution = 0.2;
                    world->AddBody(box);
                }
                  

                    break;
           
        }
    }
}

void Application::Update() {
    Graphics::ClearScreen(0xFF0F0721);
    
    // Wait some time until the reach the target frame time in milliseconds
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if (timeToWait > 0)
        SDL_Delay(timeToWait);

    // Calculate the deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > 0.016)
        deltaTime = 0.016;

    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Update world bodies (integration, collision detection, etc.)
    world->Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    // Draw all bodies
    for (auto body: world->GetBodies()) {
        if (body->shape->GetType() == ShapeType::CIRCLE) {
            CircleShape* circleShape = (CircleShape*) body->shape;
            if (!debug && body->texture) {
                Graphics::DrawTexture(body->position.x, body->position.y, circleShape->radius * 2, circleShape->radius * 2, body->rotation, body->texture);
            } else {
                Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFF00FF00);
            }
        }
        if (body->shape->GetType() == ShapeType::BOX) {
            BoxShape* boxShape = (BoxShape*) body->shape;
            if (!debug && body->texture) {
                Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture);
            } else {
                Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFF00FF00);
            }
        }
        if (body->shape->GetType() == ShapeType::POLYGON) {
            PolygonShape* polygonShape = (PolygonShape*) body->shape;
            if (!debug) {
                Graphics::DrawFillPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF444444);
            } else {
                Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
            }
        }
    }
    Graphics::RenderFrame();
}

void Application::Destroy(){
    delete world;

    Graphics::CloseWindow();
}


