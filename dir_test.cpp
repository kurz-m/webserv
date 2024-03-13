#include <dirent.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

// static bool check_file_(const std::string &file) {
//   struct stat sb;
//
//   if (stat(file.c_str(), &sb) < 0) {
//     return false;
//   }
//
//   switch (sb.st_mode & S_IFMT) {
//   case S_IFREG:
//     return false;
//   case S_IFDIR:
//     return true;
//   default:
//     return false;
//   }
// }

// static void traverse_folder(const std::string &path) {
//   DIR *dir = opendir(path.c_str());
//   if (dir == NULL) {
//     std::cerr << "Error opening directory: " << path << std::endl;
//     return;
//   }
//
//   struct dirent *entry;
//   while ((entry = readdir(dir))) {
//     if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
//     {
//       std::string current = path + "/" + entry->d_name;
//
//       if (check_file_(current)) {
//         std::cout << "Directory: " << current << std::endl;
//         traverse_folder(current);
//       }
//     }
//   }
//
//   closedir(dir);
// }

struct FileInfo {
  std::string name;
  bool is_dir;
};

const std::string list_dir_head =
    "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.5\">"
    "<title>Directory Listing</title> <style> body { font-family: Arial, "
    "sans-serif; margin: 20px;"
    "} h1 { text-align: center; } table { border-collapse: collapse; margin: "
    "20px auto; }"
    "th, td { border: 1px solid #ccc; padding: 10px; } a { text-decoration: "
    "none; } </style>"
    "</head> <body> <h1>Directory Listing</h1> <table> <thead> <tr> "
    "<th>Name</th> <th>Last Modified</th>"
    "<th>Size</th> <th>Type</th>  </tr> </thead> <tbody>";

static inline FileInfo create_list_dir_entry(const std::string &path) {
  FileInfo file;
  std::ostringstream oss;
  char m_time[30] = {0};
  oss << "<tr><td><a href=\"" << path << "\">" << path << "</a></td><td>";
  struct stat sb;
  if (stat(path.c_str(), &sb) < 0) {
    // TODO: Handle failure of the stat. What does that mean?
  }
  strftime(m_time, sizeof(m_time), "%y-%b-%d %H:%M:%S",
           std::localtime(&(sb.st_mtime)));
  oss << std::string(m_time) << "</td><td>";
  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    oss << sb.st_size << " KB</td><td>File</td></tr>";
    file.is_dir = false;
    break;
  case S_IFDIR:
    oss << "</td><td>Directory</td></tr>";
    file.is_dir = true;
    break;
  }

  file.name = oss.str();
  return file;
  ;
}

#include <vector>
#include <algorithm>

bool compare_file(const FileInfo& a, const FileInfo& b) {
  if (a.is_dir && b.is_dir) {
    return a.name < b.name;
  } else {
    return a.is_dir > b.is_dir;
  }
}

std::string create_list_dir_() {
  std::vector<FileInfo> files;
  std::ostringstream oss;
  DIR *dir = opendir(".");
  if (dir == NULL) {
    return oss.str();
  }
  oss << list_dir_head;
  struct dirent *dp = NULL;
  while ((dp = readdir(dir))) {
    std::string dir_name = dp->d_name;
    if (dir_name == ".") {
      continue;
    } else {
      files.push_back(create_list_dir_entry(dp->d_name));
    }
  }
  std::sort(files.begin(), files.end(), compare_file);
  std::vector<FileInfo>::iterator it;
  for (it = files.begin(); it != files.end(); ++it) {
    oss << it->name;
  }
  oss << "</tbody></table></body></html>";
  return oss.str();
}
int main() {
  std::string ret = create_list_dir_();
  std::cout << ret << std::endl;
  // std::string starting_dir = ".";
  // traverse_folder(starting_dir);
  return 0;
}
