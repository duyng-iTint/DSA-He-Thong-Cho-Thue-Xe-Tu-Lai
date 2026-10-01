#include "RentalSystem.h"

int findRental(
    const vector<renTal>& rentals,
    const string& bookingID
) {
    for (int i = 0; i < static_cast<int>(rentals.size()); i++) {
        if (rentals[i].bookingID == bookingID)
            return i;
    }

    return -1;
}

bool renTalSystem::addRental(
    const renTal& rental,
    bool saveHistory
) {
    if (findRental(rentals, rental.bookingID) != -1)
        return false;

    if (!isValidDate(rental.starDate))
        return false;

    if (!isValidDate(rental.endDate))
        return false;

    if (!isStartBeforeOrEqualEnd(
            rental.starDate, rental.endDate))
        return false;

    rentals.push_back(rental);

    if (saveHistory) {
        Action action;
        action.type = ActionType::ADD;
        action.newData = rental;
        action.position = static_cast<int>(rentals.size()) - 1;
        undoManager.saveAction(action);
    }

    return true;
}

bool renTalSystem::updateRental(
    const string& bookingID,
    const renTal& newrenTal,
    bool saveHistory
) {
    int index = findRental(rentals, bookingID);

    if (index == -1)
        return false;

    if (!isValidDate(newrenTal.starDate))
        return false;

    if (!isValidDate(newrenTal.endDate))
        return false;

    if (!isStartBeforeOrEqualEnd(
            newrenTal.starDate, newrenTal.endDate))
        return false;

    renTal oldRental = rentals[index];
    renTal updatedRental = newrenTal;

    updatedRental.bookingID = bookingID;
    rentals[index] = updatedRental;

    if (saveHistory) {
        Action action;
        action.type = ActionType::UPDATE;
        action.oldData = oldRental;
        action.newData = updatedRental;
        action.position = index;
        undoManager.saveAction(action);
    }

    return true;
}

bool renTalSystem::deleteRental(
    const string& bookingID,
    bool saveHistory
) {
    int index = findRental(rentals, bookingID);

    if (index == -1)
        return false;

    renTal oldRental = rentals[index];

    rentals.erase(rentals.begin() + index);

    if (saveHistory) {
        Action action;
        action.type = ActionType::DELETE_ACTION;
        action.oldData = oldRental;
        action.position = index;
        undoManager.saveAction(action);
    }

    return true;
}

bool renTalSystem::undoRental() {
    Action action;

    if (!undoManager.undo(action))
        return false;

    if (action.type == ActionType::ADD) {
        int index = findRental(
            rentals,
            action.newData.bookingID
        );

        if (index != -1)
            rentals.erase(rentals.begin() + index);
    }
    else if (action.type == ActionType::UPDATE) {
        int index = findRental(
            rentals,
            action.oldData.bookingID
        );

        if (index != -1)
            rentals[index] = action.oldData;
    }
    else if (action.type == ActionType::DELETE_ACTION) {
        int position = action.position;

        if (position < 0 ||
            position > static_cast<int>(rentals.size())) {
            position = static_cast<int>(rentals.size());
        }

        rentals.insert(
            rentals.begin() + position,
            action.oldData
        );
    }

    return true;
}

const renTal* renTalSystem::getRental(
    const string& bookingID
) const {
    int index = findRental(rentals, bookingID);

    if (index == -1)
        return nullptr;

    return &rentals[index];
}

void renTalSystem::showAll() const {
    if (rentals.empty()) {
        cout << "\nDanh sach don thue dang rong!\n";
        return;
    }

    cout << "\n===== DANH SACH DON THUE =====\n";

    for (const renTal& rental : rentals) {
        rental.print();
        cout << "-----------------------------\n";
    }
}

void renTalSystem::searchRental(
    const string& bookingID
) const {
    const renTal* rental = getRental(bookingID);

    if (rental == nullptr) {
        cout << "\nKhong tim thay don thue!\n";
        return;
    }

    cout << "\n===== KET QUA TIM KIEM =====\n";
    rental->print();
}

int renTalSystem::rentalCount() const {
    return static_cast<int>(rentals.size());
}

int renTalSystem::historySize() const {
    return undoManager.historySize();
}

bool renTalSystem::getTopUndo(Action& action) const {
    return undoManager.top(action);
}

void renTalSystem::clearHistory() {
    undoManager.clearHistory();
}
