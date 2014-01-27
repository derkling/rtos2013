#include <stdio.h>
#include "player.h"
#include "slice-and-play.h"
#include "suoni/array-sound.h"


//creo la classe ring






ring& ring::instance()
{
	static ring singleton;
	return singleton;
}

//funzione che permette di scrivere le unità
void ring::say_unit (int unit){

   
    
//crea gli stream da riprodurre
    
ADPCMSound uno  (uno_bin,uno_bin_len);
ADPCMSound due  (due_bin,due_bin_len);
ADPCMSound tre  (tre_bin,tre_bin_len);
ADPCMSound quattro  (quattro_bin,quattro_bin_len);
ADPCMSound cinque  (cinque_bin,cinque_bin_len);
ADPCMSound sei  (sei_bin,sei_bin_len);
ADPCMSound sette  (sette_bin,sette_bin_len);
ADPCMSound otto  (otto_bin,otto_bin_len);
ADPCMSound nove  (nove_bin,nove_bin_len);

 




	switch(unit){

		case 1: {
			
			
			Player::instance().play(uno);
			}	
		break;

		case 2:
                         
			Player::instance().play(due);
		break;

		case 3: 
                         
			Player::instance().play(tre);
		break;

		case 4: 
                         
			Player::instance().play(quattro);
		break;

		case 5: 
                         
			Player::instance().play(cinque);
		break;

		case 6: 
                         
			Player::instance().play(sei);
		break;

		case 7: 
                         
			Player::instance().play(sette);
		break;

		case 8: 
                         
			Player::instance().play(otto);
		break;

		case 9: 
                         
			Player::instance().play(nove);
		break;

		}

	

}
//permette di scrivere le decine
void ring::say_tens (int tens){
	
ADPCMSound venti  (venti_bin,venti_bin_len);
ADPCMSound trenta  (trenta_bin,trenta_bin_len);
ADPCMSound quaranta  (quaranta_bin,quaranta_bin_len);
ADPCMSound cinquanta  (cinquanta_bin,cinquanta_bin_len);
ADPCMSound sessanta  (sessanta_bin,sessanta_bin_len);
ADPCMSound settanta  (settanta_bin,settanta_bin_len);
ADPCMSound ottanta  (ottanta_bin,ottanta_bin_len);
ADPCMSound novanta  (novanta_bin,novanta_bin_len);


	switch(tens){

		case 2: 
                         
			Player::instance().play(venti);
		break;

		case 3: 
                         
			Player::instance().play(trenta);
		break;

		case 4: 
                         
			Player::instance().play(quaranta);
		break;

		case 5: 
                         
			Player::instance().play(cinquanta);
		break;

		case 6: 
                         
			Player::instance().play(sessanta);
		break;

		case 7: 
                         
			Player::instance().play(settanta);
		break;

		case 8:
                         
			Player::instance().play(ottanta);
		break;

		case 9: 
                         
			Player::instance().play(novanta);
		break;
	}	

	
}
// permette di scrivere i numeri dal 10 al 19
void ring::say_tens_and_unit(int tensun){


ADPCMSound dieci  (dieci_bin,dieci_bin_len);
ADPCMSound undici  (undici_bin,undici_bin_len);
ADPCMSound dodici  (dodici_bin,dodici_bin_len);
ADPCMSound tredici  (tredici_bin,tredici_bin_len);
ADPCMSound quattordici  (quattordici_bin,quattordici_bin_len);
ADPCMSound quindici  (quindici_bin,quindici_bin_len);
ADPCMSound sedici  (sedici_bin,sedici_bin_len);
ADPCMSound diciassette  (diciassette_bin,diciassette_bin_len);
ADPCMSound diciotto  (diciotto_bin,diciotto_bin_len);
ADPCMSound diciannove  (diciannove_bin,diciannove_bin_len);

	switch(tensun){
			
		case 0: 
                         
			Player::instance().play(dieci);
		break;


		case 1: 
                         
			Player::instance().play(undici);
		break;

		case 2: 
                         
			Player::instance().play(dodici);
		break;

		case 3: 
                         
			Player::instance().play(tredici);
		break;

		case 4: 
                         
			Player::instance().play(quattordici);
		break;

		case 5: 
                         
			Player::instance().play(quindici);
		break;

		case 6:
                         
			Player::instance().play(sedici);
		break;

		case 7: 
                         
			Player::instance().play(diciassette);
		break;

		case 8:
                         
			Player::instance().play(diciotto);
		break;

		case 9: 
                         
			Player::instance().play(diciannove);
		break;

		}

	
}

// corpo centrale 
//prende il numero e ne fa la divisione intera per le potenze di 10, scomponendo così il numero nelle sue cifre 



void ring::play_n_of_step(int num){

    
    
    
ADPCMSound zero  (zero_bin,zero_bin_len);
ADPCMSound cento  (cento_bin,cento_bin_len);
ADPCMSound mila  (mila_bin,mila_bin_len);
ADPCMSound mille  (mille_bin,mille_bin_len);
ADPCMSound milioni  (milioni_bin,milioni_bin_len);
ADPCMSound milione  (milione_bin,milione_bin_len);
ADPCMSound haifatto  (haifatto_bin,haifatto_bin_len);
ADPCMSound passi  (passi_bin,passi_bin_len);
ADPCMSound haivinto (haivinto_bin,haivinto_bin_len);



	//suona "Hai fatto"
	Player::instance().play(haifatto);
	
	int div=10; // potenza del 10 che divide
	int res=0; //risultato della divisione intera
	short int exp = 1; // esponente della potenza del dieci
	
      
        
        
        
	//bool like per il caso particolare dei numeri dal 10 al 19 (in questo caso vengono dette due cifre in una volta sola)
	short int just_said; 

	int mask = num;	//mask è una coppia di num che rimane inalterata per capire se le migliaia o i milioni sono singolari o plurali
	
	
	
	just_said=0;
	// se il numero è zero pronuncio lo zero e esco
	if (num == 0) {
		
                //zero 
		Player::instance().play(zero);
		
	}
	//pongo il massimo numero di passi a 9 milioni 999999		
	div = div * 100000;
	exp = 6;
	
	// quando vengono esaurite le divisioni si esce
	while(exp != -1){
		
		//calcolo la cifra da far dire
		res = num/div;
		// controllo di non avere errori (dal 10 al 19 è gestito in seguito)
		if (res>9){
			
			 //troppi passi
			Player::instance().play(haivinto);
		}
		
		// se la cifra è diversa da zero bisogna solo controllare che non si sia da dire "mille" o "milione" 
 		if (res != 0){
			
			
			//le differenze su cosa dire si basano sulla posizione della cifra all'interno del numero
			
			//centinaia
			if ((exp % 3) == 2){
				
				if (res>1){
					say_unit(res);
					
                                         
                                        Player::instance().play(cento);
				}else{
					
                                         
                                        Player::instance().play(cento);
				}

			}
			//decine
			if ((exp % 3) == 1){
				if (res>1){
                                         say_tens(res);
				}else{
					num -= res * div;
					div = div/10;
					exp--;
					res = num / div;
					say_tens_and_unit(res);
					just_said = 1;
				}

			}
			//milioni
			if (exp == 6){
				
                                         say_unit(res);

				if ( (mask/div) > 1 ){
						
					
                                         //milioni
                                        Player::instance().play(milioni);
					
				}else{ 
                                         //milione
                                        Player::instance().play(milione);
				}
				mask = mask - (res * div);
			}
			//migliaia
			if (exp == 3 ){
				 
				if (just_said == 1){
				 	
                                         // migliaia plurale
                                        Player::instance().play(mila);
				}else{
					if ( (mask/div) > 1 ){
						
                                                say_unit(res);
						
                                                 // migliaia plurale
                                                Player::instance().play(mila);
					
					}else{ 
                                                 //migliaia singolare
                                                Player::instance().play(mille);
					}	
				}

			}
			//unità
			if (exp == 0){
				
				if (just_said==0)  {say_unit(res);}

			}
			


		}else{
			// se il risultato della divisione è uguale a zero ma bisogna integrare con "mille " o "milioni"
			// e.g. 50475
			if (exp == 3 ){		
				if ( (mask/div) > 1 ){
                                   
                                   //migliaia plurale  
                                    Player::instance().play(mila);
                                
                                }	
				

			}
		}
		num -= res * div;
		div = div/10;
		exp--;
		just_said = 0;
		
	}

	//suona "passi"
	Player::instance().play(passi);
		
}
//funzione che permette di riprodurre il segnale di vittoria
void ring::victory_Song(){
    ADPCMSound haivinto (haivinto_bin,haivinto_bin_len);

                         
			Player::instance().play(haivinto);
    
    
}

//funzione che permette di riprodurre il segnale di sconfitta

void ring::looser_Song(){
    
ADPCMSound haiperso  (haiperso_bin,haiperso_bin_len);
                         
			Player::instance().play(haiperso);
}





