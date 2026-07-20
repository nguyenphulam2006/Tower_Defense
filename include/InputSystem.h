#pragma once
#include <SDL3/SDL.h>
#include "GameData.h"

class InputSystem {
public:
    void handleInput(SDL_Event& event, GameData& data);
};