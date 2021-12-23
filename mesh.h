#ifndef _MESH_H_
#define _MESH_H_

#include "Arduino.h"

#include "IPAddress.h"
#include "painlessMesh.h"


IPAddress getlocalIP();
IPAddress getAccessPointIP();

bool wanStatus(void);

bool  checkNetworking();
void meshInit(String ssid, String ssid_passwd);
#endif
