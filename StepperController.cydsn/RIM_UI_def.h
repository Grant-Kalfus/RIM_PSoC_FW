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
    #define RIM_DIRECTION 0x08
    #define RIM_MOTOR_ID 0x07
    
    #define RIM_FW 0x00
    #define RIM_BW 0x01
    
    #define CMD_NONE 0x00
    #define CMD_QUEUED 0x01
    #define CMD_RUNNING 0x02
    
    #define L6470_NOT_BUSY 0x00
    #define L6470_BUSY 0x01
    
    //RIM OpCodes
    #define RIM_OP_MOTOR_RUN           0x00
    #define RIM_OP_MOTOR_STOP          0x10
    #define RIM_OP_MOTOR_SET_PARAM     0x20
    #define RIM_OP_MOTOR_STATUS        0x30
    #define RIM_OP_ENCODER_INFO        0x40
    
    #define RIM_OP_MOTOR_EXTENDED_STEP 0x80
    

    struct motors {
        uint16 steps;
        uint8 motor_dir;  
        
        //True false
        uint8 recieved_cmd;
        
        
        uint8 command_type;
        
        uint8 is_busy;
    };

#endif