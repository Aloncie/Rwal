#include "CurlWrapper.h"
#include "internal/GlobalConstans.h"
#include "logs/logs.h"
#include "funcs/funcs.h"
#include <exception>
#include <filesystem>
#include <string>
#include <fstream>

static size_t callback (void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), realsize);    
	return realsize;
}

MyCurl::MyCurl(){
	curl = curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_USERAGENT,"rwal/1.0");
}

void MyCurl::get_request(std::string url){
	clearning();

	CURLcode res;
	if (curl){
		Logs::getInstance().write_logs("Try to request");
		curl_easy_setopt(curl,CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&callback);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&buffer);
		curl_easy_setopt(curl,CURLOPT_TIMEOUT,10L);
		curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);

		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_code);

		Logs::getInstance().write_logs("url of request: " + url);

		if (http_code != 200 || res != CURLE_OK){
			Logs::getInstance().write_logs("Http error" + std::to_string(http_code));
			return;
		}
		else {
			Logs::getInstance().write_logs("Successful request");
		}
		
		Logs::getInstance().write_logs("Try to parse");
		try {
			j = nlohmann::json::parse(buffer);
			Logs::getInstance().write_logs("Successful parse JSON");
		} catch (const std::exception& e) {
			Logs::getInstance().write_logs("JSON parse error: " + std::string(e.what()));
			return;
		}
	}
	else 
		return;

}

std::string MyCurl::get_data(std::string paragraph, std::string str){
	if (!j.contains(paragraph)){
		 
		Logs::getInstance().write_logs("Error: paragraph " + paragraph + " not found in JSON");
		return "";
	}
	try {
		if (j[paragraph].is_array()){
			for (auto& item : j[paragraph]){
				if (item.contains(str)){
					 
					if (item[str].is_string()){
						Logs::getInstance().write_logs("Data of JSON: " + item[str].get<std::string>());
						return item[str].get<std::string>();
					}
					Logs::getInstance().write_logs("Data of JSON: " + std::to_string(item[str].get<int>()));
					return std::to_string(item[str].get<int>());
				}
			}
		}

		else if (j[paragraph].is_object()){
			if (j[paragraph].contains(str)){
				 
				if (j[paragraph][str].is_string()){
					Logs::getInstance().write_logs("Data of JSON: " + j[paragraph][str].get<std::string>());
					return j[paragraph][str].get<std::string>();
				}

				Logs::getInstance().write_logs("Data of JSON: " + std::to_string(j[paragraph][str].get<int>()));
				return std::to_string(j[paragraph][str].get<int>());
			}
		}
	} catch (const std::exception& e){
		 
		Logs::getInstance().write_logs("JSON parsing error: " + std::string(e.what()));
	}
	return "";
}

std::string MyCurl::download_image(const std::string& image_url){
	 
	fs::path downloads = fs::path(SOURCE_DIR) / "downloads";
	Logs::getInstance().write_logs("Try to delete old image");

	try {
		if (!fs::exists(downloads))
			fs::create_directory(downloads);
		for (const auto &i : fs::directory_iterator(downloads)){
			if (fs::is_regular_file(i.path()))
			   	fs::remove(i.path());	
		}
	   	Logs::getInstance().write_logs("Successful deleting old image");

	} catch (const fs::filesystem_error& e){
		Logs::getInstance().write_logs("Error of delete old image: " + std::string(e.what()));
	}

	
	std::string filename = call_image(image_url);
    fs::path wallpaper = downloads / filename;

	CURL* image_curl = curl_easy_init();
	if (!image_curl){
		Logs::getInstance().write_logs("Failed to init CURL to image download");
		return "";
	}
	else {
		Logs::getInstance().write_logs("Successful init CURL to image download");
	}
	std::ofstream fp(wallpaper.c_str(), std::ios::binary);
	if (!fp){
		Logs::getInstance().write_logs("Failed to create image file");
		curl_easy_cleanup(image_curl);
		return "";
	}
	else
		Logs::getInstance().write_logs("Successful creating image file");

	curl_easy_setopt(image_curl, CURLOPT_URL, image_url.c_str());
	curl_easy_setopt(image_curl, CURLOPT_WRITEFUNCTION, +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
    	auto* stream = static_cast<std::ofstream*>(userdata);
    	size_t totalSize = size * nmemb;
   		stream->write(static_cast<char*>(ptr), totalSize);
    	return totalSize;
    });	
	curl_easy_setopt(image_curl, CURLOPT_WRITEDATA, &fp);
	curl_easy_setopt(image_curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(image_curl, CURLOPT_TIMEOUT, 15L);
	
	CURLcode res = curl_easy_perform(image_curl);

	fp.close();
	curl_easy_cleanup(image_curl);

	if (res != CURLE_OK){
		Logs::getInstance().write_logs("Failed download image: " + std::string(curl_easy_strerror(res)));
		return "";
	}
	else
		Logs::getInstance().write_logs("Successful download image" + wallpaper.string());

	return wallpaper;
}



MyCurl::~MyCurl(){
	curl_easy_cleanup(curl);
}

void MyCurl::clearning(){
	buffer.clear();
	j.clear();
}

void MyCurl::generateUniqueSuffix(std::string& filename) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < rwal::wallpaper::SUFFIX_LENGTH; ++i) {
	   	filename += chars[random(chars.size() - 1)];
    }
}

std::string MyCurl::call_image(const std::string& image_url) {
    size_t lastSlash = image_url.find_last_of('/');
    std::string filename = rwal::wallpaper::FILE_PREFIX;

    if (lastSlash == std::string::npos) {
        generateUniqueSuffix(filename);
        filename += ".jpg";
    }
    else {
        std::string raw_tail = image_url.substr(lastSlash + 1);
        size_t firstDash = raw_tail.find('-');

        if (firstDash == std::string::npos) {
            generateUniqueSuffix(filename);

            if (raw_tail.find(".png") != std::string::npos) {
                filename += ".png";
            } else {
                filename += ".jpg";
            }
        }
        else {
            filename += raw_tail.substr(firstDash + 1);
        }
    }
    return filename;
}
