#ifndef TRAINDATABASE_H
#define TRAINDATABASE_H

#include "tools.h"
#include "BPlusTree.h"
#include "tokenscanner.h"
#include "train.h"
#include "ticket.h"

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
    const int pos = train_bpt_.Find(train_id);
    if (pos == -1)return false;
    Train i_train;
    train_file_.read(i_train, pos, 1);
    if (i_train.is_release_)return false;
    //发布成功
    i_train.is_release_ = true;
    for (int k = 0; k < i_train.stationNum_; k++) {
      //遍历经过的站，在每个站里都存入过站信息
      Station tmp_st(i_train.stations_[k], pos,
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

  bool QueryTicketByTime(const string &s, const string &t, const string &d) {
    //以所需时间排序
    Date date = StringToDate(d);
    char s_name[31];
    if (s.length() < 31) {
      strcpy(s_name, s.data());
    } else return false;
    char t_name[31];
    if (t.length() < 31) {
      strcpy(t_name, t.data());
    } else return false;
    sjtu::vector<int> s_idx;
    sjtu::vector<int> t_idx;
    station_bpt_.MultiFind(s_name, s_idx); //找到经过s的信息索引
    station_bpt_.MultiFind(t_name, t_idx); //找到经过t的信息索引
    if (s_idx.empty() || t_idx.empty())return false;
    sjtu::priority_queue<int> s_train; //经过s的所有车
    sjtu::priority_queue<int> t_train; //经过t的所有车
    Station tmp;
    for (const auto it: s_idx) {
      station_file_.read(tmp, it, 1);
      s_train.push(tmp.train_idx_);
    }
    for (const auto it: t_idx) {
      station_file_.read(tmp, it, 1);
      t_train.push(tmp.train_idx_);
    }
    sjtu::vector<int> st_train; //两站都过的车
    while (!(s_train.empty() || t_train.empty())) {
      if (s_train.top() == t_train.top()) {
        st_train.push_back(s_train.top());
        s_train.pop();
        t_train.pop();
      } else if (s_train.top() > t_train.top()) {
        s_train.pop();
      } else {
        t_train.pop();
      }
    }
    sjtu::priority_queue<Direct, TimeCmp> time_pq{};
    Train tmp_train;
    for (const auto it: st_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int t_pos = -1;
      bool t_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      int delta_d = date - tmp_train.saleDate_[0];
      for (int k = 0; k < tmp_train.stationNum_; k++) {
        if (tmp_train.stations_[k] == s) {
          s_pos = k;
          s_flag = true;
          delta_d -= tmp_train.leaveTime_[k].day;
          if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])break;
        }
        if (tmp_train.stations_[k] == t) {
          t_pos = k;
          t_flag = true;
        }
        if (!s_flag && t_flag)break;
        if (s_flag && !t_flag) max_num = min(max_num, rest.rest_ticket[delta_d][k]);
      }
      if (!s_flag && t_flag)continue; //站的方向反了
      if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])continue;
      //填入火车票
      Direct direct;
      direct.from_ = s;
      direct.to_ = t;
      direct.max_num_ = max_num;
      direct.date_ = tmp_train.saleDate_[0] + delta_d;
      direct.leave_time_ = tmp_train.leaveTime_[s_pos];
      direct.arrive_time_ = tmp_train.arriveTime_[t_pos];
      direct.cost_ = tmp_train.prices_[t_pos] - tmp_train.prices_[s_pos];
      direct.trainID_ = tmp_train.trainID_;
      direct.time_ = direct.arrive_time_ - direct.leave_time_;
      time_pq.push(direct);
    }
    cout << time_pq.size() << "\n";
    while (!time_pq.empty()) {
      PrintDirect(time_pq.top());
      time_pq.pop();
    }
    return true;
  }

  bool QueryTicketByCost(const string &s, const string &t, const string &d) {
    //以票价排序
    Date date = StringToDate(d);
    char s_name[31];
    if (s.length() < 31) {
      strcpy(s_name, s.data());
    } else return false;
    char t_name[31];
    if (t.length() < 31) {
      strcpy(t_name, t.data());
    } else return false;
    sjtu::vector<int> s_idx;
    sjtu::vector<int> t_idx;
    station_bpt_.MultiFind(s_name, s_idx); //找到经过s的信息索引
    station_bpt_.MultiFind(t_name, t_idx); //找到经过t的信息索引
    if (s_idx.empty() || t_idx.empty())return false;
    sjtu::priority_queue<int> s_train; //经过s的所有车
    sjtu::priority_queue<int> t_train; //经过t的所有车
    Station tmp;
    for (const auto it: s_idx) {
      station_file_.read(tmp, it, 1);
      s_train.push(tmp.train_idx_);
    }
    for (const auto it: t_idx) {
      station_file_.read(tmp, it, 1);
      t_train.push(tmp.train_idx_);
    }
    sjtu::vector<int> st_train; //两站都过的车
    while (!(s_train.empty() || t_train.empty())) {
      if (s_train.top() == t_train.top()) {
        st_train.push_back(s_train.top());
        s_train.pop();
        t_train.pop();
      } else if (s_train.top() > t_train.top()) {
        s_train.pop();
      } else {
        t_train.pop();
      }
    }
    sjtu::priority_queue<Direct, CostCmp> cost_pq{};
    Train tmp_train;
    for (const auto it: st_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int t_pos = -1;
      bool t_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      int delta_d = date - tmp_train.saleDate_[0];
      for (int k = 0; k < tmp_train.stationNum_; k++) {
        if (tmp_train.stations_[k] == s) {
          s_pos = k;
          s_flag = true;
          delta_d -= tmp_train.leaveTime_[k].day;
          if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])break;
        }
        if (tmp_train.stations_[k] == t) {
          t_pos = k;
          t_flag = true;
        }
        if (!s_flag && t_flag)break;
        if (s_flag && !t_flag) max_num = min(max_num, rest.rest_ticket[delta_d][k]);
      }
      if (!s_flag && t_flag)continue; //站的方向反了
      if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])continue;
      //填入火车票
      Direct direct;
      direct.from_ = s;
      direct.to_ = t;
      direct.max_num_ = max_num;
      direct.date_ = tmp_train.saleDate_[0] + delta_d;
      direct.leave_time_ = tmp_train.leaveTime_[s_pos];
      direct.arrive_time_ = tmp_train.arriveTime_[t_pos];
      direct.cost_ = tmp_train.prices_[t_pos] - tmp_train.prices_[s_pos];
      direct.trainID_ = tmp_train.trainID_;
      direct.time_ = direct.arrive_time_ - direct.leave_time_;
      cost_pq.push(direct);
    }
    cout << cost_pq.size() << "\n";
    while (!cost_pq.empty()) {
      PrintDirect(cost_pq.top());
      cost_pq.pop();
    }
    return true;
  }

  bool QueryTransferByTime(const string &s, const string &t, const string &d) {
    //以时间排序
    sjtu::map<string, sjtu::vector<Direct> > time_map{};
    sjtu::priority_queue<Transfer, TransferTimeCmp> find_best{};
    Date date = StringToDate(d);
    char s_name[31];
    if (s.length() < 31) {
      strcpy(s_name, s.data());
    } else return false;
    char t_name[31];
    if (t.length() < 31) {
      strcpy(t_name, t.data());
    } else return false;
    sjtu::vector<int> s_idx;
    sjtu::vector<int> t_idx;
    station_bpt_.MultiFind(s_name, s_idx); //找到经过s的信息索引
    station_bpt_.MultiFind(t_name, t_idx); //找到经过t的信息索引
    if (s_idx.empty() || t_idx.empty())return false;
    sjtu::vector<int> s_train; //经过s的所有车
    sjtu::vector<int> t_train; //经过t的所有车
    Station tmp;
    for (const auto it: s_idx) {
      station_file_.read(tmp, it, 1);
      s_train.push_back(tmp.train_idx_);
    }
    for (const auto it: t_idx) {
      station_file_.read(tmp, it, 1);
      t_train.push_back(tmp.train_idx_);
    }
    Train tmp_train;
    for (const auto it: s_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      int delta_d = date - tmp_train.saleDate_[0];
      for (int k = 0; k < tmp_train.stationNum_; k++) {
        if (s_flag) {
          max_num = min(max_num, rest.rest_ticket[delta_d][k - 1]);
          int time1 = tmp_train.arriveTime_[k] - tmp_train.leaveTime_[s_pos];
          //填入火车票
          auto station_it = time_map.find(tmp_train.stations_[k]);
          if (station_it == time_map.end()) {
            //还没到过该站
            Direct direct;
            direct.from_ = s;
            direct.to_ = tmp_train.stations_[k];
            direct.max_num_ = max_num;
            direct.date_ = tmp_train.saleDate_[0] + delta_d;
            direct.leave_time_ = tmp_train.leaveTime_[s_pos];
            direct.arrive_time_ = tmp_train.arriveTime_[k];
            direct.cost_ = tmp_train.prices_[k] - tmp_train.prices_[s_pos];
            direct.trainID_ = tmp_train.trainID_;
            direct.time_ = time1;
            //插入该票
            sjtu::vector<Direct> vec;
            vec.push_back(direct);
            time_map.insert({tmp_train.stations_[k], vec});
          } else {
            //到过该站
            Direct direct;
            direct.from_ = s;
            direct.to_ = tmp_train.stations_[k];
            direct.max_num_ = max_num;
            direct.date_ = tmp_train.saleDate_[0] + delta_d;
            direct.leave_time_ = tmp_train.leaveTime_[s_pos];
            direct.arrive_time_ = tmp_train.arriveTime_[k];
            direct.cost_ = tmp_train.prices_[k] - tmp_train.prices_[s_pos];
            direct.trainID_ = tmp_train.trainID_;
            direct.time_ = time1;
            //插入该票
            time_map[tmp_train.stations_[k]].push_back(direct);
          }
        } else if (tmp_train.stations_[k] == s) {
          s_pos = k;
          s_flag = true;
          delta_d -= tmp_train.leaveTime_[k].day;
          if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])break;
        }
      }
      //对一辆车做以上操作
    }
    //到此得到了一个map，每个车站名对应一个vector<Direct>
    for (const auto it: t_train) {
      train_file_.read(tmp_train, it, 1);
      int t_pos = -1;
      bool t_flag = false;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      for (int k = tmp_train.stationNum_ - 1; k >= 0; k--) {
        if (t_flag) {
          auto t_it = time_map.find(tmp_train.stations_[k]);
          if (t_it == time_map.end())continue; //到不了该站
          auto vec = t_it->second;
          for (const auto &d1: vec) {
            //对vec中的一辆车
            if (d1.trainID_ == tmp_train.trainID_)continue;
            Date date1;
            if (Check(d1.arrive_time_, tmp_train.leaveTime_[k])) {
              date1 = d1.date_ + d1.arrive_time_.day + 1 - tmp_train.leaveTime_[k].day;
            } else date1 = d1.date_ + d1.arrive_time_.day - tmp_train.leaveTime_[k].day;
            //date1为可以从当前站出发往t的最早车次
            if (date1 > tmp_train.saleDate_[1])continue; //无法构成换乘票
            int dd = (date1 > tmp_train.saleDate_[0]) ? (date1 - tmp_train.saleDate_[0]) : 0;
            int max_num = tmp_train.seatNum_;
            for (int i = k; i < t_pos; i++) {
              max_num = min(max_num, rest.rest_ticket[dd][i]);
            }
            //得到max_num
            Direct d2;
            d2.from_ = tmp_train.stations_[k];
            d2.to_ = t;
            d2.max_num_ = max_num;
            d2.date_ = date1;
            d2.leave_time_ = tmp_train.leaveTime_[k];
            d2.arrive_time_ = tmp_train.arriveTime_[t_pos];
            d2.cost_ = tmp_train.prices_[t_pos] - tmp_train.prices_[k];
            d2.trainID_ = tmp_train.trainID_;
            d2.time_ = d2.arrive_time_ - d2.leave_time_;
            Transfer tran(d1, d2);
            find_best.push(tran);
          }
        }
        if (tmp_train.stations_[k] == t) {
          t_pos = k;
          t_flag = true;
        }
      }
    }
    PrintDirect(find_best.top().first_);
    PrintDirect(find_best.top().second_);
    return true;
  }

  bool QueryTransferByCost(const string &s, const string &t, const string &d) {
    //以时间排序
    sjtu::map<string, sjtu::vector<Direct> > cost_map{};
    sjtu::priority_queue<Transfer, TransferCostCmp> find_best{};
    Date date = StringToDate(d);
    char s_name[31];
    if (s.length() < 31) {
      strcpy(s_name, s.data());
    } else return false;
    char t_name[31];
    if (t.length() < 31) {
      strcpy(t_name, t.data());
    } else return false;
    sjtu::vector<int> s_idx;
    sjtu::vector<int> t_idx;
    station_bpt_.MultiFind(s_name, s_idx); //找到经过s的信息索引
    station_bpt_.MultiFind(t_name, t_idx); //找到经过t的信息索引
    if (s_idx.empty() || t_idx.empty())return false;
    sjtu::vector<int> s_train; //经过s的所有车
    sjtu::vector<int> t_train; //经过t的所有车
    Station tmp;
    for (const auto it: s_idx) {
      station_file_.read(tmp, it, 1);
      s_train.push_back(tmp.train_idx_);
    }
    for (const auto it: t_idx) {
      station_file_.read(tmp, it, 1);
      t_train.push_back(tmp.train_idx_);
    }
    Train tmp_train;
    for (const auto it: s_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      int delta_d = date - tmp_train.saleDate_[0];
      for (int k = 0; k < tmp_train.stationNum_; k++) {
        if (s_flag) {
          max_num = min(max_num, rest.rest_ticket[delta_d][k - 1]);
          int time1 = tmp_train.arriveTime_[k] - tmp_train.leaveTime_[s_pos];
          //填入火车票
          auto station_it = cost_map.find(tmp_train.stations_[k]);
          if (station_it == cost_map.end()) {
            //还没到过该站
            Direct direct;
            direct.from_ = s;
            direct.to_ = tmp_train.stations_[k];
            direct.max_num_ = max_num;
            direct.date_ = tmp_train.saleDate_[0] + delta_d;
            direct.leave_time_ = tmp_train.leaveTime_[s_pos];
            direct.arrive_time_ = tmp_train.arriveTime_[k];
            direct.cost_ = tmp_train.prices_[k] - tmp_train.prices_[s_pos];
            direct.trainID_ = tmp_train.trainID_;
            direct.time_ = time1;
            //插入该票
            sjtu::vector<Direct> vec;
            vec.push_back(direct);
            cost_map.insert({tmp_train.stations_[k], vec});
          } else {
            //到过该站
            Direct direct;
            direct.from_ = s;
            direct.to_ = tmp_train.stations_[k];
            direct.max_num_ = max_num;
            direct.date_ = tmp_train.saleDate_[0] + delta_d;
            direct.leave_time_ = tmp_train.leaveTime_[s_pos];
            direct.arrive_time_ = tmp_train.arriveTime_[k];
            direct.cost_ = tmp_train.prices_[k] - tmp_train.prices_[s_pos];
            direct.trainID_ = tmp_train.trainID_;
            direct.time_ = time1;
            //插入该票
            cost_map[tmp_train.stations_[k]].push_back(direct);
          }
        } else if (tmp_train.stations_[k] == s) {
          s_pos = k;
          s_flag = true;
          delta_d -= tmp_train.leaveTime_[k].day;
          if (delta_d < 0 || delta_d > tmp_train.saleDate_[1] - tmp_train.saleDate_[0])break;
        }
      }
      //对一辆车做以上操作
    }
    //到此得到了一个map，每个车站名对应一个vector<Direct>
    for (const auto it: t_train) {
      train_file_.read(tmp_train, it, 1);
      int t_pos = -1;
      bool t_flag = false;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      for (int k = tmp_train.stationNum_ - 1; k >= 0; k--) {
        if (t_flag) {
          auto t_it = cost_map.find(tmp_train.stations_[k]);
          if (t_it == cost_map.end())continue; //到不了该站
          auto vec = t_it->second;
          for (const auto &d1: vec) {
            //对vec中的一辆车
            if (d1.trainID_ == tmp_train.trainID_)continue;
            Date date1;
            if (Check(d1.arrive_time_, tmp_train.leaveTime_[k])) {
              date1 = d1.date_ + d1.arrive_time_.day + 1 - tmp_train.leaveTime_[k].day;
            } else date1 = d1.date_ + d1.arrive_time_.day - tmp_train.leaveTime_[k].day;
            //date1为可以从当前站出发往t的最早车次
            if (date1 > tmp_train.saleDate_[1])continue; //无法构成换乘票
            int dd = (date1 > tmp_train.saleDate_[0]) ? (date1 - tmp_train.saleDate_[0]) : 0;
            int max_num = tmp_train.seatNum_;
            for (int i = k; i < t_pos; i++) {
              max_num = min(max_num, rest.rest_ticket[dd][i]);
            }
            //得到max_num
            Direct d2;
            d2.from_ = tmp_train.stations_[k];
            d2.to_ = t;
            d2.max_num_ = max_num;
            d2.date_ = date1;
            d2.leave_time_ = tmp_train.leaveTime_[k];
            d2.arrive_time_ = tmp_train.arriveTime_[t_pos];
            d2.cost_ = tmp_train.prices_[t_pos] - tmp_train.prices_[k];
            d2.trainID_ = tmp_train.trainID_;
            d2.time_ = d2.arrive_time_ - d2.leave_time_;
            Transfer tran(d1, d2);
            find_best.push(tran);
          }
        }
        if (tmp_train.stations_[k] == t) {
          t_pos = k;
          t_flag = true;
        }
      }
    }
    PrintDirect(find_best.top().first_);
    PrintDirect(find_best.top().second_);
    return true;
  }
};
#endif //TRAINDATABASE_H
