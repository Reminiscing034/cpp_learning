// ============================================================
//  VectorBasics.cpp —— vector 的「容量」世界
// ============================================================
//  讲清楚四件事：
//    1. size 和 capacity 的区别
//    2. 扩容到底发生了什么（以及为什么危险）
//    3. reserve —— 提前把地方留够
//    4. shrink_to_fit / swap —— 把多占的内存还回去
//
//  编译选项用的是 /source-charset:utf-8，
//  所以中文注释不需要 BOM 也能正确编译，输出也能在 936 控制台正常显示。
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

void line(const string& t) {
    cout << endl << "========== " << t << " ==========" << endl;
}

// 打印一个 vector 的内容
void dump(const vector<int>& v) {
    cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << ", ";
        cout << v[i];
    }
    cout << "]";
}

int main() {
    // ------------------------------------------------------------
    line("1. size 和 capacity 是两回事");
    // ------------------------------------------------------------
    vector<int> v;
    cout << "刚创建时:  size=" << v.size() << "  capacity=" << v.capacity() << endl;
    cout << "（size = 真正装了几个；capacity = 已经申请了几个格子）" << endl << endl;

    size_t lastCap = v.capacity();
    for (int i = 1; i <= 20; i++) {
        v.push_back(i);
        cout << "push_back 第 " << i << " 个  ->  size=" << v.size()
             << "  capacity=" << v.capacity();
        if (v.capacity() != lastCap) {
            cout << "     <<<< 扩容了！";
            lastCap = v.capacity();
        }
        cout << endl;
    }
    cout << endl;
    cout << "看出来了吗：capacity 不是每次 +1，而是「跳着涨」。" << endl;
    cout << "因为每次扩容都要「另找一块更大的地 + 把旧数据搬过去」，" << endl;
    cout << "所以实现会一次多要一些，摊薄搬家的次数。" << endl;
    cout << "（MSVC 大约按 1.5 倍涨，g++ 大约按 2 倍涨 —— 具体倍数由实现决定，标准不管）" << endl;

    // ------------------------------------------------------------
    line("2. 扩容的代价：整块搬家，旧指针全部作废");
    // ------------------------------------------------------------
    vector<int> a;
    a.reserve(2);
    a.push_back(1);
    a.push_back(2);
    int* p = &a[0];                       // 记住首元素地址
    cout << "扩容前 a 的首元素地址: " << p << endl;
    cout << "此时 capacity=" << a.capacity() << "，再 push 一个就会超" << endl;

    a.push_back(3);                       // 触发扩容
    cout << "push_back(3) 之后地址: " << &a[0];
    if (p == &a[0]) {
        cout << "   （地址没变）" << endl;
    } else {
        cout << "   <<<< 地址变了！旧的 p 已经指向一块被释放的内存" << endl;
    }
    cout << endl;
    cout << "所以：扩容会让所有【指针 / 引用 / 迭代器】全部失效。" << endl;
    cout << "这是新手最常踩的坑之一 —— 记住这条规律就不会莫名其妙崩。" << endl;

    // ------------------------------------------------------------
    line("3. reserve：提前把地方留够");
    // ------------------------------------------------------------
    const int N = 200000;
    size_t reallocNo = 0, reallocYes = 0;
    {
        vector<int> x;
        size_t lc = x.capacity();
        auto t0 = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; i++) {
            x.push_back(i);
            if (x.capacity() != lc) { reallocNo++; lc = x.capacity(); }
        }
        auto t1 = chrono::high_resolution_clock::now();
        x.clear();

        vector<int> y;
        y.reserve(N);                     // ← 关键就这一行
        size_t lc2 = y.capacity();
        auto t2 = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; i++) {
            y.push_back(i);
            if (y.capacity() != lc2) { reallocYes++; lc2 = y.capacity(); }
        }
        auto t3 = chrono::high_resolution_clock::now();

        double msNo  = chrono::duration<double, milli>(t1 - t0).count();
        double msYes = chrono::duration<double, milli>(t3 - t2).count();

        cout << "元素个数: " << N << endl << endl;
        cout << "不 reserve : 扩容 " << reallocNo  << " 次,  耗时 " << msNo  << " ms" << endl;
        cout << "用 reserve : 扩容 " << reallocYes << " 次,  耗时 " << msYes << " ms" << endl;
    }

    // 上面用的元素是 int —— 「搬家」只是复制几个字节，本来就便宜，
    // 所以时间差别不明显。换成「复制起来很贵」的元素，差别立刻拉开：
    {
        const int M = 100000;
        cout << endl;
        cout << "换成 string（每个 100 字符，复制一次要申请一次堆内存）：" << endl;

        auto t0 = chrono::high_resolution_clock::now();
        vector<string> p;
        for (int i = 0; i < M; i++) {
            p.push_back(string(100, 'x'));
        }
        auto t1 = chrono::high_resolution_clock::now();

        auto t2 = chrono::high_resolution_clock::now();
        vector<string> q;
        q.reserve(M);                     // ← 同样只差这一行
        for (int i = 0; i < M; i++) {
            q.push_back(string(100, 'x'));
        }
        auto t3 = chrono::high_resolution_clock::now();

        cout << "  不 reserve : 耗时 "
             << chrono::duration<double, milli>(t1 - t0).count() << " ms" << endl;
        cout << "  用 reserve : 耗时 "
             << chrono::duration<double, milli>(t3 - t2).count() << " ms" << endl;
        cout << "  「搬家」要复制全部旧元素 —— 元素越贵（复制成本越高），" << endl;
        cout << "  reserve 省下的就越多。装 int 时省得少，装 string / 大结构体时省得多。" << endl;
    }
    cout << endl;
    cout << "用 reserve 的好处有两个：" << endl;
    cout << "  1. 省掉反复「申请内存 + 搬数据」的开销（元素越贵越明显）" << endl;
    cout << "  2. 过程中【不会扩容】，所以指针/迭代器不会突然失效" << endl;
    cout << "什么时候该用：你已经知道大概要装多少个元素的时候。" << endl;

    // ------------------------------------------------------------
    line("4. shrink_to_fit：把多出来的格子还回去");
    // ------------------------------------------------------------
    vector<int> s;
    for (int i = 0; i < 100; i++) s.push_back(i);
    cout << "装了 100 个:        size=" << s.size() << "  capacity=" << s.capacity() << endl;

    s.erase(s.begin() + 10, s.end());     // 删到只剩 10 个
    cout << "删到只剩 10 个:     size=" << s.size() << "  capacity=" << s.capacity()
         << "   <<< capacity 没跟着降！" << endl;

    s.shrink_to_fit();
    cout << "shrink_to_fit 之后: size=" << s.size() << "  capacity=" << s.capacity() << endl;
    cout << endl;
    cout << "注意：erase / clear / pop_back 都只改 size，不动 capacity。" << endl;
    cout << "      因为「留着下次用」通常比「还给系统」更划算。" << endl;
    cout << "      标准没说 shrink_to_fit 一定会生效，但主流实现都会照做。" << endl;

    // ------------------------------------------------------------
    line("5. swap：互换两个容器的内容");
    // ------------------------------------------------------------
    vector<int> x, y;
    x.push_back(1); x.push_back(2); x.push_back(3);
    y.push_back(10); y.push_back(20); y.push_back(30); y.push_back(40); y.push_back(50);

    cout << "交换前  x = "; dump(x); cout << "   size=" << x.size() << " capacity=" << x.capacity() << endl;
    cout << "交换前  y = "; dump(y); cout << "   size=" << y.size() << " capacity=" << y.capacity() << endl;

    x.swap(y);

    cout << "交换后  x = "; dump(x); cout << "   size=" << x.size() << " capacity=" << x.capacity() << endl;
    cout << "交换后  y = "; dump(y); cout << "   size=" << y.size() << " capacity=" << y.capacity() << endl;
    cout << endl;
    cout << "注意：交换的是【整个容器】，连 capacity 一起换过去了。" << endl;
    cout << "      而且速度是 O(1) —— 它只交换内部那几个指针，没搬任何元素。" << endl;

    // ------------------------------------------------------------
    line("6. 经典技巧：真的把内存还给系统");
    // ------------------------------------------------------------
    vector<int> big(1000000, 7);           // 100 万个 7
    cout << "装了 100 万个:            size=" << big.size()
         << "  capacity=" << big.capacity() << endl;

    big.clear();
    cout << "clear() 之后:             size=" << big.size()
         << "  capacity=" << big.capacity() << "   <<< 内存还占着" << endl;

    vector<int>().swap(big);               // 和一个「临时空 vector」交换
    cout << "vector<int>().swap(big) 后: size=" << big.size()
         << "  capacity=" << big.capacity() << "   <<< 还回去了" << endl;
    cout << endl;
    cout << "这行 vector<int>().swap(big) 是释放 vector 内存的标准写法：" << endl;
    cout << "  造一个空的临时 vector -> 和 big 交换 -> 临时对象析构时把旧内存带走" << endl;
    cout << "  （shrink_to_fit 也能做，但 swap 这招更「保证生效」）" << endl;

    cout << endl << "========== 结束 ==========" << endl;
    return 0;
}
