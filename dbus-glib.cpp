#include <iostream>
#include <string>
#include <unistd.h>
#include <gio/gio.h>
#include <stdlib.h>
/*#ifdef G_OS_UNIX
#include <gio/gunixfdlist.h>
#endif*/

using namespace std;

#define DBUS "org.freedesktop.DBus"
#define DBUS_PATH "/org/freedesktop/DBus"
#define DBUS_INTERFACE "org.freedesktop.DBus"
#define DEFAULT_TIMEOUT -1

GMainLoop *mainLoop;

void adapterListCallback (GDBusProxy *proxy, GAsyncResult *res, gpointer user_data)
    {
        GVariant *result;
        GError *err = NULL;
        gchar *adapterPath = NULL;

        result = g_dbus_proxy_call_finish (proxy, res, &err);
        if (err) {
            cout << "Msg: " << err->message <<endl;
        }
        else {
            g_variant_get (result, "(o)", &adapterPath);

            cout << "Adapter Path is " << adapterPath << endl;
        }
        if ( g_main_loop_is_running (mainLoop)) {
            g_main_loop_quit (mainLoop);
        }
}

int FindBlueZAdapterPath ()
{
    GDBusProxy *bproxy;
    GError *error = NULL;
    GDBusProxyFlags flags = G_DBUS_PROXY_FLAGS_NONE;

    //g_type_init ();
    bproxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM, flags, NULL,
                                            "org.bluez","/",
                                            "org.bluez.Manager",
                                           NULL, &error);
    if ( bproxy == NULL ) {
        cout << "Unable to create Proxy for org.bluez!" << endl;
        return 1;
    }

    mainLoop = g_main_loop_new (NULL, FALSE);

    g_dbus_proxy_call ( bproxy, "DefaultAdapter", g_variant_new ("()"),
                        G_DBUS_CALL_FLAGS_NONE, -1, NULL,
                        (GAsyncReadyCallback) adapterListCallback, NULL);

    g_main_loop_run (mainLoop);

    cout << "Quitting From FindBlueZAdapther Path " << endl;
    return 0;
}

void listnames(void) {
    GDBusProxy *DBus;
    GError *error = NULL;
    GDBusProxyFlags busFlags = G_DBUS_PROXY_FLAGS_NONE;
    GVariant *result;
    char *id;

    DBus = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, busFlags, NULL,
                                         DBUS, DBUS_PATH, DBUS_INTERFACE,
                                         NULL, &error);
    if (DBUS == NULL) {
        cout << "Unable to create proxy for " << DBUS << endl;
        throw "EX_CreateProxyFail";
    }

    error = NULL;

    result = g_dbus_proxy_call_sync( DBus, "GetId", g_variant_new("()"),
                                    G_DBUS_CALL_FLAGS_NONE, DEFAULT_TIMEOUT,
                                    NULL, &error);
    if (error) {
        cerr << "GetID call fail: " << error->message <<endl;
        throw "EX_CallFail";
    }
    g_variant_get(result, "(s)", &id);

    cout << "ID: " << id << endl;

}
