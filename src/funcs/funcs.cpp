#include "funcs.h"
#include "logs/logs.h"

int random(int count){
	/*
	Logs l;
	while (count > 100){
		count = count / 2;
		l.write_logs("Count = " + std::to_string(count));
	}
	*/
	std::random_device rd;
	std::mt19937 gen(rd());	
	std::uniform_int_distribution<> distrib(0,count/10);
	int num = distrib(gen);
	return num;
}

