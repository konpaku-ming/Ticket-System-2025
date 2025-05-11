#ifndef TOOLS_H
#define TOOLS_H

#include "vector.h"
#include "utility.h"
#include <string>
using string = std::string;
using ll = long long;

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

#endif //TOOLS_H
