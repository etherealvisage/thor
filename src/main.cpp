#include <iostream>
#include <memory>
#include <fstream>

#include <SDL2/SDL.h>

#include "tracer/Interface.h"
#include "tracer/Scene.h"
#include "tracer/Sphere.h"

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
    Tracer::Interface tinterface(clc, Tracer::OutputConfig(800, 600));

    Tracer::Scene scene;

    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,10), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,0,-10), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(10,0,0), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(-10,0,0), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,10,0), 3));
    scene.addSceneObject(new Tracer::Sphere(Math::Vector(0,-10,0), 3));

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Thor", 0, 0, 800, 600, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);

    int frameCount = 0;
    int start = SDL_GetTicks();

    bool move[6] = {false,false,false,false,false,false}, rot[2] = {false,false};

    while(1) {
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) break;
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
            }
        }

        if(move[0]) scene.camera().pos() -= Math::Vector(0.02,0,0);
        if(move[1]) scene.camera().pos() += Math::Vector(0.02,0,0);
        if(move[2]) scene.camera().pos() -= Math::Vector(0,0.02,0);
        if(move[3]) scene.camera().pos() += Math::Vector(0,0.02,0);
        if(move[4]) scene.camera().pos() -= Math::Vector(0,0,0.02);
        if(move[5]) scene.camera().pos() += Math::Vector(0,0,0.02);

        if(rot[0]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(0,1,0), 0.05);
        if(rot[1]) scene.camera().rot() = scene.camera().rot() * Math::Quaternion(Math::Vector(0,1,0), -0.05);

        /*std::cout << "Rot: " << scene.camera().rot().s() << std::endl;
        std::cout << "\t" << scene.camera().rot().v().toString() << std::endl;*/

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
