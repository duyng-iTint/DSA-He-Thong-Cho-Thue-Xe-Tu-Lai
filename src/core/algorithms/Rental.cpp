#include "../model/Rental.h"

namespace {
bool parseDate(const string& value, int& day, int& month, int& year) {
    if (value.size() != 10) return false;
    if (value[4] == '-' && value[7] == '-') {
        for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) if (value[i] < '0' || value[i] > '9') return false;
        year = stoi(value.substr(0, 4)); month = stoi(value.substr(5, 2)); day = stoi(value.substr(8, 2));
    } else if (value[2] == '/' && value[5] == '/') {
        for (int i : {0, 1, 3, 4, 6, 7, 8, 9}) if (value[i] < '0' || value[i] > '9') return false;
        day = stoi(value.substr(0, 2)); month = stoi(value.substr(3, 2)); year = stoi(value.substr(6, 4));
    } else return false;
    if (year <= 0 || month < 1 || month > 12) return false;
    return day >= 1 && day <= daysInMonth(month, year);
}
}

bool isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return year % 4 == 0;
}

int daysInMonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30,
                  31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;

    return days[month - 1];
}

int dateToNumber(int day, int month, int year) {
    return year * 10000 + month * 100 + day;
}

bool isValidDate(const string& date) {
    try { int day, month, year; return parseDate(date, day, month, year); }
    catch (...) { return false; }
}

bool isStartBeforeOrEqualEnd(
    const string& startDate,
    const string& endDate
) {
    int startDay, startMonth, startYear, endDay, endMonth, endYear;
    if (!parseDate(startDate, startDay, startMonth, startYear) ||
        !parseDate(endDate, endDay, endMonth, endYear)) return false;

    int start = dateToNumber(startDay, startMonth, startYear);
    int end = dateToNumber(endDay, endMonth, endYear);

    return start <= end;
}
