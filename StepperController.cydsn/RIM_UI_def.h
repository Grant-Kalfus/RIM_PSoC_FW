/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

#ifndef __RIM_UI_DEF
#define __RIM_UI_DEF
    
    #include "project.h"
    //Definitions for RIM packets
    #define RIM_DIRECTION 0x80
    #define RIM_MOTOR_ID 0x70
    
    #define RIM_FW 0x00
    #define RIM_BW 0x01
    
    struct motors {
        uint16 steps;
        uint8 motor_dir;  
        
        //True false
        uint8 recieved_cmd;
        uint8 is_busy;
    };

#endif