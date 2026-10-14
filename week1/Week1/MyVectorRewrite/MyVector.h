//1.vector在内存中是一块连续的空间，原地扩大不能保证其后有足够的未使用空间。
//2.2倍，扩容后容量大于大小，防止频繁扩容。
//delete[]，new int[n]必须用delete[]配对
#pragma once
#include<cstddef>

class MyVector {
public:
	MyVector();
	~MyVector();

	std::size_t size()const;
	std::size_t capacity()const;
	bool empty()const;

	void push_back(int value);
	void pop_back();
	int& operator[](std::size_t i);
	void clear() { size_ = 0; }
private:
	int* data_;
	std::size_t size_;
	std::size_t capacity_;
};
