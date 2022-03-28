/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
//static int Count = 0;
//static void User_Handler1()
//{
//    Led_Yellow_Toggle();
//}
//
//static void User_Handler2()
//{
//    if (Count == 10)
//    {
//        Count = 0;
//        Led_Red_Toggle();
//    }
//    Count++;
//}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

//    TMR2_CallbackRegister(User_Handler1, 0);
//    TMR4_CallbackRegister(User_Handler2, 0);
//    TMR2_Start();
//    TMR4_Start();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

