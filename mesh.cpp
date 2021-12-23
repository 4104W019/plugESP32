#include "mesh.h"

#define   MESH_PREFIX     "TPLink32"
#define   MESH_PASSWORD   "11111111"
#define   MESH_PORT       5555
#define HOSTNAME "HTTP_BRIDGE"

painlessMesh  mesh;
bool _WAN_=false;
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

void receivedCallback( const uint32_t &from, const String &msg );

void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() 
{
  return IPAddress(mesh.getStationIP());
}

IPAddress getAccessPointIP()
{ 
  return IPAddress(mesh.getAPIP());
}

bool wanStatus(void)
{
  return _WAN_;
}

bool checkNetworking()
{
  bool result= false;
  mesh.update();
  
  IPAddress IP_ = getlocalIP();
  if(myIP != IP_){
    myIP = IP_;
    Serial.print("My _WAN_ is  "); Serial.println(_WAN_);
    Serial.println("My IP is " + myIP.toString());
    
    if(myIP.toString()=="0.0.0.0")
    {
      _WAN_ = false;
      //timeval_wifi_reconnect = TIMEVAL_WIFI;
    }
    else{
      _WAN_=true;
      //timeval_wifi_reconnect = TIMEVAL_WIFI;
      result = true;
      //LineInit();
    }
  }
  return result;
}

void meshInit(String ssid, String ssid_passwd)
{
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

 // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.stationManual(ssid, ssid_passwd);
  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  myAPIP = getAccessPointIP();
  Serial.println("My AP IP is " + myAPIP.toString());
}
