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

uint16 seeval;
int8 cur_bit_field = 0;

uint8 cmd_bytes[3];

uint8 cur_motor_id;

struct motors RIM_Motors[7];


//Checks if a pin is busy
uint8 check_busy(uint8 pin_num);

//ISR for interpreting input from the computer
//Expects three bytes of data
//ISR runs a total of 6 times per full transmission due to catching the null terminators
CY_ISR(UART_INT_HANDLER)
{
//Method 1    
    UARTD_ClearRxInterruptSource(UARTD_INTR_RX_NOT_EMPTY);

    char recieved_uart_char = UARTD_UartGetChar();

    cur_bit_field++;
    if(cur_bit_field == 1)
    {
        //command = recieved_uart_char;
        cmd_bytes[0] = recieved_uart_char;
        
        //Expected bit field:
        //_ _ _ _ |_ _ _ _ 
        //O O O O | D M M M
        //O = Opcode, D = Direction, M = Motor ID, U = unused
        
        
        
        cur_motor_id = recieved_uart_char & RIM_MOTOR_ID;
        
        //Prevent command overlap
        if(RIM_Motors[cur_motor_id].is_busy)
            cur_bit_field = -1;
        

        RIM_Motors[cur_motor_id].motor_dir = cmd_bytes[0] >> 3;
    }
    else if(cur_bit_field == 3)
    {
        //LSB of 16b number of steps to take
        
        RIM_Motors[cur_motor_id].steps |= recieved_uart_char;
        
        cmd_bytes[1] = recieved_uart_char;
    }
    else if(cur_bit_field == 5)
    {
        //MSB of 16b number of steps to take
        cmd_bytes[2] = recieved_uart_char;
        
        
        RIM_Motors[cur_motor_id].steps |= ((uint16)recieved_uart_char << 8);
        
        RIM_Motors[cur_motor_id].is_busy = L6470_NOT_BUSY;
        RIM_Motors[cur_motor_id].recieved_cmd = CMD_QUEUED;
        
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
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_INT_StartEx(UART_INT_HANDLER);
    
    enable_Write(1);
    SPI_Start();
    CyDelay(1000);
    
    RST_Write(0);
    RST_Write(1);
    
    UARTD_Start();
    
    seeval = get_param(CONFIG);
    set_param(STEP_MODE, !SYNC_EN | STEP_SEL_1_4 | SYNC_SEL_1);
    set_param(MAX_SPEED, max_speed_calc(500));
    set_param(FS_SPD, fs_calc(50));
    set_param(ACC, acc_calc(50));
    set_param(DECEL, dec_calc(50));
    set_param(OCD_TH, OCD_TH_2250mA);
    set_param(CONFIG, CONFIG_PWM_DIV_1 | CONFIG_PWM_MUL_2 | CONFIG_SR_290V_us | CONFIG_OC_SD_ENABLE | CONFIG_VS_COMP_DISABLE | CONFIG_SW_HARD_STOP | CONFIG_INT_16MHZ);
    set_param(KVAL_RUN, 0xFF);
    
    get_status();
    
    char result[100];
    
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {  
        
        //Send update to PC that motor is currently running
        
        
        //The motor is moving when BUSY_Read() = 0
        RIM_Motors[0].is_busy = BUSY_Read() == 0 ? L6470_BUSY : L6470_NOT_BUSY;
        
        
        if(!RIM_Motors[0].is_busy && RIM_Motors[0].recieved_cmd == CMD_RUNNING)
        {
            transfer(SOFT_STOP);
            while(BUSY_Read() == 0);
            
            RIM_Motors[0].is_busy = L6470_NOT_BUSY;
            RIM_Motors[0].recieved_cmd = CMD_NONE;
            RIM_Motors[0].steps = 0;
            UARTD_UartPutChar(RIM_OP_MOTOR_STOP | 0x00);
        } 
        else if (RIM_Motors[0].recieved_cmd == CMD_QUEUED) 
        {
            //Start motor movement
            motor_move(RIM_Motors[0].motor_dir ^ 0x1, RIM_Motors[0].steps);
            RIM_Motors[0].recieved_cmd = CMD_RUNNING;
            //One byte information that tells the PC that a motor 1 is running
            UARTD_UartPutChar(RIM_OP_MOTOR_MOVE | 0x00);
        }
        
        
        
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

//Motor 1 is the first bit


/* [] END OF FILE */
