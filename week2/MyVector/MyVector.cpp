#include "MyVector.h"
#include <algorithm>    // std::copy / std::max 可能会用到

// ============================================================
//  MyVector 的实现 —— 这部分是【你要写的】
// ============================================================
//  怎么判断写对没有：
//      编译运行 main.cpp，它会一项一项打印 PASS / FAIL。
//      全部 PASS 就说明 MyVector v1 完成了。
//
//  建议的实现顺序（从简单到难，每完成一步就编译跑一次）：
//      ① 空的构造函数、析构函数、size/capacity/empty/data
//      ② operator[]（两个版本）
//      ③ push_back（先假定容量够，不做扩容）
//      ④ grow（真正实现扩容）
//      ⑤ push_back 里加上「容量不够就 grow」
//      ⑥ pop_back
//      ⑦ MyVector(size_t n)
// ============================================================


// ------------------------------------------------------------
// ① 构造 / 析构
// ------------------------------------------------------------
MyVector::MyVector() {
    // TODO: 空的 MyVector 应该是什么状态？
    //       没有数据 -> data_ 指向哪里？
    //       装了几个？申请了几个格子？
    //
    // 提示：这三个成员必须全部初始化。
    //       只初始化一部分，剩下的是随机垃圾值，后面必然出诡异问题。
    data_     = nullptr;
    size_     = 0;
    capacity_ = 0;
}

MyVector::MyVector(size_t n) {
    // TODO: 造 n 个元素，初值都是 0。
    //       1. 申请 n 个 int 的内存（data_ = new int[n];）
    //       2. 把 n 个元素都设成 0（循环自己写）
    //       3. size_ 和 capacity_ 都设成 n
    //
    // 注意特殊情况：n == 0 的时候，new int[0] 是合法的，
    //       但更清爽的做法是干脆不申请，data_ = nullptr。
    data_ = new int[n];
    size_ = n;
    capacity_ = n;
    for (size_t i = 0; i < n; i++) {
        data_[i] = 0;
    }
}

MyVector::~MyVector() {
    // TODO: 释放 data_ 指向的内存。
    //       ① 用 delete[]（不是 delete！这是数组）
    //       ② 释放完把 data_ 设成 nullptr，避免「重复释放」
    //
    // 不写这一句会怎样：程序不会立刻崩，但内存泄漏。
    //       VS 在调试模式下退出时会在「输出」窗口打印泄漏报告，可以自己验证。
    delete[] data_;
    data_ = nullptr;
}


// ------------------------------------------------------------
// ② 容量（这几个是一行就能写完的，已经给你了）
// ------------------------------------------------------------
size_t MyVector::size() const     { return size_; }
size_t MyVector::capacity() const { return capacity_; }
bool   MyVector::empty() const    { return size_ == 0; }
int*   MyVector::data() const     { return data_; }


// ------------------------------------------------------------
// ③ 元素访问
// ------------------------------------------------------------
int& MyVector::operator[](size_t i) {
    // TODO: 返回第 i 个元素。一行就够。
    return data_[i];
}

const int& MyVector::operator[](size_t i) const {
    // TODO: 同上，只是给 const 对象用。
    return data_[i];
}


// ------------------------------------------------------------
// ④ 扩容：申请 -> 拷贝 -> 释放
// ------------------------------------------------------------
void MyVector::grow() {
    // TODO: 这是整个练习的核心，按下面三步写，顺序不能错。
    //
    //   第一步：算出新容量
    //       size_t newCap = (capacity_ == 0) ? 1 : capacity_ * 2;
    //       （为什么不能直接 capacity_ * 2？想想 capacity_ 是 0 的情况）
    //
    //   第二步：申请新内存，把旧数据拷过去
    //       int* newData = new int[newCap];
    //       for (size_t i = 0; i < size_; i++) newData[i] = data_[i];
    //
    //   第三步：释放旧内存，切换过去
    //       delete[] data_;
    //       data_ = newData;
    //       capacity_ = newCap;
    //
    //   注意：size_ 不变！扩容不改「装了几个」，只改「能装几个」。
    size_t newCa = (capacity_ == 0) ? 1 : capacity_ * 2;
    int* newData = new int[newCa];
    for (size_t i = 0; i < size_; i++) {
        newData[i] = data_[i];
    }
    delete[]data_;
    data_ = newData;
    capacity_ = newCa;
}


// ------------------------------------------------------------
// ⑤ 增删
// ------------------------------------------------------------
void MyVector::push_back(int value) {
    // TODO: 在末尾加一个元素，两件事：
    //       ① 如果 size_ == capacity_，说明没地方了 -> 调用 grow()
    //       ② 把 value 放到 data_[size_] 的位置，然后 size_ 加 1
    //
    // 顺序很重要：先确保有地方，再放东西。
    if (size_ == capacity_) {
        grow();
         data_[size_++] = value;
    }
    else {
        data_[size_++] = value;
    }

}

void MyVector::pop_back() {
    // TODO: 删掉最后一个元素。
    //       ① 如果 size_ == 0，什么都不做（不能让它变成负数！）
    //       ② 否则 size_ 减 1
    //
    // 想一想：需要 delete 掉那个元素、或者缩小 capacity_ 吗？
    //       答案是「不需要」——
    //       元素本身是 int，没有自己的资源；
    //       capacity_ 留着下次用更划算（std::vector 也是这么做的）。
    if (size_ == 0)return; else {
        size_--;
    }
}

void MyVector::clear() {
    size_ = 0;
}

MyVector::MyVector(const MyVector& other) {
    capacity_ = other.capacity_;
    data_ = new int[capacity_];
    size_ = other.size_;
    for (int i = 0; i < size_; i++) {
        data_[i] = other.data_[i];
    }
}

MyVector&MyVector:: operator=(const MyVector& other) {
    if (&other == this)return *this;
    capacity_ = other.capacity_;
    delete[]data_;
    data_ = nullptr;
    data_ = new int[capacity_];
    size_ = other.size_;
    for (int i = 0; i < size_; i++) {
        data_[i] = other.data_[i];
    }return *this;
}