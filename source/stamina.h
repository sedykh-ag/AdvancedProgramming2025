#pragma once

struct Stamina {
    int current;
    int max;

    Stamina(int maxStamina) : current(maxStamina), max(maxStamina) {}
};