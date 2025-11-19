#pragma once

#include <SDL3/SDL_render.h>
#include <memory>

using TexturePtr = std::shared_ptr<SDL_Texture>;

TexturePtr LoadTextureFromFile(const char* filename, SDL_Renderer* renderer);
