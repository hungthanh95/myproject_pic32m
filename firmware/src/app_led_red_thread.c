/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_led_red_thread.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "FreeRTOS.h"
#include "app_led_red_thread.h"
#include "peripheral/gpio/plib_gpio.h"
#include "timers.h"
#include "definitions.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_LED_RED_THREAD_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_LED_RED_THREAD_DATA app_led_red_threadData;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static int Count = 0;
static void FreeRtos_Handler2()
{
    if (Count == 10)
    {
        Count = 0;
        Led_Red_Toggle();
    }
    Count++;
}
xTimerHandle RedLed_timerHandle1000Ms;


/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_LED_RED_THREAD_Initialize ( void )

  Remarks:
    See prototype in app_led_red_thread.h.
 */

void APP_LED_RED_THREAD_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_led_red_threadData.state = APP_LED_RED_THREAD_STATE_INIT;


    RedLed_timerHandle1000Ms = xTimerCreate("timer1000Ms",
                                        pdMS_TO_TICKS(1000U),
                                        pdTRUE,
                                        (void*)1,
                                        FreeRtos_Handler2);
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_LED_RED_THREAD_Tasks ( void )

  Remarks:
    See prototype in app_led_red_thread.h.
 */

void APP_LED_RED_THREAD_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_led_red_threadData.state )
    {
        /* Application's initial state. */
        case APP_LED_RED_THREAD_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
                xTimerStart(RedLed_timerHandle1000Ms, 0);

                app_led_red_threadData.state = APP_LED_RED_THREAD_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_LED_RED_THREAD_STATE_SERVICE_TASKS:
        {
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
