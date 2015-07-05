#include "objectcontrol.h"
#include <iostream>
#include <sstream>

#define TASKCONTROL "org.ssdev.example.control"
#define TASKCONTROL_PATH "/org/ssdev/example/control"
#define TASKCONTROL_INTERFACE "org.ssdev.objectcontrol"
#define OBJCONTROL_INTERFACE "org.ssdev.objectcontrol"
#define DEFAULT_TIMEOUT -1

void signalReceived (GDBusConnection *connection, const gchar *sender_name,
                     const gchar *object_path, const gchar *interface_name,
                     const gchar *signal_name, GVariant *parameters,
                    gpointer user_data);

ObjectControl::ObjectControl():dBusm(new DBusManager())
{
    try {
        this->taskControl = dBusm->getProxy(TASKCONTROL, TASKCONTROL_PATH,
                                     TASKCONTROL_INTERFACE);
    }catch(string msg){
        cerr << "Fail to create DBUS Proxy for "
             << TASKCONTROL_INTERFACE << " interface!" << endl;
        throw msg;
    }
    try{
        this->objControl = dBusm->getProxy(TASKCONTROL, TASKCONTROL_PATH,
                                     OBJCONTROL_INTERFACE);
    }catch(string msg1) {
        cerr << "Fail to create DBUS Proxy for "
             << OBJCONTROL_INTERFACE << " interface!" << endl;
        throw msg1;
    }
}

ObjectControl::~ObjectControl()
{
    if (this->taskControl){
        g_object_unref(this->taskControl);
        this->taskControl = 0;
    }

    if (this->objControl) {
        g_object_unref(this->objControl);
        this->objControl = 0;
    }
}

void ObjectControl::shutdown(){
    GError *error = NULL;
    GVariant *result = 0;

    result = g_dbus_proxy_call_sync( this->objControl, "shutdown", g_variant_new("()"),
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (result == NULL) {
        cerr << "Shutdown fail: " << error->message <<endl;
        stringstream errmsg("EX_CallFail: ");
        errmsg << "(Shutdown) ";
        errmsg << (char*)error->message;
        throw errmsg.str();
    }
}

void ObjectControl::TaskStart(string taskName) {
    GError *error = NULL;
    GVariant *result = 0;

    GVariantBuilder* gv_Builder = g_variant_builder_new(G_VARIANT_TYPE_TUPLE);
    g_variant_builder_add(gv_Builder,"s",(gchar*)taskName.c_str());
    GVariant *gv_TaskName = g_variant_builder_end(gv_Builder);
    g_variant_builder_unref(gv_Builder);
    cout << (char*) g_variant_get_type_string(gv_TaskName) <<endl;
    result = g_dbus_proxy_call_sync( this->taskControl, "start", gv_TaskName,
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (result == NULL) {
        cerr << (char*) g_dbus_error_get_remote_error (error) << endl;
        cerr << "Start task fail: " << error->message <<endl;
        stringstream errmsg("EX_CallFail: ");
        errmsg << '('<< taskName << ") ";
        errmsg << (char*)error->message;
        throw errmsg.str();
    }
}

void ObjectControl::TaskStop(unsigned int taskID) {
    GError *error = NULL;
    GVariant *result = 0;

    GVariantBuilder* gv_Builder = g_variant_builder_new(G_VARIANT_TYPE_TUPLE);
    g_variant_builder_add(gv_Builder,"i",taskID);
    GVariant *gv_TaskID= g_variant_builder_end(gv_Builder);
    g_variant_builder_unref(gv_Builder);
    result = g_dbus_proxy_call_sync( this->taskControl, "stop", gv_TaskID,
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (result == NULL) {
        cerr << (char*) g_dbus_error_get_remote_error (error) << endl;
        cerr << "Start task fail: " << error->message <<endl;
        stringstream errmsg("EX_CallFail: ");
        errmsg << '('<< taskID << ") ";
        errmsg << (char*)error->message;
        throw errmsg.str();
    }
}

void ObjectControl::GetTaskStatus(unsigned int taskID) {
    GError *error = NULL;
    GVariant *result = 0;

    GVariant *gv_TaskID = g_variant_new("(i)", (gint) taskID);
    result = g_dbus_proxy_call_sync( this->taskControl, "getStatus", gv_TaskID,
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (result == NULL) {
        cerr << "Start task fail: " << error->message <<endl;
        stringstream errmsg("EX_CallFail: ");
        errmsg << '(' << taskID << ") ";
        errmsg << (char*)error->message;
        throw errmsg.str();
    }
}

void ObjectControl::Finalize(){

    g_dbus_connection_signal_unsubscribe (dBusm->dBusConn,
                                      this->subscID);
    dBusm->finalize();
}

void ObjectControl::SubscribeForSignals(){

GDestroyNotify NO_USER_DATA_FREE_FUNC = NULL;
const gchar* ALL_SENDERS = NULL;
const gchar* ALL_MEMBERS = NULL;
const gchar* ALL_INTERFACES = NULL;

this->subscID =
    g_dbus_connection_signal_subscribe (dBusm->dBusConn, ALL_SENDERS,
                                    ALL_INTERFACES, ALL_MEMBERS,
                                    "/org/ssdev/example/control",
                                    NULL, G_DBUS_SIGNAL_FLAGS_NONE,
                                    signalReceived, (gpointer) this,
                                    NO_USER_DATA_FREE_FUNC);
}

void signalReceived (GDBusConnection *connection, const gchar *sender_name,
                     const gchar *object_path, const gchar *interface_name,
                     const gchar *signal_name, GVariant *parameters,
                    gpointer user_data){

    stringstream msg;
    msg << "Signal" << (char*)signal_name << " received from ";
    msg << (char*)sender_name << " [" << object_path << "]" << endl;
    msg << (char*)interface_name <<endl;
    string rmsg = msg.str();
    g_message((gchar*)rmsg.c_str());
    string signal(signal_name);
    if (signal == "shutdown") ((ObjectControl*)user_data)->Finalize();
}

