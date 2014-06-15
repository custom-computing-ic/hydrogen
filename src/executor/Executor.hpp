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
#include <MultiThreadedTCPServer.hpp>
//#include <packet.pb.h> needeed for protobuf control msgs.
/*#include "../resource/resource.h"
#include "../task/task.h"
#include "../perf_model/model.h"
#include "../implementation/implementation.h"
*/
#define NUM_THREADS 10
class Executor : public MultiThreadedTCPServer {
  public:
    Executor(const std::string& hostname,
             const std::string& clientId) :
      MultiThreadedTCPServer::super(hostname,"811"+clientId,NUM_THREADS)
    {
      //MISC
      cid = clientId;
      avg_wt = 0;
    }
    ~Executor();
    /*
     * Manipulate the Resources list
     */
    int SetResources(std::list<Resource *> res);
    int AddResource(Resource *res);
    int DelResource(Resource *res);
    Resource* FindResource(std::string);
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
    int AddImp(Task *tsk,PerfModel *p);
    int DelImp(Task *tsk,Implementation *imp);
    std::list<Implementation *> GetImplementations(Task *tsk);
    //TODO[mtottenh]: Implement this..
    virtual msg_t* handle_request(msg_t* request);
    msg_t* runImp(Implementation*,msg_t*);
    virtual void start(){ run(); }
    virtual void stop(){}


    int CreatePerfomanceModels();
    void CreatePerfModelForTask(Task *tsk);

    Implementation SelectImplementation(Task *tsk, void* input);
 private:
    void parse_protobuf(msg_t* m);
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
    int schedPort;
    size_t avg_wt;
    std::string cid;
    std::string schedName;
};

#endif
