#include <stdio.h>
// algoritmo di scomposizione di un numero intero 



//funzione che permette di scrivere le unità
int say_unit (int unit){
	
	switch(unit){

		case 1: printf("uno");
		break;

		case 2: printf("due");
		break;

		case 3: printf("tre");
		break;

		case 4: printf("quattro");
		break;

		case 5: printf("cinque");
		break;

		case 6: printf("sei");
		break;

		case 7: printf("sette");
		break;

		case 8: printf("otto");
		break;

		case 9: printf("nove");
		break;

		}

	return 0;

}
//permette di scrivere le decine
int say_tens (int tens){
	
	switch(tens){

		case 2: printf("venti");
		break;

		case 3: printf("trenta");
		break;

		case 4: printf("quaranta");
		break;

		case 5: printf("cinquanta");
		break;

		case 6: printf("sessanta");
		break;

		case 7: printf("settanta");
		break;

		case 8: printf("ottanta");
		break;

		case 9: printf("novanta");
		break;
	}	

	return 0;
}
// permette di scrivere i numeri dal 10 al 19
int say_tens_and_unit(int tensun){
	
	switch(tensun){
			
		case 0: printf("dieci");
		break;


		case 1: printf("undici");
		break;

		case 2: printf("dodici");
		break;

		case 3: printf("tredici");
		break;

		case 4: printf("quattordici");
		break;

		case 5: printf("quindici");
		break;

		case 6: printf("sedici");
		break;

		case 7: printf("diciasette");
		break;

		case 8: printf("diciotto");
		break;

		case 9: printf("diciannove");
		break;

		}

	return 0;
}

// corpo centrale 
//prende il numero e ne fa la divisione intera per le potenze di 10, scomponendo così il numero nelle sue cifre 



int play_n_of_step(int num){
	
	int div=10; // potenza del 10 che divide
	int res=0; //risultato della divisione intera
	short int exp = 1; // esponente della potenza del dieci
	
	//bool like per il caso particolare dei numeri dal 10 al 19 (in questo caso vengono dette due cifre in una volta sola)
	short int just_said; 

	int mask = num;	//mask è una coppia di num che rimane inalterata per capire se le migliaia o i milioni sono singolari o plurali
	
	// avendo avuto dei problemi mettendo alle altre funzioni void come valore di ritorno take raccoglie  L'INUTILE risultato   		// delle funzioni
	int take; 
	
	just_said=0;
	// se il numero è zero pronuncio lo zero e esco
	if (num == 0) {
		printf("zero");
		return 0;
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
			printf("troppi passi");
			return 0;
		}
		
		// se la cifra è diversa da zero bisogna solo controllare che non si sia da dire "mille" o "milione" 
 		if (res != 0){
			
			
			//le differenze su cosa dire si basano sulla posizione della cifra all'interno del numero
			
			//centinaia
			if ((exp % 3) == 2){
				
				if (res>1){
					take = say_unit(res);
					printf("cento");
				}else{
					printf("cento");
				}

			}
			//decine
			if ((exp % 3) == 1){
				if (res>1){
					take = say_tens(res);
				}else{
					num -= res * div;
					div = div/10;
					exp--;
					res = num / div;
					take = say_tens_and_unit(res);
					just_said = 1;
				}

			}
			//milioni
			if (exp == 6){
				
				take = say_unit(res);

				if ( (mask/div) > 1 ){
						
					printf("milioni");
					
				}else{ printf("milione");
				}
				mask = mask - (res * div);
			}
			//migliaia
			if (exp == 3 ){
				 
				if (just_said == 1){
				 	printf("mila");
				}else{
					if ( (mask/div) > 1 ){
						
						take = say_unit(res);
						printf("mila");
					
					}else{ printf("mille");
					}	
				}

			}
			//unità
			if (exp == 0){
				
				if (just_said==0) take = say_unit(res);

			}
			


		}else{
			// se il risultato della divisione è uguale di zero ma bisogna integrare con "mille " o "milioni"
			// e.g. 50475
			if (exp == 3 ){		
				if ( (mask/div) > 1 ) printf("mila");
					
				

			}
		}
		num -= res * div;
		div = div/10;
		exp--;
		just_said = 0;
		
	}

	return 0;
		
}
// main so simple
int main(void){
	
	int a=1001517;
	int b = 0;
		
	
	 b = play_n_of_step(a);

	return 0;

}




