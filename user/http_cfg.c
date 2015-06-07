/*
 * http_cfg.c
 *
 *  Created on: May 21, 2015
 *      Author: juliendarrah
 */

#include "html_cfg.h"


// TODO: rework pages to share logo string?
//const uint8_t HTML_logo[] = "";

const uint8_t HTML_pageIndex[] = ""\
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n" \
"<html>\r\n<body>\r\n" \
"<div style=\"background-color:black; color:white; padding:20px;\">\r\n" \
"<h1 align=\"center\"><img style=\"vertical-align: text-bottom; padding-right: 5px;\" src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACgAAAAoEAYAAADcbmQuAAAPeklEQVRo3sVae3zNV7Y/jxyJV5CX0JCQBEmQiERe+5ffSfYDzeuc/VM1JnS0DFOlM8Wnd8y4Y7TVqzq0qvReo2lHr6EGNRcd8arHFNXWq16hKoxH0ogoQoKcWef07N8kv+TnnCTMzT/7c3L22Xuvtdda3/VdaxtIEAmydjO9gqfhafJzxgzSjrSzRpqX4Fpca33CpOAteIu82jiTRJAIudJ8BBfhItlhXJWdRgrxk6ZD6CV607530B6UwAbw6+OOoqdZf75r6n4kswi+dMRENJoW22uCYomZmOVr5in4Ar4gbzatcI0HjWkkkkTKDvMHeA/eI+801rnW/9b4Lb6IL8L8eXglXil/bczDX+Ov5SOm3fgUPmUNNMfjBJwgHzAOwFfwFWuQOdJ1vjLT6ebKgb/CX8kXTXm4DJdZA8zP4bl4rnzY+BNciSvlctMDXIEr5G9MlZhjLu8z9lLlKMbFcqThOwOBP9eGsLA1ykLFQUg/0s/axbyGRJNoax+LUT5I8oe/6HMezWJd+YRJVhTHJF59+luEGFOiHQ7nyB0/jkqoe4yAMYNl8aqa82ggS+Y3/nu4NIKeLtgeaHYJ2NXCnQqQr5p2kQASIFf59CDtSXvrEz6L8DF8DBS2g3QgHWSHT5V6HnE+cd5gEiyfNuc751ljzCGe5MDluFy+bJol9hH7qudwzyNxJM4aZZ6Jb+PbcMF+pDvpLjssheIzvovvwvxNBnFTehuicfR7+9KIQ2gos/LzX/RzKcbfrSCt4oLdY5T7/3frKdI5hsHIQPGnTx9FB2itfU7P0bqKEwLGklhrhLlaT3GqgG1JW2t7yyu6inMrCN/D9+QbPjN0FSd+F07CrQNMX2gVp+5bh+tgnxsGYeKNFPcMvWpfH7kaJTNZMV4a5VKAnwfFRbr/X92E4pz/r603z8pSeOU3IZnv0MLcxR03N1KcEPAWvmUNNS3VVZxbQFVgHcWp67kVqKs4cQHuC9EqTpzDZYELjL0NamxwLyRtoSvzr7Z/gLJBwDsn2rgEbuMWPMaD4oSCwh+iuHq/c62Twfryb2b31HUpt6vrKU797HY5PcWpFi5cWE9x4qJESNAoTp3n/L/DVGVwfZEIQdW9MJrIevL3XlvewOKE4mrcn7tpXLW6nuLSGVaCamJQDovn29aOR0PAgnv/kO/6vrN7nsE9xsMoMcTvViyVe9D8nNnt+qsWJxTXjXQD0MhwxcITIJBwVbA4sNgvVMWlkBSYP1LIoVqaGIXi3PPU34l1xLpiH7GviNXiXCKkwHz5gGm0gaSRNCcaSa/TFfkLAreiJGZVfG4c9QgOejFuHnXw6Oe+FxaAXmJGHpe7roElNxUCGOvGF+bh1oKDNJ9Oyv84+HcoBbKCt6duRZQlKZa1S8GjEvnB3ZPhQtP53b8WwgVH86OvzkSbmNkem5jfyFU9gJwre1hnXGQQMA6uNIjPnzSlxeCQzLL59TszMsdQnvuCb7zqipHEnlVtCXXF0pAyuYEiO9dz7SfZYL5y8bMtBQf0PL3Ph417Bw1ikmKonOm1HBmMgudsgLE/v7tpuWSg621vRZWoFqcDcvgyvixXmKmBJJEkZ/6DMEtU/Da28QIcspSwewtQf1B46Qe5aB3zse+iU6QJNLegTccY1cQ1G6IRYAFLZ50FF5cV080LrnX71ouRElhG3b6bzQUHpLAoPm/GKlBANoScslaDXDysE1KVh37KuvDk3Ff0sgNcjavBc/caROIIC6Tw8ksBuuDAWYLiezwNzWD97BcTzuq6lDtv00NVdJ1+Zg8N+zUsO4Dv3LisngVLSseKod6CA1pD79jPpA+G31PFfP9/HjnIZTDCb97bi/JZEJ+Tt0IrB/EhPvJW4zlD1jTy56HTfRzAJKxK0INjjcAhkyXw6n1vS2fpG/lj/U/qBlWR8EaRKECn/upn9/dacMiqc6rGJxIOGsNPLZ4LMUviFY60zMs0JeeQ7zBP4ACWIvN/bI9rNsg5xx6a+X01IaUhyIEiKzdJYXStbUz3f7m2hVjAYOYbpBq6Pm9/O7kROFCWrnT67gwwh502OSRUL6iircxin5f0ASTa6fzywp0oi6XxL4sdKJGl8lt/oxDb0njp/D+gD6nDPiUjGt/H9wHdFAEOWRtIKHnNNwjAayCvXO+H7tJeBebArXrMAX1Gy+3jY0shFFA472hdkBvAMvjnnwagSewJ/uroiWAIVsX/fF0DxbldXSUGDwO5TBbJq5YdV/PAJXiJPMYYb7CeIxVDJWMxHCiTl9emg0VgJawuGD0FaBqZflH9gVvz0iK6LL804E0UCZbz+QeZcEPZcOBAXXCo7yJhrgQ6TTHspeh15mtfkNVJhABpNf2kYHrAjcwz9Oe5s/zX6DEHuKA4fnIW0gWHJIZ5VfXTqISusAUHZgpQQnNZd951ZGYDi9NRnI4cmN+9NwDtoV/ZrvUY4kxzwNN+aRDkGDZGSpeSjagXWMzlovvaGIdO0C32mL5xKBW+rzj5SZMb+jd0iQaK01q4M1/s5SiCMZmXftRXWkc3588JKvfEHNAw2P/YtmO64EDh+7p9p7QJsBQMLmgPfNMVKi7/9S9gKJm85uqDFsiRxA+/1Jv4E3+I0UkGZ1UELGsFCBLDT/x5oNSd7rUdiv0PVXHvQ550OumKK025VeH30A2jNbElzEPeGFrPVYawDCWodHe2TAj50DJNjzmgwTCvXXmgqrgUiFG3H/wDFbA+/Ps/nUe/h1BRHDvDE8hZ21PDsMMWGT3LAniabSJ4RhKvPNHRoxwyZBM7Nr7vSmMKjF0NoqyEvqI7bOf6/164KrpPi+2WPi9CLJP59fINalCtH2Q7a26qpmGQFkFbDeJRDYO8Ot8JAgNZLL/zoFyPOUiYhueN7VALHkDBcnf8CDo3l6FC1pbPzjnvkTl4ADnpADXkd2g3Ho4Twz9elaMrRypLVTqfXeIsc1njTJUGtR7ndhFJpp/bIruFgkUM4dUlB7zKq7SuGu4FB66Hni5LHgyu/KBuIl5D2lhDTAe1ZSV0kH5pLwyvc5XHfrj1LrJD4twDJeoxB9yTTMpu57NS+jXdULC+u68UQCtsfwo24XJit+b61OpxYOtbZPvQ0Zad4HGR/OT7JxvJ4aSm7av64RJcAhfxnkEUMusF6YF810dfehFU9WNctQfFiXX9GocA+Qfadnj/tlO1ZSXJn9baDsX9FhLnIP76yNe1MQ61oxd5WPz7yAYgs3f5R868kl8vO9dIjsEQI+9ceAE+xvJvX5mSWUBn560JKNQyoCyFbKNFvuXggYP5vW2vqnKksqG8quoH0ol0kicbcwzOgzorwGqaMBQy+y9feKfF4FDfVfUU569RYD0Ll34LjGZ2wBFtWUl+i0wZ9nfLbiGoNJa+azNFzIH8NZnf+/iUM/FV2jtyHoqqTcmRDOlau9IJkDC34VkZY7QMCEJZme1cxA2Yn8lv3t4I6YykhJwb4DK47829DE4XcZXO3cxB4nRTwfbum53BWWl7P6XV4PCwfKsp5rAWGMbocINeWQnNYWE87WdTQRCsdKu69NCLah7IyfxWTQLIHcufH3FMy4AgcR/Ez73mAwpH/MLml/E6vM4ZSw2i56ANqnDAeMWyrazV4FDf0rwpj52gl2zX48u1ZSVpN51f4N/tDx6ZQ2tBLgny2i73liHCgu3HR76sVnkCAGT7h/0MPcd6A6evhPQ/VF5vXPKjBR4x7dYGVTSWdebs50+2Ghwe1gJoqjw2m/lyij7XggNaQp+y/SbI6DVzaC3IAVgpvWvHo+fBtf3zigVGSLn05YKf+k8nISRErjCPMZAYEuPscmnRSCqkkwtiu2TCjchKyO2zrQUHr8tj6ZCXPftkl0YMKIE+XfBe6PBmMIdHA3KQ/yq+t4vQBNaBJ2e0V7ODQBIoHzX+wvBjZdU4QK/ngEawXnz1iv2tBQevy0pvAYoOHBWnXqjI00LoKduLEYNbzYBaKocM2cnaBTuEp+Jr+Jq81xRrUPuqOj0H4MT3eYa0vdXg4G1ZKR846/qxU7XMAY2ht+23Y8oeOQPyVo43WShvk75H7T/fwXesAeanDepN6/Qc8DAyOmuSeStQnVRecvQ3LQYHb8tKIwEFf/VMsZY5oA201r4wvfCRMyBPcmQ7ix+ln2b3IztwKaCy6NbZsE3OMNzzurEOrhwNlrGjxeDQVAW4qbLSfwKnHVnYSdtYB/TrzmU24ZEzIE9yDIREuuSN9BY31gU1sm4he4ZN9r0GVEbijrP2FvdO6gvYVFmpgIXz+cNmaBvrkBX05kVPXX1cDKiRHGkAHpGOm+gyvWGPG3Sh2Y11vbISGsWe4KvGjW51Y72+K9ZH1QP0O3tQzHVtYx29xrrwxPFXHhcDaiTHELC8A/snNbux7qkhnWUhm9lC33eBIybz6sMFrW6sC0ESnRz1Wp+sOWQ5C24Trm2sw7RUXvzCmsfGgLRyTHBmH8+kNrux7m1DGv0X5EXPZ46AvC2b365b1OKeg/hcyBL54aJwvcY6xODe/OPpSx8bA/pXDyhTCazgmUvpr3IPtd3ndWO9pa+VAJ1j+ZG5lzyCgx5zcCaqbW/uRDNZR/unUaP0GutIARD55bQzj40Bic/x4Fln551sdmO9pa+VsiRSRhf6fAjBP4q/8fZuV6O6j+OPHnsOGQwppos3UQfWiXO50tOrK7CMVL5/0tzHxoCGOJ+k1O6SutKdtr69C5rdWG/tayWVQ09ngXZHehHKY/356sUGOGA/XrM2B2G42R2LUtEiZuHDRshSCU3IS/I76+2rKzQD0HnSiG2PjQFROG9R0VktA/K6sd7q10peNtZb+uoKHaRX7JYBTz1yBpTIrLy6pjP6I1z8h72rvZVDbayLp6ytfq3UzMZ6c19dWQ+QKUMX+4SD68u8rnzjI2NAWZCulC+c3Gw5RGNdNEd0LcDL10r/rie5IHg0XzX/zVYzoCyWrgSc/4t0g47LV7pcbK4camPdGQNdj6f1FKcpK/1/P8mVMF1YUN7Z+cIgkZ861bHZDMjKJCXsRnv0Ca2xfzr4i5bK0aix7gkcPAXVf/eTXMlGlxUcCXE2fZL4+eU7AaUz+KXqgbqKG8rilTYHL6PPmI99VtzxVsuhNtbdz/UFORYcT1AVkXGLxBHPxrOd+Y+AcYFGIqiK2CBMXNyU2FA0pPFxfNwabLou2oOiy+VqcjnMEaLnIErnogKMx+KxzkKmqMeJshK6St+wnfFbBAl3T56THo2GQ+I92z4KRTM/fiDhdPb/kkKcYv7dI5MjBadkXjP83z8Bp/geoL9YovkAAAAASUVORK5CYII=\" /><b>FlipFlop Light Switch Module</b></h1>\r\n" \
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
