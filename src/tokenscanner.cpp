#include "../include/tokenscanner.h"

TokenScanner::TokenScanner() = default;

TokenScanner::~TokenScanner() = default;

TokenScanner::TokenScanner(const string &cmd) {
  sjtu::vector<string> list;
  Split(cmd, list, ' ');
  timestamp_ = list[0];
  op_ = list[1];
  for (auto it = 2; it < list.size(); it++) {
    if (list[it] == "-c")c_ = list[++it];
    else if (list[it] == "-u")u_ = list[++it];
    else if (list[it] == "-p")p_ = list[++it];
    else if (list[it] == "-n")n_ = list[++it];
    else if (list[it] == "-m")m_ = list[++it];
    else if (list[it] == "-g")g_ = list[++it];
    else if (list[it] == "-i")i_ = list[++it];
    else if (list[it] == "-s")s_ = list[++it];
    else if (list[it] == "-x")x_ = list[++it];
    else if (list[it] == "-t")t_ = list[++it];
    else if (list[it] == "-o")o_ = list[++it];
    else if (list[it] == "-d")d_ = list[++it];
    else if (list[it] == "-y")y_ = list[++it];
    else if (list[it] == "-f")f_ = list[++it];
    else if (list[it] == "-q")q_ = list[++it];
  }
}
