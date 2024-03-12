#include <dirent.h>
#include <iostream>
#include <string>
#include <sys/stat.h>

static bool check_file_(const std::string &file) {
  struct stat sb;

  if (stat(file.c_str(), &sb) < 0) {
    return false;
  }

  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    return false;
  case S_IFDIR:
    return true;
  default:
    return false;
  }
}

static void traverse_folder(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if (dir == NULL) {
    std::cerr << "Error opening directory: " << path << std::endl;
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      std::string current = path + "/" + entry->d_name;

      if (check_file_(current)) {
        std::cout << "Directory: " << current << std::endl;
        traverse_folder(current);
      }
    }
  }

  closedir(dir);
}

int main() {
  std::string starting_dir = ".";
  traverse_folder(starting_dir);
  return 0;
}
