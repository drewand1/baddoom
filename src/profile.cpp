#include <chrono>
#include "profile.h"
 
using namespace std;

double profToggle() {
	static chrono::steady_clock::time_point last = chrono::steady_clock::now();

	chrono::steady_clock::time_point now = chrono::steady_clock::now();
	chrono::duration<double> elapsed = now - last;
	last = now;
	return elapsed.count();
}