#include "../include/BPlusTree.h"
#include "../include/map.h"
#include "../include/account.h"
#include "../include/accountDatabase.h"
#include "../include/tokenscanner.h"
#include "../include/vector.h"
#include "../include/trainDatabase.h"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  //freopen("testcases/1867/1.in", "r",stdin);
  //freopen("test.out", "w",stdout);
  sjtu::map<string, int> login_map; //将登录的账户全部放进login_map便于检查登录情况
  AccountDatabase account_database; //用户信息库
  TrainDatabase train_database; //售票系统信息库
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
      if (it == login_map.end()) {
        cout << "-1\n";
        continue;
      }
      bool flag = account_database.QueryProfile(it->second, tokens.u_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "modify_profile") {
      auto it = login_map.find(tokens.c_);
      if (it == login_map.end()) {
        cout << "-1\n";
        continue;
      }
      bool flag = account_database.ModifyProfile(it->second, tokens.u_, tokens);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "add_train") {
      bool flag = train_database.AddTrain(tokens);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "delete_train") {
      bool flag = train_database.DeleteTrain(tokens.i_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "release_train") {
      bool flag = train_database.ReleaseTrain(tokens.i_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "query_train") {
      bool flag = train_database.QueryTrain(tokens.i_, tokens.d_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "query_ticket") {
      bool flag;
      if (tokens.p_ == "cost") {
        flag = train_database.QueryTicketByCost(tokens.s_, tokens.t_, tokens.d_);
      } else flag = train_database.QueryTicketByTime(tokens.s_, tokens.t_, tokens.d_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "query_transfer") {
      bool flag;
      if (tokens.p_ == "cost") {
        flag = train_database.QueryTransferByCost(tokens.s_, tokens.t_, tokens.d_);
      } else flag = train_database.QueryTransferByTime(tokens.s_, tokens.t_, tokens.d_);
      if (!flag)cout << "0\n";
    } else if (tokens.op_ == "buy_ticket") {
      auto it = login_map.find(tokens.u_);
      if (it == login_map.end()) {
        cout << "-1\n";
        continue;
      }
      bool flag = train_database.BuyTicket(tokens);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "query_order") {
      auto it = login_map.find(tokens.u_);
      if (it == login_map.end()) {
        cout << "-1\n";
        continue;
      }
      bool flag = train_database.QueryOrder(tokens.u_);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "refund_ticket") {
      auto it = login_map.find(tokens.u_);
      if (it == login_map.end()) {
        cout << "-1\n";
        continue;
      }
      int n = 1;
      if (!tokens.n_.empty())n = StringToInt(tokens.n_);
      bool flag = train_database.RefundTicket(tokens.u_, n);
      if (!flag)cout << "-1\n";
    } else if (tokens.op_ == "clean") {
      login_map.clear();
      account_database.Clean();
      train_database.Clean();
      cout << "0\n";
    }
  }
  return 0;
}
