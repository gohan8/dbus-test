# dbus-test
Sample code for connection to gdbus and signal management

TaskManager - 

  Ficticious service to manage task identified by a name and a ID
  It (should) register a task and latter start and stop it in response to notifications via Dbus.

BusManager -

  Its the main engine for handle a dbus connection
  Manager the connection to dbus. Register a name for the connection, handle all steps need to register 
  and unregister an object and can list all names associated to connections in dbus. 
  It also handle the proxy needed for sending messages via dbus and  handle the main loop
  needed by a server to respond to messages send via dbus.
  
ObjectControl - 

  It uses BusManager to connect to dbus in client mode. Allows app to answers to dbus signals.

main.cpp -

  Init the app in client or server mode. Client mode responds to signals.
  Server resgister the TaskManager obj and responds to dbus messages.


  
