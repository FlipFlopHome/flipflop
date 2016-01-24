#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__


#define FLIPFLOP_SWITCH_MODULE		1
#define FLIPFLOP_LIGHT_MODULE		0


#define DISCOVERY_UDP_PORT			(8890)

void discovery_init(void);
void discovery_ModuleFind_RecvCb(void *arg, char *pusrdata, unsigned short length);

#endif /* __DISCOVERY_H__ */
