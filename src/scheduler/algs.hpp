#ifndef __algs_h__
#define __algs_h__
#include <Scheduler.hpp>
/* Scheduling Algorithms */
Allocations FCFSMin(Scheduler &s);
Allocations FCFSMax(Scheduler &s);
Allocations FCFSAsManyAsPos(Scheduler &s);
Allocations SJTF(Scheduler &s);
Allocations CB(Scheduler &s);
Allocations ManagedMode(Scheduler &s);

/* Scoring algorithms */
bool sortByTime(Job i, Job j);
bool sortByMin(Job i, Job j);
void score(Allocations &a, Scheduler &s);
Allocations selectMaxScore(std::deque<Allocations> &a);


#endif

