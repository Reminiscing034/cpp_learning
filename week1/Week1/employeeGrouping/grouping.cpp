#include<iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;

class Worker {
public:
	string name;
	int salary;

};
void createWorker(vector<Worker>& v) {
	string nameSeed = "ABCDEFGHIJ";
	for (int i = 0; i < 10; i++) {
		Worker worker;
		worker.name = "员工";
		worker.name += nameSeed[i];
		worker.salary = rand() % 10000 + 10000;
		v.push_back(worker);
	}
}

void setGroup(vector<Worker>&v,multimap<int,Worker>&m) {
	for (auto it = v.begin(); it != v.end(); it++) {
		int ip = rand() % 3;
		m.insert(make_pair(ip, *it));
	}
}

void showWorker(const multimap<int, Worker>&m) {
	cout << "策划：" << endl;
	auto pos = m.find(0);
	int con = m.count(0);
	int i = 0;
	for (; pos != m.end(), i < con; pos++, i++) {
		cout << "员工：" << pos->second.name << " 工资：" << pos->second.salary << endl;
	}

	cout << "-------------------------" << endl;
	cout << "美术：" << endl;
	pos = m.find(1);
	con = m.count(1);
	i = 0;
	for (; pos != m.end(), i < con; pos++, i++) {
		cout << "员工：" << pos->second.name << " 工资：" << pos->second.salary << endl;
	}
	cout << "---------------------------" << endl;
	cout << "研发：" << endl;
	pos = m.find(2);
	con = m.count(2);
	i = 0;
	for (; pos != m.end(), i < con; pos++, i++) {
		cout << "员工：" << pos->second.name << " 工资：" << pos->second.salary << endl;
	}
}

int main() {
	vector<Worker>vWorker;
	createWorker(vWorker);
	multimap<int, Worker>mWorker;
	setGroup(vWorker, mWorker);
	showWorker(mWorker);
}