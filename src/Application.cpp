#include "Application.h"
#include "Graphics.h"
#include "Vec2.h"
#include "Constants.h"
#include "Body.h"
#include "Force.h"
#include "Collision.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <iostream>

bool Application::IsRunning(){
    return running;
}

void Application::Setup(){
    running = Graphics::OpenWindow();

    // Particle* smallBall = new Particle(50, 100, 1.0f);
    // smallBall->radius = 4;
    // particles.push_back(smallBall);

    // Particle* bigBall = new Particle(200, 100, 3.0);
    // bigBall->radius = 12;
    // particles.push_back(bigBall);

    // Body* c1 = new Body(CircleShape(50), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    // c1->restitution = 0.0;
    // bodies.push_back(c1);

    Body* b1 = new Body(BoxShape(200, 100), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    Body* b2 = new Body(BoxShape(Graphics::Width()/2, 100), Graphics::Width()/2, Graphics::Height() +2, 0.0f);
    b1->angularVelocity = 0.4;
    b2->angularVelocity = 0.1;

    bodies.push_back(b1);
    bodies.push_back(b2);

    // liquid.x = 0;
    // liquid.y = Graphics::Height() /2;
    // liquid.w = Graphics::Width();
    // liquid.h = Graphics::Height() /2;

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
            case SDL_MOUSEBUTTONDOWN:

                    int x,y;
                    SDL_GetMouseState(&x, &y);
                    // Particle* particle = new Particle(x, y, 1.0);
                    // particle->radius = 5;
                    // particles.push_back(particle);
                    
                //    Body* smallBall = new Body(CircleShape(40), x, y, 1.0);
                //     smallBall->restitution = 0.9;
                //     bodies.push_back(smallBall);

                    Body* box = new Body(BoxShape(50, 50), x, y, 1.0);
                    // box->restitution = 0.9;
                    bodies.push_back(box);

                break;
            // case SDL_MOUSEMOTION:
            //     int x, y;
            //     SDL_GetMouseState(&x, &y);
            //     bodies[0]->position.x = x;
            //     bodies[0]->position.y = y;
            //     break;
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

        if(particle->position.y >= liquid.y){
            Vec2 drag = GenerateDragForce(*particle, 0.05, deltaTime);
            particle->AddForce(drag);
        }

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

    for(auto body: bodies){
        // body->AddForce(wind);
        body->AddForce(weight * body->mass);
        body->AddForce(pushForce);

        // float torque = 20;
        // body->AddTorque(torque);

        // if(body->position.y >= liquid.y){
        //     Vec2 drag = GenerateDragForce(*body, 0.05, deltaTime);
        //     body->AddForce(drag);
        // }

        body->Update(deltaTime);

        

        if(body->shape->GetType() == ShapeType::CIRCLE){

            CircleShape* cs = (CircleShape*) body->shape;
            if (body->position.x - cs->radius <= 0) {
                body->position.x = cs->radius;
                body->velocity.x *= -0.9;
            } else if (body->position.x + cs->radius >= Graphics::Width()) {
                body->position.x = Graphics::Width() - cs->radius;
                body->velocity.x *= -0.9;
            }
            if (body->position.y - cs->radius <= 0) {
                body->position.y = cs->radius;
                body->velocity.y *= -0.9;
            } else if (body->position.y + cs->radius >= Graphics::Height()) {
                body->position.y = Graphics::Height() - cs->radius;
                body->velocity.y *= -0.9;
            }
        }

       
    }

    for(int i = 0; i <= bodies.size() - 1; ++i){
            for(int j = i+1; j < bodies.size(); ++j){
                Body* a = bodies[i];
                Body* b = bodies[j];

                a->isColliding = false;
                b->isColliding = false;

                Collision::Contact contact;
                if(Collision::IsColliding(a, b, contact)){

                    // Resolve the collision using the impulse method
                    contact.ResolveCollision();

                    // Draw debug contact information
                    Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
                    Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFF00FF);
                    Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15, 0xFFFF00FF);
                    a->isColliding = true;
                    b->isColliding = true;
                }

                
            }
        }

    
}

void Application::Render(){
    Graphics::ClearScreen(0xFF056263);

    // Graphics::DrawFillRect(liquid.x, liquid.y, liquid.w, liquid.h,  0xFF13376E);
    // Graphics::DrawFillRect(0, 0, 100, 50,  0xFF13376E);

    for(auto particle: particles){

        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    }

    for (auto body: bodies) {
        Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;

        if (body->shape->GetType() == ShapeType::CIRCLE) {
            CircleShape* circleShape = (CircleShape*) body->shape;
            Graphics::DrawFillCircle(body->position.x, body->position.y, circleShape->radius, color);
        }
        if (body->shape->GetType() == ShapeType::BOX) {
            BoxShape* boxShape = (BoxShape*) body->shape;
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, color);
        }
    }

    Graphics::RenderFrame();
}

void Application::Destroy(){
    // destroy all objects in the scene
    for(auto particle: particles){
        delete particle;
    }

    Graphics::CloseWindow();
}


