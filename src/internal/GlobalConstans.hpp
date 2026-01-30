#pragma once
#include <string>

namespace rwal{ 
	namespace wallpaper{
		static inline const std::string FILE_PREFIX = "wallpaper-";
		static inline const std::string DEFAULT_EXT = ".jpg";
		inline constexpr int SUFFIX_LENGTH = 6;
		static inline const std::string DONWLOADS_DIR_NAME = "downloads";
	}
	namespace io{
		namespace shell{
			void openFileSysEditor(std::string& path){
				const char* editor = getenv("EDITOR");
				if (!editor)
					editor = "nano";
				std::string command = std::string(editor) + " " + path;
				system(command.c_str());	
			}
		}	
	}
}
