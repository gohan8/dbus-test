#ifndef DBUSMANAGER_H
#define DBUSMANAGER_H

#include <string>
#include <gio/gio.h>

using namespace std;

const string ExCreateProxyFail = "EX_CreateProxyFail";
const string ExCallFail = "Ex_CallFail";
const string ExRegisterForSignalFail = "Ex_RegisterForSignalFail";
const string ExBusConnFail = "Ex_BusConnFail";

class DBusManager
{
    public:
        const static string OBJECT_PATH;
        GDBusNodeInfo *nodeInfo = 0; //struct that represents the xml node info file
        GDBusConnection *dBusConn = 0; // struct representing the connection to bus

        DBusManager();
        virtual ~DBusManager();
        void getNames();
        const string& getConnectionName();
        const string& get_UID();
        void registerName(string);
        void initEventLoop();
        void exitEventLoop();
        void finalize();
        GDBusProxy* getProxy(gchar* name, gchar* path,
                      gchar* interface);

    protected:

        GDBusProxy *DBus = 0;          //struct representing a proxy to a bus intercafe.
        gint busNameID = 0;      //ID of the name acquired from bus
        GMainLoop *mainLoop = 0;

    private:

};


#endif // DBUSMANAGER_H
