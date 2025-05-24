#ifndef TIME_H
#define TIME_H

#include <iostream>

struct Time {
  int hour;
  int minute;
  int day; //始发之后第几天

  Time();

  Time(int h, int m, int d);

  Time &operator=(const Time &other);

  Time operator+(int n) const;

  bool operator==(const Time &other) const;

  bool operator!=(const Time &other) const;

  bool operator<(const Time &other) const;

  bool operator>(const Time &other) const;

  bool operator<=(const Time &other) const;

  bool operator>=(const Time &other) const;

  int operator-(const Time &other) const; //返回两个时刻之间相差多少分钟

  friend std::ostream &operator<<(std::ostream &out, const Time &x); //以时间格式输出
};

#endif //TIME_H
