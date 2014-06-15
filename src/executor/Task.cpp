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

std::pair<Implementation*,double> Task::SelectImplementation(size_t dataSize, size_t wt) {
   std::cout << "(DEBUG): Task::SelectImplementation()\n";
   std::deque<double>::iterator it;
   std::deque<double> values;
   double predicted_time = -1;
   for( auto p: PerformanceModels) {
     if (p != NULL) {
       predicted_time = p->QueryModel(dataSize);
       std::cout << "(DEBUG): Querying Model: " << dataSize << "="
                 << predicted_time << "\n";
       if (p->getImp()->getType() == "SHARED_DFE")
         predicted_time += (double)wt;

       values.push_back(predicted_time);
     } else {
       std::cout << "(ERROR): Invalid Model\n";
     }
   }
   it = min_element(values.begin(),values.end());
   int index = std::distance(values.begin(), it);

   std::list<PerfModel*>::iterator head= PerformanceModels.begin();
   std::advance(head, index);
   std::cout << "(DEBUG): Best implementation: " << (*head)->getImp()->getType() << "\n";
   return std::make_pair((*head)->getImp(),predicted_time);
}
