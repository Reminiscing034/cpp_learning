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

	std::cout << "all tests passed\n";
}