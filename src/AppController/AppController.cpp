#include "AppController.hpp"

#include "logs/Logs.hpp"

#include <ncurses.h>
#include <unistd.h>

AppController::AppController(
    Navigator& nav, TUIManager& tui, WallpaperManager& wmanager, IWallpaperSetter& env,
    NetworkManager& netmanager, Keywords& keywords, std::jthread& wallpaperThread,
    std::atomic<bool>& refreshDone, std::string& refreshError)
    : m_navigator(nav),
      m_tui(tui),
      m_wmanager(wmanager),
      m_env(env),
      m_netmanager(netmanager),
      m_keywords(keywords),
      m_wallpaperThread(wallpaperThread),
      m_refreshDone(refreshDone),
      m_refreshError(refreshError) {
    m_navigator.printCurrentMenu();
}

bool AppController::handleStdin() {
    int ch = getch();
    if (ch == ERR)
        return true; // no input, keep going

    MenuResponse resp;
    if (m_tui.isInputActive()) {
        m_tui.processInputChar(ch);
    } else {
        const std::string validChoices = m_navigator.getCurrentValidChoices();
        char inputChar = static_cast<char>(ch);
        if (validChoices.find(inputChar) != std::string::npos) {
            std::string input(1, static_cast<char>(ch));
            resp = m_navigator.processInput(input, m_tui);
            if (resp.needQuit)
                return false; // signal the loop to shutdown
            if (resp.needRefreshWallpaper)
                launchRefreshWallpaper();
        }
        m_navigator.printCurrentMenu();
    }

    // Redraw the menu after any keystroke that leaves input inactive.
    // This covers both menu navigation AND input completion (Enter).
    if (!m_tui.isInputActive()) {
        m_navigator.printCurrentMenu();
    }

    if (!resp.Message.empty()) {
        m_tui.showMessage(resp.Message);
    }
    return true; // keep going
}

void AppController::launchRefreshWallpaper() {
    if (m_wallpaperThread.joinable()) {
        m_wallpaperThread.join();
    }

    m_wallpaperThread = std::jthread([this] {
        auto error = m_wmanager.refresh(m_env, m_netmanager, m_keywords, &m_tui);
        if (error.has_value()) {
            m_refreshError = error.value();
        } else {
            m_refreshError.clear();
        }
        m_refreshDone = true;
    });
}

void AppController::checkRefreshDone() {
    if (m_refreshDone.exchange(false)) {
        if (!m_refreshError.empty()) {
            m_tui.showMessage(m_refreshError);
        }
        m_navigator.printCurrentMenu();
    }
}
