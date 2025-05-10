#ifndef TRAIN_H
#define TRAIN_H

#include "../include/tools.h"
#include "../include/BPlusTree.h"
#include "time.h"

class Train {
  char trainID_[21]{}; //唯一标识符
  int stationNum_ = 0;
  char stations_[24][31]{};
  int seatNum_ = 0;
  int prices_[24]{};
  char type_{};
  Time leaveTime_[24]{}; //在每个站的出发时间
  Time arriveTime_[24]{}; //在每个站的到达时间

  bool is_release_ = false; //是否发布

  Train();

  Train(const std::string &u, const std::string &p, const std::string &n,
        const std::string &m, const std::string &g);

  ~Train();

  bool operator==(const Account &other) const;

  bool operator!=(const Account &other) const;

  bool operator<(const Account &other) const;

  bool operator>(const Account &other) const;

  bool operator<=(const Account &other) const;

  bool operator>=(const Account &other) const;
};

#endif //TRAIN_H
