/*
 * server.c
 *
 *  Created on: May 20, 2015
 *      Author: juliendarrah
 */

#include "server.h"
#include "html_cfg.h"
#include "user_interface.h"
#include "user_config.h"

static struct espconn serverApModeConn;
static struct espconn serverStationModeConn;

static esp_tcp serverApModeTcp;
static esp_tcp serverStationModeTcp;

static uint8_t macaddr[6];
static bool moduleWiFiCfgDoneFlag;
static uint8_t htmlString[10000];
uint8_t ssidData[32];
uint8_t pwdData[64];

static volatile os_timer_t SRVR_ConfigStationModeCheckStatusTmr;
static volatile uint16_t SRVR_ConfigStationModeCheckStatusTmrCntr;

void SRVR_ConfigApMode(void)
{
	struct softap_config ap_conf;
	uint8_t ssid_mac[32];
	uint8_t ssid[32];

	/* Set Module to be in AP Mode */
	wifi_set_opmode_current(SOFTAP_MODE);

	// setup the soft AP mode
	os_bzero(&ap_conf, sizeof(struct softap_config));
	os_bzero(&ssid_mac, sizeof(ssid_mac));
	os_bzero(&ssid, sizeof(ssid));
	/* Get Module AP Mode Basic Config */
	wifi_softap_get_config(&ap_conf);
	/* Get Module AP Mode MAC Address */
	wifi_get_macaddr(STATION_IF, macaddr);

	os_strncpy(ssid, AP_SSID, sizeof(AP_SSID));
	os_strcat(ssid, MODULE_TYPE);
	os_sprintf(ssid_mac, "%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
	os_strcat(ssid, ssid_mac);
	os_strncpy(ap_conf.ssid, ssid, sizeof(ssid));

	ap_conf.ssid_len = strlen(ap_conf.ssid);

	os_strncpy(ap_conf.password, AP_PASSWORD, sizeof(AP_PASSWORD));
	//os_snprintf(&ap_conf.password[strlen(AP_PASSWORD)], sizeof(ap_conf.password) - strlen(AP_PASSWORD), "_%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
	//os_sprintf(ap_conf.password[strlen(AP_PASSWORD)], "_%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
	ap_conf.authmode = AUTH_OPEN;
	ap_conf.channel = 6;
	/* Set Module AP Mode Configuration */
	wifi_softap_set_config_current(&ap_conf);
	//os_delay_us(10000);


}

void SRVR_ConfigStationModeCheckStatus(void *arg)
{
	struct espconn *conn;
	uint8_t connectionStatus;
	uint8_t connectionStatusString[50];
	struct ip_info info;
	uint8_t ipAddressData[4];


	conn = arg;

	connectionStatus = wifi_station_get_connect_status();
	/*
	 * ENUM
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
    */

	if( (connectionStatus != STATION_CONNECTING) || (SRVR_ConfigStationModeCheckStatusTmrCntr >= 30) )
	{
	    //Disarm timer
	    os_timer_disarm(&SRVR_ConfigStationModeCheckStatusTmr);
		SRVR_ConfigStationModeCheckStatusTmrCntr = 0;
		os_bzero(htmlString, sizeof(htmlString));
		os_bzero(ipAddressData, sizeof(ipAddressData));

		if( connectionStatus == STATION_GOT_IP )
		{
			os_sprintf(connectionStatusString, "CONNECTED!");
			wifi_get_ip_info(STATION_IF,&info);
			ipAddressData[0] = (uint8_t)((info.ip.addr) & 0xFF);
			ipAddressData[1] = (uint8_t)((info.ip.addr>>8) & 0xFF);
			ipAddressData[2] = (uint8_t)((info.ip.addr>>16) & 0xFF);
			ipAddressData[3] = (uint8_t)((info.ip.addr>>24) & 0xFF);

		}
		else if( connectionStatus == STATION_WRONG_PASSWORD )
		{
			os_sprintf(connectionStatusString, "INVALID PASSWORD");
		}
		else if( connectionStatus == STATION_NO_AP_FOUND )
		{
			os_sprintf(connectionStatusString, "SSID NOT FOUND");
		}
		else
		{
			os_sprintf(connectionStatusString, "CONNECTION TIMED OUT");
		}
		uart0_sendStr(connectionStatusString);

		os_sprintf(htmlString, HTML_pageWiFiConnectionStatus, ssidData, pwdData, connectionStatusString,
				ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3]);
		espconn_sent(conn, htmlString, strlen(htmlString));
		//espconn_disconnect(conn);
		return;
	}
	else
	{
		SRVR_ConfigStationModeCheckStatusTmrCntr++;
	}

	return;

}

uint8_t SRVR_ConfigStationMode(uint8_t *ssid, uint16_t ssidSize, uint8_t *password, uint16_t passwordSize)
{
	struct station_config station_conf;
	bool connectionStatus;
	uint16_t delayCounter;

	delayCounter = 0;
	/* Set Module to be in AP Mode and Station Mode during the config of the station parameters */
	wifi_set_opmode_current(STATIONAP_MODE);

	// connect to our station
	// Clear the data structure
	os_bzero(&station_conf, sizeof(station_conf));
	wifi_station_get_config(&station_conf);
	os_strncpy(station_conf.ssid, ssid, sizeof(station_conf.ssid));
	os_strncpy(station_conf.password, password, sizeof(station_conf.password));
	//os_strncpy(station_conf.ssid, STATION_SSID, sizeof(STATION_SSID));
	//os_strncpy(station_conf.password, STATION_PASSWORD, sizeof(STATION_PASSWORD));

	/* Disable the use of bssid */
	station_conf.bssid_set = 0;

	wifi_station_dhcpc_start();
	wifi_station_set_auto_connect(1);
	//ETS_UART_INTR_DISABLE();
	wifi_station_disconnect();
	wifi_station_set_config(&station_conf);
	//ETS_UART_INTR_ENABLE();



	moduleWiFiCfgDoneFlag = TRUE;

	connectionStatus = wifi_station_connect();
	//os_delay_us(50000);
#if(0)
	do
	{
		/* Get out of the loop after a certain amount of time if we are still trying to connect */
		if( delayCounter >= 600)
		{
			break;
		}
		connectionStatus = wifi_station_get_connect_status();
		/* Since we are waiting, release control to other tasks so we don't block the cpu */
		os_delay_us(50000);

		delayCounter++;
	} while( connectionStatus == STATION_CONNECTING );

	/*
	 * ENUM
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
    */

#endif
	return connectionStatus;

}

void SRVR_ServerApModeInit(int port)
{


	espconn_delete(&serverApModeConn);

	os_bzero(&serverApModeConn, sizeof(serverApModeConn));
	os_bzero(&serverApModeTcp, sizeof(serverApModeTcp));

	serverApModeConn.type=ESPCONN_TCP;
	serverApModeConn.state=ESPCONN_NONE;
	serverApModeTcp.local_port=port;
	serverApModeConn.proto.tcp=&serverApModeTcp;

	/* Register the Connection CallBack */
	espconn_regist_connectcb(&serverApModeConn, SRVR_ServerConnectApModeCb);
	/* Start listening to connection with Host */
	espconn_accept(&serverApModeConn);
	espconn_regist_time(&serverApModeConn, SERVER_AP_MODE_TIMEOUT, 1);
}

void SRVR_ServerStationModeInit(int port)
{
	espconn_delete(&serverStationModeConn);

	os_bzero(&serverStationModeConn, sizeof(serverStationModeConn));
	os_bzero(&serverStationModeTcp, sizeof(serverStationModeTcp));

	serverStationModeConn.type=ESPCONN_TCP;
	serverStationModeConn.state=ESPCONN_NONE;
	serverStationModeTcp.local_port=port;
	serverStationModeConn.proto.tcp=&serverStationModeTcp;

	/* Register the Connection CallBack */
	espconn_regist_connectcb(&serverStationModeConn, SRVR_ServerConnectStationModeCb);
	/* Start listening to connection with Host */
	espconn_accept(&serverStationModeConn);
	espconn_regist_time(&serverStationModeConn, SERVER_STATION_MODE_TIMEOUT, 1);


}

void SRVR_ServerConnectApModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	moduleWiFiCfgDoneFlag = FALSE;

	espconn_regist_recvcb(conn, SRVR_ServerRecvApModeCb);
	espconn_regist_reconcb(conn, SRVR_ServerReconApModeCb);
	espconn_regist_disconcb(conn, SRVR_ServerDisconApModeCb);
	espconn_regist_sentcb(conn, SRVR_ServerSentApModeCb);
	uart0_sendStr("AP Mode Connected\r\n");
}

void SRVR_ServerDisconApModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;
	uart0_sendStr("AP Mode Disconnected\r\n");
}

void SRVR_ServerReconApModeCb(void *arg, sint8 err)
{
	struct espconn *conn;

	conn = arg;
	//serverConnData *conn=serverFindConnData(arg);
	if (conn==NULL) return;
	//Yeah... No idea what to do here. ToDo: figure something out.
}

void SRVR_ServerRecvApModeCb(void *arg, char *data, unsigned short len)
{
	struct espconn *conn;
	uint8_t mac_string[20];
	uint8_t temp[40];
	uint8_t *pwdDataCopy;

	conn = arg;
	char *retSearchGET;
	char *retSearchPOST;
	static bool postInProgressFlag;

	retSearchGET = strstr(data, "GET");
	retSearchPOST = strstr(data, "POST");

	os_bzero(htmlString, sizeof(htmlString));

	/* Display Web Page */
	if( retSearchGET != NULL)
	{
		postInProgressFlag = FALSE;

		os_sprintf(htmlString, HTML_pageIndex, macaddr[0], macaddr[1], macaddr[2]
								,macaddr[3], macaddr[4], macaddr[5]);
		espconn_sent(conn, &htmlString[0], strlen(htmlString));
	}
	else if( (retSearchPOST != NULL) || (postInProgressFlag == TRUE) )
	{
		postInProgressFlag = TRUE;

		retSearchPOST = strstr(data, "pwd");
		if( retSearchPOST != NULL)
		{
			char *ssidPos;
			char *pwdPos;
			uint8_t ssidLenght;
			uint8_t pwdLenght;
			//bool connectionStatusFlag;
			uint8_t connectionStatusString[20];
			uint8_t connectionStatus;

			os_bzero(ssidData, sizeof(ssidData));
			os_bzero(pwdData, sizeof(pwdData));

			pwdPos = retSearchPOST;

			pwdLenght = strlen(pwdPos+4);
			strncpy(pwdData, pwdPos+4, pwdLenght);

			ssidPos = strstr(data, "ssid");

			ssidLenght = ((pwdPos-1) - (ssidPos+5));
			strncpy(ssidData, ssidPos+5, ssidLenght);
			//os_sprintf(temp, "SSID: %s\r\n", conn->link_cnt);
			postInProgressFlag = FALSE;

			/* Make sure there are not '+'  in the password string, if so replace with space ' ' */
			pwdDataCopy = pwdData;
			while( *pwdDataCopy != 0 )
			{
				if( *pwdDataCopy == '+' )
					*pwdDataCopy = ' ';
				pwdDataCopy++;
			}

			//os_sprintf(temp, "\r\nSSID Name: %s\r\n", ssidData);
			//uart0_sendStr(temp);
			//os_sprintf(temp, "\r\nSSID Pass: %s\r\n", pwdData);
			//uart0_sendStr(temp);
			//uart0_sendStr(data);

			/* Try to connect to the AP */
			connectionStatus = SRVR_ConfigStationMode(ssidData, ssidLenght, pwdData, pwdLenght);
			/* Start timer task to check periodically the connection status */
			/* Setup the connection timer */
		    //Disarm timer
		    os_timer_disarm(&SRVR_ConfigStationModeCheckStatusTmr);
		    //Setup timer
		    os_timer_setfn(&SRVR_ConfigStationModeCheckStatusTmr, (os_timer_func_t *)SRVR_ConfigStationModeCheckStatus, conn);
			SRVR_ConfigStationModeCheckStatusTmrCntr = 0;
		    os_timer_arm(&SRVR_ConfigStationModeCheckStatusTmr, 1000, 1);

			//os_sprintf(connectionStatusString, "Connection Status: %d", connectionStatus);
			//uart0_sendStr(connectionStatusString);
/*
			if( connectionStatusFlag == TRUE )
				os_sprintf(connectionStatusString, "Successful! :)");
			else
				os_sprintf(connectionStatusString, "Failed... :(");
				*/

			//os_sprintf(string, HTML_pageWiFiConnectionStatus, ssidData, pwdData, connectionStatusString);
			//espconn_sent(conn, string, strlen(string));
		}

		retSearchPOST = strstr(data, "restart");
		if(retSearchPOST != NULL)
		{
			espconn_disconnect(conn);
			//os_delay_us(5000);
			system_restart();
		}

#if(0)
		if(postInProgressFlag == FALSE)
		{
			/* Close Connection */
			espconn_disconnect(conn);
		}
#endif

	}

	uart0_sendStr("Data Received\r\n");
	//uart0_sendStr(&string[0]);
	//uart0_sendStr(data);
	//uart0_sendStr("\r\n");
	//os_sprintf(temp, "Link Count: %d\r\n", conn->link_cnt);
	//uart0_sendStr(temp);
	//os_sprintf(temp, "State: %d\r\n", conn->state);
	//uart0_sendStr(temp);

}

void SRVR_ServerSentApModeCb(void *arg)
{
	struct espconn *conn;
	//uint8_t local_ip_string[30];
	//uint8_t remote_ip_string[30];
	//uint8_t remote_ip_port[30];
	//uint8_t local_ip_port[30];

	conn = arg;
	if (conn==NULL) return;

#if(0)
	os_sprintf(remote_ip_string, "RemoteIP: %d.%d.%d.%d\r\n", conn->proto.tcp->remote_ip[0],
			conn->proto.tcp->remote_ip[1], conn->proto.tcp->remote_ip[2], conn->proto.tcp->remote_ip[3]);

	os_sprintf(remote_ip_port, "RemotePort: %d\r\n", conn->proto.tcp->remote_port);

	os_sprintf(local_ip_string, "LocalIP: %d.%d.%d.%d\r\n", conn->proto.tcp->local_ip[0],
			conn->proto.tcp->local_ip[1], conn->proto.tcp->local_ip[2], conn->proto.tcp->local_ip[3]);

	os_sprintf(local_ip_port, "LocalPort: %d\r\n", conn->proto.tcp->local_port);

	uart0_sendStr("Data Sent\r\n");
	uart0_sendStr(&local_ip_string[0]);
	uart0_sendStr(&local_ip_port[0]);
	uart0_sendStr(&remote_ip_string[0]);
	uart0_sendStr(&remote_ip_port[0]);
	uart0_sendStr("\r\n");
#endif

	espconn_disconnect(conn);

	if(moduleWiFiCfgDoneFlag == TRUE)
	{
		//wifi_set_opmode(STATIONAP_MODE);
		//os_delay_us(100000);
		//system_restart();
	}


}


void SRVR_ServerConnectStationModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	espconn_regist_recvcb(conn, SRVR_ServerRecvStationModeCb);
	espconn_regist_reconcb(conn, SRVR_ServerReconStationModeCb);
	espconn_regist_disconcb(conn, SRVR_ServerDisconStationModeCb);
	espconn_regist_sentcb(conn, SRVR_ServerSentStationModeCb);
	uart0_sendStr("Station Mode Connected\r\n");
}

void SRVR_ServerDisconStationModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	uart0_sendStr("Station Mode Disconnected\r\n");
}

void SRVR_ServerReconStationModeCb(void *arg, sint8 err)
{
	struct espconn *conn;

	conn = arg;
	//serverConnData *conn=serverFindConnData(arg);
	if (conn==NULL) return;
	//Yeah... No idea what to do here. ToDo: figure something out.
}

void SRVR_ServerRecvStationModeCb(void *arg, char *data, unsigned short len)
{
	struct espconn *conn;
	char dataFrameOn[9]  		= { 0x02, 0x0A, 0x03, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x00 };
	char dataFrameOff[9] 		= { 0x02, 0x0A, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
	char dataFrameGetState[8] 	= { 0x02, 0x0A, 0x02, 0x00, 0xFF, 0x00, 0x00, 0x00 };
	uint16_t i;
	conn = arg;

	//uart0_sendStr(data);
	//espconn_sent(conn, data, len);
	for(i = 0; i < 9; i++)
	{
		if(data[i] != dataFrameOff[i])
			break;
		if(i==8)
		{
			system_os_post(user_procTaskPrio, 10, 0x00);
			return;
		}
	}

	for(i = 0; i < 9; i++)
	{
		if(data[i] != dataFrameOn[i])
			break;
		if(i==8)
		{
			system_os_post(user_procTaskPrio, 10, 0x01);
			return;
		}
	}

	for(i = 0; i < 8; i++)
	{
		if(data[i] != dataFrameGetState[i])
			break;
		if(i==7)
		{
			system_os_post(user_procTaskPrio, 10, 0xFF);
			return;
		}
	}


}

void SRVR_ServerSentStationModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;
	uart0_sendStr("Station Mode - Data Sent\r\n");
}

void SRVR_ServerSendData(uint8_t switchState)
{
	char dataFrameGetStateResponse[8] 	= { 0x02, 0x0A, 0x02, 0x00, 0xFF, 0x00, 0x00, 0x00 };

	if(switchState == 0x00)
	{
		dataFrameGetStateResponse[6] = 0x00;
	}
	else
	{
		dataFrameGetStateResponse[6] = 0xFF;
	}
	/* Send response */
	espconn_sent(&serverStationModeConn, dataFrameGetStateResponse, sizeof(dataFrameGetStateResponse));

}


