#include "funcs.h"
#include <random>
#include <unistd.h>

int random(int count){
	static std::random_device rd;
    static std::mt19937 gen(rd()); 
    
    std::uniform_int_distribution<> distrib(0, count);
    return distrib(gen);
}

void printFlags(){
	//on future	
}
