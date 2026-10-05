#ifndef TESTS_H
#define TESTS_H

#include "../src/core/services/RentalSystem.h"

bool testStackLIFO();
bool testPopEmptyStack();
bool testUndoAdd();
bool testUndoUpdate();
bool testUndoDelete();
bool testUndoOrder();

void runAutomatedTests();

#endif
