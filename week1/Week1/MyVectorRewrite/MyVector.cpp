#include"MyVector.h"
#include<cassert>
MyVector::MyVector() {
	size_ = 0;
	data_ = nullptr;
	capacity_ = 0;
}
MyVector::MyVector(const MyVector& other) {
	this->capacity_ = other.capacity_;
	this->size_ = other.size_;
	this->data_ = new int[this->capacity_];
	for (std::size_t i = 0; i < this->size_; i++) {
		this->data_[i] = other.data_[i];
	}
}
MyVector::~MyVector() {
	delete[]data_;
}

std::size_t MyVector::size()const {
	return size_;
}
std::size_t MyVector::capacity()const {
	return capacity_;
}
bool MyVector::empty()const {
	return size_ == 0;
}

void MyVector::push_back(int value) {
	if (size_ == capacity_) {
		capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
		int* newdata = new int[capacity_];
		for (std::size_t i = 0; i < size_; i++) {
			newdata[i] = data_[i];
		}
		delete[]data_;
		data_ = newdata;
	}
	data_[size_] = value;
	size_++;
}
void MyVector::pop_back() {
	assert(!empty());
	size_--; 
}
int& MyVector::operator[](std::size_t i) {
	return data_[i];
}

MyVector& MyVector::operator=(const MyVector& other) {
	if (this == &other)return *this;
	this->capacity_ = other.capacity_;
	this->size_ = other.size_;
	delete[]this->data_;
	this->data_ = new int[this->capacity_];
	for (std::size_t i = 0; i < this->size_; i++) {
		this->data_[i] = other.data_[i];
	}
	return *this;
}