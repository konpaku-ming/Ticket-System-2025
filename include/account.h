#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "../include/tools.h"
#include "../include/BPlusTree.h"

class Account {
  char username_[21]{}; //唯一标识符
  char password_[31]{};
  char name_[21]{};
  char mailAddr_[31]{};
  int privilege_ = 0; //0~10
  bool is_login_ = false; //是否登录

  Account();

  Account(const string &u, const string &p, const string &n,
          const string &m, const string &g);

  ~Account();

  bool operator==(const Account &other) const;

  bool operator!=(const Account &other) const;

  bool operator<(const Account &other) const;

  bool operator>(const Account &other) const;

  bool operator<=(const Account &other) const;

  bool operator>=(const Account &other) const;
};
#endif //ACCOUNT_H
