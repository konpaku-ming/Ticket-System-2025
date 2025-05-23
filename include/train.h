#ifndef TRAIN_H
#define TRAIN_H

#include "../include/tools.h"
#include "../include/BPlusTree.h"
#include "../include/time.h"
#include "../include/date.h"

class Train {
public:
  char trainID_[21]{}; //唯一标识符
  int stationNum_ = 0;
  char stations_[24][31]{};
  int seatNum_ = 0;
  int prices_[24]{}; //以前缀和形式存储
  Time leaveTime_[24]{}; //在每个站的出发时间
  Time arriveTime_[24]{}; //在每个站的到达时间
  Date saleDate_[2]{}; //售卖时间区间
  char type_{};
  bool is_release_ = false; //是否发布

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

#endif //TRAIN_H
