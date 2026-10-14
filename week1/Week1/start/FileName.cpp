#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void printVector(vector<int>v) {
    for (auto i = v.begin(); i != v.end(); i++) {
        cout << *i << endl;
    }
}
int main() {
    
    vector<int>v;
    v.push_back(34);
    v.push_back(35);
    v.push_back(2);
    v.push_back(5);
    v.push_back(55);
    sort(v.begin(), v.end());
    printVector(v);
    cout << endl;
    reverse(v.begin(), v.end());
    printVector(v);


}