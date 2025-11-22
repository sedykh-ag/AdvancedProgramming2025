#pragma once
#include <vector>
#include <utility>


class State {
public:
    virtual void on_enter() const = 0;
    virtual void on_exit() const = 0;
    virtual void act() const = 0;

    virtual ~State();
};

class StateTransition {
public:
    virtual bool isAvailable() const = 0;

    virtual ~StateTransition();
};

class StateMachine {
    int curStateIndex = 0;
    std::vector<State*> states;
    std::vector<std::vector<std::pair<StateTransition*, int>>> transitions;
public:
    StateMachine() = default;
    StateMachine(const StateMachine &other) = default;
    StateMachine(StateMachine &&other) = default;

    ~StateMachine();

    StateMachine &operator=(const StateMachine &other) = default;
    StateMachine &operator=(StateMachine &&other) = default;

    void act();

    int addState(State *state);
    void addTransition(StateTransition *transition, int from, int to);
};
