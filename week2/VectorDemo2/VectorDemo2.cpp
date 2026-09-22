// ============================================================
//  VectorDemo2.cpp —— 补齐 vector 剩下的几个重点
// ============================================================
//  你上一个 VectorBasics 已经讲过：size / capacity / reserve / shrink_to_fit / swap
//  这个文件专门补上计划里剩下的：
//    1. push_back  vs  emplace_back
//    2. 迭代器是什么
//    3. 范围 for（其实就是迭代器的语法糖）
//    4. std::sort
//    5. reserve vs resize  ← 最容易搞混的一对，重点
//    6. 迭代器失效（怎么写出崩溃的代码）
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

using namespace std;

void line(const string& t) {
    cout << endl << "========== " << t << " ==========" << endl;
}

void dump(const vector<int>& v, const string& label) {
    cout << label << " [";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << ", ";
        cout << v[i];
    }
    cout << "]  size=" << v.size() << " capacity=" << v.capacity() << endl;
}

// ------------------------------------------------------------
// 一个会「自报家门」的类：构造 / 拷贝 / 移动 各发生几次，全都数出来
// 用它才能看清 push_back 和 emplace_back 的真实区别
// ------------------------------------------------------------
struct Item {
    int id;
    string tag;

    static int ctorCount, copyCount, moveCount;

    Item(int i, const string& t) : id(i), tag(t) {
        ctorCount++;
    }
    Item(const Item& o) : id(o.id), tag(o.tag) {
        copyCount++;
    }
    Item(Item&& o) noexcept : id(o.id), tag(std::move(o.tag)) {
        moveCount++;
    }

    static void reset() { ctorCount = copyCount = moveCount = 0; }
    static void report(const string& what) {
        cout << "      " << what
             << "  ->  构造 " << ctorCount
             << " 次,  拷贝 " << copyCount
             << " 次,  移动 " << moveCount << " 次" << endl;
    }
};
int Item::ctorCount = 0;
int Item::copyCount = 0;
int Item::moveCount = 0;

int main() {
    // ------------------------------------------------------------
    line("1. push_back 和 emplace_back 的区别");
    // ------------------------------------------------------------
    {
        vector<Item> a;
        a.reserve(4);              // 先留够地方，把「扩容搬家」的干扰排除掉
        Item::reset();
        a.push_back(Item(1, "one"));      // 先造一个临时对象，再搬进去
        Item::report("push_back(Item(1,\"one\"))");

        vector<Item> b;
        b.reserve(4);
        Item::reset();
        b.emplace_back(1, "one");         // 直接拿着参数在原地构造，不造临时对象
        Item::report("emplace_back(1, \"one\")     ");
    }
    cout << endl;
    cout << "  结论：两个都能用，结果一样。区别在「中间多不多一次移动」。" << endl;
    cout << "        · push_back  先造一个临时对象，再移动/拷贝进 vector" << endl;
    cout << "        · emplace_back 直接把构造参数传进去，在原地构造 —— 少一次移动" << endl;
    cout << "  什么时候在乎这个？元素很大（比如装 string、大结构体）的时候。" << endl;
    cout << "  装 int 这种小类型，两者完全等价，用哪个都行。" << endl;

    // ------------------------------------------------------------
    line("2. 迭代器是什么");
    // ------------------------------------------------------------
    {
        vector<int> v = { 5, 3, 8, 1 };
        dump(v, "v =");

        // 迭代器 = 一个「指向某个元素的位置标记」
        // 你可以把它想成「带箭头的下标」，但它比下标更通用（list/map 也能用）
        vector<int>::iterator it = v.begin();   // begin() 指向第 1 个元素
        cout << "  *it   = " << *it << "        （* 是「取出这个位置的值」）" << endl;
        ++it;                                    // 往后挪一格
        cout << "  ++it 后 *it = " << *it << endl;
        it += 2;                                 // 一次跳两格
        cout << "  it += 2 后 *it = " << *it << endl;

        // end() 不是「最后一个元素」，而是「最后一个元素的后面一格」
        cout << endl;
        cout << "  v.end() 指向最后一个元素的【下一格】，所以它不能解引用。" << endl;
        cout << "  遍历的标准写法是  it != v.end()  —— 而不是 it < v.end()" << endl;
        cout << endl;

        cout << "  用迭代器遍历: ";
        for (vector<int>::iterator i2 = v.begin(); i2 != v.end(); ++i2) {
            cout << *i2 << " ";
        }
        cout << endl;
        cout << "  （C++11 起可以简写成 auto i2 = v.begin()，让编译器自己推类型）" << endl;
    }

    // ------------------------------------------------------------
    line("3. 范围 for —— 其实就是迭代器的语法糖");
    // ------------------------------------------------------------
    {
        vector<int> v = { 1, 2, 3, 4 };

        cout << "  for (int x : v)          拷贝一份出来，改 x 不影响 v" << endl;
        for (int x : v) { x *= 10; }        // 改的是副本
        dump(v, "  跑完后 v =");

        cout << endl;
        cout << "  for (int& x : v)         用引用，能改到 v 本身" << endl;
        for (int& x : v) { x *= 10; }
        dump(v, "  跑完后 v =");

        cout << endl;
        cout << "  for (const int& x : v)   只读引用 —— 不拷贝、也改不了，日常首选" << endl;
        int sum = 0;
        for (const int& x : v) { sum += x; }
        cout << "  求和 = " << sum << endl;

        cout << endl;
        cout << "  它等价于手写迭代器：" << endl;
        cout << "      for (auto it = v.begin(); it != v.end(); ++it) { int& x = *it; ... }" << endl;
        cout << "  所以：范围 for 能用的地方，说明它支持迭代器。" << endl;
    }

    // ------------------------------------------------------------
    line("4. std::sort —— 用迭代器指定「排哪一段」");
    // ------------------------------------------------------------
    {
        vector<int> v = { 5, 3, 8, 1, 9, 2 };
        dump(v, "  排序前:");
        sort(v.begin(), v.end());              // 从小到大
        dump(v, "  升序后:");

        // 只排中间一段：从下标 1 到下标 4（不含）
        vector<int> w = { 5, 3, 8, 1, 9, 2 };
        sort(w.begin() + 1, w.begin() + 4);    // 迭代器可以 +n
        dump(w, "  只排 [1,4) :");

        // 降序：加一个「比较器」
        vector<int> u = { 5, 3, 8, 1, 9, 2 };
        sort(u.begin(), u.end(), greater<int>());   // 需要 <functional>，这里由 <algorithm> 带进来
        dump(u, "  降序后:");

        cout << endl;
        cout << "  记住这个模式：算法(起点迭代器, 终点迭代器, ...)" << endl;
        cout << "  终点是「不含」的 —— 和数学里的半开区间 [begin, end) 一样。" << endl;
    }

    // ------------------------------------------------------------
    line("5. reserve 和 resize —— 最容易搞混的一对");
    // ------------------------------------------------------------
    {
        cout << "  ── reserve(n)：只借地方，不造元素 ──" << endl;
        vector<int> a;
        a.reserve(5);
        cout << "      reserve(5) 之后:  size=" << a.size()
             << "  capacity=" << a.capacity() << endl;
        cout << "      注意 size 还是 0！这时候 a[0] 是越界访问，会出问题。" << endl;
        cout << "      想用必须 push_back 或 resize。" << endl;
        cout << endl;

        cout << "  ── resize(n)：真的造出 n 个元素 ──" << endl;
        vector<int> b;
        b.resize(5);
        cout << "      resize(5) 之后:   size=" << b.size()
             << "  capacity=" << b.capacity() << endl;
        cout << "      元素被真造出来了，值是 0，现在 b[0] 可以安全读写。" << endl;
        cout << endl;

        cout << "  ── resize 缩小：多出来的元素会被真的删掉 ──" << endl;
        vector<int> c = { 1, 2, 3, 4, 5 };
        dump(c, "      原始:");
        c.resize(2);
        dump(c, "      resize(2) 后:");
        cout << "      （3、4、5 被销毁了，但 capacity 没变）" << endl;
        cout << endl;

        cout << "  ── resize 放大时可以指定填充值 ──" << endl;
        vector<int> d = { 1, 2 };
        d.resize(5, 9);
        dump(d, "      resize(5, 9) 后:");
        cout << endl;

        cout << "  ┌──────────────────────────────────────────────────────┐" << endl;
        cout << "  │  reserve      只改 capacity，size 不变 → 不能直接访问 │" << endl;
        cout << "  │  resize       改 size（顺带保证 capacity 够）→ 能访问 │" << endl;
        cout << "  └──────────────────────────────────────────────────────┘" << endl;
        cout << "  一句话记法：reserve 是「占座」，resize 是「真坐下」。" << endl;
    }

    // ------------------------------------------------------------
    line("6. 迭代器失效 —— 为什么你的程序会莫名其妙崩溃");
    // ------------------------------------------------------------
    {
        cout << "  ── 场景 A：扩容后，旧的迭代器/指针全废 ──" << endl;
        vector<int> v = { 1, 2, 3 };
        vector<int>::iterator it = v.begin();
        cout << "      begin() 指向的地址: " << &(*it) << endl;
        v.push_back(4);                 // 很可能扩容
        cout << "      push_back 后 begin() 地址: " << &(*v.begin()) << endl;
        cout << "      → 地址变了，刚才那个 it 已经指向一块被释放的内存" << endl;
        cout << "        再用 *it 就是未定义行为（可能崩溃，可能读到垃圾值）" << endl;
        cout << endl;

        cout << "  ── 场景 B：边遍历边 push_back（经典 bug）──" << endl;
        cout << "      错误写法：" << endl;
        cout << "          for (auto it = v.begin(); it != v.end(); ++it)" << endl;
        cout << "              if (*it == 3) v.push_back(99);   // ← 危险！" << endl;
        cout << "      为什么危险：push_back 可能扩容，" << endl;
        cout << "                  it 失效了，v.end() 这个值也变了，循环条件就乱了。" << endl;
        cout << endl;
        cout << "      安全写法有三种，按推荐顺序：" << endl;
        cout << "        ① 先用下标遍历（size 每次重新取）" << endl;
        cout << "             size_t n = v.size();" << endl;
        cout << "             for (size_t i = 0; i < n; i++) if (v[i] == 3) v.push_back(99);" << endl;
        cout << "        ② 事先 reserve 够，保证过程中不扩容" << endl;
        cout << "        ③ 收集要加的东西，循环结束后再统一 push_back" << endl;
        cout << endl;

        cout << "  ── 到底哪些操作会让迭代器失效？──" << endl;
        cout << "      push_back / insert / resize(变大)   可能扩容 → 全部失效" << endl;
        cout << "      erase / pop_back                    被删位置及其后的失效" << endl;
        cout << "      clear                               全部失效" << endl;
        cout << "      只读操作（size / [] / 遍历）         不影响" << endl;
        cout << endl;
        cout << "  规律：只要「元素可能搬家」或「元素被删了」，相关迭代器就不能再用了。" << endl;
    }

    cout << endl << "========== 结束 ==========" << endl;
    return 0;
}
