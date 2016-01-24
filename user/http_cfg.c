/*
 * http_cfg.c
 *
 *  Created on: May 21, 2015
 *      Author: juliendarrah
 */

#include "html_cfg.h"


// TODO: rework pages to share logo string?
char HTML_logo[] = ""\
"<img style=\"vertical-align: text-bottom; padding-right: 5px;\" src=\"data:image/png;base64," \
"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\" />";

const uint16_t HTML_logoSize = sizeof(HTML_logo);
#if(0)
char HTML_pageIndex[] = ""\
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n" \
"<html>\r\n<body>\r\n" \
"<div style=\"background-color:black; color:white; padding:20px;\">\r\n" \
"<h1 align=\"center\">%s<b>FlipFlop Light Switch Module</b></h1>\r\n" \
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
#endif

char HTML_pageWiFiConnectionStatus[] = ""\
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

char HTML_pageError[] = ""\
"HTTP/1.0 408 Request Time-out\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"
"<html><body><h1>408 Request Time-out</h1>\r\n" \
"Your browser didn't send a complete request in time.\r\n" \
"</body></html>\r\n";

const uint16_t HTML_pageErrorSize = sizeof(HTML_pageError);

/**************************************************************************/
/* Main Configuration Page - Main Template                                */
/**************************************************************************/
char HTML_pageIndex[] = ""\
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n" \
"<html>" \
"<head>" \
"<style>\r\n" \
"#header {" \
    "background-color:black;" \
    "color:white;" \
    "text-align:center;" \
    "padding:5px;" \
"}\r\n" \
"#nav {" \
    "line-height:30px;" \
    "background-color:#eeeeee;" \
    "height:1000px;" \
    "width:150px;" \
    "float:left;" \
    "padding:5px;" \
"}\r\n" \
"#section {" \
    "width:500px;" \
    "float:left;" \
    "padding:10px;" \
"}\r\n" \
"#footer {" \
    "background-color:black;" \
    "color:white;" \
    "clear:both;" \
    "text-align:center;" \
   "padding:5px;" \
"}\r\n" \
"</style>" \
"</head>" \
"<body>\r\n" \
"<div id=\"header\">\r\n" \
/* Logo Img */
"<h1>%s<b>FlipFlop Light Switch Module</b></h1>\r\n" \
"</div>\r\n" \
"<div id=\"nav\">" \
"<a href=\"/home\">Home<br></a>\r\n" \
"<a href=\"/module_setup\">Module Setup<br></a>\r\n" \
"<a href=\"/module_info\">Module Info</a><br>\r\n" \
"<a href=\"/restart\">Restart Module</a>\r\n" \
"</div>\r\n" \
/* Section Content */
"%s" \
"</div>\r\n" \
"<div id=\"footer\">" \
"Copyright © flip-flop.io" \
"</div>\r\n" \
"</body>" \
"</html>";

const uint16_t HTML_pageIndexSize = sizeof(HTML_pageIndex);

/**************************************************************************/
/* Main Configuration Page "/home                                         */
/**************************************************************************/
char HTML_sectionHome[] = ""\
"<h2>Home</h2>" \
"<p>Welcome to the FlipFlop Light Switch Module.</p>\r\n" \
"<p>Select the option on the left tab menu to configure the Module." \
"The Module can be configured as an Access Point (AP) or as a Station connected to your home router.</p>\r\n";

const uint16_t HTML_sectionHomeSize = sizeof(HTML_sectionHome);

/**************************************************************************/
/* Main Configuration Page "/module_setup                                     */
/**************************************************************************/

char HTML_sectionModuleSetup[] = ""\
"<h2>Module Setup</h2>" \
 "<p>Welcome to the FlipFlop Light Switch Module.</p>\r\n" \
 "<p>This page is used to configure the Module.</p>\r\n" \
"<hr>" \
"<h3>Module Operating Mode</h3>" \
 "<p>Select the Mode to be an Access Point (AP) or a Station</p>\r\n" \
 "<form action=\"wifi_info\" method=\"POST\">\r\n" \
   "<br>"
   "<label for=\"ap_mode_select\">AP Mode</label>\r\n" \
   "<input type=\"radio\" name=\"mode\" id=\"ap_mode\" value=\"ap_mode\" %s >\r\n" \
   "<br>"
   "<label for=\"station_mode_select\">Station Mode</label>\r\n"
   "<input type=\"radio\" name=\"mode\" id=\"station_mode\" value=\"station_mode\" %s >\r\n" \
   "<br>"\
"<hr>\r\n" \
"<h3>AP Mode</h3>" \
   "<p>IP address when in AP mode</p>\r\n" \
   "IP ADDRESS: %d.%d.%d.%d" \
"<hr>\r\n" \
"<h3>Station Mode</h3>" \
   "SSID:<br>\r\n" \
   "<input type=\"text\" name=\"ssid\" value=\"\">\r\n" \
   "<br>\r\n" \
   "Password:<br>\r\n" \
   "<input type=\"text\" name=\"pwd\" value=\"\">\r\n" \
   "<br><br>\r\n" \
"<hr>\r\n" \
"<input type=\"submit\" value=\"Submit\" style=\"float: right;\">\r\n" \
"</form>";


const uint16_t HTML_sectionModuleSetupSize = sizeof(HTML_sectionModuleSetup);

/**************************************************************************/
/* Main Configuration Page "/station_setup                                */
/**************************************************************************/

char HTML_sectionStationSetup[] = ""\
"<h2>Station Setup</h2>" \
"<form action=\"wifi_info\" method=\"POST\">\r\n" \
"SSID:<br>\r\n" \
"<input type=\"text\" name=\"ssid\" value=\"\">\r\n" \
"<br>\r\n" \
"Password:<br>\r\n" \
"<input type=\"text\" name=\"pwd\" value=\"\">\r\n" \
"<br><br>\r\n" \
"<input type=\"submit\" value=\"Submit\">\r\n" \
"</form>\r\n";

const uint16_t HTML_sectionStationSetupSize = sizeof(HTML_sectionStationSetup);

/**************************************************************************/
/* Main Configuration Page "/module_info                                  */
/**************************************************************************/

char HTML_sectionModuleInfo[] = ""\
"<h2>Module Information</h2>" \
"<p>The Module MAC Address: %02X-%02X-%02X-%02X-%02X-%02X </p>\r\n" \
"<p>Module Mode: %s" \
"<br>\r\n" \
"SSID: %s" \
"<br>\r\n" \
"Connection Status - %s" \
"<br>\r\n" \
"IP ADDRESS: %d.%d.%d.%d" \
"<br>\r\n" \
"SDK Version: %s" \
"<br>\r\n" \
"Flash Type: 0x%x" \
"<br>\r\n" \
"</p>\r\n";

const uint16_t HTML_sectionModuleInfoSize = sizeof(HTML_sectionModuleInfo);

/**************************************************************************/
/* Main Configuration Page "/ap_setup                                     */
/**************************************************************************/

char HTML_sectionRestart[] = ""\
"<h2>Restart Module</h2>" \
"<p>Please restart the module for the new configuration to take effect</p>\r\n" \
"<p>To restart press the Restart button below.</p>\r\n" \
"<br><br>\r\n" \
"<form action=\"restart\" method=\"POST\">\r\n" \
"<input type=\"submit\" name=\"apply\" class=\"button\" value=\"Apply Changes\"><br><br>\r\n" \
"</form>\r\n";

const uint16_t HTML_sectionRestartSize = sizeof(HTML_sectionRestart);

/**************************************************************************/
/* Main Configuration Page "/ap_mode_status                               */
/**************************************************************************/

char HTML_sectionApModeStatus[] = ""\
"<h2>AP Mode Configuration Status</h2>" \
"<p>The connection configuration in AP Mode %s</p>\r\n" \
"<p>You can now Restart the Module.</p>\r\n";

const uint16_t HTML_sectionApModeStatusSize = sizeof(HTML_sectionApModeStatus);

char HTML_sectionStationModeStatus[] = ""\
"<h2>Station Mode Configuration Status</h2>" \
"<p>Please restart the module for the new configuration to take effect</p>\r\n" \
"<br>" \
"<p>SSID: %s" \
"<br>\r\n" \
"Password: %s" \
"<br>\r\n" \
"Connection Status - %s" \
"<br>\r\n" \
"IP ADDRESS: %d.%d.%d.%d";

const uint16_t HTML_sectionStationModeStatusSize = sizeof(HTML_sectionStationModeStatus);
