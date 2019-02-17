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
#ifndef _CONFIGH_
#define _CONFIGH_

    #include "project.h"
    #include "L6470_regdef.h"
    typedef unsigned char byte; 
    typedef unsigned int uint;
    
    //Modified
    byte transfer(byte, byte enable_id);   
    
    //Unmodified
    uint param_handler(byte param, uint value, byte enable_id);
    
    uint send_and_receive(uint value, byte blen, byte enable_id);
    
    void set_param(byte param, uint value, byte enable_id);
    
    uint get_param(byte param, byte enable_id);
    
    void motor_move(byte dir, uint16 n_step, byte enable_id);
    
    uint acc_calc(float stepsPerSecPerSec);
    
    uint dec_calc(float stepsPerSecPerSec);
    
    uint max_speed_calc(float stepsPerSec);
    
    uint min_speed_calc(float stepsPerSec);
    
    uint fs_calc(float stepsPerSec);
    
    uint get_status(byte enable_id);
    

#endif