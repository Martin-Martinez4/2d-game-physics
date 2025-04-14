#ifndef APPLICATION_H
#define APPLICATION_H

#include "World.h"
#include <SDL_rect.h>


class Application {
    private:
        bool running = false;
        bool debug = false;

        World* world;
  

        Vec2 pushForce = {0.0f, 0.0f};
        SDL_Rect liquid;
    
    public:
        Application() = default;
        ~Application() = default;

        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();

};

#endif