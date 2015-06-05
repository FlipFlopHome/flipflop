/*
 * http_cfg.c
 *
 *  Created on: May 21, 2015
 *      Author: juliendarrah
 */

#include "html_cfg.h"

const uint8_t HTML_pageIndex[] = ""\
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n" \
"<html>\r\n<body>\r\n" \
"<div style=\"background-color:black; color:white; padding:20px;\">\r\n" \
"<h1 align=\"center\"><b>FlipFlop Light Switch Module</b></h1>\r\n" \
"</div>\r\n" \
"<p>The Module MAC Address: %02X-%02X-%02X-%02X-%02X-%02X" \
"</p>\r\n" \
"<form action=\"wifi_info\" method=\"POST\">\r\n" \
"SSID:<br>\r\n" \
"<input type=\"text\" name=\"ssid\" value=\"\">\r\n" \
"<br>\r\n" \
"Password:<br>\r\n" \
"<input type=\"text\" name=\"pwd\" value=\"\">\r\n" \
"<br><br>\r\n" \
"<input type=\"submit\" value=\"Submit\">\r\n" \
"</form>\r\n" \
"</body>\r\n</html>\r\n";

const uint16_t HTML_pageIndexSize = sizeof(HTML_pageIndex);

const uint8_t HTML_pageWiFiConnectionStatus[] = ""\
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n" \
"<html>\r\n<body>\r\n" \
"<h1>FlipFlop Light Switch Module<br>Connection Status</h1>\r\n" \
"<p>SSID: %s" \
"<br>\r\n" \
"Password: %s" \
"<br>\r\n" \
"Connection Status - %s" \
"<br>\r\n" \
"IP ADDRESS: %d.%d.%d.%d" \
"<br>\r\n" \
"<form action=\"restart\" method=\"POST\">\r\n" \
"<input type=\"submit\" value=\"Restart\">\r\n" \
"</form>\r\n" \
"</p>\r\n" \
"</body>\r\n</html>\r\n";

const uint16_t HTML_pageWiFiConnectionStatusSize = sizeof(HTML_pageWiFiConnectionStatus);
