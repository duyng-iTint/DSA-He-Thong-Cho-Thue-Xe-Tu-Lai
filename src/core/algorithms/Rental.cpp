#include "../model/Rental.h"

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
    if (date.length() != 10) return false;

    if (date[2] != '/' || date[5] != '/')
        return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;

        if (date[i] < '0' || date[i] > '9')
            return false;
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (year <= 0) return false;
    if (month < 1 || month > 12) return false;

    int maxDay = daysInMonth(month, year);

    if (day < 1 || day > maxDay)
        return false;

    return true;
}

bool isStartBeforeOrEqualEnd(
    const string& startDate,
    const string& endDate
) {
    int startDay = stoi(startDate.substr(0, 2));
    int startMonth = stoi(startDate.substr(3, 2));
    int startYear = stoi(startDate.substr(6, 4));

    int endDay = stoi(endDate.substr(0, 2));
    int endMonth = stoi(endDate.substr(3, 2));
    int endYear = stoi(endDate.substr(6, 4));

    int start = dateToNumber(startDay, startMonth, startYear);
    int end = dateToNumber(endDay, endMonth, endYear);

    return start <= end;
}
