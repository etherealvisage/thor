#include <iostream>
#include <memory>
#include <fstream>
#include <cmath>

#include <SDL2/SDL.h>

#include "tracer/Interface.h"
#include "tracer/Scene.h"
#include "tracer/Sphere.h"
#include "tracer/Light.h"

#include "resource/XML.h"
#include "resource/Registry.h"


using namespace Thor;

int main() {
    std::shared_ptr<Resource::XML> config
        = Resource::Registry::get<Resource::XML>("config.xml");

    std::string vendor
        = config->getNode("/config/cl").attribute("vendor").as_string("");
    std::string device
        = config->getNode("/config/cl").attribute("device").as_string("");

    Tracer::CLConfig clc(vendor, device);
    Tracer::Interface tinterface(clc, Tracer::OutputConfig(1920, 1080));

    Tracer::Scene scene;

    scene.addSceneObject(new Tracer::Light(Math::Vector(0,0,0), Math::Vector(0.1,0.1,0.1), Math::Vector(1.0,1.0,1.0), Math::Vector(0,0,0)));

    /*scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,10), 3, Math::Vector(1.0,0.0,0.0), 0.9));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,-10), 3, Math::Vector(0.0,1.0,0.0), 0.9));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(10,0,0), 3, Math::Vector(1.0,1.0,1.0), 0.9));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(-10,0,0), 3, Math::Vector(1.0,1.0,1.0), 0.9));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,10,0), 3, Math::Vector(1.0,1.0,1.0), 0.9));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,-10,0), 3, Math::Vector(1.0,1.0,1.0), 0.9));*/
    /*for(int i = 0; i < 100; i ++) {
        scene.addSceneObject(new Tracer::Sphere(Math::Vector(10 + i*5,0,0), 3, Math::Vector(1.0,1.0,1.0), 0.9));
    }*/
    for(int i = -5; i <= 5; i ++) {
        for(int j = 0; j < 10; j ++) {
            double x = std::cos(j * M_PI / 5);
            double y = std::sin(j * M_PI / 5);
            scene.addSceneObject(new Tracer::Sphere(Math::Vector(x,y,i), .1, Math::Vector(1.0,1.0,1.0), 0.9));
        }
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Thor", 0, 0, 1920, 1080, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 1920, 1080);

    int frameCount = 0;
    int start = SDL_GetTicks();

    bool move[6] = {false,false,false,false,false,false}, rot[4] = {false,false,false,false};

    bool quit = false;
    while(!quit) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = true;
            else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                bool value = e.type == SDL_KEYDOWN;
                if(e.key.keysym.sym == SDLK_a) move[0] = value;
                else if(e.key.keysym.sym == SDLK_d) move[1] = value;
                else if(e.key.keysym.sym == SDLK_q) move[2] = value;
                else if(e.key.keysym.sym == SDLK_e) move[3] = value;
                else if(e.key.keysym.sym == SDLK_s) move[4] = value;
                else if(e.key.keysym.sym == SDLK_w) move[5] = value;
                else if(e.key.keysym.sym == SDLK_LEFT) rot[0] = value;
                else if(e.key.keysym.sym == SDLK_RIGHT) rot[1] = value;
                else if(e.key.keysym.sym == SDLK_UP) rot[2] = value;
                else if(e.key.keysym.sym == SDLK_DOWN) rot[3] = value;
                else if(e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            }
        }

        if(move[0]) scene.camera().pos() -= scene.camera().rot() * Math::Vector(0.04,0,0);
        if(move[1]) scene.camera().pos() += scene.camera().rot() * Math::Vector(0.04,0,0);
        if(move[2]) scene.camera().pos() -= scene.camera().rot() * Math::Vector(0,0.04,0);
        if(move[3]) scene.camera().pos() += scene.camera().rot() * Math::Vector(0,0.04,0);
        if(move[4]) scene.camera().pos() -= scene.camera().rot() * Math::Vector(0,0,0.04);
        if(move[5]) scene.camera().pos() += scene.camera().rot() * Math::Vector(0,0,0.04);

        if(rot[0]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(0,1,0), -0.05);
        if(rot[1]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(0,1,0), 0.05);
        if(rot[2]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(1,0,0), 0.05);
        if(rot[3]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(1,0,0), -0.05);

        void *pixels;
        int width;
        SDL_LockTexture(texture, nullptr, &pixels, &width);
        tinterface.render(scene, pixels);
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        frameCount ++;
    }

    int end = SDL_GetTicks();

    std::cout << frameCount << " frames in " << (end-start) << " ms = " << 1000*frameCount/(double)(end-start) << " FPS" << std::endl;

    SDL_Quit();

    return 0;
}
