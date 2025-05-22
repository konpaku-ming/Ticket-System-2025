#ifndef TOKENSCANNER_H
#define TOKENSCANNER_H

#include "tools.h"
#include "vector.h"

class TokenScanner {
public:
  string c_{}, u_{}, p_{}, n_{}, m_{}, g_{}, i_{},
      s_{}, x_{}, t_{}, o_{}, d_{}, y_{}, f_{}, q_{};
  string op_{};
  string timestamp_{};

  TokenScanner();

  TokenScanner(const string &cmd);

  ~TokenScanner();
};


#endif //TOKENSCANNER_H
