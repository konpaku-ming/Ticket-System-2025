#include "../include/tokenscanner.h"

TokenScanner::TokenScanner() = default;

TokenScanner::~TokenScanner() = default;

TokenScanner::TokenScanner(const string &cmd) {
  sjtu::vector<string> list;
  Split(cmd, list, ' ');
  timestamp_ = std::move(list[0]);
  op_ = std::move(list[1]);
  for (auto it = 2; it < list.size(); it++) {
    if (list[it] == "-c")c_ = std::move(list[++it]);
    else if (list[it] == "-u")u_ = std::move(list[++it]);
    else if (list[it] == "-p")p_ = std::move(list[++it]);
    else if (list[it] == "-n")n_ = std::move(list[++it]);
    else if (list[it] == "-m")m_ = std::move(list[++it]);
    else if (list[it] == "-g")g_ = std::move(list[++it]);
    else if (list[it] == "-i")i_ = std::move(list[++it]);
    else if (list[it] == "-s")s_ = std::move(list[++it]);
    else if (list[it] == "-x")x_ = std::move(list[++it]);
    else if (list[it] == "-t")t_ = std::move(list[++it]);
    else if (list[it] == "-o")o_ = std::move(list[++it]);
    else if (list[it] == "-d")d_ = std::move(list[++it]);
    else if (list[it] == "-y")y_ = std::move(list[++it]);
    else if (list[it] == "-f")f_ = std::move(list[++it]);
    else if (list[it] == "-q")q_ = std::move(list[++it]);
  }
}
