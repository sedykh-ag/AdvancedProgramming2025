
#include "world.h"
#include "camera2d.h"
#include "sprite.h"
#include "health.h"
#include "stamina.h"
#include "background_tag.h"
#include <SDL3/SDL_render.h>

void render_world(SDL_Window* window, SDL_Renderer* renderer, World& world)
{
    int screenW, screenH;
    SDL_GetWindowSize(window, &screenW, &screenH);

    const auto &camera = world.camera;

    // Draw cells
    for (int i = 0; i < world.cells.transforms.size(); i++)
    {
        const auto &sprite = world.cells.sprites[i];
        const auto &transform = world.cells.transforms[i];

        SDL_FRect dst = to_camera_space(transform, camera.transform, camera.pixelsPerMeter);
        dst.x += screenW / 2.0f;
        dst.y += screenH / 2.0f;
        DrawSprite(renderer, sprite, dst);
    }

    // Draw background sprites
    for (const auto& object : world.get_objects()) {
        auto sprite = object->get_component<Sprite>();
        auto transform = object->get_component<Transform2D>();
        auto bgTag = object->get_component<BackGroundTag>();
        if (!bgTag)
            continue;
        if (!sprite || !transform)
            continue;
        SDL_FRect dst = to_camera_space(*transform, camera.transform, camera.pixelsPerMeter);
        dst.x += screenW / 2;
        dst.y += screenH / 2;
        DrawSprite(renderer, *sprite, dst);
    }

    // Draw foreground sprites
    for (const auto& object : world.get_objects()) {
        auto sprite = object->get_component<Sprite>();
        auto transform = object->get_component<Transform2D>();
        auto bgTag = object->get_component<BackGroundTag>();
        if (bgTag)
            continue;
        if (!sprite || !transform)
            continue;
        SDL_FRect dst = to_camera_space(*transform, camera.transform, camera.pixelsPerMeter);
        dst.x += screenW / 2;
        dst.y += screenH / 2;
        DrawSprite(renderer, *sprite, dst);
    }

    for (int i = 0; i < world.characters.transforms.size(); i++) {
        const auto &sprite = world.characters.sprites[i];
        const auto &transform = world.characters.transforms[i];

        SDL_FRect dst = to_camera_space(transform, camera.transform, camera.pixelsPerMeter);
        dst.x += screenW / 2.0f;
        dst.y += screenH / 2.0f;
        DrawSprite(renderer, sprite, dst);
    }


    // Draw bars without textures and without OOP
    float grayColor[4] = {0.2f, 0.2f, 0.2f, 1.f};
    float healthColor[4] = {0.91f, 0.27f, 0.22f, 1.f};
    float staminaColor[4] = {0.f, 0.6f, 0.86f, 1.f};

    std::vector<SDL_FRect> backBars;
    std::vector<SDL_FRect> healthBars;
    std::vector<SDL_FRect> staminaBars;
    for (const auto& object : world.get_objects()) {
        auto transform = object->get_component<Transform2D>();
        auto health = object->get_component<Health>();
        auto stamina = object->get_component<Stamina>();
        if (!transform)
            continue;
        if (health)
        {
            Transform2D barTransform = *transform;
            barTransform.sizeX *= 0.1f;
            SDL_FRect dst = to_camera_space(barTransform, camera.transform, camera.pixelsPerMeter);
            dst.x += screenW / 2;
            dst.y += screenH / 2;
            backBars.push_back(dst);
            const float value = float(health->current) / float(health->max);
            dst.y += (1.f - value) * dst.h;
            dst.h *= value;
            healthBars.push_back(dst);
        }
        if (stamina)
        {
            Transform2D barTransform = *transform;
            barTransform.x += barTransform.sizeX * 0.9f;
            barTransform.sizeX *= 0.1f;
            SDL_FRect dst = to_camera_space(barTransform, camera.transform, camera.pixelsPerMeter);
            dst.x += screenW / 2;
            dst.y += screenH / 2;
            backBars.push_back(dst);
            const float value = float(stamina->current) / float(stamina->max);
            dst.y += (1.f - value) * dst.h;
            dst.h *= value;
            staminaBars.push_back(dst);
        }
    }

    SDL_SetRenderDrawColorFloat(renderer, grayColor[0], grayColor[1], grayColor[2], grayColor[3]);
    SDL_RenderFillRects(renderer, backBars.data(), int(backBars.size()));
    SDL_SetRenderDrawColorFloat(renderer, healthColor[0], healthColor[1], healthColor[2], healthColor[3]);
    SDL_RenderFillRects(renderer, healthBars.data(), int(healthBars.size()));
    SDL_SetRenderDrawColorFloat(renderer, staminaColor[0], staminaColor[1], staminaColor[2], staminaColor[3]);
    SDL_RenderFillRects(renderer, staminaBars.data(), int(staminaBars.size()));

}