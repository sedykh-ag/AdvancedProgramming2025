#pragma once
#include <memory>

#include "tileset.h"
#include "fsm.h"
#include "bt.h"


extern std::unique_ptr<TileSet> tilesetPtr;

StateMachine get_peasant_sm();
StateMachine get_predator_sm();
std::shared_ptr<RootNode> get_peasant_bt();
std::shared_ptr<RootNode> get_predator_bt();
