#include "Graphics.h"
#include <SDL.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <cstdint>
#include <iostream>


SDL_Window* Graphics::window = NULL;
SDL_Renderer* Graphics::renderer = NULL;
int Graphics::windowWidth = 10;
int Graphics::windowHeight = 10;

int Graphics::Width() {
    return windowWidth;
}

int Graphics::Height() {
    return windowHeight;
}

bool Graphics::OpenWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL" << std::endl;
        return false;
    }
    std::cout << "test \n";
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    windowWidth = display_mode.w;
    windowHeight = display_mode.h;
    window = SDL_CreateWindow(NULL, 0, 0, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
    if (!window) {
        std::cerr << "Error creating SDL window" << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error creating SDL renderer" << std::endl;
        return false;
    }
    return true;
}

void Graphics::ClearScreen(Uint32 color) {
    SDL_SetRenderDrawColor(renderer, color >> 16, color >> 8, color, 255);
    SDL_RenderClear(renderer);
}

void Graphics::RenderFrame() {
    SDL_RenderPresent(renderer);
}

void Graphics::DrawLine(int x0, int y0, int x1, int y1, Uint32 color) {
    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    return;
}

// https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
void Graphics::DrawCircle(int x, int y, int radius, float angle, Uint32 color) {
    std::vector<SDL_Point> points;

    const Uint32 d = radius * 2;

    int h = radius;
    int v = 0;
    int dx = 1; 
    int dy = 1;
    
    int error = dx - (d << 1);

    while(h >= v){
        
        

        points.push_back(SDL_Point{x+h, y-v});
        points.push_back(SDL_Point{x+h, y+v});
        points.push_back(SDL_Point{x-h, y-v});
        points.push_back(SDL_Point{x-h, y+v});

        points.push_back(SDL_Point{x+v, y-h});
        points.push_back(SDL_Point{x+v, y+h});
        points.push_back(SDL_Point{x-v, y-h});
        points.push_back(SDL_Point{x-v, y+h});

        if (error <= 0){
         ++v;
         error += dy;
         dy += 2;
      }

      if (error > 0){
         --h;
         dx += 2;
         error += (dx - d);
      }
    }
		
    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    DrawLine(x,y,x+cos(angle) * radius, y+sin(angle) * radius, color);
    return;
}

void Graphics::DrawFillCircle(int x, int y, int radius, Uint32 color) {
    std::vector<SDL_Point> points;

    for(int w = 0; w < radius * 2; ++w){
        for(int h = 0; h < radius * 2; ++h){
            int dx = radius - w;
            int dy = radius - h;

            if((dx*dx + dy*dy) <= (radius*radius)){
                SDL_Point p = {x+dx, y+dy};
                points.push_back(p);
            }
        }
    }
		
    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    return;
}

void Graphics::DrawRect(int x, int y, int width, int height, Uint32 color) {
   
    SDL_Rect rect;
    rect.x = x + width/2.0f;
    rect.y = y + height/2.0f;
    rect.w = width;
    rect.h = height;

    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Graphics::DrawFillRect(int x, int y, int width, int height, Uint32 color) {
    SDL_Rect rect;

    // if input rect has origin in center
    // rect.x = x - width/2;
    // rect.y = y - height/2;
    // rect.w = width;
    // rect.h = height;

    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;

    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Graphics::DrawPolygon(int x, int y, const std::vector<Vec2>& vertices, Uint32 color) {
    std::vector<SDL_Point> points;

    for (int i = 0; i < vertices.size(); i++) {
        points.push_back(SDL_Point{static_cast<int>(vertices[i].x), static_cast<int>(vertices[i].y)});
    }

    points.push_back(SDL_Point{static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y)});

    SDL_SetRenderDrawColor(renderer,color >> 16, color >> 8, color, 255);
    SDL_RenderDrawLines(renderer, points.data(), vertices.size()+1);
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    return;
}

void Graphics::DrawFillPolygon(int x, int y, const std::vector<Vec2>& vertices, Uint32 color) {
    // std::vector<short> vx;
    // std::vector<short> vy;
    // for (int i = 0; i < vertices.size(); i++) {
    //     vx.push_back(static_cast<int>(vertices[i].x));
    // }
    // for (int i = 0; i < vertices.size(); i++) {
    //     vy.push_back(static_cast<int>(vertices[i].y));
    // }
    // filledPolygonColor(renderer, &vx[0], &vy[0], vertices.size(), color);
    // filledCircleColor(renderer, x, y, 1, 0xFF000000);
    return;
}

void Graphics::DrawTexture(int x, int y, int width, int height, float rotation, SDL_Texture* texture) {
    SDL_Rect dstRect = {x - (width / 2), y - (height / 2), width, height};
    float rotationDeg = rotation * 57.2958;
    SDL_RenderCopyEx(renderer, texture, NULL, &dstRect, rotationDeg, NULL, SDL_FLIP_NONE);
}

void Graphics::CloseWindow(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}