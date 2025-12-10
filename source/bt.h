#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <functional>
#include <cstdio>

class World;

enum class Status {
    SUCCESS = 0,
    RUNNING,
    FAILURE
};

static const char *statusToString(Status s) {
    switch (s) {
        case Status::SUCCESS: return "SUCCESS";
        case Status::RUNNING: return "RUNNING";
        case Status::FAILURE: return "FAILURE";
    }
    return "UNKNOWN";
}

// --- Node base ---
class Node {
protected:
    virtual Status tickImpl(int entity_idx, World &world, float dt) = 0;

public:
    Status status;
    const char *name;

    Node(const char *name) : name(name) { }
    virtual ~Node() = default;

    Status tick(int entity_idx, World &world, float dt) {
        Status status = tickImpl(entity_idx, world, dt);
        printf("[BT entity=%d] %s -> %s\n", entity_idx, name, statusToString(status));
        return status;
    }
};

using NodePtr = std::shared_ptr<Node>;

// --- Root node ---
class RootNode : public Node {
protected:
    NodePtr child;

    Status tickImpl(int entity_idx, World &world, float dt) override {
        return child->tick(entity_idx, world, dt);
    }

public:
    RootNode(NodePtr child) : Node("root"), child(child) { }
};

// --- Composite node base ---
class CompositeNode : public Node {
protected:
    std::vector<NodePtr> children;

public:
    CompositeNode(const char *name) : Node(name) { }

    void addChild(NodePtr child) { children.push_back(child); }
};

// Sequence
class SequenceNode final : public CompositeNode {
    size_t curChildIdx = 0;

protected:
    Status tickImpl(int entity_idx, World &world, float dt) override {
        if (children.empty())
            return Status::SUCCESS;

        NodePtr &curChild = children[curChildIdx++];
        Status curChildStatus = curChild->tick(entity_idx, world, dt);

        if (curChildStatus == Status::FAILURE) {
            curChildIdx = 0;
            return Status::FAILURE;
        }

        if (curChildStatus == Status::RUNNING)
            curChildIdx--;

        if (curChildIdx >= children.size()) {
            curChildIdx = 0;
            return Status::SUCCESS;
        }

        return Status::RUNNING;
    }

public:
    SequenceNode(const char *name) : CompositeNode(name) { }
};

// Selector
class SelectorNode final : public CompositeNode {
    size_t curChildIdx = 0;

protected:
    Status tickImpl(int entity_idx, World &world, float dt) override {
        if (children.empty())
            return Status::SUCCESS;

        NodePtr &curChild = children[curChildIdx++];
        Status curChildStatus = curChild->tick(entity_idx, world, dt);

        if (curChildStatus == Status::SUCCESS) {
            curChildIdx = 0;
            return Status::SUCCESS;
        }

        if (curChildStatus == Status::RUNNING)
            curChildIdx--;

        if (curChildIdx >= children.size()) {
            curChildIdx = 0;
            return Status::FAILURE;
        }

        return Status::RUNNING;
    }

public:
    SelectorNode(const char *name) : CompositeNode(name) { }
};

// --- Decorators ---
class DecoratorNode : public Node {
protected:
    NodePtr child;

public:
    DecoratorNode(NodePtr child, const char *name) : Node(name), child(child) { }
};

class InverterNode : public DecoratorNode {
protected:
    Status tickImpl(int entity_idx, World &world, float dt) override {
        Status child_status = child->tick(entity_idx, world, dt);
        if (child_status == Status::SUCCESS) return Status::FAILURE;
        if (child_status == Status::FAILURE) return Status::SUCCESS;
        return Status::RUNNING;
    }

public:
    InverterNode(NodePtr child, const char *name) : DecoratorNode(child, name) { }
};

// --- Leaves ---
class ActionNode : public Node {
    std::function<Status(int, World&, float)> action;

protected:
    Status tickImpl(int entity_idx, World &world, float dt) override {
        return action(entity_idx, world, dt);
    }
public:

    ActionNode(std::function<Status(int, World&, float)> action, const char *name)
        : Node(name), action(action) { }
};

class ConditionNode : public Node {
    std::function<bool(int, World&, float)> condition;

protected:
    Status tickImpl(int entity_idx, World &world, float dt) override {
        return condition(entity_idx, world, dt) ? Status::SUCCESS : Status::FAILURE;
    }

public:
    ConditionNode(std::function<bool(int, World&, float)> condition, const char *name)
        : Node(name), condition(condition) { }
};
