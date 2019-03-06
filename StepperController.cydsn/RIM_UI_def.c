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
#include "RIM_UI_def.h"
#include "project.h"

uint8 testvar = 0;

void CUI_transfer(uint8 command, uint8 enable_id) 
{
    //Encoder_Enable_Reg_Write(enable_id);
    SPI_WriteTxData(command);
    while(!(SPI_ReadTxStatus() & SPI_STS_SPI_DONE));
    //Encoder_Enable_Reg_Write(RIM_ALL_OFF);
}

uint8 CUI_read(uint8 enable_id) 
{
    CUI_transfer(CUI_NOP,enable_id);
    
    while(!(SPI_ReadRxStatus() & SPI_STS_RX_FIFO_NOT_EMPTY));
    testvar = SPI_ReadRxData();
    return testvar;
}

uint16 CUI_get_position(uint8 enable_id) 
{
    uint16 result = 0;
    
    CUI_transfer(CUI_READ_POS, enable_id);
    //CUI_transfer(CUI_READ_POS, enable_id);
    
    while(1)
    {
        if(CUI_read(enable_id) == CUI_READ_POS) {break;}
        CyDelayUs(20);
    }
        
    
    //Read 8 upper bytes
    result = CUI_read(enable_id);
    result <<= 8;
    
    //OR with 8 lower bytes
    result |= CUI_read(enable_id);
    
    return result;      
}