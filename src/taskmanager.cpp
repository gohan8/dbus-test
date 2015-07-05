#include "taskmanager.h"

#include <iostream>

using namespace std;

const static TaskManager::InterfaceName = "org.ssdev.taskcontrol";

TaskManager::TaskManager()
{
    //ctor
}

TaskManager::~TaskManager()
{
    //dtor
}

string TaskManager::StopTask(int taskID)
{
    cout << "Stoping task "<< taskID << endl;
    return "";
}

string TaskManager::GetTaskStatus(int taskID)
{
    cout << "Checking task status for " << taskID << endl;

}

int TaskManager::StartTask(string taskName)
{
    Task* task = new Task(taskName);
    this->ActiveTasks.push_back(task);
    cout << "Starting task " << taskName << "with id " << task->TaskID << endl;
    return task->TaskID;
}

