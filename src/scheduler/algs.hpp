#ifndef __algs_h__
#define __algs_h__
#include <Scheduler.hpp>
/* Scheduling Algorithms */
Allocations *FCFSMin(Scheduler &s);
Allocations *FCFSMax(Scheduler &s);
Allocations *FCFSAsManyAsPos(Scheduler &s);
Allocations *SJTF(Scheduler &s);
Allocations *CB(Scheduler &s);
Allocations *Priority(Scheduler &s);
Allocations *ManagedMode(Scheduler &s);

/* Scoring algorithms */
bool sortByPriority(const JobTuplePtr i, const JobTuplePtr j);
bool sortByTime(const JobTuplePtr i, const JobTuplePtr j);
bool sortByMin(const JobTuplePtr i, const JobTuplePtr j);
void score(Allocations &a, Scheduler &s);
Allocations *selectMaxScore(std::deque<Allocations *> &a);

#endif
