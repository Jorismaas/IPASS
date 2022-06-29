/****************************************************************************************************************************
*	auther :: Joris Maas                                                                                                                                           
*	File :: Simon.hpp                                                                                                                                   
*	Date :: 30/06/2022                                                                                                                                                      
*	copyright :: Joris Maas 2022
*
*  Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at 
* http://www.boost.org/LICENSE_1_0.txt)
*******************************************************************************************************************************/


#ifndef SIMON_HPP
#define SIMON_HPP
#include "kleursensor.hpp"
	/// @file
	
	
	/// \brief
	/// Simon says class
	/// \details
	/// Bedoelt om het spel Simon says mee te spelen waarbij kleuren worden weergeven in een bepaalde volgorde waarna je deze volgorde moet imiteren om het spel te winnen.
	/// De klasse werkt alleen met gebruik van een kleursensor.
class Simon{
private:
Kleursensor &kleursensor;
hwlib::target::pin_out &rode_led;
hwlib::target::pin_out &blauwe_led;
hwlib::target::pin_out &groene_led;
hwlib::target::pin_out &gele_led;
hwlib::target::pin_out &actie_gezien_led;
int level = 0;
int array[6]{0,0,0,0,0,0};
int wait_time = 500;

public:
	/// \brief
	/// Constructor
	/// \details
	/// In deze constructor worden alle leds meegegeven, een kleursensor aangegeven die gebruikt zal worden en de difficulty aangegeven.
Simon(Kleursensor &kleursensor, hwlib::target::pin_out &rode_led, hwlib::target::pin_out &blauwe_led, hwlib::target::pin_out &groene_led, hwlib::target::pin_out &gele_led, hwlib::target::pin_out &actie_led
):
kleursensor(kleursensor), rode_led(rode_led), blauwe_led(blauwe_led), groene_led(groene_led), gele_led(gele_led), actie_gezien_led(actie_led)
{}


	/// \brief
	/// Add to array functie
	/// \details
	/// Voegt een nummer van de rand (random) functie toe aan een array.
	/// Dit getal is niet totaal random maar geeft genoeg verschillende waardes om bruikbaar te zijn voor het spel.
void add_value(){
	array[level] = rand() % 4 + 1;
}


	/// \brief
	/// Correct antwoord functie
	/// \details
	/// Zorgt ervoor dat je naar het volgende level komt en dat er een extra cijfer in de array erbij komt.
void correct_answer(){
	level += 1;
	add_value();
}


	/// \brief
	/// Verkeerd antwoord functie
	/// \details
	/// Zorgt ervoor dat het programma reset en de array weer vanaf begin begint met een bericht erbij.
void wrong_answer(){
	hwlib::cout << "\nverkeerd antwoord \nTry again lozer.\n";
	for(; level >= 0; level--){
		array[level] = 0;
	}
}


	/// \brief
	/// Coole lightshow
	/// \details
	/// Alleen gezien door de mensen die alles winnen in simon says.
	/// Een leuke volgorde van lichtjes voor een winnaar
void cooleLightShow(int ms_time){
	auto leds = hwlib::port_out_from( rode_led, blauwe_led, groene_led, gele_led, actie_gezien_led);
	licht_aan_uit(0x01, ms_time);
	licht_aan_uit(0x02, ms_time);
	licht_aan_uit(0x04, ms_time);
	licht_aan_uit(0x08, ms_time);
	licht_aan_uit(0x08, ms_time);
	licht_aan_uit(0x04, ms_time);
	licht_aan_uit(0x02, ms_time);
	licht_aan_uit(0x01, ms_time);
	licht_aan_uit(0x1f, ms_time);
	hwlib::wait_ms(ms_time);
	licht_aan_uit(0x1f, ms_time);
	hwlib::wait_ms(ms_time);
	licht_aan_uit(0x1f, ms_time);
	hwlib::wait_ms(ms_time);
}


	/// \brief
	/// Licht aan uit functie
	/// \details
	/// zet een lichtje naar keuze aan of uit.
void licht_aan_uit(uint8_t ledNummer, int ms_time){
	auto leds = hwlib::port_out_from( rode_led, blauwe_led, groene_led, gele_led, actie_gezien_led );
	//uint8_t binaireLeds = lednummer;
	leds.write(ledNummer);
	leds.flush();
	hwlib::wait_ms(ms_time);
	leds.write(0x00);
	leds.flush();
}


//	/// \brief
//	/// Spel start
//	/// \details
//	/// Voegd de eerste waarde van het spel toe en begint het spel.
//void game_start(){
//	add_value();
//	simon_says();
//}



	/// \brief
	/// Simon says functie
	/// \details
	/// Deze functie zorgt dat de juiste leds aangaan
	/// Ook wordt het gegeven antwoord vergelijkt met het nummer in de array en het correcte of verkeerde antwoord functie aangeroepen
	/// Vooreeuwige for loop die alleen eindigt als iemand het spel wint
void simon_says(int difficulty){
	
	add_value();
	int numb = 2;
	for(;;){
		for(int i = 0; i <= level; i++){
			if(array[i] == 1){
				licht_aan_uit(0x01, wait_time * 2 / difficulty);
				hwlib::wait_ms(wait_time/difficulty);
			}else if(array[i] == 2){
				licht_aan_uit(0x02, wait_time * 2 / difficulty);
				hwlib::wait_ms(wait_time/difficulty);
			}else if(array[i] == 3){
				licht_aan_uit(0x04, wait_time * 2 / difficulty);
				hwlib::wait_ms(wait_time/difficulty);
			}else{
				licht_aan_uit(0x08, wait_time * 2 / difficulty);
				hwlib::wait_ms(wait_time/difficulty);
			}
			hwlib::cout << array[i] << " ";
		}
		for(int i = 0; i <= level; i++){
			hwlib::cout << hwlib::endl << "Jouw kleurnummer is: " ;
			numb = kleursensor.colourRead();
			// gezien functie van maken
			licht_aan_uit(0x10, 500);
			hwlib::wait_ms(500);
			//
			if(numb != array[i]){
				wrong_answer();
			}
		}
	hwlib::cout << "Next_level" << hwlib::endl;
	correct_answer();
	if((level + 1) == 7){
		hwlib::cout << "\nCongratiolatiooonzz, you did it!";
		cooleLightShow(150);
		break;
	}
	}
}
};


#endif // SIMON_HPP
