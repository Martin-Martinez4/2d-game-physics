#ifndef APPLICATION_H
#define APPLICATION_H

#include "Particle.h"
#include <vector>
#include <SDL_rect.h>


class Application {
    private:
        bool running = false;
        std::vector<Particle*> particles;

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