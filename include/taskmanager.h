#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <string>
#include <array>
#include <vector>

using namespace std;

struct Task {
    std::string* pTaskName;
    enum {IDLE, RUNNING, BLOCKED, STOPPED, PAUSED} Status;
    std::array<std::string,5> TaskStatus = {{"Idle", "Running", "Blocked", "Stopped", "Paused"}};

    int TaskID;

    Task(std::string taskName):pTaskName(new string(taskName)),
        TaskID(std::hash<std::string>()(taskName)),
        Status(RUNNING){
    }

    ~Task(){
        if (pTaskName) delete(pTaskName);
    }

    const string & GetStatus() {return TaskStatus[Status];}
};

class TaskManager
{
    vector<Task*> ActiveTasks;

    public:
        const static string InterfaceName;
        TaskManager();
        int StartTask(string taskName);
        string GetTaskStatus(int taskID);
        string StopTask(int taskID);
        virtual ~TaskManager();

    protected:

    private:
};

#endif // TASKMANAGER_H
