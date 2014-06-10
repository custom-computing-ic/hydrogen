#include <DynLibFunc.hpp>
/*TODO remove duplication of error checking, 
  create a dlerror check class
 */
DynLibFunc::DynLibFunc(const std::string& libname, const std::string& func)
{
  libraryName = libname;
  funcName = func;
  handle = NULL;
}
DynLibFunc::~DynLibFunc() {
  this->unload();
  
}

/* Loads the library */
bool DynLibFunc::load() {
  /*clear last err message */
  dlerror();
  handle = NULL;		
  /*open a handle to the library*/
  handle = dlopen(libraryName.c_str(), RTLD_LAZY);
  char* err = dlerror();

  if (err != NULL) {
    handle = NULL; 
    std::cout << "Error loading library: " <<  err << std::endl;
  }
  funcPtr =  getFunction(); 
  if ( funcPtr == NULL) {
    return false;
  }
  return err == NULL;		
}


bool DynLibFunc::unload() {
  return dlclose(handle) == 0;
}
void* DynLibFunc::getFunction() {
  void *fptr;
  dlerror();
  fptr = dlsym(handle,funcName.c_str());
  char* err = dlerror();
  if (err != NULL) {
    std::cout << "Error loading function: " <<  err << std::endl;
    return NULL;
  }
  /* Some really nasty casting  to avoid compiler warnings,
   * unfortunately this is unavoidable */
  return fptr;

}
/* Return function from library or NULL on error */
template<typename Func> std::function<Func> DynLibFunc::getFunction() {
  void *fptr;
  dlerror();
  fptr = dlsym(handle,funcName.c_str());
  char* err = dlerror();
  if (err != NULL) {
    std::cout << "Error loading function: " <<  err << std::endl;
    return NULL;
  }
  /* Some really nasty casting  to avoid compiler warnings,
   * unfortunately this is unavoidable */
  return std::function<Func>(*reinterpret_cast<Func**>(&fptr));
}


