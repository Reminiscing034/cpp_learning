#include<cassert>
#include<iostream>
#include"MyVector.h"

int main() {
	MyVector v;
	assert(v.size() == 0);
	assert(v.empty());

	for (int i = 0; i < 10; ++i)v.push_back(i);
	assert(v.size() == 10);
	assert(v.capacity() >= 10);
	assert(v[0] == 0);
	assert(v[9] == 9);

	v.pop_back();
	assert(v.size() == 9);
	assert(v[8] == 8);

	v[0] = 100;
	assert(v[0] == 100);

	//v.clear();
	//v.pop_back();
	{
		MyVector a;
		for (int i = 0; i < 5; ++i) a.push_back(i);
		MyVector b = a;     
	}   // 作用域结束，a 和 b 都要析构
	//浅拷贝a和b指向同一块内存重复释放
	{
		MyVector a;
		for (int i = 0; i < 5; i++)a.push_back(i);
		MyVector b, c;
		b = a;
		a = a;
		c = b = a;
	}
	// 验证深拷贝构造：b 改了，a 不能跟着变
	{
		MyVector a;
		for (int i = 0; i < 5; ++i) a.push_back(i);
		MyVector b = a;
		b[0] = 999;
		assert(a[0] == 0);        // ← 这一条挂了，就说明还在共用内存
		assert(b[0] == 999);
	}

	// 验证深拷贝赋值：d 改了，a 不能跟着变
	{
		MyVector a;
		for (int i = 0; i < 5; ++i) a.push_back(i);
		MyVector d;
		d = a;
		d[0] = 777;
		assert(a[0] == 0);        // ← 同上
	}

	// 自赋值之后，内容还得在
	{
		MyVector a;
		for (int i = 0; i < 5; ++i) a.push_back(i);
		a = a;
		assert(a.size() == 5);
		assert(a[0] == 0);
	}


	std::cout << "all tests passed\n";
}