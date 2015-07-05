#ifndef OBJECTCONTROL_H
#define OBJECTCONTROL_H

#include "dbusmanager.h"

class ObjectControl
{


    public:
        DBusManager *dBusm; int c = 0;
        ObjectControl();
        virtual ~ObjectControl();
        void TaskStart(string taskName);
        void TaskStop(unsigned int taskID);
        void GetTaskStatus(unsigned int);
        void SubscribeForSignals();
        void Finalize();
        void shutdown();

    protected:
        GDBusProxy* taskControl;
        GDBusProxy* objControl;
        gint subscID = 0;

    private:

};

#endif // OBJECTCONTROL_H
