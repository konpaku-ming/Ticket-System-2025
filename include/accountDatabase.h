#ifndef ACCOUNTDATABASE_H
#define ACCOUNTDATABASE_H

#include "account.h"
#include "BPlusTree.h"
#include "tools.h"
#include "vector.h"

class AccountDatabase {
  BPT account_bpt_;
  MemoryRiver<Account, 0> account_file_;

public:
  bool AddUser(Account &x) {
    int pos = account_bpt_.Find(x.username_);
    if (pos != -1)return false;
    int idx = account_file_.push(x);
    const Data tmp_dt(x.username_, idx);
    account_bpt_.Insert(tmp_dt);
    cout << "1\n";
    return true;
  }

  //检查完-c是否登录后再Query
  bool QueryProfile(int c_idx, const string &u) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    }
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
  bool ModifyProfile(int c_idx, const string &u, sjtu::vector<string> &info) {
    char username[21];
    if (u.length() < 21) {
      strcpy(username, u.data());
    }
    int pos = account_bpt_.Find(username);
    if (pos == -1)return false; //-u不存在
    if (pos == c_idx) {
      //-u == -c
      Account u_account;
      account_file_.read(u_account, pos, 1);
      if (!info[3].empty()) {
        //privilege(最先检查)
        int privilege = StringToInt(info[3]);
        if (u_account.privilege_ <= privilege)return false;
        u_account.privilege_ = privilege;
      }
      if (!info[0].empty()) {
        //password
        strcpy(u_account.password_, info[0].data());
      }
      if (!info[1].empty()) {
        //name
        strcpy(u_account.name_, info[1].data());
      }
      if (!info[2].empty()) {
        //mailAddr
        strcpy(u_account.mailAddr_, info[2].data());
      }
      cout << u_account.GetUsername() << " " << u_account.GetName() << " "
          << u_account.GetMailAddr() << " " << u_account.GetPrivilege();
      return true;
    }
    //-c与-u不同,要求-c权限大于-u
    Account c_account;
    Account u_account;
    account_file_.read(c_account, c_idx, 1);
    account_file_.read(u_account, pos, 1);
    if (c_account.privilege_ <= u_account.privilege_)return false;
    if (!info[3].empty()) {
      //privilege(最先检查)
      int privilege = StringToInt(info[3]);
      if (c_account.privilege_ <= privilege)return false;
      u_account.privilege_ = privilege;
    }
    if (!info[0].empty()) {
      //password
      strcpy(u_account.password_, info[0].data());
    }
    if (!info[1].empty()) {
      //name
      strcpy(u_account.name_, info[1].data());
    }
    if (!info[2].empty()) {
      //mailAddr
      strcpy(u_account.mailAddr_, info[2].data());
    }
    cout << u_account.GetUsername() << " " << u_account.GetName() << " "
        << u_account.GetMailAddr() << " " << u_account.GetPrivilege() << "\n";
    return true;
  }
};
#endif //ACCOUNTDATABASE_H
