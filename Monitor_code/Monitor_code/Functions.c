void init_LTC_SPI(){
	LTC6804_CS_D |= (1<<LTC6804_CS);	// Default the Slave select pin to output
	LTC6804_CS_P |= (1<<LTC6804_CS);	// Set the Slave select pin HIGH
	
	DDRB |= (1<<PB1) | (1<<PB2);
	
	SPCR |=
		(1<<SPE)	// SPI Enable
		|(1<<MSTR)	// SPI Master select
		|(1<<CPHA)	// SPI Clock Phase. This is required in the LTC6804 datasheet (page 36)
		|(1<<CPOL)	// SPI Clock Polarity. This is required in the LTC6804 datasheet (page 36)
		|(1<<SPR1)	// SPI Clock Rate selection. I chose F_CPU/64, which results in a data rate of 250k, well within the max LTC6804 speed, which is 1Mbps.
	;
}

