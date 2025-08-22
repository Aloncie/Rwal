#include "keywords.h"
#include "logs/logs.h"

const std::string keywords_path = "/home/p1rat/code/rwal/build/keywords.txt";

std::string Keywords::look_keywords(){
	std::string result = "None";
	std::ifstream ifile;
	ifile.open(keywords_path);
	if (ifile.is_open()){
		getline(ifile,result);
		ifile.close();
	}
	else {
		Logs l;
		l.write_logs("File open error ");
		return nullptr;
	}
	Keywords k;
	k.format_str(result);

	return result;
}

std::string Keywords::get_keywords(int& count){
	std::string result = "";
	Keywords k;

	if (std::filesystem::exists(keywords_path)) {
		result = k.look_keywords();	
		if (result != ""){
			k.format_str(result);
			return result;
		}
	}
	
	std::cout << "\nThere is not file with keywords." <<  "\nPlease write keywords to find wallpaper( use ',' to devide them): ";
	count+=3;
	while (result == "")
		std::getline(std::cin,result);
	//std::cin >> result;
	k.format_str(result);
	std::ofstream ofile;

	ofile.open(keywords_path);

	if (ofile.is_open()){
		ofile << result;
		ofile.close();
	}
	else{ 
		Logs l;
		l.write_logs("File create error ");
		return nullptr;
	}
	return result;
}

std::vector<std::string> Keywords::divide_keywords(std::string str){
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

std::string Keywords::format_str(std::string& str){
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

void Keywords::open_keywords_editor(){
    Logs l;
	struct stat buffer;
    if (stat(keywords_path.c_str(), &buffer) != 0) {
		l.write_logs("Error: File doesn't exist or inaccessible: " + keywords_path);
        return;
    }

    if (access(keywords_path.c_str(), W_OK) != 0) {
		l.write_logs("Error: No write permissions for file: " + keywords_path);
        return;
    }

	const char* editor = getenv("EDITOR");
	if (!editor)
		editor = "nano";
	std::string command = std::string(editor) + " " + keywords_path;
	system(command.c_str());	
}
