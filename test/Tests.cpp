#include "Tests.h"

bool testStackLIFO() {
    Mystack stack;

    Action action1;
    action1.type = ActionType::ADD;
    action1.newData.bookingID = "A001";

    Action action2;
    action2.type = ActionType::DELETE_ACTION;
    action2.oldData.bookingID = "A002";

    stack.push(action1);
    stack.push(action2);

    Action result;

    bool pop1 = stack.pop(result);
    bool correct1 =
        pop1 && result.type == ActionType::DELETE_ACTION;

    bool pop2 = stack.pop(result);
    bool correct2 =
        pop2 && result.type == ActionType::ADD;

    return correct1 && correct2 && stack.empty();
}

bool testPopEmptyStack() {
    Mystack stack;
    Action action;

    return !stack.pop(action) && stack.empty();
}

bool testUndoAdd() {
    renTalSystem system;

    renTal rental = {
        "T001",
        "Nguyen Van A",
        "59A11111",
        "20/09/2026",
        "22/09/2026"
    };

    bool added = system.addRental(rental);
    bool existsBefore =
        system.getRental("T001") != nullptr;

    bool undone = system.undoRental();

    bool removedAfter =
        system.getRental("T001") == nullptr;

    return added && existsBefore && undone && removedAfter;
}

bool testUndoUpdate() {
    renTalSystem system;

    renTal oldRental = {
        "T002",
        "Nguyen Van A",
        "59A22222",
        "20/09/2026",
        "22/09/2026"
    };

    renTal newRental = {
        "T002",
        "Nguyen Van B",
        "59A99999",
        "25/09/2026",
        "28/09/2026"
    };

    bool added = system.addRental(oldRental);
    system.clearHistory();

    bool updated = system.updateRental(
        "T002", newRental
    );

    bool undone = system.undoRental();

    const renTal* result =
        system.getRental("T002");

    bool restored =
        result != nullptr &&
        result->customerName == "Nguyen Van A" &&
        result->carPlate == "59A22222" &&
        result->starDate == "20/09/2026" &&
        result->endDate == "22/09/2026";

    return added && updated && undone && restored;
}

bool testUndoDelete() {
    renTalSystem system;

    renTal rental = {
        "T003",
        "Nguyen Van C",
        "59A33333",
        "23/09/2026",
        "24/09/2026"
    };

    bool added = system.addRental(rental);
    system.clearHistory();

    bool deleted = system.deleteRental("T003");

    bool absent =
        system.getRental("T003") == nullptr;

    bool undone = system.undoRental();

    const renTal* result =
        system.getRental("T003");

    bool restored =
        result != nullptr &&
        result->bookingID == "T003" &&
        result->customerName == "Nguyen Van C";

    return added && deleted && absent && undone && restored;
}

bool testUndoOrder() {
    renTalSystem system;

    renTal rental1 = {
        "T004",
        "Nguyen Van D",
        "59A44444",
        "01/09/2026",
        "02/09/2026"
    };

    renTal rental2 = {
        "T005",
        "Nguyen Van E",
        "59A55555",
        "03/09/2026",
        "04/09/2026"
    };

    system.addRental(rental1);
    system.addRental(rental2);

    bool undo1 = system.undoRental();

    bool t005Removed =
        system.getRental("T005") == nullptr;

    bool t004Exists =
        system.getRental("T004") != nullptr;

    bool undo2 = system.undoRental();

    bool t004Removed =
        system.getRental("T004") == nullptr;

    return undo1 && t005Removed &&
           t004Exists && undo2 && t004Removed;
}

void runAutomatedTests() {
    cout << "\n";
    cout << "====================================\n";
    cout << " AUTOMATED TEST\n";
    cout << "====================================\n";

    bool t1 = testStackLIFO();
    bool t2 = testPopEmptyStack();
    bool t3 = testUndoAdd();
    bool t4 = testUndoUpdate();
    bool t5 = testUndoDelete();
    bool t6 = testUndoOrder();

    cout << "Test Stack LIFO : "
         << (t1 ? "PASS" : "FAIL") << endl;

    cout << "Test Pop Stack rong : "
         << (t2 ? "PASS" : "FAIL") << endl;

    cout << "Test Undo ADD : "
         << (t3 ? "PASS" : "FAIL") << endl;

    cout << "Test Undo UPDATE : "
         << (t4 ? "PASS" : "FAIL") << endl;

    cout << "Test Undo DELETE : "
         << (t5 ? "PASS" : "FAIL") << endl;

    cout << "Test Undo LIFO : "
         << (t6 ? "PASS" : "FAIL") << endl;

    int passed =
        static_cast<int>(t1) +
        static_cast<int>(t2) +
        static_cast<int>(t3) +
        static_cast<int>(t4) +
        static_cast<int>(t5) +
        static_cast<int>(t6);

    cout << "------------------------------------\n";
    cout << "Ket qua: " << passed
         << "/6 test PASS" << endl;
    cout << "====================================\n";
}
