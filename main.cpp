#include "Application.h"
#include <SDL.h>
#include <iostream>

int main(int argc, char *argv[]){
    std::clog << "test\n";
    Application app;

    app.Setup();

    while(app.IsRunning()){
        app.Input();
        app.Update();
        app.Render();
    }

    app.Destroy();

    return 0;
}