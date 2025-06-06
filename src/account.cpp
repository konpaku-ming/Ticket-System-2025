#include "../include/account.h"

Account::Account() = default;

Account::Account(const string &u, const string &p, const string &n,
                 const string &m, const string &g) {
  strcpy(username_, u.data());
  strcpy(password_, p.data());
  strcpy(name_, n.data());
  strcpy(mailAddr_, m.data());
  privilege_ = StringToInt(g);
}

Account::~Account() = default;

string Account::GetUsername() const {
  return username_;
}

string Account::GetPassword() const {
  return password_;
}

string Account::GetName() const {
  return name_;
}

string Account::GetMailAddr() const {
  return mailAddr_;
}

int Account::GetPrivilege() const {
  return privilege_;
}

bool Account::operator==(const Account &other) const {
  return strcmp(username_, other.username_) == 0;
}

bool Account::operator!=(const Account &other) const {
  return strcmp(username_, other.username_) != 0;
}

bool Account::operator>(const Account &other) const {
  return strcmp(username_, other.username_) > 0;
}

bool Account::operator<(const Account &other) const {
  return strcmp(username_, other.username_) < 0;
}

bool Account::operator>=(const Account &other) const {
  return strcmp(username_, other.username_) >= 0;
}

bool Account::operator<=(const Account &other) const {
  return strcmp(username_, other.username_) <= 0;
}
