#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <string>
#include <vector>

struct Setting {
  enum Type { STRING, INT } type;
  std::string name;
  std::string str_val;
  int inv_val;
};

struct RouteBlock {
  std::string path;
  std::vector<Setting> settings;

  Setting find(const std::string&);
};

struct ServerBlock {
  std::vector<Setting> settings;
  std::vector<RouteBlock> routes;

  Setting find(const std::string&);
};

struct HttpBlock {
  std::vector<Setting> settings;
  std::vector<ServerBlock> servers;

  Setting find(const std::string&);
};

#endif // !__SETTINGS_HPP__
