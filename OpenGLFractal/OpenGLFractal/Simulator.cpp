#include "Simulator.h"

using namespace std;

map<int, unsigned long long> Simulator::m;
 
void Simulator::init()
{
	m.insert(pair<int, unsigned long long>(FunctionType::SET_PIXEL, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::GET_FRACTAL_LEVEL_VECTOR, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::PARALLEL_MULTIPLY, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::OTHER, 0));
}

void Simulator::addClock(FunctionType type, unsigned long long cycles)
{
	switch (type) {
	case SET_PIXEL:
	case GET_FRACTAL_LEVEL_VECTOR:
	case PARALLEL_MULTIPLY:
	case OTHER:
		m[type] += cycles;
		break;
	}
}

void Simulator::resetClock(FunctionType type)
{
	auto itr = m.find(type);
	if (itr != m.end()) {
		itr->second = 0;
	}
}

void Simulator::resetAll()
{
	for (auto itr = m.begin(); itr != m.end(); ++itr) {
		itr->second = 0;
	}
}

unsigned long long Simulator::getClock(FunctionType type) 
{
	auto itr = m.find(type);
	if (itr != m.end()) {
		return itr->second;
	}
	return 0;
}

unsigned long long Simulator::getTotal()
{
	unsigned long long c = 0;
	for (auto itr = m.begin(); itr != m.end(); ++itr) {
		c += itr->second;
	}
	return c;
}

void Simulator::printClock(FunctionType type)
{
	bool found = true;
	string s;
	switch (type) {
	case SET_PIXEL:
		s = "SET_PIXEL               ";
		break;
	case GET_FRACTAL_LEVEL_VECTOR:
		s = "GET_FRACTAL_LEVEL_VECTOR";
		break;
	case PARALLEL_MULTIPLY:
		s = "PARALLEL_MULTIPLY       ";
		break;
	case OTHER:
		s = "OTHER                   ";
		break;
	default:
		found = false;
		break;
	}

	unsigned long long c = m.find(type)->second;

	if (found) {
		printf("%s: %I64u\n", s.c_str(), c);
	}
}

void Simulator::printAll()
{
	string s;
	unsigned long long c;
	
	printf("--------------------------------------\n");

	s = "SET_PIXEL               ";
	c = m.find(SET_PIXEL)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "GET_FRACTAL_LEVEL_VECTOR";
	c = m.find(GET_FRACTAL_LEVEL_VECTOR)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "PARALLEL_MULTIPLY       ";
	c = m.find(PARALLEL_MULTIPLY)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "OTHER                   ";
	c = m.find(OTHER)->second;
	//printf("%s: %I64u\n", s.c_str(), c);

	printf("--------------------------------------\n");
}

void Simulator::printTotal()
{
	unsigned long long c = getTotal();
	printf("Total clock cycles: %I64u\n", c);
}

void Simulator::printStatistics()
{
	printAll();
	printTotal();
	static unsigned long long cycles = 0;
	unsigned long long total = Simulator::getTotal();
	printf("Delta cycles:       %I64u\n", total - cycles);
	printf("--------------------------------------\n");
	cycles = total;
}
