#pragma once
#include<utility>
#include <cstddef>      // size_t 在这里

// ============================================================
//  MyVector —— 你自己实现的「动态数组」
// ============================================================
//  目标：用一块「能扩容的连续内存」，模仿 std::vector 的核心行为。
//
//  「接口先行」是什么意思：
//      先把下面这些函数的名字、参数、返回值、职责全部定下来，
//      确认没漏、没重复，再动手写实现。
//      不要边想边写 —— 那样写到一半就会发现接口要改，前面的白写了。
//
//  这个头文件就是「定好的接口」。你可以改它，但改之前先想清楚为什么。
// ============================================================

class MyVector {
public:
    // ---------- 构造 / 析构 ----------
    // 负责「申请」和「释放」这块堆内存
    MyVector();                          // 造一个空的
    explicit MyVector(size_t n);         // 造 n 个元素，初值都是 0
    ~MyVector();                         // 释放内存（不写就是内存泄漏）
    MyVector(const MyVector& other);
    /*MyVector& operator=(const MyVector& other);*/
    MyVector& operator=(MyVector other);      // ★ 按值传递，不是 const&
    void      swap(MyVector& other)noexcept;        
    // ---------- 容量 ----------
    size_t size() const;                 // 现在真正装了几个元素
    size_t capacity() const;             // 现在一共申请了几个格子
    bool   empty() const;                // size() == 0 吗

    // ---------- 元素访问 ----------
    int&       operator[](size_t i);         // v[i]，可读可写
    const int& operator[](size_t i) const;   // const 对象只能用这个，只读
    int*       data() const;                 // 底层数组的首地址

    // ---------- 增删 ----------
    void push_back(int value);           // 在末尾加一个元素
    void pop_back();                     // 删掉末尾那个元素
    void clear();
private:
    int*   data_;        // 指向堆上那块内存；空的时候是 nullptr
    size_t size_;        // 真正装了几个
    size_t capacity_;    // 申请了几个格子

    // ------------------------------------------------------------
    // 扩容用的内部函数
    // ------------------------------------------------------------
    //  什么时候需要它：push_back 发现 size_ == capacity_ 时，没地方放了
    //  要做三件事（顺序不能错）：
    //      ① 申请一块更大的新内存
    //      ② 把旧数据拷过去
    //      ③ 释放旧内存，让 data_ 指向新内存
    //
    //  如果顺序写错（比如先释放再拷贝），就会读已释放的内存 —— 这是经典错误。
    //
    //  新容量取多大？std::vector 的做法是「按倍数涨」（MSVC 是 1.5 倍），
    //  这样均摊下来每次 push_back 是 O(1)。
    //  你 v1 可以先简单点，比如 capacity_ 从 0 开始，每次 *2；
    //  但要处理一个边界：capacity_ 是 0 的时候，乘 2 还是 0，永远涨不上去。
    void grow();
};
