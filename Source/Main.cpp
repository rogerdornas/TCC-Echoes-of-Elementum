// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define SDL_MAIN_HANDLED

#include "Game.h"

#ifdef _WIN32
extern "C" {
    // Sinaliza para o driver da NVIDIA usar a GPU de alta performance
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

    // Sinaliza para o driver da AMD usar a GPU de alta performance
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

//Screen dimension constants

// const int SCREEN_WIDTH = 1920;
// const int SCREEN_HEIGHT = 1080;
// const int SCREEN_WIDTH = 1600;
// const int SCREEN_HEIGHT = 900;
// const int SCREEN_WIDTH = 1776;
// const int SCREEN_HEIGHT = 999;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
// const int SCREEN_WIDTH = 1680;
// const int SCREEN_HEIGHT = 720;
// const int SCREEN_WIDTH = 640;
// const int SCREEN_HEIGHT = 360;
// const int SCREEN_WIDTH = 1280;
// const int SCREEN_HEIGHT = 360;
// const int SCREEN_WIDTH = 500;
// const int SCREEN_HEIGHT = 1000;
const int FPS = 120;

int main(int argc, char **argv)
{
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT, FPS);
    bool success = game.Initialize();
    if (success)
    {
        game.RunLoop();
    }
    game.Shutdown();
    return 0;
}
