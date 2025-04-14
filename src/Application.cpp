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

bool Application::IsRunning(){
    return running;
}

void Application::Setup(){
    running = Graphics::OpenWindow();

    world = new World(-9.8);

    // Body* b1 = new Body(BoxShape(200, 100), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    // b1->rotation = 0.4;
    // b1->restitution = 0.25;
    // b1->SetTexture("./assets/crate.png");

    // Body* b2 = new Body(BoxShape(Graphics::Width(), 100), Graphics::Width()/2, Graphics::Height() +2, 0.0f);
    // b2->restitution = 0.64;
    // b2->SetTexture("./assets/metal.png");

    // world->AddBody(b1);
    // world->AddBody(b2);

    Body* a = new Body(CircleShape(30), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0f);
    Body* b = new Body(CircleShape(20), a->position.x - 100, a->position.y, 1.0f);
    world->AddBody(a);
    world->AddBody(b);

    JointConstraint* joint = new JointConstraint(a, b, a->position);
    world->AddConstraint(joint);

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
                   
                    
                    
                    if(event.button.button == SDL_BUTTON_LEFT){

                        Body* box = new Body(BoxShape(50, 50), x, y, 3.0);
                        box->restitution = 0.25;
                        box->SetTexture("./assets/crate.png");
                        world->AddBody(box);
                        
                    }
                    
                    if(event.button.button == SDL_BUTTON_RIGHT){

                        Body* smallBall = new Body(CircleShape(20), x, y, 1.0);
                        smallBall->restitution = 0.9;
                        smallBall->friction = 0.2;
                        smallBall->SetTexture("./assets/basketball.png");
                        world->AddBody(smallBall);
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

    // Vec2 wind = Vec2(0.2 * PIXELS_PER_METER, 0.0);

    world->Update(deltaTime);
}

void Application::Render(){
    Graphics::ClearScreen(0xFF056263);

    std::vector<Body*> bodies = world->GetBodies();

    for (auto body: bodies) {
        // Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;
        Uint32 color = 0xFFFFFFFF;

        if (body->shape->GetType() == ShapeType::CIRCLE) {
            CircleShape* circleShape = (CircleShape*) body->shape;
            if(!debug && body->texture != nullptr){
                Graphics::DrawTexture(body->position.x, body->position.y, circleShape->radius*2, circleShape->radius*2, body->rotation, body->texture);
            }else{
                Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, color);
                // Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, color);
            }
        }
        if (body->shape->GetType() == ShapeType::BOX) {
            BoxShape* boxShape = (BoxShape*) body->shape;
            if(!debug && body->texture != nullptr){
                Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture);
            }else{

                Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, color);
            }
        }
        if (body->shape->GetType() == ShapeType::POLYGON) {
            PolygonShape* polygonShape = (PolygonShape*) body->shape;
            if(!debug){
                // Graphics::DrawTexture(body->position.x, body->position.y, CircleShape->radius*2, int height, float rotation, SDL_Texture *texture)
                Graphics::DrawFillPolygon(body->position.x, body->position.y, polygonShape->worldVertices, color);
            }else{

                Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, color);
            }
        }
    }

    Graphics::RenderFrame();
}

void Application::Destroy(){
    delete world;

    Graphics::CloseWindow();
}


