#ifndef TRAINDATABASE_H
#define TRAINDATABASE_H

#include "tools.h"
#include "BPlusTree.h"
#include "tokenscanner.h"
#include "train.h"

class TrainDatabase {
  //超大类，包含火车即票的全部信息
public:
  BPT train_bpt_{"train_bpt_"};
  MemoryRiver<Train, 0> train_file_; //存放火车信息
  BPT station_bpt_{"station_bpt"};
  MemoryRiver<Station, 0> station_file_; //存放过站信息

  TrainDatabase() {
    train_file_.initialise("train_data");
    station_file_.initialise("station_data");
  }

  ~TrainDatabase() = default;

  bool AddTrain(const TokenScanner &tokens) {
    char train_id[21];
    if (tokens.i_.length() < 21) {
      strcpy(train_id, tokens.i_.data());
    } else return false;
    if (train_bpt_.Find(train_id) != -1)return false;
    Train tmp(tokens.i_, tokens.n_, tokens.m_, tokens.s_, tokens.p_,
              tokens.x_, tokens.t_, tokens.o_, tokens.d_, tokens.y_);
    int idx = train_file_.push(tmp);
    const Data tmp_dt(tmp.trainID_, idx);
    train_bpt_.Insert(tmp_dt);
    cout << "0\n";
    return true;
  }

  bool DeleteTrain(const string &i) {
    char train_id[21];
    if (i.length() < 21) {
      strcpy(train_id, i.data());
    } else return false;
    int pos = train_bpt_.Find(train_id);
    if (pos == -1)return false;
    Train i_train;
    train_file_.read(i_train, pos, 1);
    if (i_train.is_release_)return false;
    const Data tmp_dt(train_id, pos);
    train_bpt_.Remove(tmp_dt);
    cout << "0\n";
    return true;
  }

  bool ReleaseTrain(const string &i) {
    char train_id[21];
    if (i.length() < 21) {
      strcpy(train_id, i.data());
    } else return false;
    int pos = train_bpt_.Find(train_id);
    if (pos == -1)return false;
    Train i_train;
    train_file_.read(i_train, pos, 1);
    if (i_train.is_release_)return false;
    //发布成功
    i_train.is_release_ = true;
    train_file_.write(i_train, pos, 1);
    for (int k = 0; k < i_train.stationNum_; k++) {
      //遍历经过的站，在每个站里都存入过站信息
      Station tmp_st(i_train.stations_[k], i_train.trainID_,
                     i_train.arriveTime_[k], i_train.leaveTime_[k]);
      int idx = station_file_.push(tmp_st);
      const Data tmp_dt(tmp_st.station_, idx);
      station_bpt_.Insert(tmp_dt);
    }
    //TODO::存入余票信息
    cout << "0\n";
    return true;
  }
};
#endif //TRAINDATABASE_H
