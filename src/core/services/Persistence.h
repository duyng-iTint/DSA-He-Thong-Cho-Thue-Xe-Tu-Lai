// Persistence.h
// -------------
// TANG PERSISTENCE (Muc 7.3) - nhiem vu CHI la luu tru ben vung
// (load/save), KHONG duoc chua logic tra cuu/loc/sap xep.
//
// QUAN TRONG (Muc 18 - loi thuong gap bi tru diem):
// Khong dung thu vien/logic de tu tra loi MC1/MC2 thay cho cau truc
// du lieu trong RAM. O day ta chi doc/ghi THO tung dong CSV bang tay
// (khong dung thu vien CSV ngoai, chi dung <fstream>/<sstream> chuan),
// moi tra cuu thuc su (search theo booking_id/bien_so) deu chay tren
// MyHashTable o tang DSA Core, KHONG chay tren file hay tren vector
// vua doc duoc.
//
// storage_ trong PersistenceContext SO HUU cac object Booking (bang
// std::vector<std::unique_ptr<Booking>>), con 2 bang bam chi luu
// Booking* (con tro tho, khong so huu) tro vao do - tranh double free
// va tranh con tro treo khi resize (vi storage khong bi di chuyen).

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../model/Booking.h"
#include "../structures/MyHashTable.h"

// Noi so huu that su cac Booking (tranh memory leak / dangling pointer).
// Hai bang bam (theo booking_id va theo bien_so) chi giu Booking* tro
// vao cac phan tu trong storage nay.
struct PersistenceContext {
    std::vector<std::unique_ptr<Booking>> storage;

    Booking* addBooking(const Booking& b) {
        storage.push_back(std::make_unique<Booking>(b));
        return storage.back().get();
    }
};

// Doc file CSV va nap du lieu vao 2 bang bam:
// - tableById: khoa = booking_id (dung cho tra cuu theo ma don)
// - tableByPlate: khoa = bien_so (dung cho tra cuu theo bien so)
// Booking thuc su duoc luu trong ctx.storage; 2 bang bam chi giu con tro.
// Neu file chua ton tai (lan chay dau tien), tra ve 0 ma khong loi.
// Tra ve so dong da nap thanh cong.
int loadIntoHashTable(const std::string& csvPath,
                       PersistenceContext& ctx,
                       MyHashTable<Booking*>& tableById,
                       MyHashTable<Booking*>& tableByPlate);

// Ghi toan bo ban ghi hien co trong tableById ra file CSV.
// Chi can duyet 1 trong 2 bang (tableById) vi ca 2 bang cung tro toi
// cung cac Booking - khong bi trung lap du lieu.
// Tra ve so dong da ghi.
int saveFromHashTable(const std::string& csvPath,
                       const MyHashTable<Booking*>& tableById);
