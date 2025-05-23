#include "../include/BPlusTree.h"
using std::cin;
using std::cout;

bool operator==(const Data &x, const Data &y) {
  return strcmp(x.key, y.key) == 0 && x.value == y.value;
}

bool operator<(const Data &x, const Data &y) {
  const int cmp = strcmp(x.key, y.key);
  if (cmp < 0)return true;
  if (cmp > 0)return false;
  return x.value < y.value;
}

bool operator<=(const Data &x, const Data &y) {
  return x < y || x == y;
}

bool operator>(const Data &x, const Data &y) {
  const int cmp = strcmp(x.key, y.key);
  if (cmp > 0)return true;
  if (cmp < 0)return false;
  return x.value > y.value;
}

bool operator>=(const Data &x, const Data &y) {
  return x > y || x == y;
}

int lower_bound(int L, int R, const Data &dt, const Node &nd) {
  //找到第一个>=dt的坐标，没有时返回尾端
  if (nd.data[R] < dt) {
    return R;
  }
  if (L >= R) {
    return L;
  }
  auto mid = (L + R) / 2;
  if (nd.data[mid] < dt) {
    return lower_bound(mid + 1, R, dt, nd);
  } else {
    return lower_bound(L, mid, dt, nd);
  }
}

void BPT::Split() {
  Node new_node;
  new_node.size = MAX_SIZE / 2;
  new_node.is_leaf = cur.is_leaf;
  new_node.parent = cur.parent;
  new_node.left = cur_idx; //分裂出的新节点在原节点右侧
  new_node.right = cur.right;
  cur.size = MAX_SIZE / 2;
  if (cur.is_leaf) {
    //叶分裂
    for (int i = 0; i < new_node.size; i++) {
      new_node.data[i] = cur.data[i + cur.size];
    }
    if (cur.parent != -1) {
      Node fa;
      tree.read(fa, cur.parent, 1);
      Data cur_up = cur.data[cur.size - 1];
      Data new_up = new_node.data[new_node.size - 1];
      int p = lower_bound(0, fa.size - 1, new_up, fa);
      if (fa.data[p] != new_up) {
        //不应出现这种情况
        std::cerr << "Leaf:cannot find origin!\n";
      } else {
        for (int i = fa.size - 1; i >= p; i--) {
          fa.data[i + 1] = fa.data[i];
          fa.child[i + 1] = fa.child[i];
        }
        fa.data[p] = cur_up;
        fa.child[p] = cur_idx;
        fa.data[p + 1] = new_up;
        fa.child[p + 1] = tree.push(new_node); //存入new_node的索引
        fa.size++;
        //维护叶节点链
        if (cur.right != -1) {
          Node tmp;
          tree.read(tmp, cur.right, 1);
          tmp.left = fa.child[p + 1];
          tree.write(tmp, cur.right, 1);
        }
        cur.right = fa.child[p + 1];
        tree.write(fa, cur.parent, 1);
        tree.write(cur, cur_idx, 1);
        if (fa.size == MAX_SIZE) {
          cur_idx = cur.parent;
          tree.read(cur, cur.parent, 1);
          Split();
        }
        return;
      }
    } else {
      //当前为根
      Node new_root;
      new_root.is_leaf = false;
      new_root.left = -1;
      new_root.right = -1;
      new_root.parent = -1;
      new_root.size = 2;
      root = tree.push(new_root); //得到新根
      cur.parent = new_node.parent = root;
      new_root.data[0] = cur.data[cur.size - 1];
      new_root.data[1] = new_node.data[new_node.size - 1];
      new_root.child[0] = cur_idx;
      new_root.child[1] = tree.push(new_node);
      cur.right = new_root.child[1];
      tree.write(new_root, root, 1);
      tree.write(cur, cur_idx, 1);
      return;
    }
  } else {
    //非叶分裂
    int new_idx = tree.push(new_node);
    for (int i = 0; i < new_node.size; i++) {
      new_node.data[i] = cur.data[i + cur.size];
      new_node.child[i] = cur.child[i + cur.size];
    }
    //修改父节点
    Node tmp;
    for (int i = 0; i < new_node.size; i++) {
      tree.read(tmp, new_node.child[i], 1);
      tmp.parent = new_idx;
      tree.write(tmp, new_node.child[i], 1);
    }
    if (cur.parent != -1) {
      Node fa;
      tree.read(fa, cur.parent, 1);
      Data cur_up = cur.data[cur.size - 1];
      Data new_up = new_node.data[new_node.size - 1];
      int p = lower_bound(0, fa.size - 1, new_up, fa);
      if (fa.data[p] != new_up) {
        std::cerr << "Not Leaf:cannot find origin!\n";
      } else {
        for (int i = fa.size - 1; i >= p; i--) {
          fa.data[i + 1] = fa.data[i];
          fa.child[i + 1] = fa.child[i];
        }
        fa.data[p] = cur_up;
        fa.child[p] = cur_idx;
        fa.data[p + 1] = new_up;
        fa.child[p + 1] = new_idx; //存入new_node的索引
        fa.size++;
        //维护叶节点链
        if (cur.right != -1) {
          Node tmp2;
          tree.read(tmp2, cur.right, 1);
          tmp2.left = new_idx;
          tree.write(tmp2, cur.right, 1);
        }
        cur.right = new_idx;
        tree.write(fa, cur.parent, 1);
        tree.write(cur, cur_idx, 1);
        tree.write(new_node, new_idx, 1);
        if (fa.size == MAX_SIZE) {
          cur_idx = cur.parent;
          tree.read(cur, cur.parent, 1);
          Split();
        }
        return;
      }
    } else {
      //当前为根
      Node new_root;
      new_root.is_leaf = false;
      new_root.left = -1;
      new_root.right = -1;
      new_root.parent = -1;
      new_root.size = 2;
      root = tree.push(new_root); //得到新根
      cur.parent = new_node.parent = root;
      new_root.data[0] = cur.data[cur.size - 1];
      new_root.data[1] = new_node.data[new_node.size - 1];
      new_root.child[0] = cur_idx;
      new_root.child[1] = new_idx;
      tree.write(new_root, root, 1);
      tree.write(cur, cur_idx, 1);
      tree.write(new_node, new_idx, 1);
      return;
    }
  }
}


void BPT::Insert(const Data &dt) {
  if (root == 0) {
    //空树
    Node new_node;
    new_node.size = 1;
    new_node.is_leaf = true;
    new_node.parent = -1;
    new_node.left = new_node.right = -1;
    new_node.data[0] = dt;
    root = tree.push(new_node);
    return;
  }
  cur_idx = root;
  tree.read(cur, root, 1);
  while (!cur.is_leaf) {
    int i = lower_bound(0, cur.size - 1, dt, cur);
    if (cur.data[i] < dt) {
      //插入最大Data时直接修改索引
      cur.data[i] = dt;
      tree.write(cur, cur_idx, 1);
    }
    cur_idx = cur.child[i];
    tree.read(cur, cur_idx, 1);
  }
  //cur到了叶节点
  int i = lower_bound(0, cur.size - 1, dt, cur);
  if (cur.data[i] == dt) {
    //同key同value
    return;
  }
  if (cur.data[i] < dt) {
    //插入最大
    cur.data[i + 1] = dt;
    cur.size++;
  } else {
    for (int k = cur.size - 1; k >= i; k--) {
      cur.data[k + 1] = cur.data[k];
    }
    cur.data[i] = dt;
    cur.size++;
  }
  tree.write(cur, cur_idx, 1);
  if (cur.size == MAX_SIZE) {
    Split();
  }
}

int BPT::Find(char key[31]) {
  Data dt(key, -2147483648);
  if (root == 0) {
    //空树
    return -1;
  }
  cur_idx = root;
  tree.read(cur, root, 1);
  while (!cur.is_leaf) {
    int i = lower_bound(0, cur.size - 1, dt, cur);
    cur_idx = cur.child[i];
    tree.read(cur, cur_idx, 1);
  }
  //cur到了叶节点
  int i = lower_bound(0, cur.size - 1, dt, cur);
  if (strcmp(cur.data[i].key, key) != 0) {
    return -1;
  }
  return cur.data[i].value;
}

void BPT::MultiFind(char key[31], sjtu::vector<int> &value_list) {
  //一对多的情况下使用
  value_list.clear();
  Data dt(key, -2147483648);
  if (root == 0) {
    //空树
    return;
  }
  cur_idx = root;
  tree.read(cur, root, 1);
  while (!cur.is_leaf) {
    int i = lower_bound(0, cur.size - 1, dt, cur);
    cur_idx = cur.child[i];
    tree.read(cur, cur_idx, 1);
  }
  //cur到了叶节点
  int i = lower_bound(0, cur.size - 1, dt, cur);
  if (strcmp(cur.data[i].key, key) != 0) {
    return;
  }
  int p = i;
  while (strcmp(cur.data[p].key, key) == 0) {
    value_list.push_back(cur.data[p].value);
    if (p < cur.size - 1) {
      p++;
    } else {
      if (cur.right == -1) {
        break;
      } else {
        cur_idx = cur.right;
        tree.read(cur, cur.right, 1);
        p = 0;
      }
    }
  }
}

bool BPT::LeftBorrow() {
  //从cur的左边借个元素
  if (cur.parent == -1) {
    return false;
  }
  if (cur.size == 0) {
    return false;
  }
  int fa_idx = cur.parent;
  Node fa;
  tree.read(fa, fa_idx, 1);
  Data dt = cur.data[cur.size - 1];
  int i = lower_bound(0, fa.size - 1, dt, fa); //找到cur在父节点里的位置
  if (i == 0) {
    //在最左边
    return false;
  } else {
    Node sibling;
    int sibling_idx = fa.child[i - 1];
    tree.read(sibling, sibling_idx, 1);
    if (sibling.size <= MAX_SIZE / 2) {
      return false;
    } else {
      for (int m = cur.size - 1; m >= 0; m--) {
        cur.data[m + 1] = cur.data[m];
        cur.child[m + 1] = cur.child[m];
      }
      cur.data[0] = sibling.data[sibling.size - 1];
      cur.child[0] = sibling.child[sibling.size - 1];
      cur.size++;
      if (!cur.is_leaf) {
        Node son;
        tree.read(son, cur.child[0], 1);
        son.parent = cur_idx;
        tree.write(son, cur.child[0], 1);
      }
      //移动数据
      sibling.size--;
      fa.data[i - 1] = sibling.data[sibling.size - 1];
      tree.write(cur, cur_idx, 1);
      tree.write(sibling, sibling_idx, 1);
      tree.write(fa, fa_idx, 1);
      return true;
    }
  }
}

bool BPT::RightBorrow() {
  //从cur的右边借个元素
  if (cur.parent == -1) {
    return false;
  }
  if (cur.size == 0) {
    return false;
  }
  int fa_idx = cur.parent;
  Node fa;
  tree.read(fa, fa_idx, 1);
  Data dt = cur.data[cur.size - 1];
  int i = lower_bound(0, fa.size - 1, dt, fa); //找到cur在父节点里的位置
  if (i == fa.size - 1) {
    //在最右边
    return false;
  } else {
    Node sibling;
    int sibling_idx = fa.child[i + 1];
    tree.read(sibling, sibling_idx, 1);
    if (sibling.size <= MAX_SIZE / 2) {
      return false;
    } else {
      cur.data[cur.size] = sibling.data[0];
      cur.child[cur.size] = sibling.child[0];
      cur.size++;
      for (int m = 0; m <= sibling.size - 2; m++) {
        sibling.data[m] = sibling.data[m + 1];
        sibling.child[m] = sibling.child[m + 1];
      }
      sibling.size--;
      if (!cur.is_leaf) {
        Node son;
        tree.read(son, cur.child[cur.size - 1], 1);
        son.parent = cur_idx;
        tree.write(son, cur.child[cur.size - 1], 1);
      }
      //移动数据
      fa.data[i] = cur.data[cur.size - 1];
      tree.write(cur, cur_idx, 1);
      tree.write(sibling, sibling_idx, 1);
      tree.write(fa, fa_idx, 1);
      return true;
    }
  }
}

void BPT::Merge() {
  if (cur.parent == -1) {
    return;
  }
  int fa_idx = cur.parent;
  Node fa;
  tree.read(fa, fa_idx, 1);
  Data dt = cur.data[cur.size - 1];
  int i = lower_bound(0, fa.size - 1, dt, fa); //找到cur在父节点里的位置
  if (i != 0) {
    //与左边合并
    Node sibling;
    int sibling_idx = fa.child[i - 1];
    tree.read(sibling, sibling_idx, 1);
    if (!cur.is_leaf) {
      Node son;
      for (int m = 0; m <= cur.size - 1; m++) {
        tree.read(son, cur.child[m], 1);
        son.parent = sibling_idx;
        tree.write(son, cur.child[m], 1);
      }
    }
    //合并非叶节点时，其儿子的父指针要修改掉
    for (int m = 0; m <= cur.size - 1; m++) {
      sibling.data[sibling.size + m] = cur.data[m];
      sibling.child[sibling.size + m] = cur.child[m];
    }
    sibling.size += cur.size;
    cur.size = 0;
    //合并数据
    sibling.right = cur.right;
    if (cur.right != -1) {
      Node tmp;
      tree.read(tmp, cur.right, 1);
      tmp.left = sibling_idx;
      tree.write(tmp, cur.right, 1);
    }
    //维护叶节点链
    for (int m = i; m <= fa.size - 2; m++) {
      fa.data[m] = fa.data[m + 1];
      fa.child[m] = fa.child[m + 1];
    }
    fa.size--;
    fa.data[i - 1] = sibling.data[sibling.size - 1];
    tree.write(sibling, sibling_idx, 1);
    tree.write(fa, fa_idx, 1);
    //修改父节点中索引
    if (fa.size < MAX_SIZE / 2) {
      //父亲需平衡
      cur_idx = fa_idx;
      tree.read(cur, fa_idx, 1);
      Balance();
    }
    return;
  }
  if (i != fa.size - 1) {
    //与右边合并
    Node sibling;
    int sibling_idx = fa.child[i + 1];
    tree.read(sibling, sibling_idx, 1);
    if (!sibling.is_leaf) {
      Node son;
      for (int m = 0; m <= sibling.size - 1; m++) {
        tree.read(son, sibling.child[m], 1);
        son.parent = cur_idx;
        tree.write(son, sibling.child[m], 1);
      }
    }
    //合并非叶节点时，其儿子的父指针要修改掉
    for (int m = 0; m < sibling.size; m++) {
      cur.data[cur.size + m] = sibling.data[m];
      cur.child[cur.size + m] = sibling.child[m];
    }
    cur.size += sibling.size;
    //合并数据
    cur.right = sibling.right;
    if (sibling.right != -1) {
      Node tmp;
      tree.read(tmp, sibling.right, 1);
      tmp.left = cur_idx;
      tree.write(tmp, sibling.right, 1);
    }
    //维护叶节点链
    for (int m = i + 1; m <= fa.size - 2; m++) {
      fa.data[m] = fa.data[m + 1];
      fa.child[m] = fa.child[m + 1];
    }
    fa.size--;
    fa.data[i] = cur.data[cur.size - 1];
    tree.write(cur, cur_idx, 1);
    tree.write(fa, fa_idx, 1);
    //修改父节点中索引
    if (fa.size < MAX_SIZE / 2) {
      //父亲需平衡
      cur_idx = fa_idx;
      tree.read(cur, fa_idx, 1);
      Balance();
    }
    return;
  } else {
    std::cerr << "Cannot Merge!\n";
    //该情况不应出现
  }
}

//完成修改操作后，若cur失衡则Balance
void BPT::Balance() {
  if (cur.parent == -1) {
    if (cur.size == 0) {
      root = 0;
    }
    if (!cur.is_leaf && cur.size == 1) {
      root = cur.child[0];
      Node tmp;
      tree.read(tmp, cur.child[0], 1);
      tmp.parent = -1;
      tree.write(tmp, cur.child[0], 1);
    }
    return;
  }
  if (LeftBorrow()) {
    //左借成功
    return;
  }
  if (RightBorrow()) {
    //右借成功
    return;
  }
  //合并
  Merge();
}

void BPT::Remove(const Data &dt) {
  if (root == 0) {
    //空树
    return;
  }
  cur_idx = root;
  tree.read(cur, root, 1);
  while (!cur.is_leaf) {
    int i = lower_bound(0, cur.size - 1, dt, cur);
    cur_idx = cur.child[i];
    tree.read(cur, cur_idx, 1);
  }
  //cur到了叶节点
  int i = lower_bound(0, cur.size - 1, dt, cur);
  if (cur.data[i] != dt) {
    //不存在
    return;
  } else {
    for (int m = i; m <= cur.size - 2; m++) {
      cur.data[m] = cur.data[m + 1];
    }
    cur.size--;
    tree.write(cur, cur_idx, 1);
    Data new_dt = cur.data[cur.size - 1];
    Node tmp;
    int tmp_idx = cur.parent;
    while (tmp_idx != -1) {
      //std::cerr << "modify the index in parents!\n";
      //为节点最大键，需修改父索引
      tree.read(tmp, tmp_idx, 1);
      int p = lower_bound(0, tmp.size - 1, dt, tmp);
      if (tmp.data[p] == dt) {
        tmp.data[p] = new_dt;
        tree.write(tmp, tmp_idx, 1);
        tmp_idx = tmp.parent;
      } else break;
    }
    if (cur.size < MAX_SIZE / 2) {
      Balance(); //对cur进行平衡操作
    }
  }
}
