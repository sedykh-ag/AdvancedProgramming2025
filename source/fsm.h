#pragma once
#include <vector>
#include <utility>


class World;

class State {
public:
    virtual void on_enter(int entity_idx, World &world) = 0;
    virtual void on_exit(int entity_idx, World &world) = 0;
    virtual void act(int entity_idx, World &world, float dt) = 0;

    virtual ~State() { };
};

class StateTransition {
public:
    virtual bool isAvailable(int entity_idx, World &world) const = 0;

    virtual ~StateTransition() { };
};

class StateMachine {
    int curStateIndex = 0;
    std::vector<State*> states;
    std::vector<std::vector<std::pair<StateTransition*, int>>> transitions;
public:
    StateMachine() = default;
    StateMachine(const StateMachine &other) = delete;
    StateMachine(StateMachine &&other) noexcept = default;

    ~StateMachine();

    StateMachine &operator=(const StateMachine &other) = delete;
    StateMachine &operator=(StateMachine &&other) noexcept = default;

    void act(int entity_idx, World &world, float dt);

    int addState(State *state);
    void addTransition(StateTransition *transition, int from, int to);
};
