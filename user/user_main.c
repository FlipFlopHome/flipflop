#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"

#include "server.h"
#include "uart.h"

os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void user_procTask(os_event_t *events);

static volatile os_timer_t some_timer;
static volatile uint16_t buttonState;
static volatile uint16_t buttonStateSame;
static volatile uint16_t buttonStatePrevious;
static volatile uint8_t buttonStateLatchValue;
static volatile uint16_t buttonRefreshStateCntr;

#define BUTTON_REFRESH_STATE_RATE			10 /* 10 * 10ms = 100ms */

// UartDev is defined and initialized in rom code.
extern UartDevice    UartDev;


void some_timerfunc(void *arg)
{


	//Invert State, Save button State
	buttonState = buttonState + (~GPIO_INPUT_GET(12) & 0x01);


    if(buttonRefreshStateCntr >= BUTTON_REFRESH_STATE_RATE)
    {
    	/* Active High Current State */
    	if(buttonState >= BUTTON_REFRESH_STATE_RATE)
    	{
    		/* Falling Edge Detection on the button */
    		if(buttonState != buttonStatePrevious)
    		{
    			//Toggle Output
    			buttonStateLatchValue = ~buttonStateLatchValue & 0x01;
    			GPIO_OUTPUT_SET(4, buttonStateLatchValue);
    			uart0_sendStr("Toggle\r\n");
    		}
    		else
    		{
    			buttonStateSame++;
    		}
    		//buttonStatePrevious = 0;
    		if(buttonStateSame >= 50)
    		{

    			//uart0_sendStr("Start Configuration AP Mode\r\n");
    			system_os_post(user_procTaskPrio, 1, 0);
    			buttonStateSame = 0;
    		}
    	}
    	else
    	{
    		//buttonStatePrevious = 1;
    	}

    	buttonStatePrevious = buttonState;
    	buttonState = 0;
    	//GPIO_OUTPUT_SET(4, 1);
    	buttonRefreshStateCntr = 0;

    }
    else
    {
    	//uart0_sendStr("GPIO12: LOW\r\n");
    	//GPIO_OUTPUT_SET(4, 0);
    	buttonRefreshStateCntr++;
    }


}

//Do nothing function
static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{

	switch (events->sig)
	{
		case 1:
			//ets_wdt_disable();
			uart0_sendStr("Start Configuration AP Mode\r\n");
			SRVR_ConfigApMode();
			SRVR_ServerApModeInit(AP_MODE_PORT_NUMBER);
			break;

		case 10:
			if(events->par == 0)
			{
				uart0_sendStr("OFF\r\n");
				GPIO_OUTPUT_SET(4, 0);
			}
			else if(events->par == 1)
			{
				uart0_sendStr("ON\r\n");
				GPIO_OUTPUT_SET(4, 1);
			}
			else if(events->par == 0xFF)
			{
				uint8_t switchCurrentState;

				switchCurrentState = GPIO_INPUT_GET(4);
					SRVR_ServerSendData(switchCurrentState);
			}
			break;

	}




    //os_delay_us(1000);
}

//Init function 
void ICACHE_FLASH_ATTR
user_init()
{

	UartDev.data_bits = EIGHT_BITS;
	UartDev.parity = NONE_BITS;
	UartDev.stop_bits = ONE_STOP_BIT;
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	/* Init GPIO ***********************************************/
	/***********************************************************/

    // Initialize the GPIO subsystem.
    gpio_init();
	//Set GPIO12 input mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);
	//Configure as Input
	GPIO_DIS_OUTPUT(12);
	//gpio_output_set(0, 0, 0, BIT12);

	//Set GPIO04 as Output
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);
	//Set Output Low default
	GPIO_OUTPUT_SET(4, 0);

	/* Init some varables */
	buttonState = 0;
	buttonRefreshStateCntr = 0;
	buttonStatePrevious = 0;
	buttonStateLatchValue = 0;

	wifi_set_opmode_current(STATION_MODE);

	// Init AP Mode
	//SRVR_ConfigApMode();
	SRVR_ServerStationModeInit(STATION_MODE_PORT_NUMBER);

    //Disarm timer
    os_timer_disarm(&some_timer);

    //Setup timer
    os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&some_timer, 10, 1);
    
    //Start os task
    system_os_task(user_procTask, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
}
