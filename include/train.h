#ifndef TRAIN_H
#define TRAIN_H

#include "../include/tools.h"
#include "../include/BPlusTree.h"
#include "../include/time.h"
#include "../include/date.h"

class Train {
public:
  char trainID_[21]{}; //唯一标识符
  int stationNum_ = -1;
  char stations_[24][31]{}; //0-base
  int seatNum_ = -1;
  int prices_[24]{}; //以前缀和形式存储
  Time arriveTime_[24]{}; //在每个站的到达时间
  Time leaveTime_[24]{}; //在每个站的出发时间
  Date saleDate_[2]{}; //售卖时间区间
  char type_{};
  bool is_release_ = false; //是否发布
  int ticket_idx = -1; //余票信息在文件中的位置

  Train();

  Train(const string &i, const string &n, const string &m, const string &s,
        const string &p, const string &x, const string &t, const string &o,
        const string &d, const string &y);

  ~Train();

  void StringToStations(const string &s);

  void StringToPrices(const string &p);

  void StringToAllTime(const string &x, const string &t, const string &o);

  void StringToSaleDate(const string &d);
};

class Station {
public:
  char station_[31]{};
  char trainID_[21]{};
  Time arriveTime_;
  Time leaveTime_;

  Station();

  Station(const string &station, const string &i,
          const Time &arrive_time, const Time &leave_time);

  ~Station();
};

class Ticket {
public:
  char trainID_[21]{};
  int rest_ticket[93][24]{}; //第一个参数是date即车次，第二个参数为每一站到下一站之间是否有票

  Ticket();

  Ticket(const string &i, int seatNum);

  ~Ticket();
};

#endif //TRAIN_H
