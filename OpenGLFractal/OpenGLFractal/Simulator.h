#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <map>

enum FunctionType : int {
	SET_PIXEL = 0,
	GET_FRACTAL_LEVEL_VECTOR,
	GET_FRACTAL_STEP,
	MULT_14_49,
	OTHER
};

class Simulator {
public:
	static void init();
	static void addClock(FunctionType type, unsigned long long cycles);
	static void resetClock(FunctionType type);
	static void resetAll();

	static unsigned long long getClock(FunctionType type);
	static unsigned long long getTotal();

	static void printClock(FunctionType type);
	static void printAll();
	static void printTotal();

private:
	static std::map<int, unsigned long long> m;
};

#endif