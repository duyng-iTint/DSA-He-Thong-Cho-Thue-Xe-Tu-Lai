#pragma once

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace CsvCodec {
inline std::vector<std::string> parseRow(const std::string& raw) {
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < raw.size(); ++i) {
        const char ch = raw[i];
        if (ch == '"' && quoted && i + 1 < raw.size() && raw[i + 1] == '"') { field.push_back('"'); ++i; }
        else if (ch == '"') quoted = !quoted;
        else if (ch == ',' && !quoted) { fields.push_back(field); field.clear(); }
        else if (ch != '\r') field.push_back(ch);
    }
    fields.push_back(field);
    if (!fields.empty() && fields[0].size() >= 3 && static_cast<unsigned char>(fields[0][0]) == 0xEF &&
        static_cast<unsigned char>(fields[0][1]) == 0xBB && static_cast<unsigned char>(fields[0][2]) == 0xBF) fields[0].erase(0, 3);
    return fields;
}

inline std::string encodeField(const std::string& value) {
    if (value.find_first_of(",\"\r\n") == std::string::npos) return value;
    std::string out = "\"";
    for (char ch : value) { if (ch == '"') out += "\"\""; else out += ch; }
    return out + '"';
}

inline std::string encodeRow(const std::vector<std::string>& row) {
    std::ostringstream out;
    for (std::size_t i = 0; i < row.size(); ++i) { if (i) out << ','; out << encodeField(row[i]); }
    return out.str();
}

inline std::string normalizeHeader(std::string header) {
    std::string normalized;
    for (unsigned char ch : header) if (std::isalnum(ch)) normalized.push_back(static_cast<char>(std::tolower(ch)));
    return normalized;
}

inline int column(const std::vector<std::string>& header, const std::string& name) {
    const std::string target = normalizeHeader(name);
    for (std::size_t i = 0; i < header.size(); ++i) if (normalizeHeader(header[i]) == target) return static_cast<int>(i);
    return -1;
}

inline std::string value(const std::vector<std::string>& row, const std::vector<std::string>& header,
                         const std::string& name, const std::string& fallback = "") {
    const int index = column(header, name);
    return index >= 0 && static_cast<std::size_t>(index) < row.size() ? row[index] : fallback;
}
}
