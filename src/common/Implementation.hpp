#ifndef _IMPLEMENTATION_H_
#define _IMPLEMENTATION_H_
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <message.hpp>
#include <Client.hpp>
class Implementation {
public:
  Implementation();
  ~Implementation() {};
  Implementation(const std::string &libname, const std::string &funcName);

  Implementation(std::string libraryName_, std::string funcName_,
                 std::string name_, std::string url_, std::string port_,
                 std::string type_)
      : name(name_), url(url_), port(port_), type(type_),
        libraryName(libraryName_), funcName(funcName_) {}

  /* Getters */
  std::string getFuncName() { return funcName; }
  std::string getLibName() { return libraryName; }
  std::string getName() { return name; }
  std::string getUrl() { return url; }
  std::string getPort() { return port; }
  std::string getType() { return type; }
  /* Setters */
  void setFuncName(std::string arg) { funcName = arg; }
  void setLibName(std::string arg) { libraryName = arg; }
  void setName(std::string arg) { name = arg; }
  void setUrl(std::string arg) { url = arg; }
  void setPort(std::string arg) { port = arg; }
  void setType(std::string arg) { type = arg; }

private:
  std::string libraryName;
  std::string funcName;
  std::string name;
  /* Resource */
  std::string url;
  std::string port;
  std::string type;
};

#endif
