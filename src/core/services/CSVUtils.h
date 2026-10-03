// CSVUtils.h
// Doc unified booking CSV thanh RentalRecord; van doc duoc schema MC2 cu.
// Adapter giu cho demo/test MC2 doc lap; he thong dung Persistence lam nguon chinh.
// Unified schema va hai ten cot MC2 cu deu duoc nhan dang theo header.
#pragma once
#include <vector>
#include <string>
#include "../model/RentalRecord.h"

// Doc toan bo file CSV. Tra ve true neu doc thanh cong.
bool loadRentalCSV(const std::string& filePath, std::vector<RentalRecord>& outRecords);
