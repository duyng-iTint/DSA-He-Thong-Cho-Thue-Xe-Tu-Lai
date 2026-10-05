#pragma once
#include <string>

#include "../core/services/UnifiedSystem.h"


class CliApp {
private:
    UnifiedSystem sys;
    std::string csvPath;

    void ensureData();
    void saveData();

    void showMainMenu();
    void menuMC1();
    void menuMC2();
    void menuRF1();
    void menuRF2();
    void menuRF3();

public:
    void run();
};
