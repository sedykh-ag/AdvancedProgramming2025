#include "fsm.h"
#include "math2d.h"
#include "world.h"


class HasDestinationTransition : public StateTransition {
public:
    bool isAvailable(int entity_idx, World &world) const override {
        return world.characters.destinations[entity_idx] != int2{-1, -1};
    }
};

class HasPathTransition : public StateTransition {
public:
    bool isAvailable(int entity_idx, World &world) const override {
        return !world.characters.paths[entity_idx].empty();
    }
};

class PredatorCloseTransition : public StateTransition {
public:
    const int triggerDist = 4;
    bool isAvailable(int entity_idx, World &world) const override {
        auto &myTransform = world.characters.transforms[entity_idx];
        int2 myPos = {(int)myTransform.x,(int) myTransform.y};
        int2 closestPredatorPos = locate_closest_predator(world.characters, myPos);

        return dist(myPos, closestPredatorPos) <= triggerDist;
    }
};

class NegateTransition : public StateTransition {
    const StateTransition *transition; // we own it
public:
    NegateTransition(const StateTransition *in_trans) : transition(in_trans) { }
    ~NegateTransition() override { delete transition; }

    bool isAvailable(int entity_idx, World &world) const override {
        return !transition->isAvailable(entity_idx, world);
    }
};

// transition creators
StateTransition *create_has_destination_transition() {
    return new HasDestinationTransition{};
}

StateTransition *create_has_path_transition() {
    return new HasPathTransition{};
}

StateTransition *create_predator_close_transition() {
    return new PredatorCloseTransition{};
}

StateTransition *create_negate_transition(StateTransition *from) {
    return new NegateTransition{from};
}
