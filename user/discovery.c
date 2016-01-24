
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "espconn.h"
#include "discovery.h"

const char *discovery_find_request = "FlipFLop are you?";
#if FLIPFLOP_SWITCH_MODULE
const char *discovery_find_response_ok = "I'm a FlipFLop Switch";
#elif FLIPFLOP_LIGHT_MODULE
const char *discovery_find_response_ok = "I'm a FlipFLop Light";
#else
const char *discovery_find_response_ok = "I don't know who I am...";
#endif

static struct espconn discovery_Conn;
static esp_udp discovery_UdpConn;

/*---------------------------------------------------------------------------*/


void discovery_ModuleFind_RecvCb(void *arg, char *pusrdata, unsigned short length)
{
    char DeviceBuffer[40] = {0};
    char Device_mac_buffer[60] = {0};
    char hwaddr[6];

    struct ip_info ipconfig;

    if (wifi_get_opmode() != STATION_MODE) {
        wifi_get_ip_info(SOFTAP_IF, &ipconfig);
        wifi_get_macaddr(SOFTAP_IF, hwaddr);

        if (!ip_addr_netcmp((struct ip_addr *)discovery_Conn.proto.udp->remote_ip, &ipconfig.ip, &ipconfig.netmask)) {
            wifi_get_ip_info(STATION_IF, &ipconfig);
            wifi_get_macaddr(STATION_IF, hwaddr);
        }
    } else {
        wifi_get_ip_info(STATION_IF, &ipconfig);
        wifi_get_macaddr(STATION_IF, hwaddr);
    }

    if (pusrdata == NULL) {
        return;
    }

    if (length == os_strlen(discovery_find_request) &&
            os_strncmp(pusrdata, discovery_find_request, os_strlen(discovery_find_request)) == 0) {
        os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, discovery_find_response_ok,
                   MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

        os_printf("%s\n", DeviceBuffer);
        length = os_strlen(DeviceBuffer);
        espconn_sent(&discovery_Conn, DeviceBuffer, length);
    } else if (length == (os_strlen(discovery_find_request) + 18)) {
        os_sprintf(Device_mac_buffer, "%s " MACSTR , discovery_find_request, MAC2STR(hwaddr));
        os_printf("%s", Device_mac_buffer);

        if (os_strncmp(Device_mac_buffer, pusrdata, os_strlen(discovery_find_request) + 18) == 0) {
            //os_printf("%s\n", Device_mac_buffer);
            length = os_strlen(DeviceBuffer);
            os_sprintf(DeviceBuffer, "%s" MACSTR " " IPSTR, discovery_find_response_ok,
                       MAC2STR(hwaddr), IP2STR(&ipconfig.ip));

            os_printf("%s\n", DeviceBuffer);
            length = os_strlen(DeviceBuffer);
            espconn_sent(&discovery_Conn, DeviceBuffer, length);
        } else {
            return;
        }
    }
}


void discovery_init(void)
{
	os_bzero(&discovery_Conn, sizeof(discovery_Conn));
	os_bzero(&discovery_UdpConn, sizeof(discovery_UdpConn));

	discovery_Conn.type = ESPCONN_UDP;
	discovery_Conn.proto.udp = &discovery_UdpConn;
	discovery_Conn.proto.udp->local_port = DISCOVERY_UDP_PORT;
    espconn_regist_recvcb(&discovery_Conn, discovery_ModuleFind_RecvCb);
    espconn_create(&discovery_Conn);
}
