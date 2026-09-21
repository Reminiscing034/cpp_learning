#include <iostream>
#include <string>
#include <stack>
using namespace std;

int main() {
    int t;
    cin >> t;
    cin.ignore(); 

    for (int i = 0; i < t; i++) {
        string s;
        getline(cin, s);

        stack<char> st;
        bool isBalanced = true;

        for (char c : s) {
            if (c == '(') {
                st.push(c);
            }
            else if (c == ')') {
                if (st.empty()) {
                    isBalanced = false;
                    break;
                }
                else {
                    st.pop();
                }
            }
        }
        if (!st.empty()) {
            isBalanced = false;
        }
        if (isBalanced) {
            cout << "TRUE" << endl;
        }
        else {
            cout << "FALSE" << endl;
        }
    }

    return 0;
}