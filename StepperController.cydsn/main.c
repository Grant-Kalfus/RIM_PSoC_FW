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

uint16 steps;

uint8 cmd_bytes[3];

uint8 cur_motor_id;

struct motors RIM_Motors[7];


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
        //_ _ _ _ | _ _ _ _ 
        //D M M M | U U U U
        //D = Direction, M = Motor ID, U = unused
        
        cur_motor_id = ((recieved_uart_char & RIM_MOTOR_ID) >> 4) - 1;
        

        RIM_Motors[cur_motor_id].motor_dir = cmd_bytes[0] >> 7;
    }
    else if(cur_bit_field == 3)
    {
        
        steps |= recieved_uart_char;
        cmd_bytes[1] = recieved_uart_char;
    }
    else if(cur_bit_field == 5)
    {
        
        steps |= ((uint16)recieved_uart_char << 8);
        
        cmd_bytes[2] = recieved_uart_char;
        
        RIM_Motors[cur_motor_id].steps = steps;
        RIM_Motors[cur_motor_id].recieved_cmd = 1;
        RIM_Motors[cur_motor_id].is_busy = 1;
        
        cur_bit_field = -1;
        
        UARTD_UartPutChar(cmd_bytes[0]);
        UARTD_UartPutChar(cmd_bytes[1]);
        UARTD_UartPutChar(cmd_bytes[2]);
        steps = 0;

    }

//Method 2
 /*  position++;
    if(position == 3)
    {
       uint8 upper = UART_ReadRxData();
       uint8 lower = UART_ReadRxData();
       steps = lower | (upper << 8);
       command = UART_ReadRxData();
       UART_PutChar(command);
       UART_PutChar(upper);
       UART_PutChar(lower);
       position=1;
    }
    Received_ClearPending();
    */
}

int main(void)
{
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
        if(RIM_Motors[0].recieved_cmd)
        {
            motor_move(RIM_Motors[0].motor_dir ^ 0x1, RIM_Motors[0].steps);
            while(BUSY_Read() == 0);
            
            transfer(SOFT_STOP);
            while(BUSY_Read() == 0);
            
            RIM_Motors[0].is_busy = 0;
            
            RIM_Motors[0].recieved_cmd = 0;
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



/* [] END OF FILE */
