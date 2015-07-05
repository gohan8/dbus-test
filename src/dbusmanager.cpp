#include "dbusmanager.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


#define DBUS "org.freedesktop.DBus"
#define DBUS_PATH "/org/freedesktop/DBus"
#define DBUS_INTERFACE "org.freedesktop.DBus"
#define DEFAULT_TIMEOUT -1
#define SERVICE_DESCR_XML "service-descr.xml"

//Declaring callbacks used for bus connection
void busAcquiredHandler (GDBusConnection* dBusConn, const gchar* name, gpointer userdata);
void NameAcquiredHandler(GDBusConnection* dBusConn, const gchar* name, gpointer userdata);
void NameLostHandler    (GDBusConnection* dBusConn, const gchar* name, gpointer userdata);

void TaskControlHandler(GDBusConnection *connection, const gchar *sender,
                        const gchar *object_path, const gchar *interface_name,
                        const gchar *method_name, GVariant *parameters,
                        GDBusMethodInvocation *invocation, gpointer user_data);

//Handle properties set/get asyncr. The first handler will handle those request
// with interface_name = org.freedesktop.DBus.Properties
const static GDBusInterfaceVTable TaskControl_VTABLE = {
    TaskControlHandler, NULL, NULL
} ;

using namespace std;

const string DBusManager::OBJECT_PATH="/org/ssdev/example/control";


DBusManager::DBusManager(){

    GError *error = NULL;

    dBusConn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);

    if ( dBusConn == NULL ) {
        cerr << error->message << endl;

        throw ExBusConnFail;
    }

    g_message("Successifully connected to DBus.");
    cout << "Successifully connected to DBus." << endl;

}

const string& DBusManager::getConnectionName(){

    const gchar* name = g_dbus_connection_get_unique_name(this->dBusConn);
    if (name){
    //g_variant_get(result, "(s)", &name);

        return *(new string((char*)name));
    }
    else return "";
}

void DBusManager::registerName(string busName) {

    GError *error=NULL;

    ifstream xmlFile(SERVICE_DESCR_XML, ios::binary|ios::in);
    stringstream buffer;
    string tmp;

    if (!xmlFile.fail()){
        do {
            if (xmlFile.eof()) break;
            getline(xmlFile,tmp);
            buffer << tmp;
            cout << tmp <<endl;
        }while (!xmlFile.eof());
    }
    string xmlData =  buffer.str();

    nodeInfo = g_dbus_node_info_new_for_xml (
                                        (gchar*) xmlData.c_str(), &error);

    if (error) cout << error->message << endl;
    this->busNameID = g_bus_own_name_on_connection(this->dBusConn, (gchar*) busName.c_str(),
                                                G_BUS_NAME_OWNER_FLAGS_NONE, NameAcquiredHandler,
                                                NameLostHandler, (gpointer) this, NULL);
    if (this->busNameID == 0) {
        tmp.clear(); tmp="Register of the dbus name fail";
        throw tmp;
    }
    g_message("DBusManager: BusName Registered: ");

}

void DBusManager::initEventLoop(){

    this->mainLoop = g_main_loop_new(NULL,FALSE);
    g_main_loop_run(this->mainLoop);

}

void DBusManager::exitEventLoop(){
    if (this->mainLoop){
    if (g_main_loop_is_running(this->mainLoop)) {
        g_message("Exiting mainloop.");
        g_main_loop_quit(this->mainLoop);
        this->mainLoop=0;
    }else g_message("Main Loop not running");
    }
}

void DBusManager::finalize(){

    if (this->busNameID != 0) {
        g_bus_unown_name (this->busNameID);
        this->busNameID = 0;
    }
    if (this->nodeInfo){
        g_dbus_node_info_unref (this->nodeInfo);
        this->nodeInfo = 0;
    }
    g_message("Releasing proxy");
    if (this->DBus){
        g_object_unref(this->DBus);
        this->DBus = 0;
    }
    g_message("Exiting main loop.");
    this->exitEventLoop();

    g_message("Closing connection");
    GError* er=0;
    if (!g_dbus_connection_close_sync (this->dBusConn, NULL, &er)) {
        g_error("Could not close bus connection.");
        g_error(er->message);
        g_object_unref(er);
    }
    //g_object_unref(this->dBusConn);
    g_message("DBusManager Finalized. ");
}

GDBusProxy* DBusManager::getProxy(gchar* name, gchar* path, gchar* interface){
    GError *error = NULL;
    GDBusProxyFlags flags = G_DBUS_PROXY_FLAGS_NONE;
    GDBusProxy* proxy;

    proxy = g_dbus_proxy_new_sync (this->dBusConn, flags, NULL,
                                            name, path, interface,
                                           NULL, &error);
    if ( proxy == NULL ) {
        cerr << error->message << endl;

        throw ExCreateProxyFail;
    }

    cout << "Proxy for BusManager successifully created. " << endl;
    return proxy;
}

DBusManager::~DBusManager()
{
    cout << "Finalizing DBusManager" << endl;
    this->finalize();
    //Must free bus connection object
    g_object_unref(this->dBusConn);
}

const string& DBusManager::get_UID(){
    GError *error = NULL;
    GVariant *result;
    gchar *id;
    string *sid;

    if (!this->DBus)
    try {
        this->DBus = this->getProxy((gchar*)DBUS, (gchar*)DBUS_PATH, (gchar*)DBUS_INTERFACE);

    }catch(string msg){
        cerr << "Fail to create DBusProxy." << endl;
        throw msg;
    }

    result = g_dbus_proxy_call_sync( this->DBus, "GetId", g_variant_new("()"),
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (error) {
        cerr << "GetID call fail: " << error->message <<endl;
        throw "EX_CallFail";
    }

    g_variant_get(result, "(s)", &id);
    sid = new string((char*)id);

    return *sid;

}


void DBusManager::getNames(){
    GError *error = NULL;
    GVariant *result = 0;
    gsize* sz;
    const char* busNames = 0;

    if (!this->DBus)
    try {
       this->DBus =  this->getProxy(DBUS, DBUS_PATH, DBUS_INTERFACE);

    }catch(string msg){
        cerr << "Fail to create DBusProxy." << endl;
        throw msg;
    }

    result = g_dbus_proxy_call_sync( DBus, "ListNames", g_variant_new("()"),
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (result == NULL) {
        cerr << "ListNames call fail: " << error->message <<endl;
        throw "EX_CallFail";
    }

    cout << "Variant type. -> " << (char*) g_variant_get_type_string(result) << endl;
    vector<string*> names;
    GVariantIter *iter;
    gchar *str;

    g_variant_get (result, "(as)", &iter);
    while (g_variant_iter_loop (iter, "s", &str))
    {
        cout << (char*) str << endl;
        names.push_back(new string((char*)str));
    }
    g_variant_iter_free(iter);
    g_variant_unref(result);
    for(string *n:names) cout << "----> " << *n <<endl;
    string *tmp;
    while (!names.empty()) {
        tmp = names.back();
        names.pop_back();
        delete(tmp);
    }

}

void busAcquiredHandler(GDBusConnection* dBusConn, const gchar* name, gpointer user_data){
    g_message("Bus Acquired!");
}

void NameAcquiredHandler(GDBusConnection* dBusConn, const gchar* name, gpointer user_data){
    DBusManager* ptrBusManager = (DBusManager*) user_data;
    GError *error=NULL;
    GDestroyNotify NO_USER_DATA_FREE_FUNC = NULL;
    stringstream data("Name");
    data << (char*)name << " acquired!";
    g_message((gchar*)data.str().c_str());
    g_dbus_connection_register_object (dBusConn, DBusManager::OBJECT_PATH.c_str(),
                                   ptrBusManager->nodeInfo->interfaces[0],
                                   &TaskControl_VTABLE, user_data,
                                   NO_USER_DATA_FREE_FUNC, &error);
    if (error) {
        g_error("Object Register fail");
        g_error(error->message);
        throw *(new string("ObjectRegFail: "));
    }
    g_message("Interface successifully registered.");

    error = 0;
    g_dbus_connection_register_object (dBusConn, DBusManager::OBJECT_PATH.c_str(),
                                   ptrBusManager->nodeInfo->interfaces[1],
                                   &TaskControl_VTABLE, user_data,
                                   NO_USER_DATA_FREE_FUNC, &error);
    if (error) {
        g_error("Object Register fail");
        g_error(error->message);
        throw *(new string("ObjectRegFail: "));
    }
    g_message("Interface successifully registered.");
}

void NameLostHandler(GDBusConnection* dBusConn, const gchar* name, gpointer user_data){
   cout << "Name ownnership for " << (char*)name << " was lost";
}

void TaskControlHandler(GDBusConnection *connection, const gchar *sender,
                        const gchar *object_path, const gchar *interface_name,
                        const gchar *method_name, GVariant *parameters,
                        GDBusMethodInvocation *invocation, gpointer user_data){
    const char sep[] = {' ','-',' ',0};
    cout << (char*) sender <<  sep << (char*) interface_name << sep << (char*) method_name << endl;
    string methodName((char*) method_name);
    string interfaceName((char*)interface_name);

    cout << "Call to " << interfaceName << "->" << methodName;
    cout << "from " << (char*)sender << " to " << (char*)object_path <<endl;
    cout << (char*)g_variant_get_type_string(parameters) <<endl;


    if (interfaceName == "org.freedesktop.DBus.Properties") {
        //handle properties request

    }
    else if (interfaceName == "org.ssdev.taskcontrol") {

        if (methodName == "start") {

            gchar* par1; // = (char*)g_variant_get_string(parameters,0);
            g_variant_get(parameters,"(s)", &par1);
            string taskName(par1);

            g_free(par1);
            int taskID = std::hash<std::string>()(taskName);
            g_dbus_method_invocation_return_value (invocation, g_variant_new("(i)",(gint32) taskID));
        }
        else if (methodName == "stop"){
            int taskID = 0;
            g_variant_get(parameters,"(i)", &taskID);
            cout << "Stoping task "<< taskID << endl;
        }
        else if (methodName == "getStatus"){
            gchar* par1;
            g_variant_get(parameters,"(s)",&par1);
            string taskName(par1);
            cout << "Checking task status for " << taskName << endl;
            g_free(par1);
        }
    }
    else if(interfaceName == "org.ssdev.objectcontrol")
        if (methodName == "shutdown"){
            g_message("Shutdown method called.");
           ((DBusManager*)user_data)->exitEventLoop();
        }

}
