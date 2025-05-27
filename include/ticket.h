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
  Time arrive_time_;
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
  PrintDayHourMinute(x.date_, x.arrive_time_);
  cout << " " << x.cost_ << " " << x.max_num_ << "\n";
}

struct Transfer {
  //换乘票
  Direct first_;
  Direct second_;
  int total_cost_ = -1; //总时间
  int total_time_ = -1; //总价格

  Transfer(const Direct &d1, const Direct &d2) {
    first_ = d1;
    second_ = d2;
    total_cost_ = d1.cost_ + d2.cost_;
    const Date date1 = d1.date_ + d1.leave_time_.day;
    const Date date2 = d2.date_ + d2.arrive_time_.day;
    const int d = date2 - date1;
    Time time1 = d1.leave_time_;
    time1.day = 0;
    Time time2 = d2.arrive_time_;
    time2.day = d;
    total_time_ = time2 - time1;
  }
};

struct TransferTimeCmp {
  bool operator()(const Transfer &a, const Transfer &b) const {
    if (a.total_time_ != b.total_time_)return a.total_time_ > b.total_time_;
    if (a.total_cost_ != b.total_cost_)return a.total_cost_ > b.total_cost_;
    if (a.first_.trainID_ != b.first_.trainID_)return a.first_.trainID_ > b.first_.trainID_;
    return a.second_.trainID_ > b.second_.trainID_;
  }
};

struct TransferCostCmp {
  bool operator()(const Transfer &a, const Transfer &b) const {
    if (a.total_cost_ != b.total_cost_)return a.total_cost_ > b.total_cost_;
    if (a.total_time_ != b.total_time_)return a.total_time_ > b.total_time_;
    if (a.first_.trainID_ != b.first_.trainID_)return a.first_.trainID_ > b.first_.trainID_;
    return a.second_.trainID_ > b.second_.trainID_;
  }
};
#endif //TICKET_H
