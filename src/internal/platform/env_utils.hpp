#pragma once
#include <iostream>

namespace rwal::platform{
	namespace terminal {
		namespace buffer {
			inline void enter_alt_buffer() {std::cout << "\033[?1049h" << std::flush;}
			inline void leave_alt_buffer() {std::cout << "\033[?1049l" << std::flush;}
		}
		namespace cursor {
			inline void hide_cursor() { std::cout << "\033[?25l" << std::flush; }
			inline void show_cursor() { std::cout << "\033[?25h" << std::flush; }
		}
	}
	namespace executor{
		void open_editor(std::string& path);
	}
}
