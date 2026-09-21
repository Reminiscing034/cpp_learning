// ============================================================
//  SwapTrick.cpp —— 「造临时对象 + swap」到底能用在哪些类型上
// ============================================================
//  以及它能推广出的更重要的东西：copy-and-swap 惯用法。
// ============================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <utility>

using namespace std;

void line(const string& t) {
    cout << endl << "========== " << t << " ==========" << endl;
}

// ------------------------------------------------------------
// 一个自己写的类，用来演示 copy-and-swap 惯用法
// 它持有一块用 new[] 申请的数组，必须自己管理释放
// ------------------------------------------------------------
class Buffer {
public:
    Buffer(size_t n, int fill) : data_(new int[n]), n_(n) {
        for (size_t i = 0; i < n_; i++) data_[i] = fill;
    }
    ~Buffer() {
        delete[] data_;                 // 资源在这里唯一地被释放
    }
    // 拷贝构造：申请自己的内存，把对方的数据抄过来
    Buffer(const Buffer& o) : data_(new int[o.n_]), n_(o.n_) {
        for (size_t i = 0; i < n_; i++) data_[i] = o.data_[i];
    }
    // 交换：只换两个指针和长度，O(1)
    void swap(Buffer& o) noexcept {
        std::swap(data_, o.data_);
        std::swap(n_, o.n_);
    }
    // ★ 赋值运算符：copy-and-swap
    //   参数故意写成「按值传递」—— 一进函数就已经有一份完整拷贝了
    Buffer& operator=(Buffer o) {
        swap(o);            // 和这份拷贝交换
        return *this;       // o 在函数结束时析构，把「旧的内存」一起带走
    }
    size_t size() const { return n_; }
    int    at(size_t i) const { return data_[i]; }
    int*   raw() const { return data_; }

private:
    int*   data_;
    size_t n_;
};

int main() {
    // ------------------------------------------------------------
    line("1. 原理复盘：为什么 swap 能释放内存");
    // ------------------------------------------------------------
    {
        vector<int> v(1000000, 7);
        cout << "释放前:  size=" << v.size() << "  capacity=" << v.capacity() << endl;
        vector<int>().swap(v);
        cout << "释放后:  size=" << v.size() << "  capacity=" << v.capacity() << endl;
    }
    cout << endl;
    cout << "拆成三步看：" << endl;
    cout << "  ① vector<int>()   造一个空的临时对象" << endl;
    cout << "  ② .swap(v)        把 v 的『库存』换给临时对象" << endl;
    cout << "  ③ 语句结束，临时对象析构 —— 旧内存被它带走了" << endl;
    cout << endl;
    cout << "核心：C++ 里『释放资源』只会可靠地发生在【析构函数】里。" << endl;
    cout << "      临时对象的析构时机由编译器保证，所以我们借它的手。" << endl;

    // ------------------------------------------------------------
    line("2. 换个容器还灵吗？实测");
    // ------------------------------------------------------------
    {
        vector<int> c(500000, 1);
        size_t b = c.capacity();
        vector<int>().swap(c);
        cout << "vector<int>    capacity: " << b << " -> " << c.capacity() << endl;
    }
    {
        string s(200000, 'x');
        size_t b = s.capacity();
        string().swap(s);
        cout << "string         capacity: " << b << " -> " << s.capacity() << endl;
    }
    {
        deque<int> d(500000, 1);
        size_t b = d.size();
        deque<int>().swap(d);
        cout << "deque<int>     size    : " << b << " -> " << d.size()
             << "   (deque 没有 capacity()，看 size)" << endl;
    }
    {
        map<int, int> m;
        for (int i = 0; i < 100000; i++) m[i] = i;
        size_t b = m.size();
        map<int, int>().swap(m);
        cout << "map<int,int>   size    : " << b << " -> " << m.size()
             << "   (10 万个节点被释放)" << endl;
    }
    {
        set<int> s;
        for (int i = 0; i < 100000; i++) s.insert(i);
        size_t b = s.size();
        set<int>().swap(s);
        cout << "set<int>       size    : " << b << " -> " << s.size() << endl;
    }
    {
        stack<int> st;
        for (int i = 0; i < 100000; i++) st.push(i);
        size_t b = st.size();
        stack<int>().swap(st);
        cout << "stack<int>     size    : " << b << " -> " << st.size()
             << "   (容器适配器一样管用)" << endl;
    }
    {
        queue<int> q;
        for (int i = 0; i < 100000; i++) q.push(i);
        size_t b = q.size();
        queue<int>().swap(q);
        cout << "queue<int>     size    : " << b << " -> " << q.size() << endl;
    }
    cout << endl;
    cout << "结论：只要满足两个条件，这招就通用 ——" << endl;
    cout << "  ① 可以默认构造（T() 合法）" << endl;
    cout << "  ② swap 是『换内部指针』级别的快操作（STL 容器都是 O(1)）" << endl;

    // ------------------------------------------------------------
    line("3. 同一个目的，四种写法对比");
    // ------------------------------------------------------------
    const size_t N = 300000;
    {
        vector<int> v(N, 1);
        v.clear();
        v.shrink_to_fit();
        cout << "A. clear() + shrink_to_fit()   capacity = " << v.capacity()
             << "   <- 标准只说『请求』，不保证生效" << endl;
    }
    {
        vector<int> v(N, 1);
        vector<int>().swap(v);
        cout << "B. vector<int>().swap(v)       capacity = " << v.capacity()
             << "   <- 经典写法，最稳" << endl;
    }
    {
        vector<int> v(N, 1);
        v = vector<int>();
        cout << "C. v = vector<int>()           capacity = " << v.capacity()
             << "   <- 移动赋值，C++11 后更简洁" << endl;
    }
    {
        vector<int> v(N, 1);
        v = {};
        cout << "D. v = {}                      capacity = " << v.capacity()
             << "   <- 注意：走的是 initializer_list 赋值" << endl;
    }
    cout << endl;
    cout << "D 和 B/C 的区别很值得注意：" << endl;
    cout << "  B 和 C 是『把旧内存整个换掉』，保证释放；" << endl;
    cout << "  D 是『用空列表重新赋值』，标准不保证它一定把容量还回去。" << endl;
    cout << "  所以想要『确定释放』，用 B 或 C，别指望 D。" << endl;

    // ------------------------------------------------------------
    line("4. 更重要的推广：copy-and-swap 惯用法");
    // ------------------------------------------------------------
    cout << "上面那招的本质是『借析构函数的手释放资源』。" << endl;
    cout << "把它用到【赋值运算符】上，就得到 C++ 里最有名的惯用法之一。" << endl << endl;

    Buffer a(5, 1);
    Buffer b(3, 9);
    cout << "赋值前:  a.size=" << a.size() << "   b.size=" << b.size() << endl;
    cout << "         a 的数据首地址=" << a.raw() << endl;

    a = b;      // 走 copy-and-swap

    cout << "a = b 后: a.size=" << a.size() << endl;
    cout << "         a 的数据首地址=" << a.raw() << "   <- 换成了 b 那份拷贝的内存" << endl;
    cout << "旧的 5 个 int 的内存，在 operator= 返回时被安全释放了。" << endl;

    cout << endl;
    cout << "为什么这样写更好？看『中途抛异常』的情况：" << endl;
    cout << "  如果自己写 new/delete：" << endl;
    cout << "      delete[] data_;              // 先释放" << endl;
    cout << "      data_ = new int[o.n_];       // 万一这里抛 bad_alloc -> 对象已损坏" << endl;
    cout << "  用 copy-and-swap：" << endl;
    cout << "      拷贝在【进入函数前】就完成了（按值传参）" << endl;
    cout << "      拷贝失败就抛异常，*this 根本没被碰过 -> 原对象完好" << endl;
    cout << "      拷贝成功后才 swap，并交给析构函数释放旧资源" << endl;
    cout << endl;
    cout << "一句话：把『释放旧资源』交给析构函数，就自动获得了【异常安全】。" << endl;

    // 顺便：这个惯用法也让『自己给自己赋值』天然正确
    a = a;
    cout << endl << "a = a（自赋值）之后 a.size=" << a.size() << "，也没问题。" << endl;

    // ------------------------------------------------------------
    line("5. 哪些地方【不】适用");
    // ------------------------------------------------------------
    cout << "① std::array<T, N> —— swap 是逐个元素交换，O(N)，而且它本来就不占堆内存，" << endl;
    cout << "   没有『还内存』这回事。" << endl;
    cout << "② unique_ptr / shared_ptr —— 用 .reset() 表达意图更清楚：" << endl;
    cout << "      p.reset();        比   unique_ptr<T>().swap(p);   好读得多" << endl;
    cout << "③ 还有别人引用着这块内存时 —— swap 帮不了你。" << endl;
    cout << "   比如 shared_ptr 被别处持有，你 swap 掉的只是自己这一份。" << endl;
    cout << "④ std::string 的短字符串（SSO）：十几个字符以内根本不申请堆内存，" << endl;
    cout << "   数据就存在对象自己身上，这时『释放』没什么意义。" << endl;

    cout << endl << "========== 结束 ==========" << endl;
    return 0;
}
