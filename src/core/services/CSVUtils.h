// CSVUtils.h
// Doc file donthue_xe.csv thanh vector<RentalRecord>.
// LUU Y: Day la ban doc du lieu RIENG cua module MC2 de co the chay doc lap/demo/test.
// Trong he thong hoan chinh, tang Persistence (Thanh vien 1) se chiu trach nhiem
// nap/ghi file chinh thuc; MC2 chi nhan lai vector<RentalRecord> tu tang do.
// Dinh dang moi dong CSV (khong co dong tieu de bi tinh vao du lieu):
// BookingID,CarPlate,CarBrand,CarModel,RentDate,ReturnDate,Price,CustomerName,MemberRank
#pragma once
#include <vector>
#include <string>
#include "../model/RentalRecord.h"

// Doc toan bo file CSV. Tra ve true neu doc thanh cong.
bool loadRentalCSV(const std::string& filePath, std::vector<RentalRecord>& outRecords);
