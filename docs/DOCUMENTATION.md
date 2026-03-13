# 📘 Rwal – Technical Documentation (v0.3.0-alpha)
---
## 🧱 Architecture Overview

Rwal is built as a set of loosely coupled modules, each with a single responsibility. The main components are:
AppController – Qt event loop entry point; manages QSocketNotifier for asynchronous input.

- Navigator – State machine for menu navigation; holds a map of Menu objects.
- UIManager – Handles terminal output and non‑blocking input. All input requests are callback‑based.
- NetworkManager – Wrapper around libcurl; performs HTTP requests and parses JSON responses.
- Keywords – Manages keyword lists; all operations that may block (editor, config write) are asynchronous.
- Settings – Configuration file handling, systemd timer integration.
- WallpaperManager – Orchestrates wallpaper download and setting; uses callbacks to report results.
- Logs – Simple file logger with size rotation and permission fixing (still a singleton for simplicity).
  
---
## 📁 Detailed Module Breakdown
1. **AppController**
   - **Header:** `AppController.hpp`
   - **Responsibilities:** Initializes QSocketNotifier for stdin, connects to handleStdin slot, prints initial menu.
   - **Key methods:**
     - `AppController(Navigator* nav, UIManager& ui, QObject* parent = nullptr)` – constructor.
     - `void handleStdin()` – slot called when input is available; reads a character, passes it to UIManager if input is active, otherwise to Navigator. Then refreshes the menu and shows any pending message.

2. **Navigator**
   - **Header:** `navigator.hpp`
   - **Responsibilities:** Maintains current menu, processes input, handles menu transitions and quit.
   - **Key members:**
     - `std::map&lt;std::string, std::unique_ptr&lt;Menu&gt;&gt; m_menus` – registered menus by ID.
     - `Menu* m_currentMenu` – pointer to the active menu.
   - **Key methods:**
     - `void registerMenu(const std::string& name, std::unique_ptr&lt;Menu&gt; menu)` – registers a menu.
     - `void start(const std::string& initialMenu)` – sets the initial menu and prints it.
     - `MenuResponce processInput(std::string& input, UIManager& uimanager)` – delegates input to current menu; handles menu switching and quit.
     - `std::string getCurrentValidChoices() const` – returns valid keys for the current menu (used by AppController for early validation).

3. **Menu Interface** *(defined in `menus.hpp`)*
   - **Purpose:** Abstract base for all menus.
   - **Methods:**
     - `virtual std::vector&lt;std::string&gt; getLines() = 0` – returns the lines to display.
     - `virtual MenuResponce handleInput(const std::string& input) = 0` – processes a single‑character input.
     - `virtual std::string getValidChoices() const = 0` – returns a string of valid input characters.

4. **UIManager**
   - **Header:** `ui/cli/UIManager.hpp`
   - **Purpose:** Manages terminal output and asynchronous input.
   - **Key members:**
     - `bool inputActive` – whether an input request is pending.
     - `std::string inputBuffer` – accumulates characters until Enter.
     - `std::string prompt` – prompt shown during input.
     - `std::function&lt;void(std::string)&gt; inputCallback` – called when input is complete.
   - **Key methods:**
     - `void initUI()` – initialises ncurses, sets non‑blocking mode, enables colours.
     - `void shutdownUI()` – ends ncurses.
     - `void showMessage(std::string message)` – prints a coloured message at the bottom line.
     - `template&lt;typename T&gt; void requestInput(std::function&lt;void(T)&gt; callback, std::optional&lt;std::string&gt; message)` – starts an asynchronous input request. The callback is invoked with the parsed input when the user presses Enter. If parsing fails, an error message is shown and the request restarts.
     - `void processInputChar(int ch)` – called by AppController for each character when inputActive is true. Handles backspace, Enter, and normal characters.
     - `bool isInputActive() const` – returns whether input is pending.
   - **Why this design?**
     - Traditional console input with `std::cin` blocks the entire program. By using ncurses in non‑blocking mode (`nodelay(stdscr, TRUE)`) and integrating with Qt's event loop via `QSocketNotifier`, we achieve a fully responsive UI that can handle background tasks (downloads, file operations) without freezing.

5. **Keywords**
   - **Header:** `keywords.hpp`
   - **Purpose:** Manages keyword lists – reading from config, random selection, editing.
   - **Key methods:**
     - `void GetRandomKeywords(std::function&lt;void(std::string)&gt; callback, const std::string& mode)` – asynchronously obtains a random keyword. In "change" mode it returns immediately from the cached list; in "core" mode it may prompt the user if no keywords are configured, then calls the callback with the chosen keyword.
     - `void editKeywords()` – opens the system editor in a child process. Before forking, it saves and restores the terminal state using `def_prog_mode()` / `reset_prog_mode()` to prevent terminal corruption.
     - `template&lt;typename T&gt; T ShortWayGetKeywords()` – returns keywords as a `std::vector&lt;std::string&gt;` or a comma‑separated `std::string` (compile‑time decision).
   - **Asynchronous pattern:**
     - `GetRandomKeywords` accepts a callback instead of returning a value. This allows the method to potentially wait for user input (in "core" mode) without blocking the event loop. The callback is invoked when the keyword is ready.

6. **WallpaperManager**
   - **Header:** `wallpaper/WallpaperManager.hpp`
   - **Purpose:** Orchestrates wallpaper download and setting.
   - **Key methods:**
     - `void refresh(const std::string& mode = "core")` – asynchronously fetches a random keyword via `Keywords::GetRandomKeywords`, then downloads the image using `NetworkManager::fetchImage` (which itself may be blocking, but is run in a separate thread). Finally, sets the wallpaper via D‑Bus.
     - `std::string saveCurrent()` – copies the current wallpaper to the user's Pictures directory. Returns a status message.
     - `std::optional&lt;fs::path&gt; getPicturesPath()` – locates or creates the `~/Pictures/rwal` directory.
   - **Threading note:**
     - `NetworkManager::fetchImage` performs synchronous HTTP requests. To avoid blocking the UI, it is called from a `std::async` or `QtConcurrent` task (not shown in the provided code, but this is the intended upgrade). The result is delivered via a signal or a callback posted to the main thread.

7. **NetworkManager**
   - **Header:** `net/NetworkManager.hpp`
   - **Purpose:** Handles HTTP requests, checks internet connectivity, downloads images.
   - **Key methods:**
     - `bool isAvailable()` – checks connectivity by attempting to connect to `8.8.8.8:53`. Non‑blocking socket with timeout.
     - `std::string fetchImage(std::string keyword)` – synchronous download (should be called from a worker thread). Returns the local path of the downloaded image.
     - `std::string craftUrl(std::string keyword, std::optional&lt;std::string&gt; page)` – builds the Wallhaven API URL from config.

8. **Logs**
   - **Header:** `logs/logs.hpp`
   - **Purpose:** File logging with rotation. Still a singleton for simplicity, but now accepts a `UIManager*` at initialisation to display critical errors.
   - **Key methods:**
     - `static void init(UIManager& ui)` – must be called once in `main()`.
     - `static Logs& getInstance()` – returns the singleton.
     - `void writeLogs(std::string message)` – appends a timestamped message to the log file; if the file exceeds 1 MB, it is rotated.
    

## 🔧 Configuration File (config.json)
Located in ~/.config/Aloncie/rwal/config.json. Structure unchanged from previous versions.

## 🧪 Current Status & Roadmap
See ROADMAP.md for the up‑to‑date development plan.

## 🤝 Contributing
While this is a personal learning project, suggestions and bug reports are welcome via GitHub Issues.

<details>
<summary>tags</summary>

#Rwal #Dev #cpp #Architecture 

</details>