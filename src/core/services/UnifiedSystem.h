// UnifiedSystem.h
#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "../model/Booking.h"
#include "../model/RentalRecord.h"
#include "../model/Rental.h"
#include "../structures/MyHashTable.h"
#include "../structures/MyMaxHeap.h"
#include "../structures/Trie.h"
#include "../structures/UndoStack.h"
#include "Persistence.h"
#include "RentalService.h"

class UnifiedSystem {
public:
    UnifiedSystem();
    UnifiedSystem(const UnifiedSystem&) = delete;
    UnifiedSystem& operator=(const UnifiedSystem&) = delete;

    // ---------------- Persistence ----------------
    int loadFromCsv(const std::string& path);
    int saveToCsv(const std::string& path) const;
    bool dirty() const { return dirty_; }        

    // ---------------- MC1 ----------------
    const Booking* findById(const std::string& bookingId) const;
    const Booking* findByPlate(const std::string& plate) const;
    MyHashTable<Booking*>::Stats hashStats() const;
    std::size_t size() const;

    // ---------------- MC2 ----------------
    std::vector<RentalRecord> queryRange(const std::string& fromDate, const std::string& toDate) const;
    std::vector<CarStat> topCars(int k) const;
    void benchmarkRange(const std::string& fromDate, const std::string& toDate) const;
    const std::vector<RentalRecord>& records() const { return sorted_; }

    // ---------------- RF1 ----------------
    std::vector<std::string> suggest(const std::string& prefix) const;
    int catalogSize() const { return catalogCount_; }

    // ---------------- RF2 ----------------
    bool submitDispute(Booking request, std::string& error);
    bool peekDispute(RentalRequest& out) const;
    int disputeCount() const;

    bool processNextDispute(Booking& winner, std::vector<std::string>& rejectedIds, std::string& error);
    bool cancelDispute(const std::string& bookingId);
    std::vector<Booking> pendingDisputes() const;

    // ---------------- RF3 ----------------
    bool createBooking(Booking b, std::string& error);
    bool updateBooking(const std::string& bookingId,
                       const std::string& newCustomer,
                       const std::string& newPlate,
                       const std::string& newStart,
                       const std::string& newEnd,
                       std::string& error);
    bool deleteBooking(const std::string& bookingId, std::string& error);
    bool undo(std::string& message);
    int undoCount() const;
    bool topUndo(Action& action) const;

    // Chap nhan YYYY-MM-DD hoac DD/MM/YYYY, tra ve YYYY-MM-DD (rong neu khong hop le).
    static std::string normalizeDate(const std::string& raw);

private:
    PersistenceContext ctx_;                 
    MyHashTable<Booking*> byId_;            
    MyHashTable<Booking*> byPlate_;         
    MyHashTable<Booking*> removed_;         
    std::vector<RentalRecord> sorted_;       

    std::unique_ptr<CarTrie> trie_;         
    MyHashTable<int> catalogSeen_;       
    int catalogCount_;

    MyMaxHeap heap_;                         
    std::vector<std::unique_ptr<Booking>> pendingStore_;
    MyHashTable<Booking*> pendingById_;     

    UnoManager undo_;                        
    int deleteSerial_;
    bool dirty_;

    void resetAll();
    void seedCatalog();
    void registerCar(const std::string& brand, const std::string& model);

   
    void attach(Booking* b);
    void detach(Booking* b);
    void insertSorted(const RentalRecord& r);
    void removeSorted(const std::string& rentDate, const std::string& bookingId);

    bool prepare(Booking& b, std::string& error) const;   
    static renTal toRenTal(const Booking& b);
    static RentalRequest toRequest(const Booking& b);
};
