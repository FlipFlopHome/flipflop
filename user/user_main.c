#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "discovery.h"

#include "server.h"
#include "uart.h"
#include "pwm.h"

#include "maxim_onewire.h"

#if(0)
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTCK_U
#define PWM_0_OUT_IO_NUM 13
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO13
#endif

#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U
#define PWM_0_OUT_IO_NUM 15
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO15

#define PWM_NUM_OF_CHANNELS			1

os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void user_procTask(os_event_t *events);

static volatile os_timer_t some_timer;
static volatile uint16_t buttonState;
static volatile uint16_t buttonStateSame;
static volatile uint16_t buttonStatePrevious;
static volatile uint8_t buttonStateLatchValue;
static volatile uint16_t buttonRefreshStateCntr;

static volatile uint16_t TempRefreshStateCntr;

#define BUTTON_REFRESH_STATE_RATE			10 /* 10 * 10ms = 100ms */

// UartDev is defined and initialized in rom code.
extern UartDevice    UartDev;


void some_timerfunc(void *arg)
{


	//Invert State, Save button State
	buttonState = buttonState + (~GPIO_INPUT_GET(4) & 0x01);


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
    			GPIO_OUTPUT_SET(12, buttonStateLatchValue);
    			uart0_sendStr("Toggle\r\n");
    		}
    		else
    		{
    			buttonStateSame++;
    		}
    		//buttonStatePrevious = 0;
    		if(buttonStateSame >= 50)
    		{
    			//Go into Configuration in AP mode
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


    if(TempRefreshStateCntr >= 500)
    {
#if(0)
    	uint16_t adcData;
    	char adcResultString[10];

    	TempRefreshStateCntr = 0;
    	os_bzero(adcResultString, sizeof(adcResultString));

    	adcData = system_adc_read();

    	os_sprintf(adcResultString, "ADC Data: %d \r\n", adcData);

    	uart0_sendStr(adcResultString);

    	char tempResultString[40];
    	uint16_t tempResult;
    	uint16_t Tinteger;
    	uint16_t Tfraction;


    	TempRefreshStateCntr = 0;
    	tempResult = (MAX_DS18B20_GetTemp(14));
    	Tinteger = tempResult >> 4;
    	Tfraction = (uint16_t)((float)(tempResult & 0x000F) * 0.0625);
    	os_sprintf(tempResultString, "Temp Data: %d.%i Celcius\r\n", Tinteger, Tfraction);

    	uart0_sendStr(tempResultString);
#endif

    }
    else
    {
    	TempRefreshStateCntr++;
    }


}

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{

	switch (events->sig)
	{
		case 1:
			//ets_wdt_disable();
			uart0_sendStr("Start Configuration AP Mode\r\n");
			GPIO_OUTPUT_SET(5, 1);
			SRVR_ConfigApMode();
			SRVR_ServerApModeInit(AP_MODE_PORT_NUMBER, SRVR_ServerConnectApModeCb);
			break;

		case 2:
			//ets_wdt_disable();
			uart0_sendStr("Start Running Mode in AP Mode\r\n");
			GPIO_OUTPUT_SET(5, 0);
			SRVR_ConfigApMode();
			SRVR_ServerApModeInit(AP_MODE_PORT_NUMBER, SRVR_ServerConnectNormalModeCb);
			break;

		case 3:
			//ets_wdt_disable();
			uart0_sendStr("Start Running Mode in Station Mode\r\n");
			GPIO_OUTPUT_SET(5, 0);
			SRVR_ServerStationModeInit(STATION_MODE_PORT_NUMBER, SRVR_ServerConnectNormalModeCb);
			break;

		case 10:
			if(events->par == 0)
			{
				uart0_sendStr("OFF\r\n");
				GPIO_OUTPUT_SET(12, 0);
			}
			else if(events->par == 1)
			{
				uart0_sendStr("ON\r\n");
				GPIO_OUTPUT_SET(12, 1);
			}
			else if(events->par == 0xFF)
			{
				uint8_t switchCurrentState;

				switchCurrentState = GPIO_INPUT_GET(12);
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
	uint8_t moduleOperatingMode;
	//uint32 pwmIoInfo[1][3] = {PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM};
	//uint32 pwm_duty_init[PWM_NUM_OF_CHANNELS] = {0};

	UartDev.data_bits = EIGHT_BITS;
	UartDev.parity = NONE_BITS;
	UartDev.stop_bits = ONE_STOP_BIT;
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	/* Init GPIO ***********************************************/
	/***********************************************************/

    // Initialize the GPIO subsystem.
    gpio_init();
	//Set GPIO4 input mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);
	//Configure as Input - Switch Btn
	GPIO_DIS_OUTPUT(4);
	//gpio_output_set(0, 0, 0, BIT12);

	//Set GPIO12 as Output - Relay
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);
	//Set Output Low default
	GPIO_OUTPUT_SET(12, 0);

	//Set GPIO05 as Output - LED on the Btn
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
	//Set Output Low default
	GPIO_OUTPUT_SET(5, 0);

	//Set GPIO14 as Output - 1Wire Temp Sensor
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	//Disable Internal PullUp
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTMS_U);
	//Set Output Low default
	GPIO_OUTPUT_SET(14, 1);

#if(0)
	// Init PWM on GPIO15
    pwm_init(100 /* PWM Freq */,  pwm_duty_init /* Duty */ , PWM_NUM_OF_CHANNELS /*Number of PWM Channels */, pwmIoInfo /* IO Info */ );
    set_pwm_debug_en(0);//disable debug print in pwm driver
    pwm_start();
#endif

	/* Init some varables */
	buttonState = 0;
	buttonRefreshStateCntr = 0;
	buttonStatePrevious = 0;
	buttonStateLatchValue = 0;

	TempRefreshStateCntr = 0;



	//wifi_set_opmode_current(STATION_MODE);

	// Init AP Mode
	//SRVR_ConfigApMode();
#if (0)
	SRVR_ServerStationModeInit(STATION_MODE_PORT_NUMBER);
#endif
	// Init Discovery mode
	//discovery_init();

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


	/* Init the operating mode for the AP or Station as per the config saved in flash */
	moduleOperatingMode = wifi_get_opmode_default();
	/* AP Mode */

	if( moduleOperatingMode == SOFTAP_MODE )
	{
		uart0_sendStr("INIT SOFT AP\r\n");
	    //Configure AP mode for normal running operation
	    system_os_post(user_procTaskPrio, 2, 0);
	}
	else if( moduleOperatingMode == STATION_MODE )
	{
		uart0_sendStr("INIT STATION AP.\r\n");
	    //Configure Station mode for normal running operation
	    system_os_post(user_procTaskPrio, 2, 0);
	}
	else /* Station Mode and AP mode, remove AP mode used during configuration */
	{
		uart0_sendStr("INIT SOFT AP & STATION AP...\r\n");
		//just came back from a configuration, make sure we disable the AP mode
		wifi_set_opmode(STATION_MODE);
	    //Configure AP mode for normal running operation
	    system_os_post(user_procTaskPrio, 3, 0);
	}

}
