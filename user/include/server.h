/*
 * server.h
 *
 *  Created on: May 20, 2015
 *      Author: juliendarrah
 */

#ifndef USER_SERVER_H_
#define USER_SERVER_H_



#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "osapi.h"


#define AP_SSID		"FlipFlop"
#define MODULE_TYPE "_100_"		/* On/Off Switch Module */
#define AP_PASSWORD	"12345"
#define STATION_SSID	"Melrose"
#define STATION_PASSWORD "I have a porsche 914"

#define AP_MODE_PORT_NUMBER				(80)
#define STATION_MODE_PORT_NUMBER		(8899)



#define SERVER_AP_MODE_TIMEOUT				60 /* Time out in seconds - AP Mode */
#define SERVER_STATION_MODE_TIMEOUT			7200 /* Time out in seconds - STATION Mode */


void SRVR_ConfigApMode(void);
void SRVR_ConfigStationModeCheckStatus(void *arg);
uint8_t SRVR_ConfigStationMode(uint8_t *ssid, uint16_t ssidSize, uint8_t *password, uint16_t passwordSize);

void SRVR_ServerApModeInit(int port);
void SRVR_ServerStationModeInit(int port);

void SRVR_ServerConnectApModeCb(void *arg);
void SRVR_ServerDisconApModeCb(void *arg);
void SRVR_ServerReconApModeCb(void *arg, sint8 err);
void SRVR_ServerRecvApModeCb(void *arg, char *data, unsigned short len);
void SRVR_ServerSentApModeCb(void *arg);

void SRVR_ServerConnectStationModeCb(void *arg);
void SRVR_ServerDisconStationModeCb(void *arg);
void SRVR_ServerReconStationModeCb(void *arg, sint8 err);
void SRVR_ServerRecvStationModeCb(void *arg, char *data, unsigned short len);
void SRVR_ServerSentStationModeCb(void *arg);

void SRVR_ServerSendData(uint8_t switchState);



#endif /* USER_SERVER_H_ */
