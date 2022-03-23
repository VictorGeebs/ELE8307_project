#include "Simulator.h"

using namespace std;

map<int, unsigned long long> Simulator::m;
 
void Simulator::init()
{
	m.insert(pair<int, unsigned long long>(FunctionType::SET_PIXEL, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::GET_FRACTAL_LEVEL_VECTOR, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::GET_FRACTAL_STEP, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::MULT_14_49, 0));
	m.insert(pair<int, unsigned long long>(FunctionType::OTHER, 0));
}

void Simulator::addClock(FunctionType type, unsigned long long cycles)
{
	switch (type) {
	case SET_PIXEL:
	case GET_FRACTAL_LEVEL_VECTOR:
	case GET_FRACTAL_STEP:
	case MULT_14_49:
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
		s = "SET_PIXEL               :";
		break;
	case GET_FRACTAL_LEVEL_VECTOR:
		s = "GET_FRACTAL_LEVEL_VECTOR:";
		break;
	case GET_FRACTAL_STEP:
		s = "GET_FRACTAL_STEP        :";
		break;
	case MULT_14_49:
		s = "MULT_14_49              :";
		break;
	case OTHER:
		s = "OTHER                   :";
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

	s = "SET_PIXEL               :";
	c = m.find(SET_PIXEL)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "GET_FRACTAL_LEVEL_VECTOR:";
	c = m.find(GET_FRACTAL_LEVEL_VECTOR)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "GET_FRACTAL_STEP        :";
	c = m.find(GET_FRACTAL_STEP)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "MULT_14_49              :";
	c = m.find(MULT_14_49)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	s = "OTHER                   :";
	c = m.find(OTHER)->second;
	printf("%s: %I64u\n", s.c_str(), c);

	printf("--------------------------------------\n");
}

void Simulator::printTotal()
{
	unsigned long long c = getTotal();
	printf("Total clock cycles: %I64u\n", c);
}