#ifndef TRAINDATABASE_H
#define TRAINDATABASE_H

#include "tools.h"
#include "BPlusTree.h"
#include "tokenscanner.h"
#include "train.h"
#include "ticket.h"
#include "order.h"

class TrainDatabase {
  //超大数据库类，包含火车于票的全部信息
public:
  BPT train_bpt_{"train_bpt_"};
  MemoryRiver<Train, 0> train_file_; //存放火车信息
  BPT station_bpt_{"station_bpt"}; //站名为key,train_idx为value
  MemoryRiver<Ticket, 0> ticket_file_; //存放余票信息
  BPT order_bpt_{"order_bpt_"};
  MemoryRiver<Order, 0> order_file_; //存放订单信息
  BPT pending_bpt_{"pending_bpt_"};
  MemoryRiver<PendingOrder, 0> pending_file_; //存放候补订单

  TrainDatabase() {
    train_file_.initialise("train_data");
    ticket_file_.initialise("ticket_data");
    order_file_.initialise("order_data");
    pending_file_.initialise("pending_data");
  }

  ~TrainDatabase() {
    train_file_.exit();
    ticket_file_.exit();
    order_file_.exit();
    pending_file_.exit();
  }

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
      Data tmp_dt;
      tmp_dt.value = pos;
      strcpy(tmp_dt.key, i_train.stations_[k]);
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
    if (s_idx.empty() || t_idx.empty()) {
      cout << "0\n";
      return true;
    }
    int cnt[100000]{};
    sjtu::vector<int> st_train; //两站都过的车
    for (auto i: s_idx) {
      cnt[i / 1000]++;
    }
    for (auto i: t_idx) {
      if (cnt[i / 1000])st_train.push_back(i);
    }
    sjtu::priority_queue<Direct, TimeCmp> time_pq{};
    Train tmp_train;
    for (const auto it: st_train) {
      train_file_.read(tmp_train, it, 1);
      if (tmp_train.saleDate_[0] > date)continue;
      int delta_d = date - tmp_train.saleDate_[0];
      int s_pos = -1;
      bool s_flag = false;
      int t_pos = -1;
      bool t_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
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
    if (s_idx.empty() || t_idx.empty()) {
      cout << "0\n";
      return true;
    }
    int cnt[100000]{};
    sjtu::vector<int> st_train; //两站都过的车
    for (auto i: s_idx) {
      cnt[i / 1000]++;
    }
    for (auto i: t_idx) {
      if (cnt[i / 1000])st_train.push_back(i);
    }
    sjtu::priority_queue<Direct, CostCmp> cost_pq{};
    Train tmp_train;
    for (const auto it: st_train) {
      train_file_.read(tmp_train, it, 1);
      if (tmp_train.saleDate_[0] > date)continue;
      int delta_d = date - tmp_train.saleDate_[0];
      int s_pos = -1;
      bool s_flag = false;
      int t_pos = -1;
      bool t_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
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
    sjtu::vector<int> s_train;
    sjtu::vector<int> t_train;
    station_bpt_.MultiFind(s_name, s_train); //找到经过s的车索引
    station_bpt_.MultiFind(t_name, t_train); //找到经过t的车索引
    if (s_train.empty() || t_train.empty())return false;
    Train tmp_train;
    for (const auto it: s_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      if (date < tmp_train.saleDate_[0])continue;
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
              date1 = d1.date_ + d1.arrive_time_.day - tmp_train.leaveTime_[k].day;
            } else date1 = d1.date_ + d1.arrive_time_.day + 1 - tmp_train.leaveTime_[k].day;
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
            d2.date_ = dd == 0 ? tmp_train.saleDate_[0] : date1;
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
    if (find_best.empty())return false;
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
    sjtu::vector<int> s_train;
    sjtu::vector<int> t_train;
    station_bpt_.MultiFind(s_name, s_train); //找到经过s的车索引
    station_bpt_.MultiFind(t_name, t_train); //找到经过t的车索引
    if (s_train.empty() || t_train.empty())return false;
    Train tmp_train;
    for (const auto it: s_train) {
      train_file_.read(tmp_train, it, 1);
      int s_pos = -1;
      bool s_flag = false;
      int max_num = tmp_train.seatNum_;
      Ticket rest;
      ticket_file_.read(rest, tmp_train.ticket_idx_, 1);
      if (date < tmp_train.saleDate_[0])continue;
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
              date1 = d1.date_ + d1.arrive_time_.day - tmp_train.leaveTime_[k].day;
            } else date1 = d1.date_ + d1.arrive_time_.day + 1 - tmp_train.leaveTime_[k].day;
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
            d2.date_ = dd == 0 ? tmp_train.saleDate_[0] : date1;
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
    if (find_best.empty())return false;
    PrintDirect(find_best.top().first_);
    PrintDirect(find_best.top().second_);
    return true;
  }

  //先检查登录
  bool BuyTicket(const TokenScanner &tokens) {
    Date d = StringToDate(tokens.d_); //从s离开的日期
    char train_id[21];
    if (tokens.i_.length() < 21) {
      strcpy(train_id, tokens.i_.data());
    } else return false;
    int pos = train_bpt_.Find(train_id);
    if (pos == -1)return false;
    Train i_train;
    train_file_.read(i_train, pos, 1);
    if (!i_train.is_release_)return false;
    int f_pos = -1;
    int t_pos = -1;
    for (int k = 0; k < i_train.stationNum_; k++) {
      if (i_train.stations_[k] == tokens.f_)f_pos = k;
      if (i_train.stations_[k] == tokens.t_)t_pos = k;
    }
    if (f_pos == -1 || t_pos == -1 || f_pos >= t_pos)return false;
    Date date = d - i_train.leaveTime_[f_pos].day;
    if (i_train.saleDate_[0] > date || i_train.saleDate_[1] < date)return false;
    //在售票期间内
    int delta_d = date - i_train.saleDate_[0];
    Ticket rest;
    ticket_file_.read(rest, i_train.ticket_idx_, 1);
    int max_num = i_train.seatNum_;
    int n = StringToInt(tokens.n_);
    if (n > max_num)return false;
    for (int k = f_pos; k < t_pos; k++) {
      max_num = min(max_num, rest.rest_ticket[delta_d][k]);
    }
    int price = i_train.prices_[t_pos] - i_train.prices_[f_pos];
    //max_num为最大购买票数
    if (n <= max_num) {
      //可以直接买
      for (int k = f_pos; k < t_pos; k++) {
        rest.rest_ticket[delta_d][k] -= n;
      }
      ticket_file_.write(rest, i_train.ticket_idx_, 1);
      Order u_order(tokens.u_, tokens.i_, tokens.f_, tokens.t_, date,
                    i_train.leaveTime_[f_pos], i_train.arriveTime_[t_pos],
                    price, n, order_status::kSuccess, tokens.timestamp_, delta_d);
      int idx = order_file_.push(u_order);
      Data tmp_dt;
      tmp_dt.value = idx;
      strcpy(tmp_dt.key, tokens.u_.data());
      order_bpt_.Insert(tmp_dt); //idx越小订单越早
      cout << n * price << "\n";
      return true;
    }
    //余票不足
    if (tokens.q_ == "true") {
      //加入候补
      Order u_order(tokens.u_, tokens.i_, tokens.f_, tokens.t_, date,
                    i_train.leaveTime_[f_pos], i_train.arriveTime_[t_pos],
                    price, n, order_status::kPending, tokens.timestamp_, delta_d);
      int idx = order_file_.push(u_order);
      Data tmp_dt;
      tmp_dt.value = idx;
      strcpy(tmp_dt.key, tokens.u_.data());
      order_bpt_.Insert(tmp_dt); //idx越小订单越早
      PendingOrder pending(tokens.i_, tokens.f_, tokens.t_, tokens.timestamp_, n, idx);
      int p_idx = pending_file_.push(pending);
      Data dt;
      dt.value = p_idx;
      string i = tokens.i_ + IntToString(delta_d);
      strcpy(dt.key, i.data());
      pending_bpt_.Insert(dt);
      cout << "queue\n";
      Order o;
      order_file_.read(o, idx, 1);
      o.p_idx_ = p_idx;
      order_file_.write(o, idx, 1);
      return true;
    }
    return false;
  }

  //先检查登录
  bool QueryOrder(const string &u) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    } else return false;
    sjtu::vector<int> u_idx;
    order_bpt_.MultiFind(username, u_idx); //u的所有order的索引
    if (u_idx.empty()) {
      cout << "0\n";
      return true;
    }
    cout << u_idx.size() << "\n";
    auto it = u_idx.end();
    Order u_order;
    while (it != u_idx.begin()) {
      --it;
      order_file_.read(u_order, *it, 1);
      PrintOrder(u_order);
    }
    return true;
  }

  bool RefundTicket(const string &u, int n) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    } else return false;
    sjtu::vector<int> u_idx;
    order_bpt_.MultiFind(username, u_idx);
    if (u_idx.size() < n)return false;
    auto it = u_idx.end();
    Order u_order;
    while (n--)--it;
    order_file_.read(u_order, *it, 1);
    if (u_order.status_ == order_status::kRefunded)return false;
    if (u_order.status_ == order_status::kPending) {
      //清除掉该候补订单
      u_order.status_ = order_status::kRefunded;
      order_file_.write(u_order, *it, 1);
      Data dt;
      dt.value = u_order.p_idx_;
      string i = string(u_order.trainID_) + IntToString(u_order.d_);
      strcpy(dt.key, i.data());
      pending_bpt_.Remove(dt);
      cout << "0\n";
      return true;
    }
    //Success
    //清除掉该候补订单
    u_order.status_ = order_status::kRefunded;
    order_file_.write(u_order, *it, 1);
    Train i_train;
    int pos = train_bpt_.Find(u_order.trainID_);
    train_file_.read(i_train, pos, 1); //找到train
    Ticket rest;
    ticket_file_.read(rest, i_train.ticket_idx_, 1); //找到余票
    bool s_flag = false;
    bool t_flag = false;
    for (int k = 0; k < i_train.stationNum_; k++) {
      if (!s_flag && strcmp(i_train.stations_[k], u_order.from_) == 0)s_flag = true;
      if (!t_flag && strcmp(i_train.stations_[k], u_order.to_) == 0)t_flag = true;
      if (!t_flag && s_flag) {
        rest.rest_ticket[u_order.d_][k] += u_order.num_;
      }
    }
    //更新完余票信息
    string i = string(u_order.trainID_) + IntToString(u_order.d_);
    sjtu::vector<int> i_pending; //候补队列
    pending_bpt_.MultiFind(i.data(), i_pending);
    auto i_it = i_pending.begin();
    PendingOrder p_order;
    while (i_it != i_pending.end()) {
      pending_file_.read(p_order, *i_it, 1);
      //检查p_order是否可以候补成功
      int s_pos = -1;
      int t_pos = -1;
      for (int k = 0; k < i_train.stationNum_; k++) {
        if (strcmp(i_train.stations_[k], p_order.from_) == 0)s_pos = k;
        else if (strcmp(i_train.stations_[k], p_order.to_) == 0)t_pos = k;
      }
      int max_num = i_train.seatNum_;
      for (int k = s_pos; k < t_pos; k++) {
        max_num = min(max_num, rest.rest_ticket[u_order.d_][k]);
      }
      if (p_order.num_ > max_num) {
        ++i_it;
        continue;
      }
      //能候补上
      Order o;
      order_file_.read(o, p_order.idx_, 1);
      o.status_ = order_status::kSuccess;
      o.p_idx_ = -1;
      order_file_.write(o, p_order.idx_, 1);
      for (int k = s_pos; k < t_pos; k++) {
        rest.rest_ticket[u_order.d_][k] -= p_order.num_;
      }
      Data dt;
      strcpy(dt.key, i.data());
      dt.value = *i_it;
      pending_bpt_.Remove(dt);
      //在候补队列中删除p_order
      ++i_it;
    }
    //遍历完后把余票信息存起来
    ticket_file_.write(rest, i_train.ticket_idx_, 1);
    cout << "0\n";
    return true;
  }

  void Clean() {
    train_bpt_.Clean();
    train_file_.clean();
    station_bpt_.Clean();
    ticket_file_.clean();
    order_bpt_.Clean();
    order_file_.clean();
    pending_bpt_.Clean();
    pending_file_.clean();
  }
};
#endif //TRAINDATABASE_H
