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
#include "L6470_config.h"
#include <stdio.h>
#include "RIM_UI_def.h"

#define CURRENTLY_CONNECTED_MOTORS   3
#define CURRENTLY_CONNECTED_ENCODERS 0

void write_reset(int dev_id);
//uint8 check_busy(uint8 pin_num);
uint8 check_busy(byte dev_id);

uint16 seeval;
int8 cur_bit_field = 0;
//test
//Currently used for debugging
uint8 cmd_bytes[3];

uint8 cur_motor_id;

struct motors RIM_Motors[7];
struct encoders RIM_Encoders[5];

uint8 opcode;

//Checks if a pin is busy


//ISR for interpreting input from the computer
//Expects three bytes of data
//ISR runs a total of 6 times per full transmission due to catching the null terminators
CY_ISR(UART_INT_HANDLER)
{
//Method 1    
    UARTD_ClearRxInterruptSource(UARTD_INTR_RX_NOT_EMPTY);

    char recieved_uart_char = UARTD_UartGetChar();
    

    cur_bit_field++;
    //Byte 1
    if(cur_bit_field == 1)
    {
        //command = recieved_uart_char;
        cmd_bytes[0] = recieved_uart_char;
        
        //Expected bit field:
        //_ _ _ _ |_ _ _ _ 
        //O O O O | D M M M
        //O = Opcode, D = Direction, M = Motor ID, U = unused
        
        //Get Opcode
        opcode = recieved_uart_char & 0xF0;
        switch(opcode)
        {
            //If the incoming command is a motor run command
            case RIM_OP_MOTOR_RUN:
                cur_motor_id = recieved_uart_char & RIM_MOTOR_ID;
                
                //Make sure there will be no command overlap
                if(RIM_Motors[cur_motor_id].is_busy)
                    cur_bit_field = -1;
                else
                {
                    RIM_Motors[cur_motor_id].motor_dir = cmd_bytes[0] >> 3;
                    RIM_Motors[cur_motor_id].command_type = RIM_OP_MOTOR_RUN;
                }
                break;
                    
            case RIM_OP_MOTOR_STATUS:
                cur_motor_id = recieved_uart_char & RIM_MOTOR_ID;
                if(RIM_Motors[cur_motor_id].is_busy)
                    cur_bit_field = -1;
                else
                    RIM_Motors[cur_motor_id].command_type = RIM_OP_MOTOR_STATUS;
                break;
            //Command to reset device
            case RIM_OP_RESET_DEV:
                cur_motor_id = recieved_uart_char & RIM_MOTOR_ID;
                if(RIM_Motors[cur_motor_id].is_busy)
                    cur_bit_field = -1;
                else
                    RIM_Motors[cur_motor_id].command_type = RIM_OP_RESET_DEV;
                break;    
            case RIM_OP_ENCODER_INFO:
                cur_motor_id = recieved_uart_char & RIM_MOTOR_ID;
                if(RIM_Encoders[cur_motor_id].is_busy)
                    cur_bit_field = -1;
                else
                    RIM_Encoders[cur_motor_id].command_type = RIM_OP_ENCODER_INFO;
                break;

        }
        
        
    }
    //Byte 2
    else if(cur_bit_field == 3)
    {
        //LSB of 16b number of steps to take
        switch(opcode)
        {
            case RIM_OP_MOTOR_RUN:
                RIM_Motors[cur_motor_id].steps |= recieved_uart_char;
                break;
            case RIM_OP_MOTOR_STATUS:
                break;
            case RIM_OP_RESET_DEV:
                break;
            case RIM_OP_ENCODER_INFO:
                break;
        }
        
        
        cmd_bytes[1] = recieved_uart_char;
    }
    
    //Byte 3
    else if(cur_bit_field == 5)
    {
        switch(opcode)
        {
            case RIM_OP_MOTOR_RUN:
                //MSB of 16b number of steps to take
                RIM_Motors[cur_motor_id].steps |= ((uint16)recieved_uart_char << 8);
                RIM_Motors[cur_motor_id].is_busy = L6470_NOT_BUSY;
                RIM_Motors[cur_motor_id].recieved_cmd = CMD_QUEUED;
                break;
            case RIM_OP_MOTOR_STATUS:
                RIM_Motors[cur_motor_id].is_busy = L6470_NOT_BUSY;
                RIM_Motors[cur_motor_id].recieved_cmd = CMD_QUEUED;
                break;
            case RIM_OP_RESET_DEV:
                RIM_Motors[cur_motor_id].is_busy = L6470_NOT_BUSY;
                RIM_Motors[cur_motor_id].recieved_cmd = CMD_QUEUED;
                break;
            case RIM_OP_ENCODER_INFO:
                RIM_Encoders[cur_motor_id].is_busy = L6470_NOT_BUSY;
                RIM_Encoders[cur_motor_id].recieved_cmd = CMD_QUEUED;
                break;
        }
        
        cmd_bytes[2] = recieved_uart_char;
        cur_bit_field = -1;
    }

}

int main(void)
{
    int i = 0;
    
    for(i = 0; i < 7; i++){
        RIM_Motors[i].is_busy = 0;
        RIM_Motors[i].recieved_cmd = CMD_NONE;
        RIM_Motors[i].steps = 0;
    }
    
    
    //Assign enable ids
    RIM_Motors[0].enable_id = RIM_M0_ENABLE;
    RIM_Motors[1].enable_id = RIM_M1_ENABLE;
    RIM_Motors[2].enable_id = RIM_M2_ENABLE;
    RIM_Motors[3].enable_id = RIM_M3_ENABLE;
    RIM_Motors[4].enable_id = RIM_M4_ENABLE;
    
    RIM_Encoders[0].enable_id = RIM_E0_ENABLE;
    RIM_Encoders[1].enable_id = RIM_E1_ENABLE;
    RIM_Encoders[2].enable_id = RIM_E2_ENABLE;
    RIM_Encoders[3].enable_id = RIM_E3_ENABLE;
    RIM_Encoders[4].enable_id = RIM_E4_ENABLE;
    
    uint16 RIM_UI_cmd_temp = 0;
    byte cmd_content[2] = {0, 0};
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    

    
    UART_INT_StartEx(UART_INT_HANDLER);
    
    SPI_Start();
    CyDelay(1000);
    
    RST_Write(0);
    RST_Write(1);
    
    RST2_Write(0);
    RST2_Write(1);
    
    RST3_Write(0);
    RST3_Write(1);
    
    UARTD_Start();
    
    
    
    //Setup Parameters
    seeval = get_param(CONFIG, RIM_Motors[0].enable_id);
    set_param(STEP_MODE, !SYNC_EN | STEP_SEL_1_2 | SYNC_SEL_1, RIM_Motors[0].enable_id);
    set_param(MAX_SPEED, max_speed_calc(500), RIM_Motors[0].enable_id);
    set_param(FS_SPD,  0x3FF, RIM_Motors[0].enable_id);
    set_param(ACC, acc_calc(50), RIM_Motors[0].enable_id);
    set_param(DECEL, dec_calc(50), RIM_Motors[0].enable_id);
    set_param(OCD_TH, OCD_TH_6000mA, RIM_Motors[0].enable_id);
    set_param(CONFIG, CONFIG_PWM_DIV_1 | CONFIG_PWM_MUL_2 | CONFIG_SR_290V_us | CONFIG_OC_SD_ENABLE | CONFIG_VS_COMP_DISABLE | CONFIG_SW_HARD_STOP | CONFIG_INT_16MHZ, RIM_Motors[0].enable_id);
    set_param(KVAL_RUN, 0xFF, RIM_Motors[0].enable_id);
    
    seeval = get_param(CONFIG, RIM_Motors[1].enable_id);
    set_param(STEP_MODE, !SYNC_EN | STEP_SEL_1_2 | SYNC_SEL_1, RIM_Motors[1].enable_id);
    set_param(MAX_SPEED, max_speed_calc(600), RIM_Motors[1].enable_id);
    set_param(FS_SPD,  0x3FF, RIM_Motors[1].enable_id);
    set_param(ACC, acc_calc(100), RIM_Motors[1].enable_id);
    set_param(DECEL, dec_calc(100), RIM_Motors[1].enable_id);
    set_param(OCD_TH, OCD_TH_6000mA, RIM_Motors[1].enable_id);
    set_param(CONFIG, CONFIG_PWM_DIV_1 | CONFIG_PWM_MUL_2 | CONFIG_SR_530V_us | CONFIG_OC_SD_ENABLE | CONFIG_VS_COMP_DISABLE | CONFIG_SW_HARD_STOP | CONFIG_INT_16MHZ, RIM_Motors[1].enable_id);
    set_param(KVAL_RUN, 0xFF, RIM_Motors[1].enable_id);
    
    seeval = get_param(CONFIG, RIM_Motors[2].enable_id);
    set_param(STEP_MODE, !SYNC_EN | STEP_SEL_1_2 | SYNC_SEL_1, RIM_Motors[2].enable_id);
    set_param(MAX_SPEED, max_speed_calc(600), RIM_Motors[2].enable_id);
    set_param(FS_SPD,  0x3FF, RIM_Motors[2].enable_id);
    set_param(ACC, acc_calc(100), RIM_Motors[2].enable_id);
    set_param(DECEL, dec_calc(35), RIM_Motors[2].enable_id);
    set_param(OCD_TH, OCD_TH_6000mA, RIM_Motors[2].enable_id);
    set_param(CONFIG, CONFIG_PWM_DIV_1 | CONFIG_PWM_MUL_2 | CONFIG_SR_290V_us | CONFIG_OC_SD_ENABLE | CONFIG_VS_COMP_DISABLE | CONFIG_SW_HARD_STOP | CONFIG_INT_16MHZ, RIM_Motors[2].enable_id);
    set_param(KVAL_RUN, 0xFF, RIM_Motors[2].enable_id);
    
    char result[100];
    
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {  
        int i = 0;
        
        //Send update to PC that motor is currently running
        RIM_Motors[0].is_busy = BUSY_Read()  ^ 0x01;
        RIM_Motors[1].is_busy = BUSY2_Read() ^ 0x01;
        RIM_Motors[2].is_busy = BUSY3_Read() ^ 0x01;
        
        
        for(i = 0; i < CURRENTLY_CONNECTED_MOTORS; i++) {
            if(RIM_Motors[i].recieved_cmd == CMD_NONE)
            {
                continue;
            }
            RIM_Motors[0].is_busy = BUSY_Read() ^ 0x01;
            RIM_Motors[1].is_busy = BUSY2_Read() ^ 0x01;
            RIM_Motors[2].is_busy = BUSY3_Read() ^ 0x01;
            
            switch(RIM_Motors[i].command_type) {
                //If recieved command is a motor run command
                case RIM_OP_MOTOR_RUN:
                    if(!RIM_Motors[i].is_busy && RIM_Motors[i].recieved_cmd == CMD_RUNNING)
                    {
                        transfer(SOFT_STOP, RIM_Motors[i].enable_id);
                        while(check_busy(i));

                        RIM_Motors[i].is_busy = L6470_NOT_BUSY;
                        RIM_Motors[i].recieved_cmd = CMD_NONE;
                        RIM_Motors[i].steps = 0;
                        UARTD_UartPutChar(RIM_OP_MOTOR_STOP | i);
                    } 
                    else if (RIM_Motors[i].recieved_cmd == CMD_QUEUED) 
                    {
                        //Start motor movement
                        motor_move(RIM_Motors[i].motor_dir ^ 0x1, RIM_Motors[i].steps, RIM_Motors[i].enable_id);
                        RIM_Motors[i].recieved_cmd = CMD_RUNNING;
                        //One byte information that tells the PC that a motor 1 is running
                        UARTD_UartPutChar(RIM_OP_MOTOR_RUN | i);
                    }
                    break;
                //If command is asking for the motor status register
                case RIM_OP_MOTOR_STATUS:
                    if (RIM_Motors[i].recieved_cmd == CMD_QUEUED) 
                    {
                        RIM_Motors[i].recieved_cmd = CMD_RUNNING;
                        //One byte information that tells the PC that a motor 1 is running
                        UARTD_UartPutChar(RIM_OP_MOTOR_STATUS | i);
                        RIM_UI_cmd_temp = get_status(RIM_Motors[i].enable_id);
                        cmd_content[0] = RIM_UI_cmd_temp;
                        cmd_content[1] = RIM_UI_cmd_temp >> 8;
                        UARTD_UartPutChar(cmd_content[0]);
                        UARTD_UartPutChar(cmd_content[1]);
                        RIM_Motors[i].recieved_cmd = CMD_NONE;
                    }
                    break;
                case RIM_OP_RESET_DEV:
                    if (RIM_Motors[i].recieved_cmd == CMD_QUEUED)
                    {
                        //Command Resets and retrieves status
                        RIM_Motors[i].recieved_cmd = CMD_RUNNING;
                        write_reset(i);
                        UARTD_UartPutChar(RIM_OP_RESET_DEV | i);
                        
                        RIM_UI_cmd_temp = get_status(RIM_Motors[i].enable_id);
                        cmd_content[0] = RIM_UI_cmd_temp;
                        cmd_content[1] = RIM_UI_cmd_temp >> 8;
                        UARTD_UartPutChar(cmd_content[0]);
                        UARTD_UartPutChar(cmd_content[1]);
                        RIM_Motors[i].recieved_cmd = CMD_NONE;
                    }
                    
                default:
                    break;
            }
            
        }
        for(i = 0; i < CURRENTLY_CONNECTED_ENCODERS; i++) {
            
            if(RIM_Encoders[i].recieved_cmd == CMD_NONE)
                continue;
            
            switch(RIM_Encoders[i].command_type)
            {
                
                case RIM_OP_ENCODER_INFO: 
                    if(RIM_Encoders[i].recieved_cmd == CMD_QUEUED) 
                    {
                        RIM_Encoders[i].recieved_cmd = CMD_RUNNING;
                        UARTD_UartPutChar(RIM_OP_ENCODER_INFO | 0x00);
                        RIM_UI_cmd_temp = CUI_get_position(RIM_Encoders[i].enable_id);
                        cmd_content[0] = RIM_UI_cmd_temp;
                        cmd_content[1] = RIM_UI_cmd_temp >> 8;
                        UARTD_UartPutChar(cmd_content[0]);
                        UARTD_UartPutChar(cmd_content[1]);
                        RIM_Encoders[i].recieved_cmd = CMD_NONE;
                    }
                    break;
            }
        }
        //seeval = CUI_get_position(RIM_Encoders[0].enable_id);
        ////sprintf(result, "%i\r\n", test_num);
        //UARTD_UartPutString(result);
        
        //motor_move(FWD, 8000);
        //while(BUSY_Read() == 0);
        
        //transfer(SOFT_STOP);
        //while(BUSY_Read() == 0);
        
        
        
        //enable_Write(0);
        
        //while(1);
        /* Place your application code here. */
    }
}
void write_reset(int dev_id) {
    switch(dev_id) 
    {
        case 0:
            RST_Write(0);
            RST_Write(1);
            break;
        case 1:
            RST2_Write(0);
            RST2_Write(1);
        case 2:
            RST3_Write(0);
            RST3_Write(1);
        default:
            break;
    }
}

uint8 check_busy(byte dev_id)
{
    uint8 r = 0;
    
    switch(dev_id) 
    {
        case 0:
            RIM_Motors[0].is_busy = BUSY_Read() ^ 0x1;
            r = RIM_Motors[0].is_busy;
            break;
        case 1:
            RIM_Motors[1].is_busy = BUSY2_Read() ^ 0x1;
            r = RIM_Motors[1].is_busy;
            break;
        case 2:
            RIM_Motors[2].is_busy = BUSY3_Read() ^ 0x1;
            r = RIM_Motors[2].is_busy;
            break;
        default:
            break;
    }
    return r;
}



/* [] END OF FILE */
