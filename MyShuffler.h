#pragma once

#include <vector>
#include <stack>
#include <algorithm>

using std::vector;
using std::random_shuffle;

struct NotShuffled {};
struct ShufflerIsEmpty {};

template<typename T> 
class MyShuffler {
private:
	bool isShuffled;
	vector<T> vec;
public:
	MyShuffler();
	void Shuffle();
	void Push(T item);
	T Pop();
	T Peek();
	void Clear();
	vector<T> GetVector();
};

template<typename T> 
MyShuffler<T>::MyShuffler() : isShuffled(false), vec() {
}

template<typename T>
void MyShuffler<T>::Shuffle() {
	if (vec.size()/* != 0*/) {
		random_shuffle(vec.begin(), vec.end());
		isShuffled = true;
	}
	else {
		throw ShufflerIsEmpty();
	}
}

template<typename T>
void MyShuffler<T>::Push(T item) {
	vec.push_back(item);
}

template<typename T>
T MyShuffler<T>::Pop() {
	if (isShuffled) {
		if (vec.size()/*!= 0*/) {
			int tmp = vec[vec.size() - 1];
			vec.erase(vec.end() - 1);
			return tmp;
		}
		else {
			throw ShufflerIsEmpty();
		}
	}
	else {
		throw NotShuffled();
	}
}

template<typename T>
T MyShuffler<T>::Peek() {
	if (isShuffled) {
		if (vec.size()/*!= 0*/) {
			int tmp = vec[vec.size() - 1];
			//vec.erase(vec.end() - 1);
			return tmp;
		}
		else {
			throw ShufflerIsEmpty();
		}
	}
	else {
		throw NotShuffled();
	}
}

template<typename T>
void MyShuffler<T>::Clear() {
	isShuffled = false;
	vec.clear();
}

template<typename T>
vector<T> MyShuffler<T>::GetVector() {
	return vec;
}