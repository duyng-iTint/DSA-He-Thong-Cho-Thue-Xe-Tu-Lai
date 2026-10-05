#include "InputHandler.h"

renTal inputRental() {
    renTal rental;

    cout << "\n===== NHAP DON THUE =====\n";
    cout << "Booking ID: ";
    cin >> rental.bookingID;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Ten khach hang: ";
    getline(cin, rental.customerName);

    cout << "Bien so: ";
    getline(cin, rental.carPlate);

    while (true) {
        cout << "Ngay bat dau (DD/MM/YYYY): ";
        getline(cin, rental.starDate);

        if (isValidDate(rental.starDate))
            break;

        cout << "Ngay bat dau khong hop le! Vui long nhap lai.\n";
    }

    while (true) {
        cout << "Ngay ket thuc (DD/MM/YYYY): ";
        getline(cin, rental.endDate);

        if (!isValidDate(rental.endDate)) {
            cout << "Ngay ket thuc khong hop le! Vui long nhap lai.\n";
            continue;
        }

        if (!isStartBeforeOrEqualEnd(
                rental.starDate, rental.endDate)) {
            cout << "Ngay ket thuc phai >= ngay bat dau!\n";
            continue;
        }

        break;
    }

    return rental;
}

renTal inputUpdatedRental(const string& bookingID) {
    renTal rental;
    rental.bookingID = bookingID;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Ten khach hang moi: ";
    getline(cin, rental.customerName);

    cout << "Bien so moi: ";
    getline(cin, rental.carPlate);

    while (true) {
        cout << "Ngay bat dau moi (DD/MM/YYYY): ";
        getline(cin, rental.starDate);

        if (isValidDate(rental.starDate))
            break;

        cout << "Ngay bat dau khong hop le! Vui long nhap lai.\n";
    }

    while (true) {
        cout << "Ngay ket thuc moi (DD/MM/YYYY): ";
        getline(cin, rental.endDate);

        if (!isValidDate(rental.endDate)) {
            cout << "Ngay ket thuc khong hop le! Vui long nhap lai.\n";
            continue;
        }

        if (!isStartBeforeOrEqualEnd(
                rental.starDate, rental.endDate)) {
            cout << "Ngay ket thuc phai >= ngay bat dau!\n";
            continue;
        }

        break;
    }

    return rental;
}

void handleAdd(renTalSystem& system) {
    renTal rental = inputRental();

    if (system.addRental(rental))
        cout << "\nThem don thue thanh cong!\n";
    else
        cout << "\nThem that bai! Booking ID da ton tai hoac ngay khong hop le.\n";
}

void handleUpdate(renTalSystem& system) {
    string bookingID;

    cout << "\nNhap Booking ID can cap nhat: ";
    cin >> bookingID;

    if (system.getRental(bookingID) == nullptr) {
        cout << "\nKhong tim thay Booking ID!\n";
        return;
    }

    renTal newRental = inputUpdatedRental(bookingID);

    if (system.updateRental(bookingID, newRental))
        cout << "\nCap nhat thanh cong!\n";
    else
        cout << "\nCap nhat that bai!\n";
}

void handleDelete(renTalSystem& system) {
    string bookingID;

    cout << "\nNhap Booking ID can xoa: ";
    cin >> bookingID;

    if (system.deleteRental(bookingID))
        cout << "\nXoa don thue thanh cong!\n";
    else
        cout << "\nKhong tim thay don thue!\n";
}

void handleSearch(const renTalSystem& system) {
    string bookingID;

    cout << "\nNhap Booking ID can tim: ";
    cin >> bookingID;

    system.searchRental(bookingID);
}

void handleUndo(renTalSystem& system) {
    if (system.undoRental())
        cout << "\nUndo thanh cong!\n";
    else
        cout << "\nKhong co thao tac nao de Undo!\n";
}

void showMenu() {
    cout << "\n";
    cout << "====================================\n";
    cout << " HE THONG QUAN LY CHO THUE XE\n";
    cout << "====================================\n";
    cout << "1. Them don thue\n";
    cout << "2. Cap nhat don thue\n";
    cout << "3. Xoa don thue\n";
    cout << "4. Tim kiem don thue\n";
    cout << "5. Hien thi tat ca\n";
    cout << "6. Undo\n";
    cout << "7. Xem so luong thao tac Undo\n";
    cout << "8. Xem thao tac Undo gan nhat\n";
    cout << "9. Automated Test\n";
    cout << "10. Benchmark\n";
    cout << "0. Thoat\n";
    cout << "====================================\n";
}

string actionName(ActionType type) {
    switch (type) {
        case ActionType::ADD:
            return "ADD";
        case ActionType::UPDATE:
            return "UPDATE";
        case ActionType::DELETE_ACTION:
            return "DELETE_ACTION";
    }

    return "UNKNOWN";
}

void showTopUndo(const renTalSystem& system) {
    Action action;

    if (!system.getTopUndo(action)) {
        cout << "\nKhong co lich su Undo!\n";
        return;
    }

    cout << "\nThao tac gan nhat: "
         << actionName(action.type) << endl;

    if (action.type == ActionType::ADD) {
        cout << "Booking ID: "
             << action.newData.bookingID << endl;
    }
    else {
        cout << "Booking ID: "
             << action.oldData.bookingID << endl;
    }
}
