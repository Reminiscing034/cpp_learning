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
    cout << endl << "-- 8. clear --" << endl;
    {
        MyVector v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        size_t capBefore = v.capacity();

        v.clear();
        check(v.size() == 0, "clear() 后 size() == 0");
        check(v.empty(),     "clear() 后 empty() == true");
        check(v.capacity() == capBefore,
              "clear() 不改变 capacity()（std::vector 也是这个约定）");

        // 清空之后还能继续用
        v.push_back(9);
        check(v.size() == 1, "clear() 之后还能 push_back");
        if (sizeIs(v, 1)) {
            check(v[0] == 9, "新加进去的元素是 9");
        }

        // 本来就空的时候调 clear 也不该出问题
        MyVector e;
        e.clear();
        check(e.size() == 0, "对空的 MyVector 调 clear() 不出问题");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 9. 拷贝构造 --" << endl;
    {
        MyVector a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);

        MyVector b = a;                     // 用已有对象 a 造出新对象 b

        check(b.size() == 3, "拷贝出来的 size 和原对象一样");
        if (sizeIs(b, 3)) {
            check(b[0] == 1 && b[1] == 2 && b[2] == 3, "拷贝出来的元素值也一样");
        }
        check(b.data() != a.data(), "两个对象各自拥有独立的内存，没有共用同一块");

        // 关键：改一个不能影响另一个
        b[0] = 99;
        check(a[0] == 1, "改副本，原对象不受影响");
        a[1] = 88;
        check(b[1] == 2, "改原对象，副本不受影响");

        // 拷贝一个空的
        MyVector empty1;
        MyVector empty2 = empty1;
        check(empty2.size() == 0, "拷贝空的 MyVector，得到的还是空的");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 10. 拷贝赋值 --" << endl;
    {
        MyVector a;
        a.push_back(1);
        a.push_back(2);

        MyVector b;
        b.push_back(9);                     // b 本来有自己的数据
        b.push_back(8);
        b.push_back(7);

        b = a;                              // 把 a 赋给 b

        check(b.size() == 2, "赋值后 size 变成对方的");
        if (sizeIs(b, 2)) {
            check(b[0] == 1 && b[1] == 2, "赋值后元素值正确");
        }
        check(b.data() != a.data(), "赋值后两边也没有共用内存");

        b[0] = 99;
        check(a[0] == 1, "改被赋值的一方，原对象不受影响");

        // 赋值给一个空对象
        MyVector c;
        c = a;
        check(c.size() == 2, "空的 MyVector 也能被赋值");
    }

    // ------------------------------------------------------------
    cout << endl << "-- 11. 自我赋值（a = a）--" << endl;
    {
        MyVector a;
        a.push_back(5);
        a.push_back(6);

        a = a;                              // 自己赋给自己

        check(a.size() == 2, "自我赋值后 size 不变");
        if (sizeIs(a, 2)) {
            check(a[0] == 5 && a[1] == 6, "自我赋值后数据完好，没被抹掉");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "-- 12. 连续赋值（z = y = x）--" << endl;
    {
        MyVector x, y, z;
        x.push_back(7);

        z = y = x;

        check(y.size() == 1, "中间那个也被赋值了");
        check(z.size() == 1, "最左边那个也被赋值了");
        if (sizeIs(z, 1)) {
            check(z[0] == 7, "值一直传到了最左边");
        }
    }

    // ------------------------------------------------------------
    cout << endl << "-- 13. 多个互相拷贝的对象同时存在、一起退场 --" << endl;
    {
        // 这一段故意不做 check —— 它要测的就是「能不能活着走到这里」。
        // 先让 5 个互相拷贝出来的对象同时存在，再让它们一起退场。
        MyVector a;
        for (int i = 0; i < 5; i++) a.push_back(i);

        MyVector b = a;
        MyVector c;
        c = a;
        MyVector d = b;
        MyVector e;
        e = c = a;
    }
    check(true, "5 个互相拷贝的对象同时存在、一起退场，程序没有崩");

    // ------------------------------------------------------------
    cout << endl << "=======================================" << endl;
    cout << "通过 " << passed << " 项，失败 " << failed << " 项" << endl;
    if (failed == 0) {
        cout << "全部通过 —— MyVector v1 + v2 完成！" << endl;
    } else {
        cout << "还有 " << failed << " 项没过，回 MyVector.cpp 继续改。" << endl;
    }
    return 0;
}
