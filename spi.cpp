#include "spi.h"
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include "miosix.h"

using namespace miosix;

//definisco i gpio che usiamo
typedef Gpio<GPIOA_BASE,1> interruptLine;//linea interrupt del modulo wireless
typedef Gpio<GPIOB_BASE,11> cen;//linea enable del modulo wireless
typedef Gpio<GPIOB_BASE,12> cs;//ss spi
typedef Gpio<GPIOB_BASE,13> sck;//clock spi
typedef Gpio<GPIOB_BASE,14> miso;//miso spi
typedef Gpio<GPIOB_BASE,15> mosi;//mosi spi

//prossime due funzioni sono l'interrupt handler
void __attribute__((naked)) EXTI1_IRQHandler(){

	saveContext();
	asm volatile("bl _Z16EXTI1HandlerImplv");
	restoreContext();

}

/*! @brief routine di interrupt scatenato dal modulo wireless
 * 	   verrà chiamato quando arriva un pacchetto o finisce la trasmissione
 */
void __attribute__((used)) EXTI1HandlerImpl(){

	EXTI->PR=EXTI_PR_PR1;

	//TO DO gestione interrupt del modulo wireless(arriva quando trasmissione finita o pacchetto ricevuto

}

/*! @brief configura la periferica spi2 e i gpio usati per la comunicazione con il modulo wireless
 */
void configureSpi(){

	//Abilito i clock alle periferiche che uso(spi2, gpioa e gpiob)
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;//abilito il clock alla spi2
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;//abilito il clock ai gpio a e b che usiamo

	//imposto gpiob 12,13,14,15 per spi
	cs::mode(Mode::ALTERNATE);
	sck::mode(Mode::ALTERNATE);
	miso::mode(Mode::ALTERNATE);
	mosi::mode(Mode::ALTERNATE);

	cs::alternateFunction(5);
	sck::alternateFunction(5);
	miso::alternateFunction(5);
	mosi::alternateFunction(5);


	//imposto gpio 11 in output digitale per chip enable
	cen::mode(Mode::OUTPUT);
	cen::low();//non attivo il modulo wireless
	
	//impostazione interrupt(DA CONTROLLARE!!!!!!)
	//essendo pa1 interrupt non devo impostare bit in siscfg_exticr1(va messo 0 nel registro e già ci dovrebbe essere)
	interruptLine::mode(Mode::INPUT_PULL_DOWN);
	EXTI->IMR |= EXTI_IMR_MR1;
	EXTI->RTSR |= EXTI_RTSR_TR1;
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_SetPriority(EXTI1_IRQn,15);//linea che non so bene cosa faccia(copiata dal prof)

	//imposto il control register 1
	SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_2 ;//imposta a velocità di trasmissione a 2 MHz
	SPI2->CR1 &= !SPI_CR1_CPHA;//impostato campionamento sul primo fronte di clock
	SPI2->CR1 &= !SPI_CR1_CPOL;//impostato clock idle basso
	SPI2->CR1 &= !SPI_CR1_DFF;//impostato frame da 8 bit
	SPI2->CR1 &= !SPI_CR1_LSBFIRST;//impostato così a 0 manda prima msb
	SPI2->CR1 |= SPI_CR1_SSM;//il management del SS è software
	SPI2->CR1 |= SPI_CR1_SSI;//impostato SS alto
	SPI2->CR1 |= SPI_CR1_MSTR;//imposto come master
	SPI2->CR1 |= SPI_CR1_SPE;//enable della spi

	//imposto il control register 2(DA CONTROLLARE)
	//INOLTRE DA CONTROLLARE SE USARE CONNESSIONE FULL-DUPLEX O HALF-DUPLEX
	SPI2->CR2 |= SPI_CR2_SSOE;//abilito l'uscita SS 

	return;

}

/*! @brief alza il bit di enable del modulo wireless
 * 	   Così il modulo può passare a rx o tx mode
 */
void chipEnable(){
	cen::high();
}

/*! @brief abbassa il bit di CE
 *         Il modulo wireless non può entrare in stato di rx o tx, solo standy o power-down
 */
void chipDisable(){
	cen::low();
}

/*! @brief abbassa l'uscita di slave select(cioè seleziona lo slave)
 */
void slaveSelectOn(){
	SPI2->CR1 &= !SPI_CR1_SSI;
}

/*! @brief alza l'uscita di slave select(cioè non seleziona lo slave)
 */
void slaveSelectOff(){
	SPI2->CR1 |= SPI_CR1_SSI;
}

/*! @brief manda via spi il comando command al modulo wireless. Primitiva bloccante, non torna finchè errore o invio completo
 *  @param command comando da inviare
 *  @param addr indirizzo del modulo wireless
 *  @return -1 se spi transmission buffer not empty, 1 se inviato
 */
int spiSendCommand(uint8_t command, uint8_t addr){
	
	int i=0;

	while( SPI2->SR & SPI_SR_TXE == 0 ){
		i++;
		if(i>100){
			return -1;
		}
	}
	
	SPI2->DR = command | addr;//da controllare per il tipo uint16 di SPI2->DR e uint8 di command e addr

	while( SPI2->SR & SPI_SR_TXE == 0 ){}
	
	return 1;

}


int spiSendData(uint8_t* buffer, int len){


}
