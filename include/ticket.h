#ifndef TICKET_H
#define TICKET_H

#include "../include/tools.h"
#include "../include/BPlusTree.h"
#include "../include/time.h"
#include "../include/date.h"

struct Direct {
  //直达票
  string trainID_;
  string from_;
  string to_;
  Date date_;
  Time leave_time_;
  Time arrive_Time_;
  int max_num_ = -1;
  int cost_ = -1;
  int time_ = -1;
};

struct TimeCmp {
  bool operator()(const Direct &a, const Direct &b) const {
    if (a.time_ != b.time_)return a.time_ > b.time_;
    return a.trainID_ > b.trainID_;
  }
};

struct CostCmp {
  bool operator()(const Direct &a, const Direct &b) const {
    if (a.cost_ != b.cost_)return a.cost_ > b.cost_;
    return a.trainID_ > b.trainID_;
  }
};

inline void PrintDirect(const Direct &x) {
  cout << x.trainID_ << " " << x.from_ << " ";
  PrintDayHourMinute(x.date_, x.leave_time_);
  cout << " -> " << x.to_ << " ";
  PrintDayHourMinute(x.date_, x.arrive_Time_);
  cout << " " << x.cost_ << " " << x.max_num_ << "\n";
}
#endif //TICKET_H
