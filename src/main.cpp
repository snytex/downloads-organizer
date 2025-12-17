#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

std::string getDownloadsPath() {
  const char *home = nullptr;

#ifdef _WIN32
  home = std::getenv("USERPROFILE")
#else
  home = std::getenv("HOME");
#endif

      if (home == nullptr) { // If the directory could not be determined
    return "";
  }

  fs::path download_dir = fs::path(home) / "Downloads";
  return download_dir.string();
}

std::string getTime() {
  auto now = std::chrono::system_clock::now();

  // Convert to time_t
  time_t now_c = std::chrono::system_clock::to_time_t(now);

  // Convert to tm struct
  std::tm *now_tm = std::localtime(&now_c);

  // build string
  std::stringstream ss;
  ss << (now_tm->tm_year + 1900) << "-" << (now_tm->tm_mon + 1) << "-"
     << now_tm->tm_mday;

  std::string time_str = ss.str();

  return time_str;
}

void createDirectory(std::string time, std::string dir) {
  fs::create_directory(dir + time);
}

void moveFiles(std::string dir, std::string destination_dir, std::string time) {
  std::string path = dir;
  std::regex pattern("^\\d{4}-\\d{1,2}-\\d{1,2}$");

  for (auto entry : fs::directory_iterator(dir)) {
    fs::path filename = entry.path().filename();
    std::string filename_str = filename.string();
    fs::path destination = fs::path(destination_dir) / filename;

    if (std::regex_match(filename_str, pattern)) {
      continue;
    }
    std::cout << "Moving: " << filename << std::endl;

    fs::rename(entry.path(), destination);
    std::cout << "Finished moving!" << std::endl;
  }
}

int main() {
  std::string time_str = getTime();
  std::string download_dir = getDownloadsPath() + std::string(1, std::filesystem::path::preferred_separator);

  if (download_dir.empty()) {
    std::cerr << "Error: Could not find downloads folder!" << std::endl;
    return 1;
  }

  createDirectory(time_str, download_dir);
  moveFiles(download_dir, download_dir + time_str, time_str);

  return 0;
}
