#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <Resource.hpp>
#include <Task.hpp>
#include <PerfModel.hpp>
#include <Implementation.hpp>


/*#include "../resource/resource.h"
#include "../task/task.h"
#include "../perf_model/model.h"
#include "../implementation/implementation.h"
*/

class Executor {
  public:
    Executor();
 
    /* 
     * Manipulate the Resources list
     */
    int SetResources(std::list<Resource *> res);
    int AddResource(Resource *res);
    int DelResource(Resource *res);
    std::list<Resource *> GetResources(); 
    /* 
     * Manipulate the Task list
     */
    int SetTasks(std::list<Task *> tsk);
    Task* FindTask(std::string name);
    int AddTask(Task *tsk);
    int DelTask(Task *tsk);
    std::list<Task *> GetTasks();    
    /* 
     * Add/Remove Implementations
     */
    int AddImp(Task *tsk,Implementation *imp);
    int DelImp(Task *tsk,Implementation *imp);
    std::list<Implementation *> GetImplementations(Task *tsk);

    int CreatePerfomanceModels();
    Implementation SelectImplementation(Task *tsk, void* input);
 private:
    /* 
     * Given:
     *    - Set of Tasks
     *    - Set of Implementations
     * Goal:
     *    - Derrive Performance Model
     *      For each task
     */
    std::list<Resource *> AvailableRes;
    std::list<Task *> Tasks;
//    std::list<PerfModel<std::map<Implementation,int>,int>> PerfModels;

};

#endif