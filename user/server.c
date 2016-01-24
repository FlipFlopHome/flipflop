/*
 * server.c
 *
 *  Created on: May 20, 2015
 *      Author: juliendarrah
 */

#include "server.h"
#include "html_cfg.h"
#include "user_config.h"
#include "gpio.h"
#include "pwm.h"
#include "eagle_soc.h"
#include "os_type.h"



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
	wifi_softap_set_config(&ap_conf);
	//os_delay_us(10000);


}

void SRVR_ConfigStationModeCheckStatus(void *arg)
{
	struct espconn *conn;
	uint8_t connectionStatus;
	uint8_t connectionStatusString[50];
	struct ip_info info;
	uint8_t ipAddressData[4];
	sint8 status;
	char sectionTmp[1000];


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
	uart0_sendStr("Connecting...\r\n");

	if( (connectionStatus != STATION_CONNECTING) || (SRVR_ConfigStationModeCheckStatusTmrCntr >= 120) )
	{
	    //Disarm timer
	    os_timer_disarm(&SRVR_ConfigStationModeCheckStatusTmr);
		SRVR_ConfigStationModeCheckStatusTmrCntr = 0;
		os_bzero(htmlString, sizeof(htmlString));
		os_bzero(ipAddressData, sizeof(ipAddressData));

		uart0_sendStr("Checking Connection Status...\r\n");

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
		uart0_sendStr("\r\n");
		os_sprintf(sectionTmp, HTML_sectionStationModeStatus, ssidData, pwdData, connectionStatusString,
				ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3]);

		os_sprintf(htmlString, HTML_pageIndex, HTML_logo, sectionTmp);

		status = espconn_sent(conn, htmlString, strlen(htmlString));
		if(status != 0)
			uart0_sendStr("Error Sending Data\r\n");
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
	//wifi_set_opmode_current(STATIONAP_MODE);
	wifi_set_opmode(STATIONAP_MODE);
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

void SRVR_ServerApModeInit(int port, espconn_connect_callback callbackFunction)
{


	espconn_delete(&serverApModeConn);
	espconn_delete(&serverStationModeConn);

	os_bzero(&serverApModeConn, sizeof(serverApModeConn));
	os_bzero(&serverApModeTcp, sizeof(serverApModeTcp));

	serverApModeConn.type=ESPCONN_TCP;
	serverApModeConn.state=ESPCONN_NONE;
	serverApModeTcp.local_port=port;
	serverApModeConn.proto.tcp=&serverApModeTcp;

	/* Register the Connection CallBack */
	espconn_regist_connectcb(&serverApModeConn, callbackFunction);
	/* Start listening to connection with Host */
	espconn_accept(&serverApModeConn);
	espconn_regist_time(&serverApModeConn, SERVER_AP_MODE_TIMEOUT, 0);
}

void SRVR_ServerStationModeInit(int port, espconn_connect_callback callbackFunction)
{
	espconn_delete(&serverStationModeConn);
	espconn_delete(&serverApModeConn);

	os_bzero(&serverStationModeConn, sizeof(serverStationModeConn));
	os_bzero(&serverStationModeTcp, sizeof(serverStationModeTcp));

	serverStationModeConn.type=ESPCONN_TCP;
	serverStationModeConn.state=ESPCONN_NONE;
	serverStationModeTcp.local_port=port;
	serverStationModeConn.proto.tcp=&serverStationModeTcp;

	/* Register the Connection CallBack */
	espconn_regist_connectcb(&serverStationModeConn, callbackFunction);
	/* Start listening to connection with Host */
	espconn_accept(&serverStationModeConn);
	espconn_regist_time(&serverStationModeConn, SERVER_STATION_MODE_TIMEOUT, 0);


}

void SRVR_ServerConnectApModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	if( conn == NULL )
		return;

	moduleWiFiCfgDoneFlag = FALSE;

	espconn_regist_recvcb(conn, SRVR_ServerRecvApModeCb);
	espconn_regist_reconcb(conn, SRVR_ServerReconApModeCb);
	espconn_regist_disconcb(conn, SRVR_ServerDisconApModeCb);
	espconn_regist_sentcb(conn, SRVR_ServerSentApModeCb);
	uart0_sendStr("AP Mode Connected\r\n");
}

void SRVR_ServerConnectNormalModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	if( conn == NULL )
		return;

	espconn_regist_recvcb(conn, SRVR_ServerRecvNormalModeCb);
	espconn_regist_reconcb(conn, SRVR_ServerReconNormalModeCb);
	espconn_regist_disconcb(conn, SRVR_ServerDisconNormalModeCb);
	espconn_regist_sentcb(conn, SRVR_ServerSentNormalModeCb);
	uart0_sendStr("Running Mode Connected\r\n");

}

void SRVR_ServerDisconApModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	if( conn == NULL )
		return;

	uart0_sendStr("Disconnected\r\n");
}

void SRVR_ServerReconApModeCb(void *arg, sint8 err)
{
	struct espconn *conn;

	conn = arg;

	if( conn == NULL )
		return;

	uart0_sendStr("Reconnected\r\n");

	switch (err)
	{

	case 0:
		uart0_sendStr("ESPCONN_OK - No error, everything OK.\r\n");
		break;
	case -1:
		uart0_sendStr("ESPCONN_MEM - Out of memory error.\r\n");
		break;
	case -3:
		uart0_sendStr("ESPCONN_TIMEOUT - Timeout.\r\n");
		break;
	case -4:
		uart0_sendStr("ESPCONN_RTE - Routing problem.\r\n");
		break;
	case -5:
		uart0_sendStr("ESPCONN_INPROGRESS - Operation in progress\r\n");
		break;
	case -8:
		uart0_sendStr("ESPCONN_ABRT - Connection aborted.\r\n");
		break;
	case -9:
		uart0_sendStr("ESPCONN_RST - Connection reset.\r\n");
		break;
	case -10:
		uart0_sendStr("ESPCONN_CLSD - Connection closed.\r\n");
		break;
	case -11:
		uart0_sendStr("ESPCONN_CONN - Not connected.\r\n");
		break;
	case -12:
		uart0_sendStr("ESPCONN_ARG - Illegal argument.\r\n");
		break;
	case -15:
		uart0_sendStr("ESPCONN_ISCONN -Already connected.\r\n");
		break;
	default:
		break;
	}


#if(0)
	if( conn->sent_callback == NULL )
		uart0_sendStr("Recon Send callback Invalid\r\n");
	else
		uart0_sendStr("Recon Send callback Valid\r\n");

	uart0_sendStr(&htmlString);
	espconn_sent(conn, htmlString, strlen(htmlString));

	os_bzero(htmlString, sizeof(htmlString));

	os_sprintf(htmlString, HTML_pageError);
	espconn_sent(conn, &htmlString[0], strlen(htmlString));
#endif
	//serverConnData *conn=serverFindConnData(arg);
	//if (conn==NULL) return;
	//Yeah... No idea what to do here. ToDo: figure something out.
}

void SRVR_ServerRecvApModeCb(void *arg, char *data, unsigned short len)
{
	struct espconn *conn;
	uint8_t mac_string[20];
	uint8_t temp[40];
	uint8_t *pwdDataCopy;
	char sectionTmp[1500];

	char *retSearchGET;
	char *retSearchPOST;
	static bool postInProgressFlag;

	conn = arg;

	if( conn == NULL )
		return;

	retSearchGET =(char*)os_strstr(data, "GET");
	retSearchPOST = (char*)os_strstr(data, "POST");

	os_bzero(htmlString, sizeof(htmlString));
	os_bzero(sectionTmp, sizeof(sectionTmp));

	/* Display Web Page */
	if( retSearchGET != NULL)
	{

		char pathData[20];
		char* pathStart;
		char* pathEnd;
		uint16_t pathLenght;

		postInProgressFlag = FALSE;

		os_bzero(pathData, sizeof(pathData));


		pathStart = (char*)os_strstr(data, "/");
		pathEnd = (char*)os_strstr(pathStart, " ");
		if(pathEnd != NULL)
		{
			pathLenght = (pathEnd - pathStart);
			os_strncpy(pathData, pathStart, pathLenght);
			//os_sprintf(pathData, "%s%s", pathData, "\0");
		}
		else
		{
			os_bzero(pathData, sizeof(pathData));
		}

		uart0_sendStr("PATH: ");
		uart0_sendStr(pathData);
		uart0_sendStr("\r\n");
		//os_sprintf(htmlString, HTML_pageIndex, HTML_logo, macaddr[0], macaddr[1], macaddr[2]
		//						,macaddr[3], macaddr[4], macaddr[5]);

		if( !os_strcmp(pathData, "/module_setup") )
		{
			uart0_sendStr("IN...\r\n");
			struct ip_info info;
			uint8_t ipAddressData[4];
			uint8_t moduleOperatingMode;

			wifi_get_ip_info(SOFTAP_IF,&info);
			ipAddressData[0] = (uint8_t)((info.ip.addr) & 0xFF);
			ipAddressData[1] = (uint8_t)((info.ip.addr>>8) & 0xFF);
			ipAddressData[2] = (uint8_t)((info.ip.addr>>16) & 0xFF);
			ipAddressData[3] = (uint8_t)((info.ip.addr>>24) & 0xFF);

			//os_sprintf(htmlString, HTML_pageIndex, /*HTML_logo*/"", HTML_sectionModuleSetup);
			//os_sprintf(htmlString, htmlString, ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3]);

			uart0_sendStr("BETWEEN...\r\n");
			moduleOperatingMode = wifi_get_opmode_default();

			if( moduleOperatingMode == SOFTAP_MODE )
			{
			    //AP mode for normal running operation
				os_sprintf(sectionTmp, HTML_sectionModuleSetup, "checked=\"checked\"", "",
						ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3]);
				uart0_sendStr("AP MODE\r\n");
			}
			else
			{
			    //Station mode for normal running operation
				os_sprintf(sectionTmp, HTML_sectionModuleSetup, "", "checked=\"checked\"",
						ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3]);
				uart0_sendStr("STATION MODE\r\n");
			}
			uart0_sendStr("OUT...\r\n");

		}
		else if( !os_strcmp(pathData, "/module_info") )
		{
			struct ip_info info;
			uint8_t ipAddressData[4];
			uint8_t moduleOperatingMode;
			char moduleOperationString[15];

			os_bzero(moduleOperationString, sizeof(moduleOperationString));

			moduleOperatingMode = wifi_get_opmode_default();
			if( moduleOperatingMode == SOFTAP_MODE )
			{
			    //AP mode for normal running operation
				os_sprintf(moduleOperationString, "%s", "AP Mode");
				wifi_get_ip_info(SOFTAP_IF,&info);
			}
			else
			{
			    //Station mode for normal running operation
				os_sprintf(moduleOperationString, "%s", "Station Mode");
				wifi_get_ip_info(STATION_IF,&info);
			}


			ipAddressData[0] = (uint8_t)((info.ip.addr) & 0xFF);
			ipAddressData[1] = (uint8_t)((info.ip.addr>>8) & 0xFF);
			ipAddressData[2] = (uint8_t)((info.ip.addr>>16) & 0xFF);
			ipAddressData[3] = (uint8_t)((info.ip.addr>>24) & 0xFF);

			//os_sprintf(htmlString, HTML_pageIndex, HTML_logo, HTML_sectionModuleInfo);
			os_sprintf(sectionTmp, HTML_sectionModuleInfo, macaddr[0], macaddr[1],
					macaddr[2],macaddr[3], macaddr[4], macaddr[5], moduleOperationString, "ToDo", "ToDo",
					ipAddressData[0], ipAddressData[1], ipAddressData[2], ipAddressData[3], system_get_sdk_version(), spi_flash_get_id());
		}
		else if( !os_strcmp(pathData, "/restart") )
		{
			os_sprintf(sectionTmp, HTML_sectionRestart);
		}
		else
		{
			//uint8_t moduleMode;

			//moduleMode = wifi_get_opmode();

			os_sprintf(sectionTmp, HTML_sectionHome);

		}

		{
			char temp[25];
			os_sprintf(temp, "html size: %d\r\n\0", strlen(htmlString));
			uart0_sendStr(temp);

		}
		os_sprintf(htmlString, HTML_pageIndex, HTML_logo, sectionTmp);

		espconn_sent(conn, &htmlString[0], strlen(htmlString));
	}
	/* POST Request */
	else if( (retSearchPOST != NULL) || (postInProgressFlag == TRUE) )
	{

		postInProgressFlag = TRUE;


		if(strstr(data, "mode=ap_mode") != NULL)
		{
			bool status;
			postInProgressFlag = FALSE;
			/*Set Module in AP mode for Normal Operation */
			status = wifi_set_opmode(SOFTAP_MODE);
			SRVR_ConfigApMode();

			if(status == TRUE)
			{
				os_sprintf(sectionTmp, HTML_sectionApModeStatus, "Succeeded...");
			}
			else
				{
				os_sprintf(sectionTmp, HTML_sectionApModeStatus, "Failed!");
				}

			os_sprintf(htmlString, HTML_pageIndex, HTML_logo, sectionTmp);
			espconn_sent(conn, &htmlString[0], strlen(htmlString));

		}
		/* Station Mode */
		else if( strstr(data, "pwd") != NULL)
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

			retSearchPOST = strstr(data, "pwd");
			pwdPos = retSearchPOST;

			pwdLenght = os_strlen(pwdPos+4);
			strncpy(pwdData, pwdPos+4, pwdLenght);

			ssidPos = (char*)os_strstr(data, "ssid");

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

			/* Try to connect to the Network */
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

		retSearchPOST = (char*)os_strstr(data, "restart");
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
	uart0_sendStr(data);
	uart0_sendStr("\r\n\r\n\r\n");
	//os_sprintf(temp, "Link Count: %d\r\n", conn->link_cnt);
	//uart0_sendStr(temp);
	//os_sprintf(temp, "State: %d\r\n", conn->state);
	//uart0_sendStr(temp);
	//uart0_sendStr(htmlString);
	//uart0_sendStr("\r\n");
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
	uart0_sendStr("Data Sent\r\n");

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

void SRVR_ServerDisconNormalModeCb(void *arg)
{
	SRVR_ServerDisconApModeCb(arg);
}
void SRVR_ServerReconNormalModeCb(void *arg, sint8 err)
{
	SRVR_ServerReconApModeCb(arg, err);
}
void SRVR_ServerRecvNormalModeCb(void *arg, char *data, unsigned short len)
{
	struct espconn *conn;
	char* retSearch;
	char responsePacket[1000];
	char pathData[30];
	char *pathStart;
	char *pathEnd;
	uint16_t pathLenght;

	conn = arg;

	if( conn == NULL )
		return;

	//uart0_sendStr(data);
	os_bzero(responsePacket, sizeof(responsePacket));

	retSearch = (char*)os_strstr(data, "GET");

	if( retSearch != NULL )
	{
		os_bzero(pathData, sizeof(pathData));

		pathStart = (char*)os_strstr(data, "/");
		pathEnd = (char*)os_strstr(pathStart, " ");
		if(pathEnd != NULL)
		{
			pathLenght = (pathEnd - pathStart);
			os_strncpy(pathData, pathStart, pathLenght);
			//os_sprintf(pathData, "%s%s", pathData, "\0");
		}
		else
		{
			os_bzero(pathData, sizeof(pathData));
		}

        // Respond with the path that was accessed.
        // First send the success response code.
        os_sprintf(responsePacket, "%s\r\n", "HTTP/1.1 200 OK");
        // Then send a few headers to identify the type of data returned and that
        // the connection will not be held open.
        os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "Content-Type: text/plain");
        os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "Connection: close");
        os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "Server: FlipFlop Switch Module");
        // Send an empty line to signal start of body.
        os_sprintf(responsePacket, "%s\r\n", responsePacket);


        if( !os_strcmp(pathData, "/tmp") )
		{
	    	int16_t tempResult;
	    	int16_t Tinteger;
	    	int16_t Tfraction;

	    	tempResult = (int16_t)(MAX_DS18B20_GetTemp(14));
	    	Tinteger = tempResult >> 4;
	    	Tfraction = (uint16_t)(tempResult & 0x000F);

	    	/* round the value if necessary */
	    	if( Tfraction > (0x000F>>0x01) )
	    	{
	    		Tinteger = Tinteger + 1;
	    	}

			os_sprintf(responsePacket, "%s%s%d\r\n", responsePacket, "{\"tmp\": ", Tinteger);
		}
        else if( !os_strcmp(pathData, "/tmp_float") )
		{
	    	int16_t tempResult;
	    	int16_t Tinteger;
	    	int16_t Tfraction;

	    	tempResult = (int16_t)(MAX_DS18B20_GetTemp(14));
	    	Tinteger = tempResult >> 4;
	    	Tfraction = (uint16_t)((tempResult & 0x000F) * 625);

			os_sprintf(responsePacket, "%s%s%d.%04d%s\r\n", responsePacket, "{\"tmp\": ", Tinteger, Tfraction, " C}");
		}
		else if( !os_strcmp(pathData, "/led/0") )
		{
			GPIO_OUTPUT_SET(5, 0);
			os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"led\": 0}");
		}
		else if( !os_strcmp(pathData, "/led/1") )
		{
			GPIO_OUTPUT_SET(5, 1);
			os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"led\": 1}");
		}
		else if( !os_strcmp(pathData, "/led") )
		{
            // read LED state/return as JSON
            if( GPIO_INPUT_GET(5) == 0x01 )
            {
            	os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"led\": 1}");
            }
            else
            {
            	os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"led\": 0}");
            }
		}
		else if( !os_strcmp(pathData, "/adc") )
		{
	    	uint16_t adcData;
	    	char adcResultString[10];

	    	os_bzero(adcResultString, sizeof(adcResultString));

	    	adcData = system_adc_read();

	    	os_sprintf(responsePacket, "%s%s%4d%s\r\n", responsePacket, "{\"adc\": ", adcData, " }");


		}
		else if( !os_strcmp(pathData, "/relay") )
		{

            // read LED state/return as JSON
            if( GPIO_INPUT_GET(12) == 0x01 )
            {
            	os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"relay\": 1}");
            }
            else
            {
            	os_sprintf(responsePacket, "%s%s\r\n", responsePacket, "{\"relay\": 0}");
            }


		}
#if(0)
		else if( !os_strcmp(pathData, "/pwm") )
        {
        	uint32_t duty;
            duty = pwm_get_duty(0);

            os_sprintf(responsePacket, "%s%s%d%s\r\n", responsePacket, "{\"pwm\": ", duty, " }");

        }
        else if( os_strcmp(pathData, "/pwm/") > 0 )
        {
        	uint32_t duty;
        	char dutyString[10];
        	char* startPtr;
        	char* endPtr;

        	os_bzero(dutyString, sizeof(dutyString));

        	startPtr = (char*)os_strstr(data, "/pwm");
        	endPtr = (char*)os_strstr(startPtr, " ");

        	os_strncpy(dutyString, (startPtr+5), (endPtr) - (startPtr+5) );


            duty = atoi(dutyString);
            pwm_set_duty(duty, 0);
            pwm_start();
            duty = pwm_get_duty(0);

            os_sprintf(responsePacket, "%s%s%d%s\r\n", responsePacket, "{\"pwm\": ", duty, " }");

        }
#endif
		else
		{
			os_sprintf(responsePacket, "%s%s%s\r\n", responsePacket, "Invalid Path...", pathData);
		}

	}
	else
	{
        // Unsupported action, respond with an HTTP 405 method not allowed error.
        os_sprintf(responsePacket, "HTTP/1.1 405 Method Not Allowed\r\n");
	}

	//uart0_sendStr(responsePacket);
	espconn_sent(conn, responsePacket, os_strlen(responsePacket));

	//espconn_disconnect(conn);

}
void SRVR_ServerSentNormalModeCb(void *arg)
{
	struct espconn *conn;

	conn = arg;

	if (conn==NULL) return;

    // Wait a short period to make sure the response had time to send before
    // the connection is closed
	os_delay_us(100);

	espconn_disconnect(conn);
}





