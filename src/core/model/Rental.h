#ifndef RENTAL_H
#define RENTAL_H

#include <iostream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

struct renTal {
    string bookingID;
    string customerName;
    string carPlate;
    string starDate;
    string endDate;

    void print() const {
        cout << "Booking ID: " << bookingID << endl;
        cout << "Ten khach hang: " << customerName << endl;
        cout << "Bien so: " << carPlate << endl;
        cout << "Ngay bat dau: " << starDate << endl;
        cout << "Ngay ket thuc: " << endDate << endl;
    }
};

// Cac ham xu ly ngay
bool isLeapYear(int year);
int daysInMonth(int month, int year);
int dateToNumber(int day, int month, int year);
bool isValidDate(const string& date);
bool isStartBeforeOrEqualEnd(const string& startDate, const string& endDate);

#endif
