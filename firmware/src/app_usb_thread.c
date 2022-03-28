/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_usb_thread.c

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

#include "app_usb_thread.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
uint8_t CACHE_ALIGN cdcReadBuffer[APP_READ_BUFFER_SIZE];
uint8_t CACHE_ALIGN cdcWriteBuffer[APP_READ_BUFFER_SIZE];
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_USB_THREAD_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_USB_THREAD_DATA app_usb_threadData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

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
    void APP_USB_THREAD_Initialize ( void )

  Remarks:
    See prototype in app_usb_thread.h.
 */

void APP_USB_THREAD_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_usb_threadData.state = APP_USB_THREAD_STATE_INIT;
    
    /* Device Layer Handle  */
    app_usb_threadData.deviceHandle = USB_DEVICE_HANDLE_INVALID ;

    /* Device configured status */
    app_usb_threadData.isConfigured = false;

    /* Initial get line coding state */
    app_usb_threadData.getLineCodingData.dwDTERate = 9600;
    app_usb_threadData.getLineCodingData.bParityType = 0;
    app_usb_threadData.getLineCodingData.bCharFormat = 0;
    app_usb_threadData.getLineCodingData.bDataBits = 8;

    /* Read Transfer Handle */
    app_usb_threadData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Write Transfer Handle */
    app_usb_threadData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Initialize the read complete flag */
    app_usb_threadData.isReadComplete = true;

    /*Initialize the write complete flag*/
    app_usb_threadData.isWriteComplete = true;

    /* Reset other flags */
    app_usb_threadData.sofEventHasOccurred = false;

    /* Set up the read buffer */
    app_usb_threadData.cdcReadBuffer = &cdcReadBuffer[0];

    /* Set up the read buffer */
    app_usb_threadData.cdcWriteBuffer = &cdcWriteBuffer[0];  
}

/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void * pData,
    uintptr_t userData
)
{
    APP_USB_THREAD_DATA * appDataObject;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE * eventDataRead;
    
    appDataObject = (APP_USB_THREAD_DATA *)userData;

    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

            USB_DEVICE_ControlSend(appDataObject->deviceHandle,
                    &appDataObject->getLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(appDataObject->deviceHandle,
                    &appDataObject->setLineCodingData, sizeof(USB_CDC_LINE_CODING));

            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            appDataObject->controlLineStateData.carrier = controlLineStateData->carrier;

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);

            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            appDataObject->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            
            /* Complete the control transfer by sending a ZLP  */
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            /* This means that the host has sent some data*/
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            appDataObject->isReadComplete = true;
            appDataObject->numBytesRead = eventDataRead->length; 
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. We don't
             * do much with this data in this demo. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. We can schedule
             * the next read. */

            appDataObject->isWriteComplete = true;
            break;

        default:
            break;
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * Application USB Device Layer Event Handler.
 ***********************************************/
void APP_USBDeviceEventHandler 
(
    USB_DEVICE_EVENT event, 
    void * eventData, 
    uintptr_t context 
)
{
    USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData;

    switch(event)
    {
        case USB_DEVICE_EVENT_SOF:

            app_usb_threadData.sofEventHasOccurred = true;
            
            break;

        case USB_DEVICE_EVENT_RESET:

            /* Update LED to show reset state */
            Led_USB_Clear();

            app_usb_threadData.isConfigured = false;

            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            
            if ( configuredEventData->configurationValue == 1)
            {
                /* Update LED to show configured state */
                Led_USB_Set();
                
                /* Register the CDC Device application event handler here.
                 * Note how the appData object pointer is passed as the
                 * user data */

                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t)&app_usb_threadData);

                /* Mark that the device is now configured */
                app_usb_threadData.isConfigured = true;
            }
            
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(app_usb_threadData.deviceHandle);
            
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:

            /* VBUS is not available any more. Detach the device. */
            USB_DEVICE_Detach(app_usb_threadData.deviceHandle);
            
            Led_USB_Clear();
            
            break;

        case USB_DEVICE_EVENT_SUSPENDED:

            Led_USB_Clear();
            
            break;

        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*****************************************************
 * This function is called in every step of the
 * application state machine.
 *****************************************************/

bool APP_StateReset(void)
{
    /* This function returns true if the device
     * was reset  */

    bool retVal;

    if(app_usb_threadData.isConfigured == false)
    {
        app_usb_threadData.state = APP_USB_THREAD_STATE_WAIT_FOR_CONFIGURATION;
        app_usb_threadData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        app_usb_threadData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        app_usb_threadData.isReadComplete = true;
        app_usb_threadData.isWriteComplete = true;
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}

/******************************************************************************
  Function:
    void APP_USB_THREAD_Tasks ( void )

  Remarks:
    See prototype in app_usb_thread.h.
 */

void APP_USB_THREAD_Tasks ( void )
{
    int i;

    /* Check the application's current state. */
    switch ( app_usb_threadData.state )
    {
        /* Application's initial state. */
        case APP_USB_THREAD_STATE_INIT:
        {
            /* Open the device layer */
            app_usb_threadData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );

            if(app_usb_threadData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(app_usb_threadData.deviceHandle, APP_USBDeviceEventHandler, 0);

                app_usb_threadData.state = APP_USB_THREAD_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }
            break;
        }

        case APP_USB_THREAD_STATE_WAIT_FOR_CONFIGURATION:

            /* Check if the device was configured */
            if(app_usb_threadData.isConfigured)
            {
                /* If the device is configured then lets start reading */
                app_usb_threadData.state = APP_USB_THREAD_STATE_SCHEDULE_READ;
            }
            
            break;

        case APP_USB_THREAD_STATE_SCHEDULE_READ:

            if(APP_StateReset())
            {
                break;
            }

            /* If a read is complete, then schedule a read
             * else wait for the current read to complete */

            app_usb_threadData.state = APP_USB_THREAD_STATE_WAIT_FOR_READ_COMPLETE;
            if(app_usb_threadData.isReadComplete == true)
            {
                app_usb_threadData.isReadComplete = false;
                app_usb_threadData.readTransferHandle =  USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

                USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0,
                        &app_usb_threadData.readTransferHandle, app_usb_threadData.cdcReadBuffer,
                        APP_READ_BUFFER_SIZE);
                
                if(app_usb_threadData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                    app_usb_threadData.state = APP_USB_THREAD_STATE_ERROR;
                    break;
                }
            }

            break;

        case APP_USB_THREAD_STATE_WAIT_FOR_READ_COMPLETE:

            if(APP_StateReset())
            {
                break;
            }

            /* Check if a character was received the isReadComplete flag gets 
             * updated in the CDC event handler. */

            if(app_usb_threadData.isReadComplete)
            {
                app_usb_threadData.state = APP_USB_THREAD_STATE_SCHEDULE_WRITE;
//                Led_USB_Toggle();
            }

            break;


        case APP_USB_THREAD_STATE_SCHEDULE_WRITE:

            if(APP_StateReset())
            {
                break;
            }

            /* Setup the write */

            app_usb_threadData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            app_usb_threadData.isWriteComplete = false;
            app_usb_threadData.state = APP_USB_THREAD_STATE_WAIT_FOR_WRITE_COMPLETE;

            /* Else echo each received character by adding 1 */
            for(i = 0; i < app_usb_threadData.numBytesRead; i++)
            {
                if((app_usb_threadData.cdcReadBuffer[i] != 0x0A) && (app_usb_threadData.cdcReadBuffer[i] != 0x0D))
                {
                    app_usb_threadData.cdcWriteBuffer[i] = app_usb_threadData.cdcReadBuffer[i] + 1;
                }
            }
            USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0,
                    &app_usb_threadData.writeTransferHandle,
                    app_usb_threadData.cdcWriteBuffer, app_usb_threadData.numBytesRead,
                    USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);

            break;

        case APP_USB_THREAD_STATE_WAIT_FOR_WRITE_COMPLETE:

            if(APP_StateReset())
            {
                break;
            }

            /* Check if a character was sent. The isWriteComplete
             * flag gets updated in the CDC event handler */

            if(app_usb_threadData.isWriteComplete == true)
            {
                app_usb_threadData.state = APP_USB_THREAD_STATE_SCHEDULE_READ;
                Led_USB_Toggle();
            }

            break;

        case APP_USB_THREAD_STATE_ERROR:
        default:
            
            break;
    }
}


/*******************************************************************************
 End of File
 */
