// ============================================================
//  main.cpp —— MyVector 的自测程序
// ============================================================
//  你不用改这个文件。它就是你的「验收标准」。
//
//  编译运行，看每一条是 PASS 还是 FAIL：
//      全部 PASS  ->  MyVector v1 完成
//      有 FAIL    ->  照着提示去改 MyVector.cpp
//
//  设计说明：每项取值检查前面都先确认 size 对得上，
//            这样即使你还没实现 push_back，程序也不会崩，
//            而是安静地报 FAIL 让你知道该做哪一步。
// ============================================================

#include "MyVector.h"
#include <iostream>
#include <string>

using namespace std;

int passed = 0;
int failed = 0;

void check(bool ok, const string& what) {
    if (ok) {
        cout << "  [PASS] " << what << endl;
        passed++;
    } else {
        cout << "  [FAIL] " << what << endl;
        failed++;
    }
}

// 取值之前先确认 size 对得上，避免越界崩溃
bool sizeIs(const MyVector& v, size_t want) {
    if (v.size() == want) return true;
    cout << "         （size() 是 " << v.size() << "，不是 " << want
         << "，跳过下面的取值检查）" << endl;
    return false;
}

int main() {
    cout << "============ MyVector 自测 ============" << endl;

    // ------------------------------------------------------------
    cout << endl << "-- 1. 空的 MyVector --" << endl;
    {
        MyVector v;
        check(v.size() == 0,     "size() == 0");
        check(v.capacity() == 0, "capacity() == 0");
        check(v.empty(),         "empty() == true");
        check(v.data() == nullptr, "data() == nullptr");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 2. push_back 和 size --" << endl;
    {
        MyVector v;
        v.push_back(10);
        check(v.size() == 1, "push 一个后 size() == 1");
        if (sizeIs(v, 1)) {
            check(v[0] == 10, "v[0] == 10");
        }

        v.push_back(20);
        v.push_back(30);
        check(v.size() == 3, "再 push 两个后 size() == 3");
        if (sizeIs(v, 3)) {
            check(v[0] == 10, "v[0] 还是 10");
            check(v[2] == 30, "v[2] == 30");
        }
        check(v.capacity() >= v.size(), "capacity() >= size()");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 3. 扩容：数据不能丢 --" << endl;
    {
        MyVector v;
        for (int i = 0; i < 100; i++) {
            v.push_back(i);
        }
        check(v.size() == 100, "push 100 个后 size() == 100");
        check(v.capacity() >= 100, "capacity() 至少涨到 100");

        if (sizeIs(v, 100)) {
            bool allRight = true;
            for (size_t i = 0; i < 100; i++) {
                if (v[i] != (int)i) { allRight = false; break; }
            }
            check(allRight, "100 个元素的值都正确（说明扩容时没丢数据）");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "-- 4. 元素可读可写 --" << endl;
    {
        MyVector v;
        v.push_back(1);
        v.push_back(2);
        if (sizeIs(v, 2)) {
            v[0] = 99;
            check(v[0] == 99, "v[0] = 99 写进去生效了");
            check(v[1] == 2,  "改 v[0] 没影响到 v[1]");
        } else {
            check(false, "v[0] = 99 写进去生效了");
            check(false, "改 v[0] 没影响到 v[1]");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "-- 5. pop_back --" << endl;
    {
        MyVector v;
        v.push_back(7);
        v.push_back(8);
        v.pop_back();
        check(v.size() == 1, "pop 之后 size() == 1");
        if (sizeIs(v, 1)) {
            check(v[0] == 7, "剩下的元素还是 7");
        }

        v.pop_back();
        check(v.size() == 0, "再 pop 一次 size() == 0");
        check(v.empty(),     "empty() == true");

        // 边界：空的再 pop 不应该崩，也不应该变成「负数」
        v.pop_back();
        check(v.size() == 0, "空的时候再 pop，size() 仍然是 0（没有崩）");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 6. 带参数的构造 --" << endl;
    {
        MyVector v(5);
        check(v.size() == 5,     "MyVector(5) 的 size() == 5");
        check(v.capacity() >= 5, "capacity() 至少是 5");
        if (sizeIs(v, 5)) {
            bool allZero = true;
            for (size_t i = 0; i < 5; i++) {
                if (v[i] != 0) { allZero = false; break; }
            }
            check(allZero, "5 个元素的初值都是 0");
        }

        // 造出来的元素还能继续 push
        v.push_back(42);
        check(v.size() == 6, "在 MyVector(5) 基础上还能 push");
        if (sizeIs(v, 6)) {
            check(v[5] == 42, "新加的元素在末尾");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "-- 7. data() 是连续内存 --" << endl;
    {
        MyVector v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        check(v.data() != nullptr, "data() 不是空指针");
        if (sizeIs(v, 3)) {
            check(v.data()[0] == 1, "data()[0] == 1");
            check(v.data()[2] == 3, "data()[2] == 3");
            check(&v.data()[1] == &v[1], "data()[1] 和 v[1] 是同一个位置（内存连续）");
        } else {
            check(false, "data()[0] == 1");
            check(false, "data()[2] == 3");
            check(false, "data()[1] 和 v[1] 是同一个位置（内存连续）");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "=======================================" << endl;
    cout << "通过 " << passed << " 项，失败 " << failed << " 项" << endl;
    if (failed == 0) {
        cout << "全部通过 —— MyVector v1 完成！" << endl;
    } else {
        cout << "还有 " << failed << " 项没过，回 MyVector.cpp 继续改。" << endl;
    }
    return 0;
}
