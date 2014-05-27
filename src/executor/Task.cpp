#include <Task.hpp>

Task::Task(std::string s)
{
	this->name = s;
}


std::list<Implementation *> Task::GetImplementations()
{
        return this->Implementations;
}


Task::~Task() {
  size_t s = Implementations.size();
  for ( size_t i = 0; i < s; i++) {
    auto t = Implementations.front();
    Implementations.pop_front();
      delete t;
  }
}
