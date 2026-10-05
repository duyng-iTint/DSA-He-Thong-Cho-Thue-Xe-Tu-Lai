// project/tools/DataGenerator.h
// -------------------------------
// Cong cu ho tro (KHONG phai DSA Core, KHONG phai Persistence that):
// sinh file CSV gia lap voi N ban ghi, dung de demo va benchmark.
// Du lieu ngau nhien nhung co seed co dinh de tai lap duoc.

#pragma once

#include <string>

void generateCsv(const std::string& csvPath, int nRecords, unsigned int seed = 42);
