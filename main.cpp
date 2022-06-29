/****************************************************************************************************************************
*	auther :: Joris Maas                                                                                                                                           
*	File :: main.cpp                                                                                                                                   
*	Date :: 30/06/2022                                                                                                                                                      
*	copyright :: Joris Maas 2022
*                            
*  Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at 
* http://www.boost.org/LICENSE_1_0.txt)
*******************************************************************************************************************************/



#include "TCS34725.hpp"
#include "kleursensor.hpp"
#include "Simon.hpp" 


// misschien alleen rood een value geven, de rest wordt dan steeds met 1 hoger?
// int hoeft er eigenlijk niet bij want het wordt sowieso al int
enum Kleuren : int {
rood = 1,
blauw = 2,
groen = 3,
geel = 4
};


void testFunctieUitvoeren(Kleursensor & kleursensor, int getal){
	if(getal == 1){
		hwlib::cout << "geeft rood: " << (getal == kleursensor.colourRead()) << hwlib::endl;
	}else if(getal == 2){
		hwlib::cout << "geeft blauw: " << (getal == kleursensor.colourRead()) << hwlib::endl;
	}else if(getal == 3){
		hwlib::cout << "geeft groen: " << (getal == kleursensor.colourRead()) << hwlib::endl;
	}else if(getal == 4){
		hwlib::cout << "geeft geel: " << (getal == kleursensor.colourRead()) << hwlib::endl;
	}else{
		hwlib::cout << "Dit is niet een van de opgeslagen kleuren, probeer het ingevoerde getal aan te passen" << hwlib::endl;
	}
}

void connectieTest(Kleursensor & kleursensor){
	if(kleursensor.checkConnection()){
		hwlib::cout << "Je bent verbonden met de chip!" << hwlib::endl;
	}else{
		hwlib::cout << "Je bent verbonden met de chip!" << hwlib::endl;
	}
}


int main( void )
{	 
	
	auto blauwe_led = hwlib::target::pin_out( hwlib::target::pins::d13 );
	auto groene_led = hwlib::target::pin_out( hwlib::target::pins::d12 );
	auto rode_led = hwlib::target::pin_out( hwlib::target::pins::d11 );
	auto gele_led = hwlib::target::pin_out( hwlib::target::pins::d10 );
	auto acknowledge_led = hwlib::target::pin_out( hwlib::target::pins::d9 );
	auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
	auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);


	hwlib::i2c_bus_bit_banged_scl_sda i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
	TCS34725 kleurtjes(i2c_bus, 2, 1);
	Kleursensor* ding = &kleurtjes;
	Simon a(*ding, rode_led, blauwe_led, groene_led, gele_led, acknowledge_led);
	connectieTest(*ding);
	kleurtjes.setHerhalingAccuracy(2);
	ding->initializer();
	a.simon_says(2);
	testFunctieUitvoeren(*ding, rood);
	ding->colourRead();
	
	//hwlib::cout << readData(ID_Register, bus);
	
//   helloWorld_Lees_AN0();
   //helloWorld_Lees_AN0_tm_AN3();
}
