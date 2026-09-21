// ============================================================
//  BracketMatching.cpp —— 括号匹配：从「只认圆括号」到「通用解法」
// ============================================================
//  这个程序里有两个函数，用来对比：
//    isBalancedSimple —— 只认 () —— 也就是你原来那题的写法
//    isBalanced       —— 通用版，支持 () [] {} 三种括号
//
//  跑一遍，你会看到「简单版」在哪些输入上会判错。
//
//  注意：本文件含中文注释，必须存成 UTF-8 带 BOM。
// ============================================================

#include <iostream>
#include <stack>
#include <string>

using namespace std;

// ------------------------------------------------------------
// 版本 1：简单版 —— 只认圆括号（就是你 OJ 那题的解法）
// ------------------------------------------------------------
// 思路：遇到 ( 就记住，遇到 ) 就抵消一个。最后看还有没有没抵消的。
bool isBalancedSimple(const string& s) {
    stack<char> st;

    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if (c == '(') {
            st.push(c);
        }
        else if (c == ')') {
            if (st.empty()) {
                return false;      // 冒出多余的 )
            }
            st.pop();
        }
        // 其他字符（包括 [ ] { }）一律无视
    }

    return st.empty();             // 还有没抵消的 ( 就不平衡
}

// ------------------------------------------------------------
// 版本 2：通用版 —— 支持 () [] {} 三种括号
// ------------------------------------------------------------

// 判断是不是「开括号」
bool isOpen(char c) {
    return c == '(' || c == '[' || c == '{'||c=='<';
}

// 判断是不是「闭括号」
bool isClose(char c) {
    return c == ')' || c == ']' || c == '}'||c=='>';
}

// 给一个闭括号，返回它应该配对的「开括号」；不是闭括号就返回 '\0'（空字符）
// 【核心思想】把「配对关系」集中放在一个地方，代码主体就不用写一堆 if-else
char partnerOf(char close) {
    if (close == ')') return '(';
    if (close == ']') return '[';
    if (close == '}') return '{';
    if (close == '>') return'<';
    return '\0';
}

bool isBalanced(const string& s) {
    stack<char> st;

    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];

        if (isOpen(c)) {
            // 开括号：入栈，等着被闭合
            st.push(c);
        }
        else if (isClose(c)) {
            // 闭括号：必须和「栈顶」配对

            // 情况 ① 栈空 —— 说明这个闭括号前面没有对应的开括号
            if (st.empty()) {
                return false;
            }

            // 情况 ② 类型不符 —— 这就是「交叉嵌套」，比如 ([)]
            // 此时栈顶是 '['，但我们遇到的是 ')'，配对失败
            if (st.top() != partnerOf(c)) {
                return false;
            }

            // 情况 ③ 配对成功 —— 把这对方括号一起消掉
            st.pop();
        }
        // 其他字符（字母、空格等）直接忽略
    }

    // 情况 ④ 扫完了，栈必须掏空。
    // 还剩下东西，说明有开括号一直没被闭合，比如 "((("
    return st.empty();
}

// ------------------------------------------------------------
// 测试
// ------------------------------------------------------------
struct TestCase {
    string input;
    bool   expected;
};

int main() {
    // 测试用例，包含最容易判错的几种情况
    TestCase cases[] = {
        { "",            true  },   // 空串：算平衡
        { "()",          true  },
        { "()[]{}",      true  },   // 三种括号各来一对
        { "{[]}",        true  },   // 正确的嵌套
        { "([{}])",      true  },   // 三层嵌套
        { "a(b[c]{d}e)", true  },   // 夹杂字母
        { "(]",          false },   // 类型不符
        { "([)]",        false },   // 交叉嵌套 ← 简单版会判错！
        { "[(])",        false },   // 交叉嵌套的另一种 ← 简单版也会判错
        { "(((",         false },   // 开括号没闭合
        { ")))",         false },   // 闭括号没开过
        { "(",           false },
        { ")",           false },
    };
    int total = sizeof(cases) / sizeof(cases[0]);
    int wrongSimple = 0;
    int wrongGeneral = 0;

    cout << "括号匹配测试：简单版(只认圆括号) vs 通用版(三种括号)" << endl;
    cout << string(66, '=') << endl;

    for (int i = 0; i < total; i++) {
        bool gotSimple = isBalancedSimple(cases[i].input);
        bool gotGeneral = isBalanced(cases[i].input);

        bool okSimple = (gotSimple == cases[i].expected);
        bool okGeneral = (gotGeneral == cases[i].expected);
        if (!okSimple)  wrongSimple++;
        if (!okGeneral) wrongGeneral++;

        cout << "输入 \"" << cases[i].input << "\"" << endl;
        cout << "   期望   : " << (cases[i].expected ? "平衡" : "不平衡") << endl;
        cout << "   简单版 : " << (gotSimple  ? "平衡" : "不平衡")
             << (okSimple ? "   OK" : "   <<< 判错了") << endl;
        cout << "   通用版 : " << (gotGeneral ? "平衡" : "不平衡")
             << (okGeneral ? "   OK" : "   <<< 判错了") << endl;
        cout << endl;
    }

    cout << string(66, '=') << endl;
    cout << "共 " << total << " 个测试用例" << endl;
    cout << "简单版 判错 " << wrongSimple << " 个" << endl;
    cout << "通用版 判错 " << wrongGeneral << " 个" << endl;

    if (wrongSimple > 0) {
        cout << endl;
        cout << "结论：简单版亏在「看不见 [ 和 ]」。" << endl;
        cout << "      它只能数圆括号是否够数，数不出「配对顺序」。" << endl;
        cout << "      通用版把三种括号都入栈，遇到闭括号时检查栈顶，就能发现交叉嵌套。" << endl;
    }

    return 0;
}
