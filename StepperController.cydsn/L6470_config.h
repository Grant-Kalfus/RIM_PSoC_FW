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
    
    byte transfer(byte);   
    
    uint param_handler(byte param, uint value);
    
    uint send_and_receive(uint value, byte blen);
    
    void set_param(byte param, uint value);
    
    uint get_param(byte param);
    
    void motor_move(byte dir, uint16 n_step);
    
    uint acc_calc(float stepsPerSecPerSec);
    
    uint dec_calc(float stepsPerSecPerSec);
    
    uint max_speed_calc(float stepsPerSec);
    
    uint min_speed_calc(float stepsPerSec);
    
    uint fs_calc(float stepsPerSec);
    
    uint get_status();
    

#endif