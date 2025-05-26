#include "../include/order.h"

Order::Order() = default;

Order::Order(const string &u, const string &i, const string &from, const string &to,
             const Date &date, const Time &leave_time, const Time &arrive_time,
             const int price, const int num, const order_status os,
             const string &timestamp, const int d) {
  strcpy(username_, u.data());
  strcpy(trainID_, i.data());
  strcpy(from_, from.data());
  strcpy(to_, to.data());
  date_ = date;
  leave_time_ = leave_time;
  arrive_time_ = arrive_time;
  price_ = price;
  num_ = num;
  status_ = os;
  timestamp_ = TimestampToInt(timestamp);
  d_ = d;
}

Order::~Order() = default;

bool Order::operator!=(const Order &other) const {
  return timestamp_ != other.timestamp_;
}

bool Order::operator<(const Order &other) const {
  return timestamp_ > other.timestamp_; //新订单在前
}

bool Order::operator>(const Order &other) const {
  return timestamp_ < other.timestamp_;
}

bool Order::operator==(const Order &other) const {
  return timestamp_ == other.timestamp_;
}

bool Order::operator<=(const Order &other) const {
  return timestamp_ >= other.timestamp_;
}

bool Order::operator>=(const Order &other) const {
  return timestamp_ <= other.timestamp_;
}

PendingOrder::PendingOrder() = default;

PendingOrder::PendingOrder(const string &i, const string &from, const string &to,
                           const string &timestamp, const int num, const int idx) {
  strcpy(trainID_, i.data());
  strcpy(from_, from.data());
  strcpy(to_, to.data());
  timestamp_ = TimestampToInt(timestamp);
  num_ = num;
  idx_ = idx;
}

PendingOrder::~PendingOrder() = default;

bool PendingOrder::operator!=(const PendingOrder &other) const {
  return timestamp_ != other.timestamp_;
}

bool PendingOrder::operator>(const PendingOrder &other) const {
  return timestamp_ > other.timestamp_; //先订先补
}

bool PendingOrder::operator<(const PendingOrder &other) const {
  return timestamp_ < other.timestamp_;
}

bool PendingOrder::operator==(const PendingOrder &other) const {
  return timestamp_ == other.timestamp_;
}

bool PendingOrder::operator>=(const PendingOrder &other) const {
  return timestamp_ >= other.timestamp_;
}

bool PendingOrder::operator<=(const PendingOrder &other) const {
  return timestamp_ <= other.timestamp_;
}

void PrintOrder(const Order &x) {
  if (x.status_ == order_status::kSuccess)cout << "[success] ";
  else if (x.status_ == order_status::kPending)cout << "[pending] ";
  else if (x.status_ == order_status::kRefunded)cout << "[refunded] ";
  cout << x.trainID_ << " " << x.from_ << " ";
  PrintDayHourMinute(x.date_, x.leave_time_);
  cout << " -> " << x.to_ << " ";
  PrintDayHourMinute(x.date_, x.arrive_time_);
  cout << " " << x.price_ << " " << x.num_ << "\n";
}
