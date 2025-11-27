#include <vector>
#include <utility>

#include "fsm.h"


StateMachine::~StateMachine() {
    for (State *state : states)
        delete state;
    states.clear();
    for (auto &transitionsFromState : transitions)
        for (auto &transition : transitionsFromState)
            delete transition.first;
    transitions.clear();
}

void StateMachine::act(int entity_idx, World &world, float dt) {
    if (!transitions.empty()) {
        for (const auto &transition : transitions[curStateIndex])
            if (transition.first->isAvailable(entity_idx, world)) {
                states[curStateIndex]->on_exit(entity_idx, world);
                curStateIndex = transition.second;
                states[curStateIndex]->on_enter(entity_idx, world);
                break;
            }
    }

    if (!states.empty())
        states[curStateIndex]->act(entity_idx, world, dt);
}

int StateMachine::addState(State *state) {
    int idx = states.size();
    states.push_back(state);
    transitions.push_back(std::vector<std::pair<StateTransition*, int>>());
    return idx;
}

void StateMachine::addTransition(StateTransition *transition, int from, int to) {
    transitions[from].push_back(std::make_pair(transition, to));
}
