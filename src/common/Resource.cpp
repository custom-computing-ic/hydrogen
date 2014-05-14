#include <Resource.hpp>

virtual void Resource::start() { 
  Client::start();
}

virtual void Resource::stop() { 
  Client::stop(); 

}
