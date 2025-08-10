#include "keywords.h"

std::string get_keywords(){
	std::string result = "";
	std::string c = "";
	std::ifstream ifile;

	if (std::filesystem::exists("keywords.txt")) {
		
		ifile.open("keywords.txt");
		if (ifile.is_open()){
			getline(ifile,result);
			if (result != ""){
				ifile.close();
				return result;
			}
		}
		else {
			std::cout << "File open error " << std::endl;
			return nullptr;
		}
	}
	
	std::cout << "\nThere is not file with keywords." <<  "\nPlease write keywords to find wallpaper( use ',' to devide them): ";
	std::getline(std::cin,result);
	format_str(result);
	std::ofstream ofile;

	ofile.open("keywords.txt");

	if (ofile.is_open()){
		ofile << result;
		ofile.close();
	}
	else{ 
		std::cout << "File create error " << std::endl;
		return nullptr;
	}

	return result;
}

std::vector<std::string> divide_keywords(std::string str){
	std::vector<std::string> keywords;
	str += ' ';
	std::string t = "";
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] != ' ')
			t += str[i];
		else {
			keywords.push_back(t);
			t = "";
		}
	}
	return keywords;
}

std::string choose_keyword(std::vector<std::string> keywords){
	std::random_device rd;
	std::mt19937 gen(rd());	
	std::uniform_int_distribution<> distrib(0,keywords.size()-1);
	int num = distrib(gen);
	std::cout << "The keywords on this time: " <<  keywords[num] << std::endl;
	return keywords[num];
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
