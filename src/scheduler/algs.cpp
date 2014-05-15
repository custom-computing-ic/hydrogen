#include <algs.hpp>
#include <algorithm>

#define MAGIC_NUMBER 100
#define MAX_RES 10

/***** Priority Agnostic algorithms (FCFS) *****/
Allocations FCFSMin(Scheduler &s) {
  Allocations a;
  auto rq = s.getQueuePointer("readyQ");
  std::cout << "Using FCFSMin" << "\n";
  int w_size = s.getWindow();
  for (int i = 0; i < w_size && i < s.readyQSize(); i++) {
    Job j = s.getJobFromQ(rq,i);
    if (s.allocate(j,j.getMin(),j.getMin()) > 0) {
      /* successfully allocated the min resources */
//      std::cout << "Adding " << j  << " To allocations\n";
      a.jobs.push_back(j);
    } else {
      break;
      /* if we can't allocate then just stop */
    }
  }
  return a;
}

Allocations FCFSAsManyAsPos(Scheduler &s) {
  Allocations a;
  auto rq = s.getQueuePointer("readyQ");
  std::cout << "Using FCFSAMAP" << "\n";
  int w_size = s.getWindow();
  for (int i = 0; i < w_size && i < s.readyQSize(); i++) {
    Job j = s.getJobFromQ(rq,i);
    if (s.allocate(j,j.getMax(),j.getMin()) > 0) {
      /* successfully allocated the min resources */
      a.jobs.push_back(j);
    } else {
      break;
      /* if we can't allocate then just stop */
    }
  }
  return a;
}
Allocations FCFSMax(Scheduler &s) {
  Allocations a;
  auto rq = s.getQueuePointer("readyQ");
  std::cout << "Using FCFS Max" << "\n";
  int w_size = s.getWindow();
  for (int i = 0; i < w_size && i < s.readyQSize(); i++) {

    Job j = s.getJobFromQ(rq,i);
//    std::cout << "Trying to allocate " << j << "[" << s.resPoolSize() << "]\t";
    if (s.allocate(j,j.getMax(),j.getMax()) > 0) {
      /* successfully allocated the min resources */
      a.jobs.push_back(j);
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
bool sortByTime(Job i, Job j) { return (i.minCost() < j.minCost()); }
bool sortByMin(Job i, Job j) { return (i.getMin() < j.getMin()); }
/***** Shortest Job Time First Mode *****/
Allocations SJTF(Scheduler &s) {
  int w_size = s.getWindow();
  std::cout << "Using SJTF\n";
  auto rq = s.getQueuePointer("readyQ");
  Allocations aloc; 
  if (rq == NULL) {
    std::cout << "SOMETHING WENT HORRIBLY WRONG :O ";
    return aloc;
  }
  if (rq->size() == 0) {
    std::cout << "No items to process\n";
    return aloc;
  }
  /* Get the minimum Job size (min number of req resources) */ 
  std::deque<Job> job_window;
  for (int i = 0; i < w_size && i < rq->size(); i++) {
    job_window.push_back(s.getJobFromQ(rq,i));
  }
  std::sort(job_window.begin(), job_window.end(), sortByTime);
  auto min_elem =  std::min_element(job_window.begin(), job_window.end(), sortByMin);
  auto nend = job_window.end();
  while(min_elem->getMin() < s.resPoolSize() && min_elem < nend) {
   s.allocate(*min_elem, min_elem->getMin(), min_elem->getMin());
   aloc.jobs.push_back(*min_elem);
   nend = remove(job_window.begin(),nend,*min_elem);
   min_elem =  std::min_element(job_window.begin(), nend, sortByMin);   
    
  }
  return aloc;
}

Allocations ManagedMode(Scheduler &s) {
  auto av = s.getAlgVecPtr();
  std::deque<Allocations> allocations;
  std::cout << "**Scheduling using the managed mode**\n"
            << "Res[" <<s.resPoolSize() << "]\t" 
            << "RQ[" << s.readyQSize() << "]\n";
  for(unsigned int i = 0; i < av->size()-1; i++) {
    allocations.push_back( s.schedule(i,true));
    s.returnResources(allocations.back());
  }
  std::deque<Allocations>::iterator a = allocations.begin();
  for (;a < allocations.end(); a++) {
    score(*a,s);
  }
  Allocations mChoice = selectMaxScore(allocations);
  std::deque<Job>::iterator j = mChoice.jobs.begin();
  for (;j < mChoice.jobs.end(); j++) {
    s.realloc(*j);
  }
  return mChoice;
}


void score(Allocations &a, Scheduler &s) {
  std::string strat = s.getStrategy();
  std::cout << "starting score: " << a.score << "\n"; 
  if (!strat.compare("Completion Time")) {
    std::cout << "MAKESPAN: " << a.makespan() << "\tJobs: " << a.jobs.size() << "\t" ;
    if (a.jobs.size() == 0 || a.makespan() == 0) {
      a.score = 0;
    } else {
      int q_const = s.readyQSize();
      q_const = q_const > 0 ? q_const : 1;
      a.score = (q_const*a.jobs.size()) + ( a.jobs.size() / a.makespan() );  
    }
  }
  if (!strat.compare("Fairness")) {

  } 
  std::cout << "Score: " << a.score << "\n";
}

Allocations selectMaxScore(std::deque<Allocations> &a) {
  float maxScore = a[0].score;
  int index = 0;
  for (unsigned int i = 0; i < a.size() ; i++ ) {
    std::cout << "a[" << i << "].score: " << a[i].score << " maxScore: " << maxScore << "\n";
    if (a[i].score >= maxScore) {
      maxScore = a[i].score;
      index = i;
    }
  }

  std::cout << "Chose: ";
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
        std::cout << "Err should never reach here\n";
      break;
 }
 return a[index]; 
}



