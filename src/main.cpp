#include "../include/BPlusTree.h"
#include "../include/map.h"
#include "../include/account.h"
#include "../include/accountDatabase.h"
#include "../include/tokenscanner.h"
#include "../include/vector.h"
#include "../include/trainDatabase.h"

sjtu::map<string, int> login_map; //将登录的账户全部放进login_map便于检查登录情况
AccountDatabase account_database; //用户信息库
TrainDatabase train_database; //售票系统信息库

int main() {
  string cmd;
  while (getline(cin, cmd)) {
    TokenScanner tokens(cmd);
    cout << tokens.timestamp_ << " ";
    if (tokens.op_ == "exit") {
      login_map.clear();
      cout << "bye\n";
      break;
    }
    if (tokens.op_ == "add_user") {
      if (account_database.isAdmin) {
        //初次加入用户
        account_database.isAdmin = false;
        Account tmp(tokens.u_, tokens.p_, tokens.n_, tokens.m_, "10");
        bool flag = account_database.AddUser(tmp);
        if (!flag)cout << "-1\n";
      } else {
        auto it = login_map.find(tokens.c_);
        if (it == login_map.end()) cout << "-1\n";
        else {
          int c_idx = it->second;
          Account tmp(tokens.u_, tokens.p_, tokens.n_, tokens.m_, tokens.g_);
          Account c_account;
          account_database.account_file_.read(c_account, c_idx, 1);
          if (tmp.privilege_ >= c_account.privilege_)cout << "-1\n";
          else {
            bool flag = account_database.AddUser(tmp);
            if (!flag)cout << "-1\n";
          }
        }
      }
    } else if (tokens.op_ == "login") {
      auto it = login_map.find(tokens.u_);
      if (it != login_map.end()) cout << "-1\n";
      else {
        int pos = account_database.Login(tokens.u_, tokens.p_);
        if (pos == -1)cout << "-1\n";
        else {
          login_map.insert({tokens.u_, pos});
          cout << "0\n";
        }
      }
    } else if (tokens.op_ == "logout") {
      auto it = login_map.find(tokens.u_);
      if (it == login_map.end()) cout << "-1\n";
      else {
        login_map.erase(it);
        cout << "0\n";
      }
    } else if (tokens.op_ == "query_profile") {
      auto it = login_map.find(tokens.c_);
      if (it == login_map.end()) cout << "-1\n";
      bool flag = account_database.QueryProfile(it->second, tokens.u_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "modify_profile") {
      auto it = login_map.find(tokens.c_);
      if (it == login_map.end()) cout << "-1\n";
      bool flag = account_database.ModifyProfile(it->second, tokens.u_, tokens);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "add_train") {
      bool flag = train_database.AddTrain(tokens);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "delete_train") {
      bool flag = train_database.DeleteTrain(tokens.i_);
      if (!flag)cout << "-1\n";
    }
  }
  return 0;
}
