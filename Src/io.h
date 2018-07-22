
#ifndef INCLUDE_IO
#define INCLUDE_IO

#define IOSetDDR
#define IOSetPinLo(port,pin)	port &= ~(1 << pin)
#define IOSetPinHi(port,pin)	port |= (1 << pin)
#define IOSetPin(port,pin,val)	port = ((port & ~(1 << pin)) | (val << pin))
#define IOGetPin(port,pin)		((port & (1 << pin)) >> (pin))
#define IOSetPort(port,val)		port = val

///// Any changes to the below pins should also be reflected in the DDR settings in IOInit()
//// Mitsumi based modules

#ifdef _MITSUMI_

#define PORT_POWERUP			PORTA
#define PORT_LED				PORTA
#define PORT_BT_CONNECT			PINC
#define PORT_BT_CFG				PORTC
#define PORT_BT_ACTIVE_MODE		PORTC
#define PORT_BT_BUSY			PINC
#define PORT_BT_RESET			PORTA
#define PORT_BUTTON				PINE
#define PORT_RECHARGE			PING

#define PIN_POWERUP				PINA2
#define PIN_LED_GREEN			PINA3
#define PIN_LED_RED				PINA4
#define PIN_BT_CFG				PINC0
#define PIN_BT_CONNECT			PINC1
#define PIN_BT_BUSY				PINC2
#define PIN_BT_ACTIVE_MODE		PINC3
#define PIN_BT_RESET			PINA7
#define PIN_BUTTON				PINE4
#define PIN_RECHARGE			PING0

// TI6700 IO ports and pins
#define SCLK_PORT				PORTE
#define SCLK_PORT_IN			PINE
#define SCLK_PIN				PINE5
#define DIN_PORT				PORTB
#define DIN_PIN					PINB5
#define DOUT_PORT				PINA
#define DOUT_PIN				PINA1
#define MERR_PORT				PORTA
#define MERR_PIN				PINA0

//// old non-Mitsumi based modules
#else

#define PORT_POWERUP			PORTA
#define PORT_LED				PORTA
#define PORT_BT_CONNECT			PIND
#define PORT_BT_CFG				PORTA
#define PORT_BUTTON				PIND
#define PORT_BT_BUSY			PINC


#define PIN_POWERUP				PINA2
#define PIN_LED_GREEN			PINA3
#define PIN_LED_RED				PINA4
#define PIN_BT_CONNECT			PIND1
#define PIN_BT_CFG				PINA7
#define PIN_BT_BUSY				PINC2
#define PIN_BUTTON				PIND0

// TI6700 IO ports and pins
#define SCLK_PORT				PORTE
#define SCLK_PORT_IN			PINE
#define SCLK_PIN				PIN5
#define DIN_PORT				PORTB
#define DIN_PIN					PIN5
#define DOUT_PORT				PINA
#define DOUT_PIN				PIN1
#define MERR_PORT				PORTA
#define MERR_PIN				PINA0

#endif

void		IOInit(void);
BOOLEAN 	IODebounce(INT8U port, INT8U pin);


#endif



