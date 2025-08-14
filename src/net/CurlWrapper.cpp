#include "CurlWrapper.h"

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), realsize);    
	return realsize;
}

MyCurl::MyCurl(std::string api_key, std::string keyword){
	url += keyword + "&purity=100%20&sorting=random&resolutions=1920x1080&page=1&apikey=" + api_key;
	//distrib.param(decltype(distrib)::param_type{1,});
	curl = curl_easy_init();
}

void MyCurl::get_request(){
	Logs l;
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

		//std::cout << "Full url: " << url << std::endl;			
		//std::cout << "http code:" << http_code << std::endl;

		if (http_code != 200 || res != CURLE_OK){
			l.write_logs("Http error" + http_code);
			return;
		}
		else {
			l.write_logs("Successful request");
		}

		try {
			j = nlohmann::json::parse(buffer);
			l.write_logs("Successful parsing");
		} catch (const std::exception& e) {
			l.write_logs("JSON parse error: " + std::string(e.what()));
			return;
		}
	}
	else return;

}

std::string MyCurl::get_data(std::string paragraph, std::string str){
	std::string data;

	for (auto& item : j[paragraph]){
		data = item[str];
	}	
	Logs l;
	l.write_logs(str + ": " + data);
	return data;
}

std::string MyCurl::get_count_pages(){
	return "";		
}

std::string MyCurl::download_image(const std::string& image_url){
	Logs l;
	//delete old image
	fs::path dir_path("/home/p1rat/code/rwal");
	
	try {
		for (const auto& item : fs::directory_iterator(dir_path)){
			if (fs::is_regular_file(item.path())){
				auto name = item.path().filename().string();
				if (name.rfind("wallpaper-", 0) == 0) {
					fs::remove(item.path());
				}
			}
		}
		l.write_logs("Successful deleting old image");

	} catch (const fs::filesystem_error& e){
		l.write_logs("Error of delete old image: " + std::string(e.what()));
	}

	//create images's name
	std::string image_name = "/home/p1rat/code/rwal/wallpaper-";
	for (int i = 0;i < 5;i++){
		image_name+=image_url[42+i];
	}
	std::unique_ptr<std::string> t (new std::string);
	*t = get_data("data","file_type");
	image_name += ".";
	for (size_t i = 6; i < t->size(); i++){
		image_name += (*t)[i];
	}

	CURL* image_curl = curl_easy_init();
	if (!image_curl){
		l.write_logs("Failed to init CURL to image download");
		return dir_path;
	}
	else {
		l.write_logs("Successful init CURL to image download");
	}
	//create image
	FILE* fp = fopen(image_name.c_str(),"wb");
	if (!fp){
		l.write_logs("Failed to create image file");
		return dir_path;
	}
	else {
		l.write_logs("Successful creating image file");
	}
	curl_easy_setopt(image_curl, CURLOPT_URL, image_url.c_str());
	curl_easy_setopt(image_curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(image_curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(image_curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(image_curl, CURLOPT_TIMEOUT, 15L);
	
	CURLcode res = curl_easy_perform(image_curl);
	if (res != CURLE_OK){
		l.write_logs("Failed download image: " + std::string(curl_easy_strerror(res)));
		return dir_path;
	}
	else {
		l.write_logs("Successful download image" + image_name);
	}
	fclose(fp);
	curl_easy_cleanup(image_curl);
	return image_name;
}



MyCurl::~MyCurl(){
	curl_easy_cleanup(curl);
}
