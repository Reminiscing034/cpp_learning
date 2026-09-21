// ============================================================
//  StackBasics.cpp —— 演示 std::stack 的全部基本操作
//  目的：亲眼看到「后进先出」(LIFO) 的行为，以及新手最容易踩的坑
// ============================================================
//
//  注意：MSVC 默认按系统编码(GBK)读源码。本文件含中文注释，
//  必须存成 UTF-8 带 BOM，否则中文会乱码并可能报 C4819 警告。
// ============================================================

#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

// 打印栈的当前状态
// 【关键技巧】std::stack 故意不提供遍历功能（它就是不想让你随便翻），
// 所以想看内容，只能拷贝一份出来，边 pop 边记录。
// 参数写成 stack<int> s（按值传递）而不是 stack<int>& s，
// 就是为了操作副本，不影响外面的真栈。
void show(stack<int> s, const string& label) {
    cout << label;
    if (s.empty()) {
        cout << "(空栈)";
    }
    vector<int> tmp;
    while (!s.empty()) {
        tmp.push_back(s.top());
        s.pop();
    }
    for (size_t i = 0; i < tmp.size(); i++) {
        cout << tmp[i] << " ";
    }
    cout << "  [size=" << tmp.size() << "]" << endl;
}

int main() {
    cout << "========== 1. push —— 往栈顶放东西 ==========" << endl;
    stack<int> st;
    int put[] = { 1, 2, 3 };
    for (int i = 0; i < 3; i++) {
        st.push(put[i]);
        cout << "push(" << put[i] << ")  ->  size=" << st.size()
             << ", top=" << st.top() << endl;
    }
    show(st, "现在栈里(顶->底): ");

    cout << endl;
    cout << "========== 2. top —— 只看不拿 ==========" << endl;
    cout << "st.top() = " << st.top()
         << "    size 仍然是 " << st.size() << "，说明没被拿走" << endl;

    cout << endl;
    cout << "========== 3. pop —— 拿走栈顶，但不返回它 ==========" << endl;
    // 【新手最大的坑】pop() 的返回值是 void，它只负责扔掉栈顶，
    // 不告诉你扔掉的是什么。想知道就先 top() 再 pop()。
    while (!st.empty()) {
        int topValue = st.top();   // 先记下来
        st.pop();                  // 再扔掉
        cout << "拿走 " << topValue << "  ->  size=" << st.size() << endl;
    }
    cout << endl;
    cout << "反面教材（取消注释会编译报错或崩溃，你可以自己试试）：" << endl;
    cout << "   int x = st.pop();   // 编译错误：pop() 返回 void" << endl;
    cout << "   st.top();           // 空栈时调用是未定义行为，可能直接崩溃" << endl;
    cout << "   st.pop();           // 空栈时调用同理" << endl;

    cout << endl;
    cout << "========== 4. empty / size —— 安全检查 ==========" << endl;
    cout << "st.empty() = " << (st.empty() ? "true" : "false") << endl;
    cout << "st.size()  = " << st.size() << endl;
    cout << "所以取值前永远先检查:  if (!st.empty()) { x = st.top(); st.pop(); }" << endl;

    cout << endl;
    cout << "========== 5. LIFO 有什么用 —— 反转顺序 ==========" << endl;
    stack<int> rev;
    for (int i = 1; i <= 5; i++) {
        rev.push(i);
    }
    cout << "放入顺序: 1 2 3 4 5" << endl;
    cout << "取出顺序: ";
    while (!rev.empty()) {
        cout << rev.top() << " ";
        rev.pop();
    }
    cout << "  <- 正好反过来了，这就是「后进先出」" << endl;

    cout << endl;
    cout << "========== 6. 换个类型也能用 —— 栈是模板 ==========" << endl;
    stack<string> words;
    words.push("first");
    words.push("second");
    words.push("third");
    cout << "放入: first, second, third" << endl;
    cout << "弹出: ";
    while (!words.empty()) {
        cout << words.top() << " ";
        words.pop();
    }
    cout << endl;
    cout << "（栈不关心你存什么类型，int / string / 自己写的结构体都行）" << endl;

    return 0;
}
