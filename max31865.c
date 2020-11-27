 /* This library is written for Stm32f4 microcontrollers using the stm32-HAL software library. */

#include <max31865.h>
#define MAX31865_SPI1_CS_LOW HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, RESET)	  /* MAX31865 SPI1 CS LOW(ENABLE) - 3wire */
#define MAX31865_SPI2_CS_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, RESET)  /* MAX31865 SPI2 CS LOW(ENABLE) - 3wire*/
#define MAX31865_SPI3_CS_LOW HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, RESET)  /* MAX31865 SPI3 CS LOW(ENABLE) - 3wire*/
#define MAX31865_SPI4_CS_LOW HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4, RESET)	  /* MAX31865 SPI4 CS LOW(ENABLE) - 3wire*/
#define MAX31865_SPI5_CS_LOW HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6, RESET)	  /* MAX31865 SPI5 CS LOW(ENABLE) - 2 or 4 wire*/


void MAX31865_3WIRE_SPI_CS_LOW();
void MAX31865_3WIRE_SPI_CS_HIGH();
void MAX31865_2or4WIRE_SPI_CS_LOW();
void MAX31865_2or4WIRE_SPI_CS_HIGH();

/*****************************************************************************
  SUB-RUTINE..: Init_MAX31865
  Function....: 포트 초기화 및 센서 종류 설정
  Description.:
****************************************************************************/
void Init_MAX31865(void)
{
	MAX31865_setWires(MAX31865_3WIRE);
		MAX31865_enableBias(i, 0);
		/*MAX31865_autoConvert(i, false);*/
		MAX31865_autoConvert(i, 1);
		MAX31865_clearFault(i);

}
/*

void MAX31865_Init(uint8_t wires)									 /* SP11 ~ SPI6 Init                    */
{																     /* wires = Set 2 or 3 or 4 wire sensor */

    // Datalines in reset state
	MAX31865_3WIRE_SPI_CS_HIGH();
	__HAL_SPI_ENABLE(&hspi1);
	HAL_SPI_Transmit(&hspi1, 0xFF, 1, 10);

    setWires(3);          			/* wires = input 2 or 3 or 4 wire sensor */
    enableBias(OFF);           // Disable bias voltage
    autoConvert(OFF);          // Disable auto conversion
}

*/

void setWires(uint8_t numwires)
{
    uint8_t status;
    MAX31865_read(MAX31856_CONFIG_REG, &status, 1);			  /* MAX31856_CONFIG_REG   = 0x00 */

    if (numwires == 3)  								/* 3-wire  */
    {
        status |= MAX31856_CONFIG_3WIRE;						/* MAX31856_CONFIG_3WIRE = 0x10 */
    } else 			 									/* 2or4 wire*/
    {
        status &= ~MAX31856_CONFIG_3WIRE;						/* ~MAX31856_CONFIG_3WIRE = 0xEF */
    }

    MAX31865_write(MAX31856_CONFIG_REG, status);       /* MAX31856_CONFIG_REG   = 0x00 */
}

void MAX31865_enableBias(uint8_t ch, uint8_t b)
{
	uint8_t t = MAX31865_readRegister8(ch, MAX31865_CONFIG_REG);

	if (b == 1)
	{
		t |= 0x80;		/* enable bias*/
	}
	else
	{
		t &= ~0x80;		/* disable bias*/
	}

	MAX31865_writeRegister8(ch, MAX31865_CONFIG_REG, t);
}

void MAX31865_setWires(max31865_numwires_t wires )
{
	uint8_t t = MAX31865_readRegister8(wires, MAX31865_CONFIG_REG);     /* t = 0x01 */

	if (wires == MAX31865_3WIRE)
	{
		t |= MAX31865_CONFIG_3WIRE;                                // 0x11 = t(0x01) |MAX31865_CONFIG_3WIRE(0x10)
	}
	else
	{
		// 2 or 4 wire
		t &= ~MAX31865_CONFIG_3WIRE;								//0x01  &=   0xEF   11101111
	}

	MAX31865_writeRegister8(ch, MAX31865_CONFIG_REG, t);
}



void MAX31865_readRegister8(uint8_t wires, const uint8_t raddr)   /* raddr=  MAX31856_CONFIG_REG(0x00) or MAX31856_RTDMSB_REG(0x01)  */
{

	uint8_t rdata = 0;

	if(wires==3)
	{
		MAX31865_3WIRE_SPI_CS_LOW(); 							/* 3wire CS Enable*/
	}

	else
	{
		MAX31865_4WIRE_SPI_CS_LOW();							/* 4wire CS Enable*/
	}

	MAX31865_add_write((raddr&(uint8_t)0x7F));          /* raddr = 0x00 -> raddr&(uint8_t)0x7F = 0   or  0x01 -> raddr&(uint8_t)0x7F = 1 */

	rdata = (uint8_t)MAX31865_read_N(ch, 8);			/* setwire - rdata=0x01 */

	if(wire==3)
	{
		MAX31865_3WIRE_SPI_CS_HIGH(); 							/* 3wire CS Disable*/
	}
	else
	{
		MAX31865_4WIRE_SPI_CS_HIGH();							/* 3wire CS enable*/
	}

	return rdata;

}
void MAX31865_read(uint8_t addr, uint8_t *buffer, uint8_t len)	/* addr = MAX31856_CONFIG_REG = 0x00
																		or MAX31856_RTDMSB_REG = 0x01       */
{
    addr &= ~MAX31865_READ;                                     /*   0x00 = addr & MAX31865_READ = read bit on address = 0x00,
    																					~MAX31865_READ = 0xFF*/

    MAX31865_3WIRE_SPI_CS_LOW();        							/* MAX31865 ALL SPI CS LOW(ENABLE)  */

    spi_write(&addr, 1);                                        // Write addr      addr= 0x00 or 0x01
    spi_read(buffer, len);                                      // read data

    MAX31865_3WIRE_SPI_CS_HIGH();         						/* MAX31865 SPI1 CS HIGH(DISABLE) */
}

void MAX31865_add_write(const uint8_t wAdd)              /*  setwire-> wAdd=0x00     	  */
{
	uint8_t i = 0;
	uint8_t ade_aw_mask = 0x80;
	uint8_t send3_2byte[2]={0x00,};

	for (i = 0; i < 8U; i++)
	{

	//	if ((wAdd & ade_aw_mask) != 0U)

		  	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, RESET);

			    	  HAL_SPI_Transmit(&hspi1, send3_2byte, 2, 1);

			        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, SET);

														/*	setwire-> wAdd & ade_aw_mask =0x00     */



		ade_aw_mask = (ade_aw_mask >> 1);         		     /*    10000000 >> 1    */
	}
}

uint16_t MAX31865_read_N(uint8_t ch, const uint8_t dwLength) /* setwire - MAX31865_readRegister8 = dwlength 8 */
{
	uint8_t i = 0;
	uint16_t read_mask = 0;
	uint8_t ade_dw_length = 0;
	uint16_t r_data = 0;
	uint8_t miso_mask;

	if(ch == 1U)
	{
		miso_mask = 0x40;
	}
	else if(ch == 2U)
	{
		miso_mask = 0x80;
	}
	else
	{
		miso_mask = 0x20;
	}

	if (dwLength > 8U)
	{
		ade_dw_length = 16;
		read_mask = 0x8000;
	}
	else							/* setwire - MAX31865_readRegister8 = dwLength 8    */
	{
		ade_dw_length = 8;
		read_mask = 0x80;
	}

	for (i = 0; i < ade_dw_length; i++)       /*setwire - MAX31865_readRegister8 =ade_dw_length 8 */
	{
		uint8_t dumydata[1]= {0};
		HAL_SPI_Transmit(&hspi,(uint8_t *)dumydata, 1, 1000);


			r_data |= read_mask;         	  /* 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01  */

		read_mask = (read_mask >> 1);        /* 10000000 >> 1 */
	}
	return r_data;							/* 8 r_data = 0x01 */
}

void MAX31865_writeRegister8(uint8_t ch, const uint8_t reg_wAdd, const uint16_t reg_wData)   /*        0    , 1  */
{
	if()
	MAX31865_3WIRE_SPI_CS_LOW();					/* 해당 채널 Enable*/

	MAX31865_add_wriuint8_t)0x80te((reg_wAdd | ());
	//_delay_us(2);
	MAX31865_data_write(reg_wData, 8);

	MAX31865_Disable_Ch(ch);				/* 해당 채널 Disable*/
}

void spi_write(uint8_t *data, uint8_t len)
{
    for (uint8_t x = 0; x < len; x++)
    {
        for (int8_t i = 7; i >= 0; i--)
        {
           HAL_SPI_Transmit(&hspi1, data[x], len, 1000);
           HAL_SPI_Transmit(&hspi2, data[x], len, 1000);
           HAL_SPI_Transmit(&hspi3, data[x], len, 1000);
           HAL_SPI_Transmit(&hspi4, data[x], len, 1000);

        }
    }

}



void MAX31865_3WIRE_SPI_CS_LOW()
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, RESET);			/* SPI1 CS HIGH */
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, RESET);			/* SPI2 CS HIGH */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, RESET);			/* SPI3 CS HIGH */
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4, RESET);			/* SPI4 CS HIGH */

}

void MAX31865_3WIRE_SPI_CS_HIGH()
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, SET);			/* SPI1 CS HIGH */
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12, SET);			/* SPI2 CS HIGH */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, SET);			/* SPI3 CS HIGH */
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4, SET);			/* SPI4 CS HIGH */

	//HAL_GPIO_WritePin(GPIOG,GPIO_PIN_8, SET);			/* SPI6 CS HIGH */
}

void MAX31865_2or4WIRE_SPI_CS_LOW()
{

	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6, RESET);			/* SPI5 CS LOW */

}


void MAX31865_2or4WIRE_SPI_CS_HIGH()
{

	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6, SET);			/* SPI5 CS HIGH */

}
/*

void MAX31865_3WIRE_SPI_Transmit(uint8_t x)
{

	  HAL_SPI_Transmit(&hspi2, (uint8_t *)data, 1, 1000);
	  HAL_SPI_Transmit(&hspi3, (uint8_t *)data, 1, 1000);
	  HAL_SPI_Transmit(&hspi4, (uint8_t *)data, 1, 1000);
}

*/
