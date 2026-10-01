#ifndef RENTALSYSTEM_H
#define RENTALSYSTEM_H

#include "UndoStack.h"
#include <vector>

class renTalSystem {
private:
    vector<renTal> rentals;
    UnoManager undoManager;

public:
    bool addRental(const renTal& rental, bool saveHistory = true);
    bool updateRental(const string& bookingID,
                      const renTal& newrenTal,
                      bool saveHistory = true);
    bool deleteRental(const string& bookingID,
                      bool saveHistory = true);

    bool undoRental();

    const renTal* getRental(const string& bookingID) const;
    void showAll() const;
    void searchRental(const string& bookingID) const;

    int rentalCount() const;
    int historySize() const;
    bool getTopUndo(Action& action) const;
    void clearHistory();
};

int findRental(const vector<renTal>& rentals, const string& bookingID);

#endif
