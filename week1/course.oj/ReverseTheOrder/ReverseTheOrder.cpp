#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
using namespace std;

int main() {
	int t;
	cin >> t;
	cin.ignore();
	while (t--) {
		string s;
		
		getline(cin, s);
		reverse(s.begin(), s.end());
		cout << s;
		if (t != 0) {
			cout << endl;
		}
	}
	return 0;
}