#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <curl/curl.h>
#include <random>
#include <string>
#include <iostream>
#include <cctype>
#include <vector>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp);

class Settings{
	void see_settings();
	void edit_keywords(); //Make with setenv("EDITOR", "/usr/bin/vim", 1);
	void edit_timer();
};

class MyCurl{
private:
	CURL* curl;
	long http_code = 0;
	std::string url = "https://wallhaven.cc/api/v1/search?q=";
	std::string page, buffer;
	nlohmann::json j;
public:

	MyCurl(std::string api_key, std::string keyword){
		url += keyword + "&purity=100%20&sorting=random&resolutions=1920x1080&page=1&apikey=" + api_key;
		//distrib.param(decltype(distrib)::param_type{1,});
		curl = curl_easy_init();
	}

	void get_request(){
		CURLcode res;
		if (curl){
			curl_easy_setopt(curl,CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&callback);
			curl_easy_setopt(curl,CURLOPT_WRITEDATA,&buffer);
			curl_easy_setopt(curl,CURLOPT_TIMEOUT,10L);
			curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
			//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 

			res = curl_easy_perform(curl);
			curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_code);

			std::cout << "Full url: " << url << std::endl;			
			std::cout << "http code:" << http_code << std::endl;

			if (http_code != 200 || res != CURLE_OK){
				std::cout << "Http error" << std::endl;
				return;
			}
			//else std::cout << "All good!" << std::endl;

			try {
				j = nlohmann::json::parse(buffer);
			} catch (const std::exception& e) {
				std::cerr << "JSON parse error: " << e.what() << std::endl;
				return;
			}
		}
		else return;

	}

	std::string get_image_url(){
		std::string image_url;

		for (auto& item : j["data"]){
			image_url = item["path"];
		}	
		return image_url;	
	}

	std::string download_image(const std::string& image_url){
		//delete old image
		std::filesystem::path dir_path("/home/p1rat/code/rwal");
		
		try {
			for (const auto& item : std::filesystem::directory_iterator(dir_path)){
				if (std::filesystem::is_regular_file(item.path())){
					auto name = item.path().filename().string();
					if (name.rfind("wallpaper-", 0) == 0) {
						std::filesystem::remove(item.path());
					}
				}
			}

		} catch (const std::filesystem::filesystem_error& e){
			std::cerr << "Error of delete old image: " << e.what() << std::endl;
		}

		//create images's name
		std::string image_name = "/home/p1rat/code/rwal/wallpaper-";
		std::cout << image_url << std::endl << image_url.size() << std::endl;
		for (int i = 0;i < 5;i++){
			image_name+=image_url[42+i];
		}
		image_name += ".jpg";

		CURL* image_curl = curl_easy_init();
		if (!image_curl){
			std::cerr << "Failed to init CURL to image download" << std::endl;
			return dir_path;
		}
		//create image
		FILE* fp = fopen(image_name.c_str(),"wb");
		if (!fp){
			std::cerr << "Failed to create image file" << std::endl;
			return dir_path;
		}
		curl_easy_setopt(image_curl, CURLOPT_URL, image_url.c_str());
		curl_easy_setopt(image_curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(image_curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(image_curl, CURLOPT_FOLLOWLOCATION,1L);
		curl_easy_setopt(image_curl, CURLOPT_TIMEOUT, 15L);
		
		CURLcode res = curl_easy_perform(image_curl);
		if (res != CURLE_OK){
			std::cerr << "Failed download image: " << curl_easy_strerror(res) << "\n";
			return dir_path;
		}
		fclose(fp);
		curl_easy_cleanup(image_curl);
		return image_name;
	}



   	~MyCurl(){
		curl_easy_cleanup(curl);
	}
};


static size_t callback (void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), realsize);    
	return realsize;
}


std::string define_OS(){
	return "";
}

std::string format_str(std::string& str){
	for (int i = 0;i < str.size(); i++){
		if (!std::isalpha(str[i])){
			str.erase(i,1);
		}
	}

	return str;	
}

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

int main(int argc, char *argv[]){
	std::string url;
	std::string local;
	auto keywords = divide_keywords(get_keywords());

	do {
		std::string kw = choose_keyword(keywords);
		MyCurl c("an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a", kw);
		c.get_request();
		url = c.get_image_url();
		if (!url.empty()) {
			local = c.download_image(url);
			break;
		}
	} while (true);	

	QCoreApplication app(argc, argv);

	QString local_q = QString::fromStdString(local);

	QVariantMap params;
	params.insert(QStringLiteral("Image"), local_q);
	
	auto msg = QDBusMessage::createMethodCall(
		QStringLiteral("org.kde.plasmashell"),
		QStringLiteral("/PlasmaShell"),
		QStringLiteral("org.kde.PlasmaShell"),
		QStringLiteral("setWallpaper")	
	);
	msg << QStringLiteral("org.kde.image")
		<< params
		<< quint32(0);

	auto reply = QDBusConnection::sessionBus().call(msg);
	if (reply.type() == QDBusMessage::ErrorMessage) {
		qWarning() << "Error D-Bus: " << reply.errorMessage();
	}

	return 0;
}
