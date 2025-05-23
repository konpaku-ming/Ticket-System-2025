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
  //TODO

  TrainDatabase() {
    train_file_.initialise("train_data");
  }

  ~TrainDatabase() = default;

  bool AddTrain(const TokenScanner &tokens) {
    char train_id[21];
    if (tokens.i_.length() < 21) {
      strcpy(train_id, tokens.i_.data());
    }
    if (train_bpt_.Find(train_id) != -1)return false;
    Train tmp(tokens.i_, tokens.n_, tokens.m_, tokens.s_, tokens.p_,
              tokens.x_, tokens.t_, tokens.o_, tokens.d_, tokens.y_);
    int idx = train_file_.push(tmp);
    const Data tmp_dt(tmp.trainID_, idx);
    train_bpt_.Insert(tmp_dt);
    cout << "0\n";
    return true;
  }
};
#endif //TRAINDATABASE_H
