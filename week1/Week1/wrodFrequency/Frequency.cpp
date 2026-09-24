#include<iostream>
#include<string>
#include<map>
using namespace std;



int main() {
	map<string, int>mString;
	string putin = "";
	
	while (cin >> putin) {
		/*auto pos = mString.find(putin);
		if (pos != mString.end()) {
			pos->second += 1;
		}
		else {
			mString.insert(make_pair(putin, 1));
		}*/
		mString[putin]++;
	}
	for (auto it = mString.begin(); it != mString.end(); it++) {
		cout << it->first << "->" << it->second << " ";
		
	}
}