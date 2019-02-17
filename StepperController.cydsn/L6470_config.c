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
#include "L6470_config.h"
#include "RIM_UI_def.h"

void set_param(byte param, uint value, byte enable_id) 
{
  transfer(SET_PARAM | param, enable_id);
  param_handler(param, value, enable_id);
}

uint get_param(byte param, byte enable_id)
{
  transfer(GET_PARAM | param, enable_id);
  return param_handler(param, 0, enable_id);
}

// Copyed from dSPIN_Move of Sparkfun arduino librarys for L6470
// MOVE will send the motor n_step steps (size based on step mode) in the
//  direction imposed by dir (FWD or REV constants may be used). The motor
//  will accelerate according the acceleration and deceleration curves, and
//  will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well.
void motor_move(byte dir, uint16 n_step, byte enable_id)
{
  transfer(MOVE | dir, enable_id);
  transfer((byte)(n_step >> 16), enable_id);
  transfer((byte)(n_step >> 8), enable_id);
  transfer((byte)(n_step), enable_id);
}


//Adapted from dSPIN_Xfer of Sparkfun arduino librarys for L6470
byte transfer(byte data, byte enable_id) {
    Enable_Reg_Write(enable_id);
    SPI_WriteTxData(data);
    while(!(SPI_ReadTxStatus() & SPI_STS_SPI_DONE));
    while(!(SPI_ReadRxStatus() & SPI_STS_RX_FIFO_NOT_EMPTY));
    Enable_Reg_Write(RIM_ALL_OFF);
    CyDelayUs(2);
    return SPI_ReadRxData();
}

//Copyed from dSPIN_ParamHandler of Sparkfun arduino librarys for L6470
uint param_handler(byte param, uint value, byte enable_id )
{
  unsigned long ret_val = 0;  
  switch (param)
  {
    // ABS_POS is the current absolute offset from home. It is a 22 bit number expressed
    //  in two's complement. At power up, this value is 0. It cannot be written when
    //  the motor is running, but at any other time, it can be updated to change the
    //  interpreted position of the motor.
    case ABS_POS:
      ret_val = send_and_receive(value, 22, enable_id);
      break;
    
    // EL_POS is the current electrical position in the step generation cycle. It can
    //  be set when the motor is not in motion. Value is 0 on power up.
    case EL_POS:
      ret_val = send_and_receive(value, 9, enable_id);
      break;
    
    // MARK is a second position other than 0 that the motor can be told to go to. As
    //  with ABS_POS, it is 22-bit two's complement. Value is 0 on power up.
    case MARK:
      ret_val = send_and_receive(value, 22, enable_id);
      break;
    
    
    // SPEED contains information about the current speed. It is read-only. It does 
    //  NOT provide direction information.
    case SPEED:
      ret_val = send_and_receive(0, 20, enable_id);
      break; 
    
    
    // ACC and DEC set the acceleration and deceleration rates. Set ACC to 0xFFF 
    //  to get infinite acceleration/decelaeration- there is no way to get infinite
    //  deceleration w/o infinite acceleration (except the HARD STOP command).
    //  Cannot be written while motor is running. Both default to 0x08A on power up.
    // AccCalc() and DecCalc() functions exist to convert steps/s/s values into
    //  12-bit values for these two registers.
    case ACC: 
      ret_val = send_and_receive(value, 12, enable_id);
      break;
    case DECEL: 
      ret_val = send_and_receive(value, 12, enable_id);
      break;
    
    
    // MAX_SPEED is just what it says- any command which attempts to set the speed
    //  of the motor above this value will simply cause the motor to turn at this
    //  speed. Value is 0x041 on power up.
    // MaxSpdCalc() function exists to convert steps/s value into a 10-bit value
    //  for this register.
    case MAX_SPEED:
      ret_val = send_and_receive(value, 10, enable_id);
      break;
    
    
    // MIN_SPEED controls two things- the activation of the low-speed optimization
    //  feature and the lowest speed the motor will be allowed to operate at. LSPD_OPT
    //  is the 13th bit, and when it is set, the minimum allowed speed is automatically
    //  set to zero. This value is 0 on startup.
    // MinSpdCalc() function exists to convert steps/s value into a 12-bit value for this
    //  register. SetLSPDOpt() function exists to enable/disable the optimization feature.
    case MIN_SPEED: 
      ret_val = send_and_receive(value, 12, enable_id);
      break;
    
    
    // FS_SPD register contains a threshold value above which microstepping is disabled
    //  and the dSPIN operates in full-step mode. Defaults to 0x027 on power up.
    // FSCalc() function exists to convert steps/s value into 10-bit integer for this
    //  register.
    case FS_SPD:
      ret_val = send_and_receive(value, 10, enable_id);
      break;
    
    
    // KVAL is the maximum voltage of the PWM outputs. These 8-bit values are ratiometric
    //  representations: 255 for full output voltage, 128 for half, etc. Default is 0x29.
    // The implications of different KVAL settings is too complex to dig into here, but
    //  it will usually work to max the value for RUN, ACC, and DEC. Maxing the value for
    //  HOLD may result in excessive power dissipation when the motor is not running.
    case KVAL_HOLD:
      ret_val = transfer((byte)value, enable_id);
      break;
    case KVAL_RUN:
      ret_val = transfer((byte)value, enable_id);
      break;
    case KVAL_ACC:
      ret_val = transfer((byte)value, enable_id);
      break;
    case KVAL_DEC:
      ret_val = transfer((byte)value, enable_id);
      break;
    
    
    // INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back EMF
    //  compensation functionality. Please see the datasheet for details of this
    //  function- it is too complex to discuss here. Default values seem to work
    //  well enough.
    case INT_SPD:
      ret_val = send_and_receive(value, 14, enable_id);
      break;
    case ST_SLP: 
      ret_val = transfer((byte)value, enable_id);
      break;
    case FN_SLP_ACC: 
      ret_val = transfer((byte)value, enable_id);
      break;
    case FN_SLP_DEC: 
      ret_val = transfer((byte)value, enable_id);
      break;
    
    
    // K_THERM is motor winding thermal drift compensation. Please see the datasheet
    //  for full details on operation- the default value should be okay for most users.
    case K_THERM: 
      ret_val = transfer((byte)value & 0x0F, enable_id);
      break;
    
    
    // ADC_OUT is a read-only register containing the result of the ADC measurements.
    //  This is less useful than it sounds; see the datasheet for more information.
    case ADC_OUT:
      ret_val = transfer(0, enable_id);
      break;
    
    
    // Set the overcurrent threshold. Ranges from 375mA to 6A in steps of 375mA.
    //  A set of defined constants is provided for the user's convenience. Default
    //  value is 3.375A- 0x08. This is a 4-bit value.
    case OCD_TH: 
      ret_val = transfer((byte)value & 0x0F, enable_id);
      break;
    
    
    // Stall current threshold. Defaults to 0x40, or 2.03A. Value is from 31.25mA to
    //  4A in 31.25mA steps. This is a 7-bit value.
    case STALL_TH: 
      ret_val = transfer((byte)value & 0x7F, enable_id);
      break;
    
    
    // STEP_MODE controls the microstepping settings, as well as the generation of an
    //  output signal from the dSPIN. Bits 2:0 control the number of microsteps per
    //  step the part will generate. Bit 7 controls whether the BUSY/SYNC pin outputs
    //  a BUSY signal or a step synchronization signal. Bits 6:4 control the frequency
    //  of the output signal relative to the full-step frequency; see datasheet for
    //  that relationship as it is too complex to reproduce here.
    // Most likely, only the microsteps per step value will be needed; there is a set
    //  of constants provided for ease of use of these values.
    case STEP_MODE:
      ret_val = transfer((byte)value, enable_id);
      break;
    
    
    // ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of constants
    //  is provided to make this easy to interpret. By default, ALL alarms will trigger the
    //  FLAG pin.
    case ALARM_EN: 
      ret_val = transfer((byte)value, enable_id);
      break;
    
    
    // CONFIG contains some assorted configuration bits and fields. A fairly comprehensive
    //  set of reasonably self-explanatory constants is provided, but users should refer
    //  to the datasheet before modifying the contents of this register to be certain they
    //  understand the implications of their modifications. Value on boot is 0x2E88; this
    //  can be a useful way to verify proper start up and operation of the dSPIN chip.
    case CONFIG: 
      ret_val = send_and_receive(value, 16, enable_id);
      break;
    
    
    // STATUS contains read-only information about the current condition of the chip. A
    //  comprehensive set of constants for masking and testing this register is provided, but
    //  users should refer to the datasheet to ensure that they fully understand each one of
    //  the bits in the register.
    case STATUS:  // STATUS is a read-only register
      ret_val = send_and_receive(0, 16, enable_id);
      break;
    
    
    default:
      ret_val = transfer((byte)(value), enable_id);
      break;
  }
  return ret_val;
}



//Adapted from dSPIN_Param of Sparkfun arduino librarys for L6470
uint send_and_receive(uint value, byte bit_len, byte enable_id) {
    
    uint r = 0;
    
    byte byte_len = bit_len / 8; //number of bits needed
    if (bit_len%8 > 0) byte_len++; // Take ceiling
    uint mask = 0xFFFFFFFF >> (32 - bit_len);  //Mask to take off unessisary bits
    if (value > mask) value = mask; 
    
    
    if (byte_len == 3) {
        r |= transfer((byte)(value>>16), enable_id) << 16;
    }
    if (byte_len >= 2) {
        r |= transfer((byte)(value>>8), enable_id) << 8;
    }
    if (byte_len >= 1) {
        r |= transfer((byte)value, enable_id);
  }
    return (r & mask);
}

// Copyed from AccCalc of Sparkfun arduino librarys for L6470
uint acc_calc(float stepsPerSecPerSec)
{
  float temp = stepsPerSecPerSec * 0.137438;
  if( (uint) temp > 0x00000FFF) return 0x00000FFF;
  else return (uint) temp;
}

// Copyed from DecCalc of Sparkfun arduino librarys for L6470
// The calculation for DEC is the same as for ACC. Value is 0x08A on boot.
// This is a 12-bit value, so we need to make sure the value is at or below 0xFFF.
uint dec_calc(float stepsPerSecPerSec)
{
  float temp = stepsPerSecPerSec * 0.137438;
  if( (uint) temp > 0x00000FFF) return 0x00000FFF;
  else return (uint) temp;
}

// Copyed from MaxSpdCalc of Sparkfun arduino librarys for L6470
// The value in the MAX_SPD register is [(steps/s)*(tick)]/(2^-18) where tick is 
//  250ns (datasheet value)- 0x041 on boot.
// Multiply desired steps/s by .065536 to get an appropriate value for this register
// This is a 10-bit value, so we need to make sure it remains at or below 0x3FF
uint max_speed_calc(float stepsPerSec)
{
  float temp = stepsPerSec * .065536;
  if( (uint) temp > 0x000003FF) return 0x000003FF;
  else return (uint) temp;
}

// Copyed from MinSpdCalc of Sparkfun arduino librarys for L6470
// The value in the MIN_SPD register is [(steps/s)*(tick)]/(2^-24) where tick is 
//  250ns (datasheet value)- 0x000 on boot.
// Multiply desired steps/s by 4.1943 to get an appropriate value for this register
// This is a 12-bit value, so we need to make sure the value is at or below 0xFFF.
uint min_speed_calc(float stepsPerSec)
{
  float temp = stepsPerSec * 4.1943;
  if( (uint) temp > 0x00000FFF) return 0x00000FFF;
  else return (uint) temp;
}

// The value in the FS_SPD register is ([(steps/s)*(tick)]/(2^-18))-0.5 where tick is 
//  250ns (datasheet value)- 0x027 on boot.
// Multiply desired steps/s by .065536 and subtract .5 to get an appropriate value for this register
// This is a 10-bit value, so we need to make sure the value is at or below 0x3FF.
uint fs_calc(float stepsPerSec)
{
  float temp = (stepsPerSec * .065536)-.5;
  if( (uint) temp > 0x000003FF) return 0x000003FF;
  else return (uint) temp;
}

uint get_status(byte enable_id) {
  int temp = 0;
  transfer(GET_STATUS, enable_id);
  temp = transfer(0, enable_id)<<8;
  temp |= transfer(0, enable_id);
  return temp;
}