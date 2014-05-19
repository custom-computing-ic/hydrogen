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
  std::list<Implementation *>::iterator i = this->Implementations.begin();
	for(;i != this->Implementations.end(); i++) {
		delete(*i);
	}
}
