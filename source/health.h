#pragma once

struct Health {
public:
    int current;
    int max;

    Health(int maxHealth) : current(maxHealth), max(maxHealth) {}
    Health(int curHealth, int maxHealth) : current(curHealth), max(maxHealth) {}
};