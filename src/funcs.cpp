#include <string>
#include "funcs.h"

std::string define_OS(){
	return "";
}

std::string format_str(std::string& str){
	for (int i = 0;i < str.size(); i++){
		if (!std::isalpha(str[i])&&str[i+1] != ' '){
			str[i] = ' ';
		}
		else if (!std::isalpha(str[i])){
			str.erase(i,1);
		}
	}

	return str;	
}


