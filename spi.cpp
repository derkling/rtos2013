#include "spi.h"
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include "miosix.h"

using namespace miosix;
using namespace std;

//definizione gpio per l'uso della spi
typedef Gpio<GPIOA_BASE,1> interruptLine;//linea interrupt del modulo wireless
typedef Gpio<GPIOA_BASE,8> powerLine;//linea per alimentare il modulo wireless(da chiedere al prof se compatibile con altri moduli)
typedef Gpio<GPIOB_BASE,11> cen;//linea enable del modulo wireless
typedef Gpio<GPIOB_BASE,12> cs;//ss spi
typedef Gpio<GPIOB_BASE,13> sck;//clock spi
typedef Gpio<GPIOB_BASE,14> miso;//miso spi
typedef Gpio<GPIOB_BASE,15> mosi;//mosi spi


//abilita la spi configurando i registri
void configureSpi(){

	//Abilito i clock alle periferiche che uso(spi2, gpioa e gpiob)
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;//abilito il clock alla spi2
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;//abilito il clock ai gpio a e b che usiamo

	//imposto gpiob 12,13,14,15 per spi
	cs::mode(Mode::OUTPUT);
	sck::mode(Mode::ALTERNATE);
	miso::mode(Mode::ALTERNATE);
	mosi::mode(Mode::ALTERNATE);

	cs::high();
	sck::alternateFunction(5);
	miso::alternateFunction(5);
	mosi::alternateFunction(5);

	//imposto gpioa 9 per l'alimentazione
	powerLine::mode(Mode::OUTPUT);
	powerLine::low();

	//imposto gpiob 11 in output digitale per chip enable
	cen::mode(Mode::OUTPUT);
	cen::low();
	
	//configurazione gestione interrupt
	interruptLine::mode(Mode::INPUT_PULL_UP);
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR1_EXTI1_PA;
	EXTI->IMR |= EXTI_IMR_MR1;
	EXTI->RTSR &= ~EXTI_RTSR_TR1;
	EXTI->FTSR |= EXTI_FTSR_TR1;
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_SetPriority(EXTI1_IRQn,15);

	

	//imposto il control register 1 e 2 della spi
	SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 ;//imposta a velocità di trasmissione a 0.65mbps(più lenta possibile)
	SPI2->CR1 &= ~SPI_CR1_CPHA;//impostato campionamento sul primo fronte di clock
	SPI2->CR1 &= ~SPI_CR1_CPOL;//impostato clock idle basso
	SPI2->CR1 &= ~SPI_CR1_DFF;//impostato frame da 8 bit
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;//impostato msb
	SPI2->CR1 |= SPI_CR1_SSM;//il management del SS è software
	SPI2->CR2 |= SPI_CR2_SSOE;//abilito l'uscita SS
	SPI2->CR1 |= SPI_CR1_MSTR;//imposto come master
	SPI2->CR1 |= SPI_CR1_SPE;//enable della spi

	return;

}

//alza il pin di alimentazione del modulo wireless
void powerLineUp(){
	powerLine::high();
}

//abbassa il pin di alimentazione del modulo wireless
void powerLineDown(){
	powerLine::low();
}

//alza il pin CE
void chipEnable(){
	cen::high();
}

//abbassa il pin CE
void chipDisable(){
	cen::low();
}

//manda via spi un comando e eventuali dati
int spiSendCommandWriteData(uint8_t command, uint8_t addr,uint8_t* sr, uint8_t* data, int lenght){
	
	int i=0;
	uint16_t temp;

	if ( lenght > 0 && data == NULL ){//errore nell'uso dei parametri
		return -1;
	}

	temp = (uint16_t)command | (uint16_t)addr;

	while( ( SPI2->SR & SPI_SR_BSY ) != 0 ){//controllo che non sia occupata spi, se si ritorno errore
		i++;//i usato come timeout
		if(i>100){
			return -1;
		}
	}

	i=0;//i usato per tenere traccia del prossimo dato da inviare

	while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//aspetto che registro trasmissione sia vuoto(probabilmente istruzione inutile)
	
	cs::low();//attivo il cs(lo abbasso)
	usleep(1);//tempo da attendere prima che il modulo wireless sia attivo

	SPI2->DR = temp;//inserisco comando nel data register
	
	while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//aspetto che sia copiato nel registro di invio

	if( lenght > 0 ){//inserisco nel data register il prossimo dato da inviare via spi(se c'è)
		temp = (uint16_t)data[i];
		i++;
		SPI2->DR = temp;
	}

	while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto che arrivi lo status register

	temp = SPI2->DR;//leggo lo status register

	if ( sr != NULL ){//se sr punta a un indirizzo di memoria valido passo lo status register
			*sr = (uint8_t)temp;
	}

	if( lenght == 0 ){//se non c'erano dati da inviare finito
		usleep(1);
		cs::high();//disabilito il cs(alzo cs)
		return 1;
	}

	while( i < lenght ){//c'è ancora da scrivere? se si entro nel loop

		while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//attendo che sia copiato il dato precedente dal DR allo shift register

		temp = (uint16_t)data[i];
		i++;
		SPI2->DR = temp;//inserisco prossimo dato nel DR

		while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto che arrivi la risposta al vecchio dato(sensa significato)

		temp = SPI2->DR;//leggo la risposta solo per non far andare in overrun(lettura non usata)

	}

	while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto la trasmissione dell'ultimo dato

	temp = SPI2->DR;

	usleep(1);
	cs::high();//disabilito cs(lo alzo)

	return 1;

}

//manda via spi un comando e riceve dati
int spiSendCommandReadData(uint8_t command, uint8_t addr,uint8_t* sr, uint8_t* data, int lenght){
	
	int i=0;
	uint16_t temp;

	if ( lenght > 0 && data == NULL ){//errore nell'uso dei parametri
		return -1;
	}

	temp = (uint16_t)command | (uint16_t)addr;

	while( ( SPI2->SR & SPI_SR_BSY ) != 0 ){//controllo che non sia occupata spi, se si ritorno errore
		i++;//i usato come timeout
		if(i>100){
			return -1;
		}
	}

	i=0;//i usato per tenere traccia del numero di dati ricevuti

	while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//aspetto che registro trasmissione sia vuoto(probabilmente istruzione inutile)

	cs::low();//abilito il cs(lo abbasso)
	usleep(1);

	SPI2->DR = temp;//inserisco comando nel data register
	
	while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//aspetto che sia copiato nel registro di invio

	if( lenght > 0 ){//siccome devo ricevere dati invio dati senza senso via spi
		temp = 0x0000;
		i++;
		SPI2->DR = temp;
	}

	while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto che arrivi lo status register

	temp = SPI2->DR;//leggo lo status register

	if ( sr != NULL ){//se sr punta a un indirizzo di memoria valido passo lo status register
			*sr = (uint8_t)temp;
	}

	if( lenght == 0 ){//se non c'erano dati da inviare finito
		usleep(1);
		cs::high();//disabilito cs(lo alzo)
		return 1;
	}

	while( i < lenght ){//c'è ancora da leggere

		while( ( SPI2->SR & SPI_SR_TXE ) == 0 ){}//attendo che sia copiato il dato precedente dal DR allo shift register

		temp = 0x0000;
		i++;
		SPI2->DR = temp;//inserisco prossimo dato nel DR

		while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto che arrivi la risposta precedente

		temp = SPI2->DR;//leggo la risposta precedente

		data[i-2] = (uint8_t)temp;
	}

	while( ( SPI2->SR & SPI_SR_RXNE ) == 0 ){}//aspetto la ricezione dell'ultimo dato

	temp = SPI2->DR;

	usleep(1);
	cs::high();//disabilito cs(lo alzo)

	data[i-1] = (uint8_t)temp;

	return 1;

}
