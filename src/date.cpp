#include "../include/date.h"

Date::Date() = default;

Date::Date(const int m, const int d) {
  month = m;
  day = d;
}

Date &Date::operator=(const Date &other) {
  if (this == &other) {
    return *this;
  }
  month = other.month;
  day = other.day;
  return *this;
}

Date Date::operator+(int n) const {
  Date new_date(this->month, this->day + n);
  while (new_date.day > day_num[new_date.month]) {
    new_date.day -= day_num[new_date.month];
    new_date.month++;
  }
  return new_date;
}

Date Date::operator-(int n) const {
  Date new_date(this->month, this->day - n);
  while (new_date.day <= 0) {
    new_date.month--;
    new_date.day += day_num[new_date.month];
  }
  return new_date;
}

bool Date::operator==(const Date &other) const {
  return month == other.month && day == other.day;
}

bool Date::operator!=(const Date &other) const {
  return month != other.month || day != other.day;
}

bool Date::operator>(const Date &other) const {
  if (month != other.month) {
    return month > other.month;
  }
  return day > other.day;
}

bool Date::operator<(const Date &other) const {
  if (month != other.month) {
    return month < other.month;
  }
  return day < other.day;
}

bool Date::operator>=(const Date &other) const {
  return *this > other || *this == other;
}

bool Date::operator<=(const Date &other) const {
  return *this < other || *this == other;
}

int Date::operator-(const Date &other) const {
  int ret = 0;
  for (int i = other.month; i <= month - 1; i++) {
    ret += day_num[i];
  }
  ret += day - other.day;
  return ret;
}

std::ostream &operator<<(std::ostream &out, const Date &x) {
  if (x.month < 10) {
    out << 0;
  }
  out << x.month << "-";
  if (x.day < 10) {
    out << 0;
  }
  out << x.day;
  return out;
}
