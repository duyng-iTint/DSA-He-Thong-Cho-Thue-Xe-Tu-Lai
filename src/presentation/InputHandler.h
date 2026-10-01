#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "RentalSystem.h"

renTal inputRental();
renTal inputUpdatedRental(const string& bookingID);

void handleAdd(renTalSystem& system);
void handleUpdate(renTalSystem& system);
void handleDelete(renTalSystem& system);
void handleSearch(const renTalSystem& system);
void handleUndo(renTalSystem& system);

void showMenu();
string actionName(ActionType type);
void showTopUndo(const renTalSystem& system);

#endif
