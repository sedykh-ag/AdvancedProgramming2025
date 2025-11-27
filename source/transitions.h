#pragma once
#include "fsm.h"


StateTransition *create_has_destination_transition();
StateTransition *create_has_path_transition();
StateTransition *create_predator_close_transition();
StateTransition *create_negate_transition(StateTransition *from);
