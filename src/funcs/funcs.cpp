#include "funcs.h"
#include <random>
#include <unistd.h>

int random(int count){
	std::random_device rd;
	std::mt19937 gen(rd());	
	std::uniform_int_distribution<> distrib(0,count);
	int num = distrib(gen);
	return num;
}

void printFlags(){
	
}
