#include <algs.hpp>
#include <algorithm>

#define MAGIC_NUMBER 100
#define MAX_RES 10

/***** Priority Agnostic algorithms (FCFS) *****/
Allocations* FCFSMin(Scheduler &s) {
  Allocations* a = new Allocations();
  JobQueuePtr rq = s.getReadyQPtr();
#ifdef DEBUG
//  std::cout << "(DEBUG): Using FCFSMin" << "\n";
#endif
  size_t w_size = s.getWindow();
  for (size_t i = 0; i < w_size && i < s.readyQSize(); i++) {
    JobTuplePtr j = s.copyJobFromQ(rq,i);    //TODO: COPY the job accross
    if (j == nullptr) {
  //    std::cout << "(ERROR): FATAL ERROR COPYING JOB!\n";
    }

    ResourceList resourceList =  s.allocate(*std::get<0>(*j),
                                    std::get<0>(*j)->getMin(),
                                    std::get<0>(*j)->getMin());
    auto resourceListPtr = std::make_shared<ResourceList>(resourceList);
    //TODO: make allocate leave resources in the pool?
    if (resourceList.size() > 0) {
      /* successfully allocated the min resources */
      a->addJobResourcePair(j,resourceListPtr);
    } else {
#ifdef DEBUG
//      std::cout << "(DEBUG): FCFSMin Can't allocate resources to job\n";
#endif
      break;
      /* if we can't allocate then just stop */
    }
  }
  return a;
}

Allocations* FCFSAsManyAsPos(Scheduler &s) {
  Allocations *a = new Allocations();
  JobQueuePtr rq = s.getReadyQPtr();

#ifdef DEBUG
 // std::cout << "(DEBUG) Using FCFSAMAP" << "\n";
#endif
  size_t w_size = s.getWindow();
  for (size_t i = 0; i < w_size && i < s.readyQSize(); i++) {
    JobTuplePtr j = s.copyJobFromQ(rq,i);    //TODO: COPY the job accross
    ResourceList resourceList =  s.allocate(*std::get<0>(*j),
                                    std::get<0>(*j)->getMax(),
                                    std::get<0>(*j)->getMin());

    if ( resourceList.size() > 0) {
      /* successfully allocated the min resources */
      auto resourceListPtr = std::make_shared<ResourceList>(resourceList);
      a->addJobResourcePair(j,resourceListPtr);
    } else {
      break;
      /* if we can't allocate then just stop */
    }
  }
  return a;

}
Allocations* FCFSMax(Scheduler &s) {
  Allocations *a = new Allocations();
  JobQueuePtr rq = s.getReadyQPtr();
#ifdef DEBUG
//  std::cout << "(DEBUG): Using FCFS Max" << "\n";
#endif
  size_t w_size = s.getWindow();
  for (size_t i = 0; i < w_size && i < s.readyQSize(); i++) {
    JobTuplePtr j = s.copyJobFromQ(rq,i);    //TODO: COPY the job accross
    ResourceList resourceList =  s.allocate(*std::get<0>(*j),
                                    std::get<0>(*j)->getMax(),
                                    std::get<0>(*j)->getMax());
    if ( resourceList.size() > 0) {
      /* successfully allocated the min resources */
      auto resourceListPtr = std::make_shared<ResourceList>(resourceList);
      a->addJobResourcePair(j,resourceListPtr);
    } else {
      break;
      /* if we can't allocate then just stop */
    }
  }
  return a;
}
/***** Max Throughput *****/
//Allocations MThroughput(Scheduler &s) {
//  return nullptr;
//}

Allocations* Priority(Scheduler &s) {
  Allocations *alloc = new Allocations();
  JobQueue job_window;
  JobQueuePtr rq = s.getReadyQPtr();

  size_t w_size = s.getWindow();
  for (size_t i = 0; i < w_size && i < rq->size(); i++) {
    job_window.push_back(s.copyJobFromQ(rq,i));
  }


  std::sort(job_window.begin(), job_window.end(), sortByPriority);
  auto max_elem = std::max_element(job_window.begin(), job_window.end(), sortByPriority);
  JobQueue::iterator nend = job_window.end();

  while(*max_elem != nullptr &&
       (std::get<0>(**max_elem))->getMax() < s.resPoolSize() &&
       max_elem != nend) {

   ResourceList resourceList =  s.allocate(*std::get<0>(**max_elem),
                                    std::get<0>(**max_elem)->getMax(),
                                    std::get<0>(**max_elem)->getMax());

   auto resourceListPtr = std::make_shared<ResourceList>(resourceList);
   alloc->addJobResourcePair(*max_elem,resourceListPtr);

   nend = remove(job_window.begin(),nend,*max_elem);
   max_elem =  std::max_element(job_window.begin(), nend, sortByPriority);

  }
  return alloc;
}

/***** Priority Scheduler with Backfill *****/
/*Allocations PBackfill(Scheduler &s) {
  int w_size = s.getWindow();
  JobQueue* rq = s.getQueuePointer("readyQ");

  if (rq == nullptr) {
    std::cout << "SOMETHING WENT HORRIBLY WRONG :O ";
    return nullptr;
  }
  if (rq->size() == 0) {
    std::cout << "No items to process\n";
    return nullptr;
  }

  std::map< int , std::deque<Job>> sjtmap;
  for ( auto a : *(rq->getContainer())) {
    sjtmap[a.getPriority()].push_back(a);
  }
  while (allocateable()) {
    for (i = MAX_PRIORITY; MAX_PRIORITY; i--) {

    }

  }

  return (Allocations)nullptr;
}*/
bool sortByPriority(const JobTuplePtr i, const JobTuplePtr j)
{
  return    (std::get<0>(*i))->getPriority() < (std::get<0>(*j))->getPriority();
}
bool sortByTime(const JobTuplePtr i, const JobTuplePtr j)
{
  return    (std::get<0>(*i))->minCost() < (std::get<0>(*j))->minCost();
}
bool sortByMin(const JobTuplePtr i, const JobTuplePtr j) {

  return (std::get<0>(*i)->getMin() < std::get<0>(*j)->getMin());
}
/***** Shortest Job Time First Mode *****/
Allocations* SJTF(Scheduler &s) {
  Allocations *aloc = new Allocations();
  size_t w_size = s.getWindow();
#ifdef DEBUG
//  std::cout << "(DEBUG): Using SJTF\n";
#endif
  JobQueuePtr rq = s.getReadyQPtr();
  if (rq == nullptr) {
//    std::cout << "(ERROR): SOMETHING WENT HORRIBLY WRONG :O ";
    return aloc;
  }
  if (rq->size() == 0) {
#ifdef DEBUG
//    std::cout << "(DEBUG): No items to process\n";
#endif
    return aloc;
  }
  // Get the minimum Job size (min number of req resources)
  JobQueue job_window;
  for (size_t i = 0; i < w_size && i < rq->size(); i++) {
    job_window.push_back(s.copyJobFromQ(rq,i));
  }
  std::sort(job_window.begin(), job_window.end(), sortByTime);
  JobQueue::iterator min_elem =  std::min_element(job_window.begin(), job_window.end(), sortByMin);
  JobQueue::iterator nend = job_window.end();

  while(*min_elem != nullptr &&
       (std::get<0>(**min_elem))->getMin() < s.resPoolSize() &&
       min_elem < nend) {

   ResourceList resourceList =  s.allocate(*std::get<0>(**min_elem),
                                    std::get<0>(**min_elem)->getMax(),
                                    std::get<0>(**min_elem)->getMax());
   auto resourceListPtr = std::make_shared<ResourceList>(resourceList);
   aloc->addJobResourcePair(*min_elem,resourceListPtr);

   nend = remove(job_window.begin(),nend,*min_elem);
   min_elem =  std::min_element(job_window.begin(), nend, sortByMin);

  }
  return aloc;
}

Allocations* ManagedMode(Scheduler &s) {
  AlgVecType* av = s.getAlgVecPtr();
  std::deque<Allocations*> allocations;
  if (!s.useGui()) {
    std::cout << "(DEBUG): Scheduling using the managed mode\n"
              << "(DEBUG):\t- Resource Pool Size [" <<s.resPoolSize() << "]\t"
              << "# Waiting Jobs [" << s.readyQSize() << "]\n";
  }
  /* Create a list of allocations */
  for(unsigned int i = 0; i < av->size()-1; i++) {
    allocations.push_back( s.schedule(i,true) );
    /* This means that we don't actually allocate the resources for each job */
    allocations.back()->returnResources(s);
  }
  if (!s.useGui()){
    std::cout << "(DEBUG):\t- Scoring Allocations\n";
  }
  /* Score each allocation */
  std::deque<Allocations* >::iterator a = allocations.begin();
  for (;a != allocations.end(); a++) {

    score(**a,s);
  }
  /* Find the allocation with the highest score */
  Allocations *mChoice = selectMaxScore(allocations,s.useGui());
  return mChoice;
}


void score(Allocations &a, Scheduler &s) {
  std::string strat = s.getStrategy();
  if (!s.useGui()) {
    std::cout << "(DEBUG):\t\t* Score: ";
    std::cout << "starting score: " << a.getScore() << "\n";
  }
  if (!strat.compare("Completion Time")) {
    if (a.noJobs() == 0 || a.makespan() <= 0) {
      a.setScore(0);
    } else {
      size_t q_const = s.readyQSize();
      if (!s.useGui()) {
        std::cout << "\t\tQCONST: " << q_const;
        std::cout << "\tNOJOBS: " << a.noJobs();
        std::cout << "\tMKSPN: " << a.makespan();
      }
      q_const = q_const > 0 ? q_const : 1;
      a.setScore((float)((q_const*a.noJobs()) +
                ((float) a.noJobs() / (float) a.makespan() )));
    }
  }
  if (!strat.compare("Fairness")) {
    a.setScore(a.totalPriorities());

  }
  if (!s.useGui()) {

    std::cout <<"\tRESULT:" << a.getScore() << "\n";
  }
}

Allocations* selectMaxScore(std::deque<Allocations *> &a, bool gui) {
  float maxScore = a[0]->getScore();
  unsigned int index = 0;
  for (unsigned int i = 0; i < a.size() ; i++ ) {
 //   std::cout << "a[" << i << "].score: " << a[i]->getScore()
//              << " maxScore: " << maxScore << "\n";
    if (a[i]->getScore() >= maxScore) {
      maxScore = a[i]->getScore();
      index = i;
    }
  }
  if (!gui) {
    std::cout << "(DEBUG):\t - Managed mode chose: ";
    switch(index) {
      case 0:
        std::cout << "FCFS MAX\n";
        break;
      case 1:
        std::cout << "FCFS MIN\n";
        break;
      case 2:
        std::cout << "FCFS AMAP\n";
        break;
      case 3:
        std::cout << "SJTF\n";
        break;
      case 4:
        std::cout << "Priority\n";
        break;
      default:
        std::cout << "\n(ERROR): Manage Mode decided on an algorithm "
                  << "that doesn't exist!\n";
  }
}
 for (unsigned int i = 0; i < a.size(); i++) {
   if (i != index) {
     delete a[i];
   }
 }
 return a[index];
}



