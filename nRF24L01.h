#ifndef NRF24L01_H
#define NRF24L01_H

#include<stdint.h>

/********************  DEFINIZIONE COMANDI DA PASSARE AL MODULO WIRELESS  ********************/
#define R_REGISTER 				((uint8_t)0x00)		/*!<Read command and status register */
#define W_REGISTER 				((uint8_t)0x20)		/*!<Write command and status register */
#define R_RX_PAYLOAD 				((uint8_t)0x61)		/*!<Read RX-payload */
#define W_TX_PAYLOAD 				((uint8_t)0xA0)		/*!<Write TX-payload */
#define FLUSH_TX 				((uint8_t)0xD1)		/*!<Flush TX-fifo, used in TX mode */
#define FLUSH_RX 				((uint8_t)0xD2)		/*!<Flush RX-fifo. used in RX mode */
#define REUSE_TX_PL 				((uint8_t)0xD3)		/*!<Used for a PTX device reuse last transmitted payload */
#define ACTIVATE 				((uint8_t)0x50)		/*!<Active the extra features.(see down) */
#define NOP 					((uint8_t)0xFF)		/*!<NoOperation. Can be used for read status register */
//i successivi sono attivabili solo dopo aver inviato il comando ACTIVATE e il dato ACTIVATE_BYTE
#define R_RX_PL_WID 				((uint8_t)0x60)		/*!<Read RX-payload width for the top R_RX_PAYLOAD in the RX fifo */
#define W_ACK_PAYLOAD 				((uint8_t)0xA8)		/*!<Used in RX mode */
#define W_TX_PAYLOAD_NOACK 			((uint8_t)0xB0)		/*!<Used in TX mode. Disable autoack */

/********************  DEFINIZIONE DI INDIRIZZI DEI REGISTRI MODULO NRF24L  ********************/ 
#define COMMAND_WITHOUT_ADDRESS			((uint8_t)0x00)		/*!<Indirizzo vuoto(se il comando non usa il campo indirizzo */
#define CONFIG					((uint8_t)0x00)		/*!<Configuration register */
#define EN_AA 					((uint8_t)0x01)		/*!<Enable auto acknowledgment in ShockBurst */
#define EN_RXADDR 				((uint8_t)0x02)		/*!<Enable RX addresses */
#define SETUP_AW 				((uint8_t)0x03)		/*!<Setup of address widths */
#define SETUP_RETR				((uint8_t)0x04)		/*!<Setup of automatic retransmission */
#define RF_CH					((uint8_t)0x05)		/*!<RF channel */
#define RF_SETUP				((uint8_t)0x06)		/*!<RF setup register */
#define STATUS					((uint8_t)0x07)		/*!<Status register. Transmitted in parallel with command */
#define OBSERVE_TX				((uint8_t)0x08)		/*!<Transmit observe register */
#define CD					((uint8_t)0x09)		/*!<Carrier detect */
#define RX_ADDR_P0				((uint8_t)0x0A)		/*!<Receive address data pipe 0 */
#define RX_ADDR_P1				((uint8_t)0x0B)		/*!<Receive address data pipe 1 */
#define RX_ADDR_P2				((uint8_t)0x0C)		/*!<Receive address data pipe 2 */
#define RX_ADDR_P3				((uint8_t)0x0D)		/*!<Receive address data pipe 3 */
#define RX_ADDR_P4				((uint8_t)0x0E)		/*!<Receive address data pipe 4 */
#define RX_ADDR_P5				((uint8_t)0x0F)		/*!<Receive address data pipe 5 */
#define TX_ADDR					((uint8_t)0x10)		/*!<Transmit address. Used for PTX device only */
#define RX_PW_P0				((uint8_t)0x11)
#define RX_PW_P1				((uint8_t)0x12)
#define RX_PW_P2				((uint8_t)0x13)
#define RX_PW_P3				((uint8_t)0x14)
#define RX_PW_P4				((uint8_t)0x15)
#define RX_PW_P5				((uint8_t)0x16)
#define FIFO_STATUS				((uint8_t)0x17)		/*!<FIFO status register */
//registri ACK_PLD TX_PLD RX_PLD scritti con apposito comando spi
#define DYNPD					((uint8_t)0x1C)		/*!<Enable dynamic payload lenght */
#define FEATURE					((uint8_t)0x1D)		/*!<Feature register */

/********************  BIT DEFINITION FOR NRF24L REGISTERS  ********************/

/********************  bit definition for CONFIG register  ********************/
#define CONFIG_MASK_RX_DR			((uint8_t)0x40)		/*!<Mask Interrept caused by RX_DR */
#define CONFIG_MASK_TX_DS			((uint8_t)0x20)		/*!<Mask interrupt caused by TX_DS */
#define CONFIG_MASK_MAX_RT			((uint8_t)0x10)		/*!<Mask interrupt caused by MAX_RT */
#define CONFIG_EN_CRC				((uint8_t)0x08)		/*!<Enable CRC */
#define CONFIG_CRCO				((uint8_t)0x04)		/*!<CRC encoding scheme */
#define CONFIG_PWR_UP				((uint8_t)0x02)		/*!<1: POWER UP, 0: POWER DOWN */
#define CONFIG_PRIM_RX				((uint8_t)0x01)		/*!<RX/TX control. 1: PRX, 0: PTX */

/********************  bit definition for EN_AA register  ********************/
#define EN_AA_ENAA_P5				((uint8_t)0x20)		/*!<Enable auto acknowledgement data pipe 5 */
#define EN_AA_ENAA_P4				((uint8_t)0x10)		/*!<Enable auto acknowledgement data pipe 4 */
#define EN_AA_ENAA_P3				((uint8_t)0x08)		/*!<Enable auto acknowledgement data pipe 3 */
#define EN_AA_ENAA_P2				((uint8_t)0x04)		/*!<Enable auto acknowledgement data pipe 2 */
#define EN_AA_ENAA_P1				((uint8_t)0x02)		/*!<Enable auto acknowledgement data pipe 1 */
#define EN_AA_ENAA_P0				((uint8_t)0x01)		/*!<Enable auto acknowledgement data pipe 0 */

/********************  DEFINIZIONE DATI PARTICOLARI  ********************/
#define ACTIVATE_BYTE 				((uint8_t)0x73)		/*!<Dato da inviare dopo ACTIVATE per usare altre features */

/*********************  CONFIGURAZIONE  ************************************/
#define RF_SETUP_1MBPS                          ((uint8_t)0x07) 	/*!<Air Data 1mbps, TX Power 0dbm, LNA Gain Enabled*/
#define FEATURE_EN_DPL                          ((uint8_t)0x05) 	/*!<EN_DPL 1, EN_ACK_PAY 0, EN_DYN_ACK 1*/
#define DYNPD_DPL_P0                            ((uint8_t)0x01) 	/*!<DPL_P0 1: Enable dynamic payload on data pipe 0*/
#define EN_AA_ENAA_P0                           ((uint8_t)0x01) 	/*!<Enable auto ack on data pipe 0*/
#define SETUP_RETR_ARC0                         ((uint8_t)0x00) 	/*!<Automatic Retransmission Count 0*/
#define EN_RXADDR_ERX_P0                        ((uint8_t)0x01) 	/*!<Activate only data pipe 0*/
#define CONFIG_PWRUP_PRIM_RX                    ((uint8_t)0x0B) 	/*!<Power Up, Primary RX*/
#define CONFIG_PWRUP_PRIM_TX                    ((uint8_t)0x0A) 	/*!<Power Up, Primary TX*/
#define CONFIG_PWRDOWN_PRIM_RX                  ((uint8_t)0x09) 	/*!<Power Down, Primary TX*/

#endif //NRF24L01_H
