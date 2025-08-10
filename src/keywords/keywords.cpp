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
				//std::cout << result;
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

	/*
		std::cin.clear();
		std::cout << "\n Are u end?(y/n): ";
		while (c[0] != 'y'&&c[0] != 'Y'&& c[0]  != 'n' && c[0]  != 'n'){
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> c;	
		}
		if (c[0] == 'y'){
			ofile->open("keywords.txt");
			*ofile << result;
			ofile->close();
		}
		else if (c[0] == 'n'){
			std::cout << "\nWrite keywords again( use ',' to devide them): ";
			std::cin >> result;
			std::cin.clear();
			std::cout << "\n Are u end?(y/n): ";
			while (c[0] != 'y'&&c[0] != 'Y'&& c[0]  != 'n' && c[0]  != 'n'){
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cin >> c;	
			}
		}
	*/

	return result;
}

std::vector<std::string> divide_keywords(std::string str){
	std::vector<std::string> keywords;
	str += ' ';
	std::string t = "";
	//std::cout << str.size() << std::endl;	
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] != ' ')
			t += str[i];
		else {
			keywords.push_back(t);
			//std::cout << t << std::endl;
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


