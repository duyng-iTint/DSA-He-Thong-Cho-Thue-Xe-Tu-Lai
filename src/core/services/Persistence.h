// Persistence.h

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../model/Booking.h"
#include "../structures/MyHashTable.h"

struct PersistenceContext {
    std::vector<std::unique_ptr<Booking>> storage;

    Booking* addBooking(const Booking& b) {
        storage.push_back(std::make_unique<Booking>(b));
        return storage.back().get();
    }
};

int loadIntoHashTable(const std::string& csvPath,
                       PersistenceContext& ctx,
                       MyHashTable<Booking*>& tableById,
                       MyHashTable<Booking*>& tableByPlate);
int saveFromHashTable(const std::string& csvPath,
                       const MyHashTable<Booking*>& tableById);
