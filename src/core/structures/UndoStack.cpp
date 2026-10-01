#include "UndoStack.h"

Mystack::Mystack() {
    topNode = nullptr;
    size = 0;
}

Mystack::~Mystack() {
    clear();
}

void Mystack::push(const Action& action) {
    StackNode* newNode = new StackNode(action);
    newNode->next = topNode;
    topNode = newNode;
    size++;
}

bool Mystack::pop(Action& action) {
    if (topNode == nullptr)
        return false;

    StackNode* temp = topNode;
    action = temp->action;
    topNode = topNode->next;
    delete temp;
    size--;

    return true;
}

bool Mystack::empty() const {
    return topNode == nullptr;
}

bool Mystack::top(Action& action) const {
    if (topNode == nullptr)
        return false;

    action = topNode->action;
    return true;
}

int Mystack::getSize() const {
    return size;
}

void Mystack::clear() {
    while (topNode != nullptr) {
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }

    size = 0;
}

void UnoManager::saveAction(const Action& action) {
    undoStack.push(action);
}

bool UnoManager::undo(Action& action) {
    return undoStack.pop(action);
}

bool UnoManager::canUndo() const {
    return !undoStack.empty();
}

bool UnoManager::top(Action& action) const {
    return undoStack.top(action);
}

int UnoManager::historySize() const {
    return undoStack.getSize();
}

void UnoManager::clearHistory() {
    undoStack.clear();
}
