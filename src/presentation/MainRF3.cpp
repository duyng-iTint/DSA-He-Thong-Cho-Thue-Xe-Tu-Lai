#include "presentation/InputHandler.h"
#include "../../test/Tests.h"
#include "../../benchmark/Benchmark.h"

void runProgram() {
    renTalSystem system;
    int choice;

    do {
        showMenu();

        cout << "Nhap lua chon: ";
        cin >> choice;

        switch (choice) {
            case 1:
                handleAdd(system);
                break;

            case 2:
                handleUpdate(system);
                break;

            case 3:
                handleDelete(system);
                break;

            case 4:
                handleSearch(system);
                break;

            case 5:
                system.showAll();
                break;

            case 6:
                handleUndo(system);
                break;

            case 7:
                cout << "\nSo thao tac co the Undo: "
                     << system.historySize()
                     << endl;
                break;

            case 8:
                showTopUndo(system);
                break;

            case 9:
                runAutomatedTests();
                break;

            case 10:
                runBenchmark();
                break;

            case 0:
                cout << "\nKet thuc chuong trinh!\n";
                break;

            default:
                cout << "\nLua chon khong hop le!\n";
        }

    } while (choice != 0);
}

int main() {
    runProgram();
    return 0;
}
