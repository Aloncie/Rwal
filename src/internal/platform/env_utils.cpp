#include "env_utils.hpp"

namespace rwal::platform{
	namespace executor{
		void open_editor(std::string& path){
			const char* editor = getenv("EDITOR");
			if (!editor)
				editor = "nano";
			std::string command = std::string(editor) + " " + path;
			system(command.c_str());	
		}			
	}
}
