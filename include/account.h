#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "tools.h"

class Account {
public:
  char username_[21]{}; //唯一标识符
  char password_[31]{};
  char name_[21]{};
  char mailAddr_[31]{};
  int privilege_ = 0; //0~10

  Account();

  Account(const string &u, const string &p, const string &n,
          const string &m, const string &g);

  ~Account();

  string GetUsername() const;

  string GetPassword() const;

  string GetName() const;

  string GetMailAddr() const;

  int GetPrivilege() const;

  bool operator==(const Account &other) const;

  bool operator!=(const Account &other) const;

  bool operator<(const Account &other) const;

  bool operator>(const Account &other) const;

  bool operator<=(const Account &other) const;

  bool operator>=(const Account &other) const;
};
#endif //ACCOUNT_H
