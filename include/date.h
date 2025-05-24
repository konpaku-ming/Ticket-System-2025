#ifndef DATE_H
#define DATE_H
#include <iostream>

inline int day_num[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct Date {
  int month;
  int day;

  Date();

  Date(int m, int d);

  Date &operator=(const Date &other);

  Date operator+(int n) const;

  Date operator-(int n) const;

  bool operator==(const Date &other) const;

  bool operator!=(const Date &other) const;

  bool operator<(const Date &other) const;

  bool operator>(const Date &other) const;

  bool operator<=(const Date &other) const;

  bool operator>=(const Date &other) const;

  int operator-(const Date &other) const; //返回两个日期相差的天数

  friend std::ostream &operator<<(std::ostream &out, const Date &x);
};

#endif //DATE_H
