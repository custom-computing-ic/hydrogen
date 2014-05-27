#ifndef _TASK_H_
#define _TASK_H_
#include <list>
#include <algorithm>
#include <iostream>
#include <string>
#include <Implementation.hpp>


class Task {
  public:
    Task(std::string s);
    ~Task();

    std::list<Implementation *> GetImplementations();
	  void AddImp(Implementation *imp) { this->Implementations.push_back(imp);};
  	void DelImp(Implementation *imp) { 

            this->Implementations.remove(imp); 
            delete(imp);}; 
	  friend std::ostream& operator<<(std::ostream& os, const Task& obj) { 
	    os << obj.getName();
	    return os;
	  }
  	friend bool operator==(const Task& lhs, const Task& rhs) {
      return lhs.getName().compare(rhs.getName()) == 0;
    }
  	friend bool operator!=(const Task& lhs, const Task& rhs) {
      return !operator==(lhs,rhs);
    }
  	friend bool operator<(const Task& lhs, const Task& rhs) {
      return lhs.getName().compare(rhs.getName()) < 0;
    }
  	friend bool operator>(const Task& lhs, const Task& rhs) {
      return  operator< (rhs,lhs);
    }
  	friend bool operator<=(const Task& lhs, const Task& rhs) {
      return !operator> (lhs,rhs);
    }
	  friend bool operator>=(const Task& lhs, const Task& rhs) {
      return !operator< (lhs,rhs);
    }
	  std::string getName() const { 
      return name; 
    }
  private:
	  std::list<Implementation *> Implementations;
  	std::string name;
    // add stuff here
};

#endif
