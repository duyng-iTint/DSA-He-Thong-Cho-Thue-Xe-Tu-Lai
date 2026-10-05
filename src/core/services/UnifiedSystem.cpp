// UnifiedSystem.cpp
#include "UnifiedSystem.h"

#include <chrono>
#include <utility>

#include "../algorithms/BinarySearch.h"

namespace {

    std::string trimCopy(const std::string& s) {
        std::size_t start = 0;
        while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) ++start;
        std::size_t end = s.size();
        while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) --end;
        return s.substr(start, end - start);
    }

    long long nowMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    const char* DEFAULT_CARS[][2] = {
        {"Toyota", "Vios"}, {"Toyota", "Innova"}, {"Hyundai", "Accent"},
        {"Hyundai", "i10"}, {"Hyundai", "Grand i10"}, {"Kia", "Morning"},
        {"Kia", "Seltos"}, {"Honda", "City"}, {"Mazda", "CX-5"},
        {"Mitsubishi", "Xpander"}, {"Ford", "Everest"}, {"VinFast", "VF5"}
    };

}
// Khoi tao / reset


UnifiedSystem::UnifiedSystem()
    : trie_(new CarTrie()), catalogCount_(0), deleteSerial_(0), dirty_(false) {
    seedCatalog();
}

void UnifiedSystem::seedCatalog() {
    for (const auto& car : DEFAULT_CARS) registerCar(car[0], car[1]);
}

void UnifiedSystem::registerCar(const std::string& brand, const std::string& model) {
    if (brand.empty() && model.empty()) return;
    const std::string key = brand + "|" + model;
    if (catalogSeen_.contains(key)) return;
    catalogSeen_.insert(key, 1);
    trie_->insertCar(brand, model);
    ++catalogCount_;
}

void UnifiedSystem::resetAll() {
    byId_.clear();
    byPlate_.clear();
    removed_.clear();
    pendingById_.clear();
    pendingStore_.clear();
    ctx_.storage.clear();
    sorted_.clear();
    heap_.BuildHeap(std::vector<RentalRequest>());
    undo_.clearHistory();
    trie_.reset(new CarTrie());
    catalogSeen_.clear();
    catalogCount_ = 0;
    deleteSerial_ = 0;
    seedCatalog();
    dirty_ = false;
}


// Persistence
int UnifiedSystem::loadFromCsv(const std::string& path) {
    resetAll();

    loadIntoHashTable(path, ctx_, byId_, byPlate_);

    const auto items = byId_.allItems();
    sorted_.reserve(items.size());
    for (const auto& kv : items) sorted_.push_back(RentalRecord::fromBooking(*kv.second));
    RentalService::sortByRentDate(sorted_);

    for (const auto& r : sorted_) registerCar(r.carBrand, r.carModel);
    return static_cast<int>(byId_.size());
}

int UnifiedSystem::saveToCsv(const std::string& path) const {
    return saveFromHashTable(path, byId_);
}
// MC1


const Booking* UnifiedSystem::findById(const std::string& bookingId) const {
    Booking* p = nullptr;
    if (byId_.search(bookingId, p)) return p;
    return nullptr;
}

const Booking* UnifiedSystem::findByPlate(const std::string& plate) const {
    Booking* p = nullptr;
    if (byPlate_.search(plate, p)) return p;
    return nullptr;
}

MyHashTable<Booking*>::Stats UnifiedSystem::hashStats() const { return byId_.stats(); }

std::size_t UnifiedSystem::size() const { return byId_.size(); }

// MC2

std::vector<RentalRecord> UnifiedSystem::queryRange(const std::string& fromDate, const std::string& toDate) const {
    return RentalService::queryByDateRange(sorted_, fromDate, toDate);
}

std::vector<CarStat> UnifiedSystem::topCars(int k) const {
    return RentalService::topRentedCars(RentalService::buildCarStats(sorted_), k);
}

void UnifiedSystem::benchmarkRange(const std::string& fromDate, const std::string& toDate) const {
    RentalService::benchmarkRangeQuery(sorted_, fromDate, toDate);
}

// RF1

std::vector<std::string> UnifiedSystem::suggest(const std::string& prefix) const {
    return trie_->getAllSuggestions(prefix);
}

void UnifiedSystem::insertSorted(const RentalRecord& r) {
    const int pos = upperBoundByDate(sorted_, r.rentDate);
    sorted_.insert(sorted_.begin() + pos, r);
}

void UnifiedSystem::removeSorted(const std::string& rentDate, const std::string& bookingId) {
    const int lo = lowerBoundByDate(sorted_, rentDate);
    const int hi = upperBoundByDate(sorted_, rentDate);
    for (int i = lo; i < hi; ++i) {
        if (sorted_[i].bookingId == bookingId) {
            sorted_.erase(sorted_.begin() + i);
            return;
        }
    }
}

void UnifiedSystem::attach(Booking* b) {
    byPlate_.insert(b->bien_so, b);
    insertSorted(RentalRecord::fromBooking(*b));
}

void UnifiedSystem::detach(Booking* b) {
    Booking* mapped = nullptr;
    if (byPlate_.search(b->bien_so, mapped) && mapped == b) byPlate_.remove(b->bien_so);
    removeSorted(b->ngay_bat_dau, b->booking_id);
}

// Kiem tra du lieu

std::string UnifiedSystem::normalizeDate(const std::string& raw) {
    const std::string s = trimCopy(raw);
    if (!isValidDate(s)) return "";
    if (s[4] == '-') return s;                                        // da la YYYY-MM-DD
    return s.substr(6, 4) + "-" + s.substr(3, 2) + "-" + s.substr(0, 2); // DD/MM/YYYY -> YYYY-MM-DD
}

bool UnifiedSystem::prepare(Booking& b, std::string& error) const {
    b.booking_id = trimCopy(b.booking_id);
    b.bien_so = trimCopy(b.bien_so);
    b.ten_khach = trimCopy(b.ten_khach);
    b.hang_xe = trimCopy(b.hang_xe);
    b.dong_xe = trimCopy(b.dong_xe);

    if (b.booking_id.empty()) { error = "Booking_ID khong duoc de trong."; return false; }
    if (b.bien_so.empty()) { error = "Bien so khong duoc de trong.";    return false; }

    const std::string start = normalizeDate(b.ngay_bat_dau);
    if (start.empty()) { error = "Ngay bat dau khong hop le."; return false; }
    const std::string end = normalizeDate(b.ngay_ket_thuc);
    if (end.empty()) { error = "Ngay ket thuc khong hop le."; return false; }
    if (!isStartBeforeOrEqualEnd(start, end)) { error = "Ngay ket thuc phai >= ngay bat dau."; return false; }
    b.ngay_bat_dau = start;
    b.ngay_ket_thuc = end;

    if (b.hang_thanh_vien < 0 || b.hang_thanh_vien > 3) { error = "Hang thanh vien phai tu 0 den 3."; return false; }
    if (b.trang_thai.empty()) b.trang_thai = "DANG_THUE";
    if (b.thoi_diem_dat == 0) b.thoi_diem_dat = nowMillis();
    return true;
}

renTal UnifiedSystem::toRenTal(const Booking& b) {
    renTal r;
    r.bookingID = b.booking_id;
    r.customerName = b.ten_khach;
    r.carPlate = b.bien_so;
    r.starDate = b.ngay_bat_dau;
    r.endDate = b.ngay_ket_thuc;
    return r;
}

RentalRequest UnifiedSystem::toRequest(const Booking& b) {
    RentalRequest q;
    q.bookingId = b.booking_id;
    q.customerId = b.ten_khach;
    q.carId = b.bien_so;
    q.membershipTier = b.hang_thanh_vien;
    q.bookingTimestamp = b.thoi_diem_dat;
    return q;
}

// RF3 - Tao / Sua / Xoa / Undo (dung UnoManager cua ban RF3)

bool UnifiedSystem::createBooking(Booking b, std::string& error) {
    if (!prepare(b, error)) return false;
    if (byId_.contains(b.booking_id)) { error = "Booking_ID da ton tai."; return false; }

    Booking* p = ctx_.addBooking(b);
    byId_.insert(p->booking_id, p);
    attach(p);
    registerCar(p->hang_xe, p->dong_xe);

    Action a;
    a.type = ActionType::ADD;
    a.newData = toRenTal(*p);
    undo_.saveAction(a);

    dirty_ = true;
    return true;
}

bool UnifiedSystem::updateBooking(const std::string& bookingId,
    const std::string& newCustomer,
    const std::string& newPlate,
    const std::string& newStart,
    const std::string& newEnd,
    std::string& error) {
    Booking* p = nullptr;
    if (!byId_.search(trimCopy(bookingId), p)) { error = "Khong tim thay Booking_ID."; return false; }

    Booking candidate = *p;
    if (!trimCopy(newCustomer).empty()) candidate.ten_khach = newCustomer;
    if (!trimCopy(newPlate).empty())    candidate.bien_so = newPlate;
    if (!trimCopy(newStart).empty())    candidate.ngay_bat_dau = newStart;
    if (!trimCopy(newEnd).empty())      candidate.ngay_ket_thuc = newEnd;
    if (!prepare(candidate, error)) return false;

    Action a;
    a.type = ActionType::UPDATE;
    a.oldData = toRenTal(*p);
    a.newData = toRenTal(candidate);

    detach(p);
    p->ten_khach = candidate.ten_khach;
    p->bien_so = candidate.bien_so;
    p->ngay_bat_dau = candidate.ngay_bat_dau;
    p->ngay_ket_thuc = candidate.ngay_ket_thuc;
    attach(p);

    undo_.saveAction(a);
    dirty_ = true;
    return true;
}

bool UnifiedSystem::deleteBooking(const std::string& bookingId, std::string& error) {
    const std::string id = trimCopy(bookingId);
    Booking* p = nullptr;
    if (!byId_.search(id, p)) { error = "Khong tim thay Booking_ID."; return false; }

    const int serial = ++deleteSerial_;
    Action a;
    a.type = ActionType::DELETE_ACTION;
    a.oldData = toRenTal(*p);
    a.position = serial;

    detach(p);
    byId_.remove(id);
    removed_.insert(std::to_string(serial), p);

    undo_.saveAction(a);
    dirty_ = true;
    return true;
}

bool UnifiedSystem::undo(std::string& message) {
    Action a;
    if (!undo_.undo(a)) { message = "Khong co thao tac nao de Undo."; return false; }

    Booking* p = nullptr;
    switch (a.type) {
    case ActionType::ADD:
        if (byId_.search(a.newData.bookingID, p)) {
            detach(p);
            byId_.remove(a.newData.bookingID);
        }
        message = "Da hoan tac THEM don " + a.newData.bookingID + ".";
        break;

    case ActionType::UPDATE:
        if (byId_.search(a.oldData.bookingID, p)) {
            detach(p);
            p->ten_khach = a.oldData.customerName;
            p->bien_so = a.oldData.carPlate;
            p->ngay_bat_dau = a.oldData.starDate;
            p->ngay_ket_thuc = a.oldData.endDate;
            attach(p);
        }
        message = "Da hoan tac SUA don " + a.oldData.bookingID + ".";
        break;

    case ActionType::DELETE_ACTION: {
        const std::string key = std::to_string(a.position);
        if (removed_.search(key, p)) {
            byId_.insert(p->booking_id, p);
            attach(p);
            removed_.remove(key);
        }
        message = "Da hoan tac XOA don " + a.oldData.bookingID + ".";
        break;
    }
    }

    dirty_ = true;
    return true;
}

int UnifiedSystem::undoCount() const { return undo_.historySize(); }

bool UnifiedSystem::topUndo(Action& action) const { return undo_.top(action); }

// RF2 - Hang doi uu tien khi tranh chap xe (MyMaxHeap)

bool UnifiedSystem::submitDispute(Booking request, std::string& error) {
    if (!prepare(request, error)) return false;
    if (byId_.contains(request.booking_id) || pendingById_.contains(request.booking_id)) {
        error = "Booking_ID da ton tai (don that hoac yeu cau dang cho).";
        return false;
    }

    if (!heap_.InsertRequest(toRequest(request))) { error = "Khong the them vao hang doi."; return false; }

    pendingStore_.push_back(std::make_unique<Booking>(request));
    pendingById_.insert(request.booking_id, pendingStore_.back().get());
    return true;
}

bool UnifiedSystem::peekDispute(RentalRequest& out) const {
    if (heap_.Empty()) return false;
    out = heap_.Top();
    return true;
}

int UnifiedSystem::disputeCount() const { return heap_.Size(); }

bool UnifiedSystem::processNextDispute(Booking& winner,
    std::vector<std::string>& rejectedIds,
    std::string& error) {
    rejectedIds.clear();
    if (heap_.Empty()) { error = "Khong co yeu cau nao dang cho."; return false; }

    const RentalRequest top = heap_.ExtractMax();
    Booking* pending = nullptr;
    if (!pendingById_.search(top.bookingId, pending)) { error = "Du lieu yeu cau bi mat dong bo."; return false; }
    Booking booking = *pending;
    pendingById_.remove(top.bookingId);

    if (!createBooking(booking, error)) return false;
    winner = booking;

    for (const auto& kv : pendingById_.allItems()) {
        if (kv.second->bien_so == booking.bien_so) {
            heap_.RemoveById(kv.first);
            pendingById_.remove(kv.first);
            rejectedIds.push_back(kv.first);
        }
    }
    return true;
}

bool UnifiedSystem::cancelDispute(const std::string& bookingId) {
    const std::string id = trimCopy(bookingId);
    if (!pendingById_.contains(id)) return false;
    heap_.RemoveById(id);
    pendingById_.remove(id);
    return true;
}

std::vector<Booking> UnifiedSystem::pendingDisputes() const {
    std::vector<Booking> result;
    for (const auto& kv : pendingById_.allItems()) result.push_back(*kv.second);
    return result;
}
