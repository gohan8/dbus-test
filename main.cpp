#include <iostream>
#include <array>
#include <string>
#include "dbusmanager.h"
#include "objectcontrol.h"

int FindBlueZAdapterPath ();
void listnames(void);
void initClient();

using namespace std;

const int MAX_SIZE = 3;

int main(int argc, char** argv)
{
    char is_server=1;
    char test = 0;
    cout << "DBus - Example App." << endl
         << "------------------------------" << endl;

    for (int i=1; i < argc; i++){
        if (argv[i][0]=='-')
            switch(argv[i][1]) {
                case 'c': is_server=0; break;
                case 't': test = 1; break;
                default: cout << "Unknow option." << endl;
            }
    }
    if (!is_server) {
        cout << "Running in client mode." <<endl;
        initClient();
        return 0;
    }
    try {
        DBusManager busm;
        cout << "ID: " << busm.get_UID() <<endl;
        cout << "Connection name" << busm.getConnectionName() << endl;
        if (!test){
        busm.registerName("org.ssdev.example.control");
        busm.initEventLoop();
        }
        //busm.getNames();
    }
    catch (char const* msg){
        cout << "Exception " << msg << endl;
    }
    return 0;
}

void initClient(){
    ObjectControl client;
    try {
        client.TaskStart("Task 1");
        client.TaskStop(12);
    }catch(string msg){
        cout << "Exception received:" << endl << msg << endl;
    }
    client.SubscribeForSignals();
    cout << "Client loop initialized. Waiting for Signals" << endl;
    client.dBusm->initEventLoop();

}
