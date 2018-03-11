uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2];	// GPIO conversion command.

void LTC_wakeup_sleep(){
	LTC6804_CS_P &= ~(1<<LTC6804_CS);
	_delay_ms(1);
	LTC6804_CS_P |= (1<<LTC6804_CS);
}

/*!******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands
 
@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command
 
 Command Code: \n
			|command	|  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
			|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
			|ADCV:	    |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
			|ADAX:	    |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
 ******************************************************************************************************************/
void set_adc(uint8_t MD, //ADC Mode
			 uint8_t DCP, //Discharge Permit
			 uint8_t CH, //Cell Channels to be measured
			 uint8_t CHG //GPIO Channels to be measured
			 )
{
  uint8_t md_bits;
  
  md_bits = (MD & 0x02) >> 1;
  ADCV[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;
 
  md_bits = (MD & 0x02) >> 1;
  ADAX[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  ADAX[1] = md_bits + 0x60 + CHG ;
  
}


/*!******************************************************************************************************
 \brief Start an GPIO Conversion
 
  Starts an ADC conversions of the LTC6804 GPIO inputs.
  The type of ADC conversion done is set using the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |
 
*********************************************************************************************************/
void LTC6804_adax()
{
  uint8_t cmd[4];
  uint16_t temp_pec;
 
  cmd[0] = ADAX[0];
  cmd[1] = ADAX[1];
  temp_pec = pec15_calc(2, ADAX);
  cmd[2] = (uint8_t)(temp_pec >> 8);
  cmd[3] = (uint8_t)(temp_pec);
 
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  LTC6804_CS_P &= ~(1<<LTC6804_CS);
  spi_write_array(4,cmd);
  LTC6804_CS_P |= (1<<LTC6804_CS);
}
/*
  LTC6804_adax Function sequence:
  
  1. Load adax command into cmd array
  2. Calculate adax cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adax command to LTC6804 stack
*/

/*
@param[in]  uint8_t len: the length of the data array being passed to the function

@param[in]  uint8_t data[] : the array of data that the PEC will be generated from


@return  The calculated pec15 as an unsigned int16_t
***********************************************************/
uint16_t pec15_calc(uint8_t len, uint8_t *data)
{
	uint16_t remainder,addr;

	remainder = 16;//initialize the PEC
	for(uint8_t i = 0; i<len;i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
		remainder = (remainder<<8)^crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}