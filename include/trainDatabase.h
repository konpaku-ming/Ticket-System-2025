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
  MemoryRiver<Ticket, 0> ticket_file_; //存放余票信息

  TrainDatabase() {
    train_file_.initialise("train_data");
    station_file_.initialise("station_data");
    ticket_file_.initialise("ticket_data");
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
    for (int k = 0; k < i_train.stationNum_; k++) {
      //遍历经过的站，在每个站里都存入过站信息
      Station tmp_st(i_train.stations_[k], i_train.trainID_,
                     i_train.arriveTime_[k], i_train.leaveTime_[k]);
      int idx = station_file_.push(tmp_st);
      const Data tmp_dt(tmp_st.station_, idx);
      station_bpt_.Insert(tmp_dt);
    }
    Ticket i_ticket(i_train.trainID_, i_train.seatNum_);
    i_train.ticket_idx_ = ticket_file_.push(i_ticket); //存入余票信息
    train_file_.write(i_train, pos, 1);
    cout << "0\n";
    return true;
  }

  bool QueryTrain(const string &i, const string &d) {
    Date date = StringToDate(d);
    char train_id[21];
    if (i.length() < 21) {
      strcpy(train_id, i.data());
    } else return false;
    int pos = train_bpt_.Find(train_id);
    if (pos == -1)return false;
    Train i_train;
    train_file_.read(i_train, pos, 1);
    if (i_train.saleDate_[0] > date || i_train.saleDate_[1] < date)return false;
    //查询成功
    if (i_train.is_release_) {
      int delta_d = date - i_train.saleDate_[0];
      Ticket ticket;
      ticket_file_.read(ticket, i_train.ticket_idx_, 1); //读出余票信息
      cout << i_train.trainID_ << " " << i_train.type_ << "\n";
      //车辆信息
      cout << i_train.stations_[0] << " xx-xx xx:xx -> ";
      PrintDayHourMinute(date, i_train.leaveTime_[0]);
      cout << " 0 " << ticket.rest_ticket[delta_d][0] << "\n";
      //始发站特殊处理
      for (int k = 1; k < i_train.stationNum_ - 1; k++) {
        cout << i_train.stations_[k] << " ";
        PrintDayHourMinute(date, i_train.arriveTime_[k]);
        cout << " -> ";
        PrintDayHourMinute(date, i_train.leaveTime_[k]);
        cout << " " << i_train.prices_[k] << " " << ticket.rest_ticket[delta_d][k] << "\n";
      }
      //遍历中间站
      cout << i_train.stations_[i_train.stationNum_ - 1] << " ";
      PrintDayHourMinute(date, i_train.arriveTime_[i_train.stationNum_ - 1]);
      cout << " -> xx-xx xx:xx " << i_train.prices_[i_train.stationNum_ - 1]
          << " x" << "\n";
      //终点站特殊处理
    } else {
      //未发布
      cout << i_train.trainID_ << " " << i_train.type_ << "\n";
      //车辆信息
      cout << i_train.stations_[0] << " xx-xx xx:xx -> ";
      PrintDayHourMinute(date, i_train.leaveTime_[0]);
      cout << " 0 " << i_train.seatNum_ << "\n";
      //始发站特殊处理
      for (int k = 1; k < i_train.stationNum_ - 1; k++) {
        cout << i_train.stations_[k] << " ";
        PrintDayHourMinute(date, i_train.arriveTime_[k]);
        cout << " -> ";
        PrintDayHourMinute(date, i_train.leaveTime_[k]);
        cout << " " << i_train.prices_[k] << " " << i_train.seatNum_ << "\n";
      }
      //遍历中间站
      cout << i_train.stations_[i_train.stationNum_ - 1] << " ";
      PrintDayHourMinute(date, i_train.arriveTime_[i_train.stationNum_ - 1]);
      cout << " -> xx-xx xx:xx " << i_train.prices_[i_train.stationNum_ - 1]
          << " x" << "\n";
      //终点站特殊处理
    }
    return true;
  }
};
#endif //TRAINDATABASE_H
