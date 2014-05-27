#ifndef _IMPLEMENTATION_H_
#define _IMPLEMENTATION_H_
#include <dlfcn.h>
#include <string>
#include <functional>
#include <iostream>

class DynLibFunc {
public:
  DynLibFunc();
  DynLibFunc(const std::string& libname, const std::string& funcName);
  ~DynLibFunc(); 

  template<typename Rettype, typename... Args> 
  Rettype run(Args... args) {
    std::function<Rettype(Args...)> f;
    Rettype (*x) (Args...);
    if (funcPtr != NULL) { 
      f = std::function<Rettype(Args...)>(*reinterpret_cast<decltype(x)*>(&funcPtr));
      return f(args...);
    }
    return NULL;
  }

  /*double adapter(int input,std::function<double(int)> f) {
      return f(input);
  }*/

  bool load();
  bool unload();
  std::string getFuncName() { return funcName;}
  std::string getLibName() { return libraryName;}  

private:
  template<typename Func> std::function<Func> getFunction();
  void* getFunction();
  void* funcPtr; /* Nasty... TODO: look at std::function alternatives*/
  std::string libraryName;
  std::string funcName;
  void *handle; /* Nasty, but needed for dynamic linking*/
};

#endif
