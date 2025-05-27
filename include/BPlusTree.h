#ifndef BPlusTree_H
#define BPlusTree_H
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <utility>
#include "tools.h"
#include "vector.h"

constexpr int MAX_SIZE = 100;

class BPT;

//MemoryRiver,用于绑定文件并提供一些操作
template<class T, int info_len>
class MemoryRiver {
  friend BPT;

private:
  std::fstream file;
  std::string file_name;
  int sizeofT = sizeof(T);

public:
  MemoryRiver() = default;

  MemoryRiver(const std::string &file_name) : file_name(file_name) {
  }

  bool initialise(std::string FN = "") {
    if (FN != "")file_name = FN;
    file.open(file_name, std::ios::in | std::ios::out);
    if (file.is_open()) {
      return false; //文件已存在，未初始化
    }
    file.open(file_name, std::ios::out);
    file.close();
    file.open(file_name, std::ios::in | std::ios::out);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i) {
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int)); //初始化文件
    }
    return true; //成功初始化
  }

  void clean() {
    file.close();
    file.open(file_name, std::ios::out);
    file.close();
    file.open(file_name, std::ios::in | std::ios::out);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i) {
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int)); //初始化文件
    }
  }

  void exit() {
    file.close();
  }

  void get_info(int &tmp, int n) {
    //读入第n个整数
    if (n > info_len) return;
    file.seekg((n - 1) * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  void write_info(int tmp, int n) {
    //写入第n个整数
    if (n > info_len) return;
    file.seekp((n - 1) * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  void write(T &t, int place, int size) {
    //一次性在place位置写入size个数据
    file.seekp(place);
    file.write(reinterpret_cast<char *>(&t), sizeofT * size);
  }

  int push(T &t) {
    //将数据写到文件尾部，并返回其地址
    file.seekp(0, file.end);
    const int index = file.tellp();
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    return index;
  }

  void read(T &t, const int idx, int size) {
    //一次性在idx位置读出size个数据
    int position = idx;
    file.seekg(position, std::ios::beg);
    file.read(reinterpret_cast<char *>(&t), sizeofT * size);
  }
};

struct Data {
  char key[31]{};
  int value{};

  Data() = default;

  Data(char key_[31], int value_) {
    strcpy(key, key_);
    value = value_;
  }

  Data &operator=(const Data &other) {
    if (this == &other) {
      return *this;
    }
    strcpy(key, other.key);
    value = other.value;
    return *this;
  }

  friend bool operator==(const Data &, const Data &);

  friend bool operator<(const Data &, const Data &);

  friend bool operator<=(const Data &, const Data &);

  friend bool operator>(const Data &, const Data &);

  friend bool operator>=(const Data &, const Data &);
};

struct Node {
  bool is_leaf = true; //是否为叶节点
  Data data[MAX_SIZE + 2]{}; //节点里的元素
  int parent = -1; //父节点索引，root为-1
  int child[MAX_SIZE + 1]{}; //子节点索引
  int left = -1; //左兄弟索引
  int right = -1; //右兄弟索引
  int size = 0; //元素数量

  Node() = default;

  Node(const Node &other) = default;
};

class BPT {
public:
  BPT() {
    tree.initialise("Tree");
    tree.get_info(root, 1);
  }

  explicit BPT(std::string FN) {
    tree.initialise(std::move(FN));
    tree.get_info(root, 1);
  }

  ~BPT() {
    tree.write_info(root, 1);
    tree.exit();
  }

private:
  int root;
  MemoryRiver<Node, 2> tree;
  Node cur;
  int cur_idx;

  void Split(); //将cur节点分裂，在节点大小达到MAX_SIZE时执行

  bool LeftBorrow(); //删除元素时为维持平衡向左借元素

  bool RightBorrow(); //删除元素时为维持平衡向右借元素

  void Merge(); //与左右节点合并，优先左，次优右，仅当左右均无法借时执行

  void Balance(); //平衡cur节点

public:
  int Find(char key[31]);

  void MultiFind(char key[31], sjtu::vector<int> &value_list);

  void Insert(const Data &);

  void Remove(const Data &);

  void Clean(); //清除文件
};
#endif //BPlusTree
