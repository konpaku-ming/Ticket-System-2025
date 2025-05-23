#ifndef ACCOUNTDATABASE_H
#define ACCOUNTDATABASE_H

#include "account.h"
#include "BPlusTree.h"
#include "tokenscanner.h"
#include "tools.h"

class AccountDatabase {
public:
  BPT account_bpt_{"account_bpt"};
  MemoryRiver<Account, 0> account_file_;
  bool isAdmin = true;

  AccountDatabase() {
    bool flag = account_file_.initialise("account_data");
    if (!flag)isAdmin = false;
  }

  ~AccountDatabase() = default;

  bool AddUser(Account &x) {
    int pos = account_bpt_.Find(x.username_);
    if (pos != -1)return false;
    int idx = account_file_.push(x);
    const Data tmp_dt(x.username_, idx);
    account_bpt_.Insert(tmp_dt);
    cout << "0\n";
    return true;
  }

  //先检查-u是否登录
  int Login(const string &u, const string &p) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    } else return -1;
    int pos = account_bpt_.Find(username);
    if (pos == -1)return -1; //-u不存在
    Account u_account;
    account_file_.read(u_account, pos, 1);
    if (strcmp(u_account.password_, p.data()) != 0) {
      return -1;
    }
    return pos;
  }

  //检查完-c是否登录后再Query
  bool QueryProfile(const int c_idx, const string &u) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    } else return false;
    int pos = account_bpt_.Find(username);
    if (pos == -1)return false; //-u不存在
    if (pos == c_idx) {
      //-u == -c
      Account u_account;
      account_file_.read(u_account, pos, 1);
      cout << u_account.GetUsername() << " " << u_account.GetName() << " "
          << u_account.GetMailAddr() << " " << u_account.GetPrivilege() << "\n";
      return true;
    }
    //-c与-u不同,要求-c权限大于-u
    Account c_account;
    Account u_account;
    account_file_.read(c_account, c_idx, 1);
    account_file_.read(u_account, pos, 1);
    if (c_account.privilege_ <= u_account.privilege_)return false;
    cout << u_account.GetUsername() << " " << u_account.GetName() << " "
        << u_account.GetMailAddr() << " " << u_account.GetPrivilege() << "\n";
    return true;
  }

  //同样要先检查完-c是否登录
  bool ModifyProfile(int c_idx, const string &u, const TokenScanner &tokens) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    } else return false;
    int pos = account_bpt_.Find(username);
    if (pos == -1)return false; //-u不存在
    if (pos == c_idx) {
      //-u == -c
      Account u_account;
      account_file_.read(u_account, pos, 1);
      if (!tokens.g_.empty()) {
        //privilege(最先检查)
        int privilege = StringToInt(tokens.g_);
        if (u_account.privilege_ <= privilege)return false;
        u_account.privilege_ = privilege;
      }
      if (!tokens.p_.empty()) {
        //password
        strcpy(u_account.password_, tokens.p_.data());
      }
      if (!tokens.n_.empty()) {
        //name
        strcpy(u_account.name_, tokens.n_.data());
      }
      if (!tokens.m_.empty()) {
        //mailAddr
        strcpy(u_account.mailAddr_, tokens.m_.data());
      }
      cout << u_account.GetUsername() << " " << u_account.GetName() << " "
          << u_account.GetMailAddr() << " " << u_account.GetPrivilege();
      account_file_.write(u_account, pos, 1);
      return true;
    }
    //-c与-u不同,要求-c权限大于-u
    Account c_account;
    Account u_account;
    account_file_.read(c_account, c_idx, 1);
    account_file_.read(u_account, pos, 1);
    if (c_account.privilege_ <= u_account.privilege_)return false;
    if (!tokens.g_.empty()) {
      //privilege(最先检查)
      int privilege = StringToInt(tokens.g_);
      if (c_account.privilege_ <= privilege)return false;
      u_account.privilege_ = privilege;
    }
    if (!tokens.p_.empty()) {
      //password
      strcpy(u_account.password_, tokens.p_.data());
    }
    if (!tokens.n_.empty()) {
      //name
      strcpy(u_account.name_, tokens.n_.data());
    }
    if (!tokens.m_.empty()) {
      //mailAddr
      strcpy(u_account.mailAddr_, tokens.m_.data());
    }
    cout << u_account.GetUsername() << " " << u_account.GetName() << " "
        << u_account.GetMailAddr() << " " << u_account.GetPrivilege() << "\n";
    account_file_.write(u_account, pos, 1);
    return true;
  }
};
#endif //ACCOUNTDATABASE_H
