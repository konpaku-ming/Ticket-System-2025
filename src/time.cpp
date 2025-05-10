#include "../include/time.h"

Time::Time() = default;

Time::Time(const int h, const int m, const int d = 0) {
  hour = h;
  minute = m;
  day = d;
}

Time &Time::operator=(const Time &other) {
  if (this == &other) {
    return *this;
  }
  hour = other.hour;
  minute = other.minute;
  day = other.day;
  return *this;
}

bool Time::operator==(const Time &other) const {
  return hour == other.hour && minute == other.minute && day == other.day;
}

bool Time::operator!=(const Time &other) const {
  return hour != other.hour || minute != other.minute || day != other.day;
}

bool Time::operator>(const Time &other) const {
  if (day != other.day)return day > other.day;
  if (hour != other.hour)return hour > other.hour;
  return minute > other.minute;
}

bool Time::operator<(const Time &other) const {
  if (day != other.day)return day < other.day;
  if (hour != other.hour)return hour < other.hour;
  return minute < other.minute;
}

bool Time::operator>=(const Time &other) const {
  return *this > other || *this == other;
}

bool Time::operator<=(const Time &other) const {
  return *this < other || *this == other;
}

Time Time::operator+(const int n) const {
  Time new_time(this->hour, this->minute + n, this->day);
  new_time.hour += new_time.minute / 60;
  new_time.minute %= 60;
  new_time.day += new_time.hour / 24;
  new_time.hour %= 24;
  return new_time;
}

int Time::operator-(const Time &other) const {
  int ret = 0;
  ret += minute - other.minute;
  ret += (hour - other.hour) * 60;
  ret += (day - other.day) * 1440;
  return ret;
}
