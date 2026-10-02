#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include "../model/Rental.h"

enum class ActionType {
    ADD,
    UPDATE,
    DELETE_ACTION
};

struct Action {
    ActionType type;
    renTal oldData;
    renTal newData;
    int position = -1;
};

struct StackNode {
    Action action;
    StackNode* next;

    StackNode(const Action& a) {
        action = a;
        next = nullptr;
    }
};

class Mystack {
private:
    StackNode* topNode;
    int size;

public:
    Mystack();
    ~Mystack();

    void push(const Action& action);
    bool pop(Action& action);
    bool empty() const;
    bool top(Action& action) const;
    int getSize() const;
    void clear();
};

class UnoManager {
private:
    Mystack undoStack;

public:
    void saveAction(const Action& action);
    bool undo(Action& action);
    bool canUndo() const;
    bool top(Action& action) const;
    int historySize() const;
    void clearHistory();
};

#endif
