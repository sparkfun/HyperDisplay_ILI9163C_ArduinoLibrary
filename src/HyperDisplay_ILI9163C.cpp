#include "HyperDisplay_ILI9163C.h"



ILI9163C::ILI9163C(uint8_t xSize, uint8_t ySize, ILI9163C_INTFC_t intfc ) : hyperdisplay(xSize, ySize)
{
	_intfc = intfc;
}


uint8_t ILI9163C::getBytesPerPixel( void )
{
	uint8_t bpp = 0;
	switch(_pxlfmt)
	{
		case ILI9163C_PXLFMT_18 :
			bpp = offsetof( ILI9163C_color_18_t, b) + 1;
			break;

		case ILI9163C_PXLFMT_16 :
			bpp = offsetof( ILI9163C_color_16_t, glb) + 1;
			break;

		case ILI9163C_PXLFMT_12 :
			bpp = offsetof( ILI9163C_color_12_t, glb) + 1;
			break;

		default :
			break;
	}
	return bpp;
}


// Pure virtual functions from HyperDisplay Implemented:
color_t ILI9163C::getOffsetColor(color_t base, uint32_t numPixels)
{
	switch(_pxlfmt)
	{
		case ILI9163C_PXLFMT_18 :
			return (color_t)((( ILI9163C_color_18_t*)base) + numPixels );
			break;

		case ILI9163C_PXLFMT_16 :
			return (color_t)((( ILI9163C_color_16_t*)base) + numPixels );
			break;

		case ILI9163C_PXLFMT_12 :
			return (color_t)((( ILI9163C_color_12_t*)base) + numPixels );
			break;

		default :
			return base;
	}
}

void 	ILI9163C::hwpixel(hd_hw_extent_t x0, hd_hw_extent_t y0, color_t data, hd_colors_t colorCycleLength, hd_colors_t startColorOffset)
{
	if(data == NULL){ return; }

	startColorOffset = getNewColorOffset(colorCycleLength, startColorOffset, 0);	// This line is needed to condition the user's input start color offset
	color_t value = getOffsetColor(data, startColorOffset);

	setColumnAddress( (uint16_t)x0, (uint16_t)x0);
	setRowAddress( (uint16_t)y0, (uint16_t)y0);
	uint8_t len = getBytesPerPixel( );
	
	// Serial.println("RIGH HERE");
	// Serial.print(x0); 
	// Serial.print(", "); Serial.print(y0); 
	// Serial.print(" : "); Serial.print(1); 
	// Serial.print(" : 0x"); Serial.print((uint32_t)value, HEX); 
	// Serial.print(" { "); Serial.print(*(((uint8_t *)value) + 0)); 
	// Serial.print(", "); Serial.print(*(((uint8_t *)value) + 1)); 
	// Serial.print(", "); Serial.print(*(((uint8_t *)value) + 2)); 
	// Serial.print("}"); 
	// Serial.println();


	writeToRAM( (uint8_t*)value, len );
}



// Basic Control Functions
ILI9163C_STAT_t ILI9163C::swReset( void )
{
 	ILI9163C_STAT_t retval = 	ILI9163C_STAT_Nominal;

 	ILI9163C_CMD_t cmd = ILI9163C_CMD_SWRST;
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::sleepIn( void )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;
	ILI9163C_CMD_t cmd = ILI9163C_CMD_SLPIN;
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::sleepOut( void )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_SLPOUT;
	retval = writePacket(&cmd );
	return retval;
}

ILI9163C_STAT_t ILI9163C::partialModeOn( void )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_PTLON;
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::normalDisplayModeOn( void )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_PTLON;
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setInversion( bool on )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_INVOFF;
	if( on )
	{
		cmd = ILI9163C_CMD_INVON;
	}
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPower( bool on )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_OFF;
	if( on )
	{
		cmd = ILI9163C_CMD_ON;
	}
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setColumnAddress( uint16_t start, uint16_t end )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_CASET;
	uint8_t buff[4] = {(start >> 8), (start & 0x00FF), (end >> 8), (end & 0x00FF)};
	retval = writePacket(&cmd, buff, 4);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setRowAddress( uint16_t start, uint16_t end )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_RASET;
	uint8_t buff[4] = {(start >> 8), (start & 0x00FF), (end >> 8), (end & 0x00FF)};
	retval = writePacket(&cmd, buff, 4);
	return retval;
}

ILI9163C_STAT_t ILI9163C::writeToRAM( uint8_t* pdata, uint16_t numBytes )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRRAM;
	retval = writePacket(&cmd, pdata, numBytes);
	return retval;
}



// Functions to configure the display fully
ILI9163C_STAT_t ILI9163C::setMemoryAccessControl( bool mx, bool my, bool mv, bool ml, bool bgr, bool mh )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRMADCTL;
	uint8_t buff = 0x00;
	if( my ){ buff |= 0x80; }
	if( mx ){ buff |= 0x40; }
	if( mv ){ buff |= 0x20; }
	if( ml ){ buff |= 0x10; }
	if( bgr ){ buff |= 0x08; }
	if( mh ){ buff |= 0x04; }
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::selectGammaCurve( uint8_t bmNumber )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_GAMST;
	uint8_t buff = bmNumber;
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPartialArea(uint16_t start, uint16_t end )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_PTLAREA;
	uint8_t buff[4] = {(start >> 8), (start & 0x00FF), (end >> 8), (end & 0x00FF)};
	retval = writePacket(&cmd, buff, 4);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setVerticalScrolling( uint16_t tfa, uint16_t vsa, uint16_t bfa )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRVSCRL;
	uint8_t buff[6] = {(tfa >> 8), (tfa & 0x00FF), (vsa >> 8), (vsa & 0x00FF), (bfa >> 8), (bfa & 0x00FF)};
	retval = writePacket(&cmd, buff, 6);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setVerticalScrollingStartAddress( uint16_t ssa )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRVSSA;
	uint8_t buff[2] = {(ssa >> 8), (ssa & 0x00FF)};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setIdleMode( bool on )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_IDLOFF;
	if( on )
	{
		cmd = ILI9163C_CMD_IDLON;
	}
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setInterfacePixelFormat( uint8_t CTRLintfc )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPXFMT;
	uint8_t buff = (CTRLintfc & 0x07);

	if( buff == ILI9163C_PXLFMT_12 ){ _pxlfmt = ILI9163C_PXLFMT_12; }
	if( buff == ILI9163C_PXLFMT_16 ){ _pxlfmt = ILI9163C_PXLFMT_16; }
	if( buff == ILI9163C_PXLFMT_18 ){ _pxlfmt = ILI9163C_PXLFMT_18; }

	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setTearingEffectLine( bool on )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_TELOFF;
	if( on )
	{
		cmd = ILI9163C_CMD_TELON;
	}
	retval = writePacket(&cmd);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setNormalFramerate( uint8_t diva, uint8_t vpa )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRNMLFRCTL;
	uint8_t buff[2] = {diva, vpa};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setIdleFramerate( uint8_t divb, uint8_t vpb )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRIDLFRCTL;
	uint8_t buff[2] = {divb, vpb};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPartialFramerate( uint8_t divc, uint8_t vpc )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPTLFRCTL;
	uint8_t buff[2] = {divc, vpc};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPowerControl1( uint8_t vrh, uint8_t vc )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPWCTL1;
	uint8_t buff[2] = {(vrh & 0x1F), (vc & 0x07)};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPowerControl2( uint8_t bt )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPWCTL2;
	uint8_t buff = (bt & 0x07);
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPowerControl3( uint8_t apa )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPWCTL3;
	uint8_t buff = (apa & 0x07);
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPowerControl4( uint8_t apb )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPWCTL4;
	uint8_t buff = (apb & 0x07);
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPowerControl5( uint8_t apc )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPWCTL5;
	uint8_t buff = (apc & 0x07);
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setVCOMControl1( uint8_t vmh, uint8_t vml )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRVCOMCTL1;
	uint8_t buff[2] = {(vmh & 0x7F), (vml & 0x7F)};
	retval = writePacket(&cmd, buff, 2);
	return retval;
}

// ILI9163C_STAT_t ILI9163C::setVCOMControl2( uint8_t vma )
// {
// 	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

// 	ILI9163C_CMD_t cmd = ;
// 	uint8_t buff[] = {};
// 	retval = writePacket(&cmd, &buff, );
// 	return retval;
// }

ILI9163C_STAT_t ILI9163C::setVCOMOffsetControl( bool nVM, uint8_t vmf )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRVCMOFSTCTL;
	uint8_t buff = (vmf & 0x7F);
	if( nVM )
	{
		buff |= 0x80;
	}
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setSrcDriverDir( bool crl )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRSDRVDIR;
	uint8_t buff = 0x00;
	if( crl )
	{
		buff |= 0x01;
	}
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setGateDriverDir( bool ctb )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRGDRVDIR;
	uint8_t buff = 0x00;
	if( ctb )
	{
		buff |= 0x01;
	}
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setGamRSel( bool gamrsel )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRGAMRS;
	uint8_t buff = 0x00;
	if( gamrsel )
	{
		buff |= 0x01;
	}
	retval = writePacket(&cmd, &buff, 1);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setPositiveGamCorr( uint8_t* gam16byte )
{ 
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRPGCS;
	retval = writePacket(&cmd, gam16byte, 16);
	return retval;
}

ILI9163C_STAT_t ILI9163C::setNegativeGamCorr( uint8_t* gam16byte )
{ 
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRNGCS;
	retval = writePacket(&cmd, gam16byte, 16);
	return retval;
}

















////////////////////////////////////////////////////////////
//		SSD1357_Arduino_SPI_OneWay Implementation		  //
////////////////////////////////////////////////////////////
ILI9163C_4WSPI::ILI9163C_4WSPI(uint16_t xSize, uint16_t ySize) : hyperdisplay( xSize, ySize ), ILI9163C(xSize, ySize, ILI9163C_INTFC_4WSPI)
{
	SPISettings tempSettings(ILI9163C_SPI_MAX_FREQ, ILI9163C_SPI_DATA_ORDER, ILI9163C_SPI_MODE);
	_spisettings = tempSettings;
}

////////////////////////////////////////////////////////////
//				Display Interface Functions				  //
////////////////////////////////////////////////////////////
ILI9163C_STAT_t ILI9163C_4WSPI::writePacket(ILI9163C_CMD_t* pcmd, uint8_t* pdata, uint16_t dlen)
{
	selectDriver();
	_spi->beginTransaction(_spisettings);

	if(pcmd != NULL)
	{
		digitalWrite(_dc, LOW);
		_spi->transfer(*(pcmd));
	}

	if( (pdata != NULL) && (dlen != 0) )
	{
		digitalWrite(_dc, HIGH);
		// Serial.print("Whatsup ketchup?");
		_spi->transfer(pdata, dlen);
	}		

	_spi->endTransaction();	
	deselectDriver();
	return ILI9163C_STAT_Nominal;
}

ILI9163C_STAT_t ILI9163C_4WSPI::selectDriver( void )
{
	digitalWrite(_cs, LOW);
	return ILI9163C_STAT_Nominal;
}

ILI9163C_STAT_t ILI9163C_4WSPI::deselectDriver( void )
{
	digitalWrite(_cs, HIGH);
	return ILI9163C_STAT_Nominal;
}

ILI9163C_STAT_t ILI9163C_4WSPI::setSPIFreq( uint32_t freq )
{
	SPISettings tempSettings(freq, ILI9163C_SPI_DATA_ORDER, ILI9163C_SPI_MODE);
	_spisettings = tempSettings;
	return ILI9163C_STAT_Nominal;
}

void    ILI9163C_4WSPI::hwxline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data, hd_colors_t colorCycleLength, hd_colors_t startColorOffset, bool goLeft)
{
	if(data == NULL){ return; }
	if( len < 1 ){ return; }

	startColorOffset = getNewColorOffset(colorCycleLength, startColorOffset, 0);	// This line is needed to condition the user's input start color offset

	color_t value = getOffsetColor(data, startColorOffset);
	uint8_t bpp = getBytesPerPixel( );

	if( goLeft )
	{ 
		setMemoryAccessControl( false, true, false, false, true, false ); 
		x0 = (xExt - 1) - x0;
	}
	hd_hw_extent_t x1 = x0 + (len - 1);

	// Setup the valid area to draw...
	setColumnAddress( x0, x1);
	setRowAddress(y0, y0);

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRRAM;
	writePacket(&cmd);					// Send the command to enable writing to RAM but don't send any data yet

	// Now, we need to send data with as little overhead as possible, while respecting the start offset and color cycle length and everything else...
	selectDriver();
	digitalWrite(_dc, HIGH);
	_spi->beginTransaction(_spisettings);

	if(colorCycleLength == 1)
	{
		// Special case that can be handled with a lot less thinking (so faster)
		uint8_t speedupArry[ILI9163C_MAX_X*ILI9163C_MAX_BPP];
		for(uint16_t indi = 0; indi < len; indi++)
		{
			for(uint8_t indj = 0; indj < bpp; indj++)
			{
				speedupArry[ indj + (indi*bpp) ] = *((uint8_t*)(data) + indj);
			}
		}
		_spi->transfer(speedupArry, len*bpp);
	}
	else
	{
		uint16_t pixelsToDraw = 0;
		uint16_t pixelsDrawn = 0;

		while(len != 0)
		{
			// Let's figure out how many pixels we can draw right now contiguously.. (thats from the start offset to the full legnth of the cycle)
			uint16_t pixelsAvailable = colorCycleLength - startColorOffset;
			value = getOffsetColor(data, startColorOffset);
			
			if( pixelsAvailable >= len ){ pixelsToDraw = len; }
			else if( pixelsAvailable < len ){ pixelsToDraw = pixelsAvailable; }

			// Draw "pixelsToDraw" pixels using "bpp*pixelsToDraw" bytes
			_spi->transfer(value, bpp*pixelsToDraw);

			len -= pixelsToDraw;
			pixelsDrawn = pixelsToDraw;
			startColorOffset = getNewColorOffset(colorCycleLength, startColorOffset, pixelsDrawn);
		}
	}

	_spi->endTransaction();	
	deselectDriver();

	if( goLeft ){ setMemoryAccessControl( true, true, false, false, true, false ); } // Reset to defaults
}



void    ILI9163C_4WSPI::hwyline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data, hd_colors_t colorCycleLength, hd_colors_t startColorOffset, bool goUp)
{
	if(data == NULL){ return; } 
	if( len < 1 ){ return; }

	startColorOffset = getNewColorOffset(colorCycleLength, startColorOffset, 0);	// This line is needed to condition the user's input start color offset
	color_t value = getOffsetColor(data, startColorOffset);
	uint8_t bpp = getBytesPerPixel( );

	if( goUp )
	{ 
		setMemoryAccessControl( true, false, false, false, true, false ); 
		y0 = (yExt - 1) - y0; 
	}
	hd_hw_extent_t y1 = y0 + (len - 1);
	
	// Setup the valid area to draw...
	setColumnAddress( x0, x0);
	setRowAddress(y0, y1);

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRRAM;
	writePacket(&cmd);					// Send the command to enable writing to RAM but don't send any data yet

	// Now, we need to send data with as little overhead as possible, while respecting the start offset and color cycle length and everything else...
	selectDriver();
	digitalWrite(_dc, HIGH);
	_spi->beginTransaction(_spisettings);


	if(colorCycleLength == 1)
	{
		// Special case that can be handled with a lot less thinking (so faster)
		uint8_t speedupArry[ILI9163C_MAX_Y*ILI9163C_MAX_BPP];
		for(uint16_t indi = 0; indi < len; indi++)
		{
			for(uint8_t indj = 0; indj < bpp; indj++)
			{
				speedupArry[ indj + (indi*bpp) ] = *((uint8_t*)(data) + indj);
			}
		}
		_spi->transfer(speedupArry, len*bpp);
	}
	else
	{
		uint16_t pixelsToDraw = 0;
		uint16_t pixelsDrawn = 0;

		while(len != 0)
		{
			// Let's figure out how many pixels we can draw right now contiguously.. (thats from the start offset to the full legnth of the cycle)
			uint16_t pixelsAvailable = colorCycleLength - startColorOffset;
			value = getOffsetColor(data, startColorOffset);
			
			if( pixelsAvailable >= len ){ pixelsToDraw = len; }
			else if( pixelsAvailable < len ){ pixelsToDraw = pixelsAvailable; }

			// Draw "pixelsToDraw" pixels using "bpp*pixelsToDraw" bytes
			_spi->transfer(value, bpp*pixelsToDraw);

			len -= pixelsToDraw;
			pixelsDrawn = pixelsToDraw;
			startColorOffset = getNewColorOffset(colorCycleLength, startColorOffset, pixelsDrawn);
		}
	}

	_spi->endTransaction();	
	deselectDriver();

	if( goUp )
	{ 
		setMemoryAccessControl( true, true, false, false, true, false ); 
	}
}

// void 	ILI9163C_4WSPI::hwrectangle(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, bool filled, color_t data, hd_colors_t colorCycleLength, hd_colors_t startColorOffset, bool reverseGradient, bool gradientVertical)
// {
// // Hardware rectangle is left unimplemented because it's hard to squeeze out much more performance than the built-in function that uses our more efficient versions of hwxline and hwyline	
// }

void ILI9163C_4WSPI::hwfillFromArray(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, color_t data, hd_pixels_t numPixels, bool Vh)
{
	if(numPixels == 0){ return; }
	if(data == NULL ){ return; }

	uint8_t bpp = getBytesPerPixel();

	if( Vh )
	{ 
		setMemoryAccessControl( true, true, true, false, true, false );

		setColumnAddress( y0, y1);
		setRowAddress(x0, x1);
	}
	else
	{
		setColumnAddress( x0, x1);
		setRowAddress(y0, y1);
	}
	

	ILI9163C_CMD_t cmd = ILI9163C_CMD_WRRAM;
	writePacket(&cmd);					// Send the command to enable writing to RAM but don't send any data yet

	selectDriver();
	digitalWrite(_dc, HIGH);
	_spi->beginTransaction(_spisettings);

	_spi->transfer((uint8_t*)data, bpp*numPixels);

	_spi->endTransaction();	
	deselectDriver();

	if( Vh ){ setMemoryAccessControl( true, true, false, false, true, false ); }
}









////////////////////////////////////////////////////////////
//					KWH018ST03 Implementation			  //
////////////////////////////////////////////////////////////
char_info_t KWH018ST03_Default_CharInfo;
wind_info_t KWH018ST03_Default_Window;

KWH018ST03::KWH018ST03() : hyperdisplay(KWH018ST03_WIDTH, KWH018ST03_HEIGHT), ILI9163C_4WSPI(KWH018ST03_WIDTH, KWH018ST03_HEIGHT)
{

}

ILI9163C_STAT_t KWH018ST03::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, uint8_t blPin, uint8_t rdPin, SPIClass &spiInterface, uint32_t spiFreq)
{
	// Call the functions to setup the super classes
// Associate 
	_dc = dcPin;
	_rst = rstPin;
	_cs = csPin;
	_bl = blPin;
	_rd = rdPin;
	_spi = &spiInterface;

	if(spiFreq != ILI9163C_SPI_DEFAULT_FREQ)
	{
		setSPIFreq( spiFreq );
	}

	_spi->begin();

	// Set pinmodes
	pinMode(_cs, OUTPUT);
	pinMode(_rst, OUTPUT);
	pinMode(_dc, OUTPUT);
	pinMode(_bl, OUTPUT);
	pinMode(_rd, OUTPUT);

	// Set pins to default positions
	digitalWrite(_cs, HIGH);
	digitalWrite(_rst, HIGH);
	digitalWrite(_dc, HIGH);
	digitalWrite(_bl, HIGH);
	digitalWrite(_rd, HIGH);

	// Setup the default window
	setWindowDefaults(pCurrentWindow);

	// Power up the device

	// try starting SPI with a simple byte transmisssion to 'set' the SPI peripherals
	uint8_t temp_buff[1];
	_spi->beginTransaction(_spisettings);
	_spi->transfer(temp_buff, 1);
	_spi->endTransaction();

	startup();	
	defaultConfigure();

	return ILI9163C_STAT_Nominal;
}

ILI9163C_STAT_t KWH018ST03::defaultConfigure( void )
{
	ILI9163C_STAT_t retval = ILI9163C_STAT_Nominal;


	retval = sleepOut(); // WriteComm(0x11);
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

  	delay(20);

  	retval = selectGammaCurve( 0x04 );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setNormalFramerate( 0x0C, 0x14 );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setPowerControl1( 0x0C, 0x05 );
 	if(retval != ILI9163C_STAT_Nominal){ return retval; }

  	retval = setPowerControl2( 0x02 );
  	if(retval != ILI9163C_STAT_Nominal){ return retval; }

  	retval = setPowerControl3( 0x02 );
  	if(retval != ILI9163C_STAT_Nominal){ return retval; }

  	retval = setVCOMControl1( 0x20, 0x55 );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setVCOMOffsetControl( false, 0x40 );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setInterfacePixelFormat( 0x06 );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setColumnAddress( KWH018ST03_START_COL, KWH018ST03_STOP_COL );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setRowAddress( KWH018ST03_START_ROW, KWH018ST03_STOP_ROW );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setMemoryAccessControl( true, true, false, false, true, false );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setSrcDriverDir( false );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	retval = setGamRSel( true );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	uint8_t pgam[16] =  { 0x36, 0x29, 0x12, 0x22, 0x1C, 0x15, 0x42, 0xB7, 0x2F, 0x13, 0x12, 0x0A, 0x11, 0x0B, 0x06 };
	retval = setPositiveGamCorr( pgam );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

	uint8_t ngam[16] = { 0x09, 0x16, 0x2D, 0x0D, 0x13, 0x15, 0x40, 0x48, 0x53, 0x0C, 0x1D, 0x25, 0x2E, 0x34, 0x39 };
	setNegativeGamCorr( ngam );
	if(retval != ILI9163C_STAT_Nominal){ return retval; }

  	delay(20);

  	setPower( true );

  // WriteComm(0x2C);
  	return ILI9163C_STAT_Nominal;
}

void KWH018ST03::startup( void )
{
	// Assume that VDD and VCC are stable when this function is called
	digitalWrite( _rst , HIGH);
  	delay(10);
  	digitalWrite( _rst , LOW);
  	delay(10);
  	digitalWrite( _rst , HIGH);
  	delay(120);
	// Now you can do initialization
}

// void UG6464TDDBG01::getCharInfo(uint8_t val, char_info_t * pchar)
// {
// 	char_info_t * pcharinfo = &UG6464TDDBG01_Default_CharInfo;
// 	// Do stuff to fill out the default char info structure, then return a pointer to it
// 	pcharinfo->data = NULL;						// Color information for each pixel to be drawn
// 	pcharinfo->xLoc = NULL;						// X location wrt upper-left corner of char location(cursor) for each pixel
// 	pcharinfo->yLoc = NULL;						// Y location wrt upper-left corner of char location(cursor) for each pixel
//     pcharinfo->numPixels = 0;					// Number of pixels contained in the data, xLoc, and yLoc arrays
// 	pcharinfo->show = false;					// Whether or not to actually show the character
// 	pcharinfo->causedNewline = false;			// Whether or not the character triggered/triggers a new line
// }


void KWH018ST03::clearDisplay( void )
{
	// Store the old window pointer: 
	wind_info_t * ptempWind = pCurrentWindow;

	// Make a new default window
	wind_info_t window;
	pCurrentWindow = &window;

	// Ensure the window is set up so that you can clear the whole screen
	setWindowDefaults(&window);

	// Make a local 'black' color 
	ILI9163C_color_18_t black;
	black.r = 0;
	black.g = 0;
	black.b = 0;

	// Fill the temporary window with black
	fillWindow((color_t)&black);												// Pass the address of the buffer b/c we know it will be safe no matter what SSD1357 color mode is used

	// Restore the previous window
	pCurrentWindow = ptempWind;
}

void KWH018ST03::setWindowDefaults(wind_info_t * pwindow)
{
	// Fills out the default window structure with more or less reasonable defaults
	pwindow->xMin = KWH018ST03_START_COL;
	pwindow->yMin = KWH018ST03_START_ROW;
	pwindow->xMax = KWH018ST03_STOP_COL;
	pwindow->yMax = KWH018ST03_STOP_ROW;
	pwindow->cursorX = 0;							// cursor values are in window coordinates
	pwindow->cursorY = 0;
	pwindow->xReset = 0;
	pwindow->yReset = 0;
	
	pwindow->lastCharacter.data = NULL;
	pwindow->lastCharacter.xLoc = NULL;
	pwindow->lastCharacter.yLoc = NULL;
	pwindow->lastCharacter.xDim = 0;
	pwindow->lastCharacter.yDim = 0;
	pwindow->lastCharacter.numPixels = 0;
	pwindow->lastCharacter.show = false;
	pwindow->lastCharacter.causesNewline = false;
	
	pwindow->data = NULL;				// No window data yet
	setCurrentWindowColorSequence(NULL, 1, 0);	// Setup the default color (Which is NULL, so that you know it is not set yet)
}



