#include "../include/BPlusTree.h"
#include "../include/map.h"
#include "../include/account.h"
#include "../include/accountDatabase.h"
#include "../include/tokenscanner.h"
#include "../include/vector.h"

sjtu::map<string, int> login_map; //将登录的账户全部放进login_map便于检查登录情况
AccountDatabase account_database; //用户信息库

int main() {
  string cmd;
  while (getline(cin, cmd)) {
    TokenScanner tokens(cmd);

  }
  return 0;
}
