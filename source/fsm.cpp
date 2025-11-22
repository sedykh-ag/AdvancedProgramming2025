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

void StateMachine::act() {
    for (const auto &transition : transitions[curStateIndex])
        if (transition.first->isAvailable()) {
            states[curStateIndex]->on_exit();
            curStateIndex = transition.second;
            states[curStateIndex]->on_enter();
            break;
        }

    states[curStateIndex]->act();
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
