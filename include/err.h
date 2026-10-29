#ifndef ERR_H
#define ERR_H

#include <iostream>

template <typename T>
void assertEx(T* ptr, const char* msg) {
	if (!ptr) {
		std::cerr << msg;
		exit(-1);
	}
}

void assertEx(bool cond, const char* msg) {
	if (!cond) {
		std::cerr << msg;
		exit(-1);
	}
}

#endif