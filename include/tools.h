#ifndef TOOLS_H
#define TOOLS_H

#include "vector.h"
#include "utility.h"
#include <string>
#include "../include/time.h"
#include "../include/date.h"

using string = std::string;
using ll = long long;
using std::cin;
using std::cout;

inline int day_num[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//用于分割指令和带'|'的东西
inline void Split(const string &str, sjtu::vector<string> &list, char c) {
  list.clear();
  if (c == ' ' && str.empty())
    return;
  string::size_type last_pos = 0;
  string::size_type pos;
  do {
    pos = str.find(c, last_pos);
    if (pos > last_pos || c != ' ')
      list.push_back(str.substr(last_pos, pos - last_pos));
    last_pos = pos + 1;
  } while (last_pos < str.length() && pos != string::npos);
  if (c != ' ' && str.back() == c) {
    list.push_back(string());
  }
}

//字符串转整数
inline ll StringToInt(const string &str) {
  ll num = 0;
  bool negative = false;
  int i = 0;
  if (str[0] == '-') {
    negative = true;
    i++;
  }
  for (; i < str.length(); i++) {
    num = num * 10 + (str[i] - '0');
  }
  if (negative) {
    num = -num;
  }
  return num;
}

inline ll TimestampToInt(const string &str) {
  ll num = 0;
  for (int i = 1; i < str.length() - 1; i++) {
    num = num * 10 + (str[i] - '0');
  }
  return num;
}


inline bool SplitPair(const string &str, sjtu::pair<string, string> &p,
                      char c = '=') {
  string::size_type pos = str.find(c);
  if (pos == string::npos) {
    return false;
  }
  string s1 = str.substr(0, pos);
  string s2 = str.substr(pos + 1);
  p.first = s1;
  p.second = s2;
  return true;
}

inline void PrintDayHourMinute(const Date &date, const Time &time) {
  cout << (date + time.day);
  cout << " ";
  cout << time;
}

inline Date StringToDate(const string &d) {
  sjtu::vector<string> date;
  Split(d, date, '-');
  return Date(StringToInt(date[0]), StringToInt(date[1]));
}

#endif //TOOLS_H
