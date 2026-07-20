#include <SDL3/SDL.h>
#include <iostream>
#include "GameData.h"
#include "LogicSystem.h"
#include "RenderSystem.h"
#include "InputSystem.h"

int main(int argc, char* argv[]) {
    GameData data;
    LogicSystem logic;
    RenderSystem render;
    InputSystem input;

    if (!render.init()) return -1;

    bool isRunning = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
            input.handleInput(event, data);
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        logic.update(data, deltaTime);

        render.draw(data);
        SDL_Delay(16);
    }

    render.cleanup();
    return 0;
}