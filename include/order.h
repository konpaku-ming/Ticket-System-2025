#ifndef ORDER_H
#define ORDER_H

#include "BPlusTree.h"
#include "../include/date.h"
#include "../include/time.h"

using std::string;

enum class order_status { kSuccess, kPending, kRefunded };

//Order以username为键建立BPT
class Order {
public:
  int timestamp_ = -1;
  char username_[21]{};
  char trainID_[21]{};
  char from_[31]{};
  char to_[31]{};
  Date date_; //从始发站离开的日期
  Time leave_time_; //离开from的时间
  Time arrive_time_; //到达to的时间
  int price_ = -1;
  int num_ = -1;
  order_status status_{};
  int p_idx_ = -1; //对应候补订单的索引
  int d_ = -1; //开始发售的第d天的车

  Order();

  Order(const string &u, const string &i, const string &from, const string &to,
        const Date &date, const Time &leave_time, const Time &arrive_time,
        int price, int num, order_status os, const string &timestamp, int d);

  ~Order();

  bool operator==(const Order &other) const;

  bool operator!=(const Order &other) const;

  bool operator<(const Order &other) const;

  bool operator>(const Order &other) const;

  bool operator<=(const Order &other) const;

  bool operator>=(const Order &other) const;

  friend void PrintOrder(const Order &x);
};

//用于处理候补车票的类，以trainID+车次号为键
//e.g. "HAPPY_TRAIN1" "HAPPY_TRAIN39"
class PendingOrder {
public:
  int timestamp_ = -1;
  char trainID_[21]{};
  char from_[31]{};
  char to_[31]{};
  int num_ = -1; //订单数量
  int idx_ = -1; //对应的Order在文件中的位置

  PendingOrder();

  PendingOrder(const string &i, const string &from, const string &to,
               const string &timestamp, int num, int idx);

  ~PendingOrder();

  bool operator==(const PendingOrder &other) const;

  bool operator!=(const PendingOrder &other) const;

  bool operator<(const PendingOrder &other) const;

  bool operator>(const PendingOrder &other) const;

  bool operator<=(const PendingOrder &other) const;

  bool operator>=(const PendingOrder &other) const;
};

#endif //ORDER_H
