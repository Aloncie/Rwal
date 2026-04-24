#include "wallpaper/WallpaperManager.hpp"
#include "mocks/MockIWallpaperSetter.hpp" 
#include "mocks/MockLogs.hpp"
#include "mocks/MockKeywords.hpp"
#include "mocks/MockNetworkManager.hpp"
#include "mocks/MockTUIManager.hpp"
#include "mocks/MockConfigReader.hpp"
#include "mocks/MockFileSystem.hpp"
#include "mocks/MockCurlWrapper.hpp"
#include "internal/GlobalConstans.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <filesystem>
#include <QCoreApplication>

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace fs = std::filesystem;

class WallpaperManagerTest : public ::testing::Test {
protected:
	std::shared_ptr<testing::NiceMock<MockLogs>> mockLogs;
	std::shared_ptr<testing::NiceMock<MockIWallpaperSetter>> mockSetter;
	std::shared_ptr<testing::NiceMock<MockKeywords>> mockKeywords;
	std::shared_ptr<testing::NiceMock<MockNetworkManager>> mockNetworkManager;
	std::shared_ptr<testing::NiceMock<MockConfigReader>> mockConfigReader;
	std::shared_ptr<testing::NiceMock<MockFileSystem>> mockFileSystem;
	std::shared_ptr<testing::NiceMock<MockTUIManager>> mockTUIManager;
	std::shared_ptr<testing::NiceMock<MockCurlWrapper>> mockCurlWrapper;
	std::unique_ptr<WallpaperManager> wallpaperManager;
	fs::path temp_dir;


	void SetUp() override {
		mockLogs = std::make_shared<testing::NiceMock<MockLogs>>();
		mockCurlWrapper = std::make_shared<testing::NiceMock<MockCurlWrapper>>(*mockLogs);
		mockSetter = std::make_shared<testing::NiceMock<MockIWallpaperSetter>>();
		mockConfigReader = std::make_shared<testing::NiceMock<MockConfigReader>>(*mockLogs);
		mockKeywords = std::make_shared<testing::NiceMock<MockKeywords>>(*mockConfigReader, *mockLogs);
		mockNetworkManager = std::make_shared<testing::NiceMock<MockNetworkManager>>(*mockCurlWrapper, *mockConfigReader, *mockLogs);
		mockTUIManager = std::make_shared<testing::NiceMock<MockTUIManager>>();
		mockFileSystem = std::make_shared<testing::NiceMock<MockFileSystem>>();
		wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);

		temp_dir = fs::temp_directory_path() / "wallpaper_manager_test";
		fs::create_directory(temp_dir);
	}

	void TearDown() override {
		fs::remove_all(temp_dir);
	}

	fs::path createFakeWallpaper(const std::string& filename) {
		fs::path filePath = temp_dir / filename;
		std::ofstream file(filePath);
		file << "test wallpaper content";
		file.close();
		return filePath;
	}
};

// ========== Tests for refresh method ==========

TEST_F(WallpaperManagerTest, Refresh_Successful) {
	fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");

	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(fakeWallpaper));
	EXPECT_CALL(*mockSetter, setWallpaper(fakeWallpaper)).WillOnce(Return(true));

	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	auto result = wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_FALSE(result.has_value());
}

TEST_F(WallpaperManagerTest, Refresh_FetchImageFails) {
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(std::nullopt));
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	auto result = wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result.value(), "Failed to fetch image");
}

TEST_F(WallpaperManagerTest, Refresh_SetWallpaperFails) {
	fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(fakeWallpaper));
	EXPECT_CALL(*mockSetter, setWallpaper(fakeWallpaper)).WillOnce(Return(false));
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	auto result = wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result.value(), "Failed to set wallpaper");
}

TEST_F(WallpaperManagerTest, Refresh_LogsFetchImageFails) {
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(std::nullopt));
	ON_CALL(*mockLogs, writeLogs(_, _, "Failed to fetch image")).WillByDefault(Invoke([this](std::string_view, std::string_view, std::string_view message) {
		mockLogs->lastLogMessage = message;
	}));
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(mockLogs->contains("Failed to fetch image"));
}

TEST_F(WallpaperManagerTest, Refresh_LogsSetWallpaperFails) {
	fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(fakeWallpaper));
	EXPECT_CALL(*mockSetter, setWallpaper(fakeWallpaper)).WillOnce(Return(false));
    ON_CALL(*mockLogs, writeLogs(_, _, _)).WillByDefault(Invoke([this](std::string_view, std::string_view, std::string_view msg) {
		mockLogs->lastLogMessage = msg;
	}));	
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(mockLogs->contains("Failed to set wallpaper"));
}

TEST_F(WallpaperManagerTest, Refresh_LogsSetWallpaperSuccess) {
	fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(fakeWallpaper));
	EXPECT_CALL(*mockSetter, setWallpaper(fakeWallpaper)).WillOnce(Return(true));
	ON_CALL(*mockLogs, writeLogs(_, _, _))
		.WillByDefault(Invoke([this](std::string_view, std::string_view, std::string_view msg) {
			mockLogs->lastLogMessage = msg;
    }));
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(mockLogs->contains("Wallpaper set successfully"));
}

TEST_F(WallpaperManagerTest, Refresh_LogsSetWallpaperPath) {
	fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");
	EXPECT_CALL(*mockKeywords, SilentGetKeyword()).WillOnce(Return("test_keyword"));
	EXPECT_CALL(*mockNetworkManager, fetchImage("test_keyword")).WillOnce(Return(fakeWallpaper));
	EXPECT_CALL(*mockSetter, setWallpaper(fakeWallpaper)).WillOnce(Return(true));
	ON_CALL(*mockLogs, writeLogs(_, _, _)) .WillByDefault(Invoke([this](std::string_view, std::string_view, std::string_view msg) {
			mockLogs->lastLogMessage = msg;
    }));
	wallpaperManager = std::make_unique<WallpaperManager>(*mockLogs, *mockFileSystem);
	wallpaperManager->refresh(*mockSetter, *mockNetworkManager, *mockKeywords);
	EXPECT_TRUE(mockLogs->contains(fakeWallpaper.string()));
}

// ========== Tests for saveCurrent method ==========

TEST_F(WallpaperManagerTest, SaveCurrent_ReturnsSuccess) {
    fs::path fakeWallpaper = createFakeWallpaper("wallpaper.jpg");
    fs::path downloadsDir = "/fake/downloads";
    fs::path picturesDir = temp_dir;
	fs::path rwalDir = picturesDir / "rwal";
    fs::path expectedDest = picturesDir / fakeWallpaper.filename();

    EXPECT_CALL(*mockFileSystem, getAppLocalDataLocation()).WillOnce(Return(downloadsDir));
    EXPECT_CALL(*mockFileSystem, exists(downloadsDir)).WillOnce(Return(true));
    EXPECT_CALL(*mockFileSystem, listDirectory(downloadsDir, rwal::wallpaper::FILE_PREFIX)).WillOnce(Return(std::vector<fs::path>{fakeWallpaper}));

    EXPECT_CALL(*mockFileSystem, getPicturesLocation()).WillOnce(Return(picturesDir));
    EXPECT_CALL(*mockFileSystem, getApplicationName()).WillOnce(Return("rwal"));
	EXPECT_CALL(*mockFileSystem, exists(rwalDir)).WillOnce(Return(false));
    EXPECT_CALL(*mockFileSystem, createDirectories(rwalDir)).WillOnce(Return(true));

    EXPECT_CALL(*mockFileSystem, copyFile(fakeWallpaper, expectedDest)).WillOnce(Return(true));

    std::string result = wallpaperManager->saveCurrent();
    EXPECT_EQ(result, "Wallpaper saved successfully");
}
