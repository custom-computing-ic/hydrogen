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

Implementation* Task::SelectImplementation(size_t dataSize) {
   std::cout << "(DEBUG): Task::SelectImplementation()\n";
   std::deque<double>::iterator it;
   std::deque<double> values; 
   for( auto p: PerformanceModels) {
     if (p != NULL) {
       std::cout << "(DEBUG): Querying Model: " << dataSize << "=" 
                 << p->QueryModel(dataSize) << "\n";
       values.push_back(p->QueryModel(dataSize));
     } else {
       std::cout << "(ERROR): Invalid Model\n";
     }
   }
   it = min_element(values.begin(),values.end());
   int index = std::distance(values.begin(), it);
   std::cout << "(DEBUG): Best implementation index: " << index << "\n";
   std::list<PerfModel*>::iterator head= PerformanceModels.begin();
   std::advance(head, index);
   return (*head)->getImp();
}
