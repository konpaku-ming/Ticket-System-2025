#include "../include/train.h"

void Train::StringToStations(const string &s) {
  sjtu::vector<string> station;
  Split(s, station, '|');
  for (int i = 0; i < station.size(); i++) {
    strcpy(stations_[i], station[i].data());
  }
}

void Train::StringToPrices(const string &p) {
  sjtu::vector<string> price;
  Split(p, price, '|');
  for (int i = 0; i < price.size(); i++) {
    prices_[i + 1] = prices_[i] + StringToInt(price[i]);
  }
}

void Train::StringToAllTime(const string &x, const string &t, const string &o) {
  sjtu::vector<string> start_time;
  sjtu::vector<string> travel_time;
  sjtu::vector<string> stopover_time;
  Split(x, start_time, ':');
  Split(t, travel_time, '|');
  Split(o, stopover_time, '|');
  leaveTime_[0] = Time(StringToInt(start_time[0]), StringToInt(start_time[1]), 0);
  for (int i = 1; i < stationNum_ - 1; i++) {
    arriveTime_[i] = leaveTime_[i - 1] + StringToInt(travel_time[i - 1]);
    leaveTime_[i] = arriveTime_[i] + StringToInt(stopover_time[i - 1]);
  }
  arriveTime_[stationNum_ - 1] = leaveTime_[stationNum_ - 2] + StringToInt(travel_time[stationNum_ - 2]);
}

void Train::StringToSaleDate(const string &d) {
  sjtu::vector<string> sale_date;
  Split(d, sale_date, '|');
  sjtu::vector<string> date;
  Split(sale_date[0], date, '-');
  saleDate_[0] = Date(StringToInt(date[0]), StringToInt(date[1]));
  Split(sale_date[1], date, '-');
  saleDate_[1] = Date(StringToInt(date[0]), StringToInt(date[1]));
}

Train::Train() = default;

Train::Train(const string &i, const string &n, const string &m, const string &s,
             const string &p, const string &x, const string &t, const string &o,
             const string &d, const string &y) {
  strcpy(trainID_, i.data());
  stationNum_ = StringToInt(n);
  seatNum_ = StringToInt(m);
  this->StringToStations(s);
  this->StringToPrices(p);
  this->StringToAllTime(x, t, o);
  this->StringToSaleDate(d);
  type_ = y[0];
}

Train::~Train() = default;
