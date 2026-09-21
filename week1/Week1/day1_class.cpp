//#include<iostream>
//using namespace std;
//
//int main() {
//	cout << "hello world"<<endl;
//	return 0;
//}
//
//#include<iostream>
//using namespace std;
//
//const double Pi = 3.1416;
//
//class Shape {
//public:
//	virtual double area() = 0;
//	virtual ~Shape() {}
//
//};
//class Circle:public Shape {
//private:
//	int r = 0;
//public:
//	Circle(int R) :r(R) {}
//	int getR() {
//		return r;
//	}
//	void setR(int R) {
//		r = r;
//	}
//	double area ()override {
//		return r * r * Pi;
//	}
//};
//class Retangle :public Shape {
//private:
//	int a = 0;
//	int b = 0;
//public:
//	Retangle(int A, int B)  {
//		a = A, b = B;
//	}
//	double area()override {
//		return a * b;
//	}
//	void setAB(int A,int B) {
//		a = A;
//		b = B;
//	}
//	int getA() { return a;}
//	int getB() { return b;}
//	
//};
//int main() {
//	int r, a, b;
//	cin >> r >> a >> b;
//	Circle c(r);
//	Retangle re(a, b);
//	cout << c.area() <<endl<< re.area() << endl;
//	return 0;
//}
#include<iostream>
using namespace std;
#include<vector>
#include<algorithm>
#include<string>
void teat01() {
	vector<int>v;
	v.push_back(10);
	v.push_back(20);
	v.push_back(30);
	v.push_back(40);
	//vector<int>::iterator itBegin = v.begin();
	//vector<int>::iterator itEnd = v.end();
	//for (vector<int>::iterator it = v.begin(); it != v.end(); it++) {}
	 
}
class Person {
public:
	Person(string name, int age) {
		m_Name = name;
		m_Age = age;
	}
	string m_Name;
	int m_Age;
};
void test02() {
	vector<Person>v;
	Person p1("aa", 10);
	Person p2("bb",20);
	Person p3("cc",30);
	Person p4("dd",40);
	Person p5("ff",50);
	v.push_back(p1);
	v.push_back(p2);
	v.push_back(p3);
	v.push_back(p4);
	v.push_back(p5);
	for (vector<Person>::iterator it = v.begin(); it != v.end(); it++) {
		cout << "name:" << (*it).m_Name << endl;
	}

}
void test03() {
	vector<Person*>v;
	Person p1("aa", 10);
	Person p2("bb", 20);
	Person p3("cc", 30);
	Person p4("dd", 40);
	Person p5("ff", 50);
	v.push_back(&p1);
	v.push_back(&p2);
	v.push_back(&p3);
	v.push_back(&p4);
	v.push_back(&p5);
	for (vector<Person*>::iterator it = v.begin(); it != v.end(); it++) {
		cout << "age:" << (*it)->m_Age << endl;
	}

}
void test04() {
	vector<vector<int>>v;
	vector<int>v1;
	vector<int>v2;
	vector<int>v3;
	vector<int>v4;
	for (int i = 0; i < 4; i++) {
		v1.push_back(i + 1);
		v2.push_back(i + 2);
		v3.push_back(i + 3);
		v4.push_back(i + 4);

	}
	v.push_back(v1);
	v.push_back(v2);
	v.push_back(v3);
	v.push_back(v4);
	for (vector<vector<int>>::iterator it = v.begin(); it != v.end(); it++) {
		for (vector<int>::iterator vit = (*it).begin(); vit != (*it).end(); vit++) {
			cout << *vit << " ";
		}cout << endl;
	}
}
//day2
void printVector(vector<int>&v) {
	for (vector<int>::iterator it = v.begin(); it != v.end(); it++) {
		cout << *it << " ";
	}cout << endl;
}
void test05() {
	vector<int>v1;//默认构造
	for (int i = 0; i < 10; i++) {
		v1.push_back(i);
	}printVector(v1);

	vector<int>v2(v1.begin(), v1.end());
	printVector(v2);

	vector<int>v3(10, 100);
	printVector(v3);

	vector<int>v4(v3);
	printVector(v4);
}

void test06() {
	vector<int>v1;
	for (int i = 0; i < 10; i++) {
		v1.push_back(i);
	}
	printVector(v1);

	//赋值
	vector<int>v2;
	v2 = v1;
	printVector(v2);

	vector<int>v3;
	v3.assign(v1.begin(), v2.end());
	printVector(v3);

	vector<int>v4;
	v4.assign(10, 100);
	printVector(v4);
}
void test07() {//容量大小
	vector<int>v1;
	for (int i = 0; i < 10; i++) {
		v1.push_back(i);
	}
	printVector(v1);

	if (v1.empty()) {
		cout << "v1为空" << endl;
	}
	else {
		cout << "v1容量为：" << v1.capacity() << endl;
		cout << "v1大小为：" << v1.size() << endl;
	}
	v1.resize(15);//过长默认补0
	cout << v1.capacity() << endl;
	v1.resize(20, 100);//指定默认填充值
	v1.resize(10);//过短删除
	printVector(v1);
	
}

void test08() {//插入删除
	vector<int>v1;
	v1.push_back(10);
	v1.push_back(20);
	v1.push_back(30);
	cout << v1.capacity() << endl;
	printVector(v1);
	v1.pop_back();
	printVector(v1);
    v1.insert(v1.begin(), 2, 100);
	cout << v1.capacity() << endl;
	printVector(v1);
	v1.erase(v1.begin(), v1.begin() + 2);
    printVector(v1);
	v1.clear();
	printVector(v1);
	cout << v1.capacity() << endl;
	cout<<v1.size()<<endl;
}
void test09() {//数据存取
	vector<int>v1;
	for (int i = 0; i < 10; i++) {
		v1.push_back(i);
	}
	for (int i = 0; i < 10; i++) {
		cout << v1[i] << " ";
	}cout << endl;
	for (int i = 0; i < 10; i++) {
		cout << v1.at(i)<< " ";
	}cout << endl;
	cout << "第一个元素：" << v1.front() << endl;
	cout << "最后一个元素" << v1.back() << endl;

}
void test10() {//vector容器互换
	vector<int>v1;
	for (int i = 0; i < 10; i++) {
		v1.push_back(i);
	}printVector(v1);
	vector<int>v2;
	for (int i = 10; i > 0; i--) {
		v2.push_back(i);
	}printVector(v2);
	v1.swap(v2);
	printVector(v1);
	printVector(v2);
	vector<int>v3;
	for (int i = 0; i < 100000; i++) {
		v3.push_back(i);
	}
	cout << "大小" << v3.size() << endl;
	cout << "容量" << v3.capacity() << endl;
	v3.resize(3);
	cout << "大小" << v3.size() << endl;
	cout << "容量" << v3.capacity() << endl;
	//swap收缩内存 
	vector<int>(v3).swap(v3);
	cout << "大小" << v3.size() << endl;
	cout << "容量" << v3.capacity() << endl;
}

void test11() {//预留空间
	vector<int>v;
	v.reserve(100000);
	int num = 0;
	int* p = NULL;
	for (int i = 0; i < 100000; i++) {
		v.push_back(i);
		if (p != &v[0]) {//扩展次数
			p = &v[0];
			num++;
		}
	}
	cout << "num=" << num << endl;
}

int main() {
	test11();
}