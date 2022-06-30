/****************************************************************************************************************************
*	auther :: Joris Maas                                                                                                                                           
*	File :: TCS34725.cpp                                                                                                                                   
*	Date :: 30/06/2022                                                                                                                                                      
*	copyright :: Joris Maas 2022
*
* Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at 
* http://www.boost.org/LICENSE_1_0.txt)
*******************************************************************************************************************************/



#include "TCS34725.hpp"
	/// @file

	/// \brief
	/// Constructor
	/// \details
	/// Voor het meegeven van de juiste bus en het instellen van de gevoeligheid van de chip.
TCS34725::TCS34725(hwlib::i2c_bus_bit_banged_scl_sda &i2c_bus, int controlRegisterSet, int RGBCTimingRegisterGevoeligheid):
i2c_bus(i2c_bus), controlRegisterSet(controlRegisterSet), RGBCTimingRegisterGevoeligheid(RGBCTimingRegisterGevoeligheid), integ_cycles(0)
{}


TCS34725::TCS34725(hwlib::i2c_bus_bit_banged_scl_sda &i2c_bus):
i2c_bus(i2c_bus), controlRegisterSet(1), RGBCTimingRegisterGevoeligheid(1), integ_cycles(0) 
{}

	/// \brief
	/// Hulp write
	/// \details
	/// mini write functie om het process van schrijven te verkleinen in minder lines.
	/// Deze functie kan herbruikt worden zodat deze basis code niet herhaald hoeft te worden.
	/// write( locatie van het register dat je wilt gebruiken bijv 0x00 )
void TCS34725::write(uint8_t location){
	hwlib::i2c_write_transaction writer = ((hwlib::i2c_bus*)(&i2c_bus))->write(slaveAddress);
	writer.write(location + commandBitje);
} 


	/// \brief
	/// Read functie
	/// \details
	/// Gebruikt om te lezen wat de chip verteld aan de master.
	/// Alles wat wordt uitgelezen wordt in bits teruggegeven en gereturned.
	/// in dit geval geef je readData de locatie mee van een register en returned hij een databyte
uint8_t TCS34725::readData(uint8_t location){
	uint8_t resultaten[2];
	{write(location);}
	{hwlib::i2c_read_transaction reader = ((hwlib::i2c_bus*)(&i2c_bus))->read(slaveAddress);
	reader.read(resultaten, 1);}
	return resultaten[0];
}


	/// \brief
	/// Herhaling setter
	/// \details
	/// Verander hoevaak een kleur herhaald moet worden voordat de colourRead deze kleur returned.
	/// Default value van het aantal herhalingen staat op 5. Hoe lager je hem set hoe sneller je een return krijgt maar hoe meer kans dat de gereturnde kleur minder accuraat is.
void TCS34725::setHerhalingAccuracy(int nummer){
		hoeveelheidHerhalingen = nummer;
	}


	/// \brief
	/// Connection check
	/// \details
	/// Checkt of er een goede connectie met de chip is.
	/// Als er met de juiste chip naar het ID register geschreven wordt dan zal deze chip een waarde van 68 returnen.
	/// Als deze waarde dus wordt teruggegeven nadat je schrijft naar de chip dan weet je dat er een goede connectie is met de chip
	/// De boolean waarde bij 68 is True en anders False, dit wordt gereturned
bool TCS34725::checkConnection(){
	int gelezenData = readData(ID_Register);
	if(gelezenData == 68){
		return true;
	}else{
		return false;
	}
}


	/// \brief
	/// I2C write functie
	/// \details
	/// De main schrijf functie gebruikt om te communiceren met de chip.
	/// Als eerst wordt met de i2c_bus geschreven naar het juiste adress van de chip.
	/// Daarna wordt er een commando geschreven waarbij het command bit van belang is zodat deze chip weet dat het een command is.
	/// Daarna kan het stuk data dat je wilt schrijven, geschreven worden naar de chip.
void TCS34725::writeData(uint8_t location, uint8_t databyte){
	{hwlib::i2c_write_transaction writer = ((hwlib::i2c_bus*)(&i2c_bus))->write(slaveAddress);
	writer.write(location + commandBitje);
	writer.write(databyte);}
}


	/// \brief
	/// Set de multiplier van de vergelijker
	/// \details
	/// Deze functie bepaalt hoe groot de multiplier moet zijn van de keersom die ervoor verzorgt dat de resultaten die
	/// de kleuren lezen nogsteeds even hoog zullen zijn zodat hij nogsteeds de juiste kleur zal teruggeven ookal verschilt de sensitiviteit of gain van de chip.
int TCS34725::setMultiplier(){
	if(controlRegisterSet < 3){
		if(controlRegisterSet == 1){
			return 1;
		}
		return 2;
	}else{
		if(controlRegisterSet == 3){
			return 16;
		}
		return 60;
	}
}


	/// \brief
	/// Set de multiplier van het control register
	/// \details
	/// Zet het control register op één van zijn vier standen 1x, 4x, 16x, 60x, bij het invoeren van 1 wordt het 1x, bij 2 wordt de gain 4x en zo door.
void TCS34725::gainMultiplierSet(const int sensitiviteit){
	if(sensitiviteit == 2){
		writeData(Control_address, Control_Register::x4gain);
	}else if(sensitiviteit == 3){
		writeData(Control_address, Control_Register::x16gain);
	}else if(sensitiviteit == 4){
		writeData(Control_address, Control_Register::x60gain);
	}else{
		writeData(Control_address, Control_Register::x1gain);
	}
}


	/// \brief
	/// RGBC gevoeligheid instellen
	/// \details
	/// Stuurt een write naar het RGBC Timing Register met een waarde waarmee bepaald wordt hoeveel tijd
	/// er voor accuracy wordt opgeofferd. Zet de stand van het register, de wachttijd tussen reads en de
	/// integ_cycles
	///
	/// Er zijn 5 standen die ook de tijd significant beinvloeden. Hoe meer accuraat je wilt dat het resultaat
	/// is des te langer het duurt om de registers vol te zetten en voordat ze dus leesbaar zijn.
	/// Kan geset worden buiten de initializer
void TCS34725::RGBCGevoeligheidsset(int RGBCTimingRegisterGevoeligheid){
	if(RGBCTimingRegisterGevoeligheid == 1){
		writeData(RGBC_Timing_address, RGBC_Timing_Register::stand1);
		integ_cycles = 256;
		RGBC_Timing_Register_wait_time = 700;
		difference_multiplier *= 10;
		yellow_multiplier = 3;
	}else if(RGBCTimingRegisterGevoeligheid == 2){
		writeData(RGBC_Timing_address, RGBC_Timing_Register::stand2);
		integ_cycles = 64;
		RGBC_Timing_Register_wait_time = 154;
		difference_multiplier *= 8;
		yellow_multiplier = 3;
	}else if(RGBCTimingRegisterGevoeligheid == 3){
		writeData(RGBC_Timing_address, RGBC_Timing_Register::stand3);
		integ_cycles = 42;
		RGBC_Timing_Register_wait_time = 101;
		difference_multiplier *= 4;
		yellow_multiplier = 2;
	}else if(RGBCTimingRegisterGevoeligheid == 4){
		writeData(RGBC_Timing_address, RGBC_Timing_Register::stand4);
		integ_cycles = 10;
		RGBC_Timing_Register_wait_time = 24;
		difference_multiplier *= 3;
		yellow_multiplier = 2;
	}else{
		writeData(RGBC_Timing_address, RGBC_Timing_Register::stand5);
		RGBC_Timing_Register_wait_time = 3;
		integ_cycles = 1;
		difference_multiplier *= 3;
		yellow_multiplier = 1;
	}
	
}
	/// \brief
	/// Initializer
	/// \details
	/// Voert alle benodigheden uit om het product klaar te maken voor gebruik.
	/// Denk hierbij aan de registers die te maken hebben met het resultaat dat wordt meegegeven wanneer je dit
	/// opvraagd.
void TCS34725::initializer(){
	difference_multiplier = setMultiplier();
	RGBCGevoeligheidsset(RGBCTimingRegisterGevoeligheid);
	gainMultiplierSet(controlRegisterSet);
}

	/// \brief
	/// Optellen highs and low
	/// \details
	/// Je krijgt bij het lezen van de kleuren twee registers aan data terug. De high, de hoogste 8 bits 
	/// en de low, de laagste 8 bits.
	/// In deze functie worden deze twee values bij elkaar in 1 cijfer teruggegeven
uint16_t TCS34725::optellenHighAndLow(uint8_t high, uint8_t low){
// probeer high <<= 8, return high + low
//	high <<= 8;
//	return high + low;
	uint16_t data = high;
	data <<= 8;
	return data + low;
}


	/// \brief
	/// Kleuren lezen
	/// \details
	/// Een functie die de waardes van kleuren opleest, bij elkaar laat optellen en dit returned voor de colourRead functie.
	/// Er wordt een kleur meegegeven zodat de functie weet welke waarde hij moet lezen en teruggeven.
uint16_t TCS34725::value_Colours(const unsigned int kleurSelect){
	if(kleurSelect == 1){
		uint8_t redLow_value = readData(redLow);
		uint8_t redHigh_value = readData(redHigh);
		return optellenHighAndLow(redHigh_value, redLow_value) - 300;
	}else if(kleurSelect == 2){
		uint8_t blueLow_value = readData(blueLow);
		uint8_t blueHigh_value = readData(blueHigh);
		return optellenHighAndLow(blueHigh_value, blueLow_value) + 250;
	}else{
		uint8_t greenLow_value = readData(greenLow);
		uint8_t greenHigh_value = readData(greenHigh);
		return optellenHighAndLow(greenHigh_value, greenLow_value);
	}
}


	/// \brief
	/// Colour reader
	/// \details
	/// Bepaalt welke kleur hij zal returnen gebaseerd op de gegeven waardes.
	/// Returned alleen een kleur als hij deze 5 keer heeft gelezen zodat hij meer accuraat kan zeggen of dit echt de juiste kleur is.
	/// Hoe hoger het aantal keer is dat hij het zou moeten lezen des te langer het process maar hoe meer accuraat het resultaat.
	///
	/// returned rood als 1, blauw als 2, groen als 3, geel als 4
int TCS34725::colourRead(){
	//int test = 0;
	int herhalingKleur = 0;
	int kleurCode = 0;
	int minusvalue = 20 * difference_multiplier;
	
	writeData(Enable_address, Enable_Register::PON);
	hwlib::wait_ms(3);
	writeData(Enable_address, (Enable_Register::PON | Enable_Register::AEN));
//	writeData(enable_Register, 0x1b);
	writeData(RGBCtimingRegister, RGBCTimingRegisterGevoeligheid);
	// 12 / 5 als vervanging van de non int 2.4 die nodig is voor deze formule
	hwlib::wait_ms((256 - integ_cycles) * 12 / 5 + 1);
	for(;;){
		//test += 1;
		uint16_t Red_value = value_Colours(1);
		uint16_t Blue_value =  value_Colours(2);
		uint16_t Green_value =  value_Colours(3);
		
		//hwlib::cout << "Test " << test << hwlib::endl;
		//hwlib::cout << "Red: " << Red_value << hwlib::endl;
		//hwlib::cout << "Blue: " << Blue_value << hwlib::endl;
		//hwlib::cout << "Green: " << Green_value << hwlib::endl << hwlib::endl;
		//hwlib::cout << "Geel zou werken als: " << Blue_value << "   " << Green_value - minusvalue * yellow_multiplier << "  &&  " <<  Red_value - minusvalue * yellow_multiplier * 2 - minusvalue << "  &&  " << Red_value << "   " << Green_value - minusvalue * yellow_multiplier * 2 << hwlib::endl;
		//hwlib::cout << minusvalue << hwlib::endl;
		// max count kan met double / max count, colour read functie toevoegen of een timer aan de read
		// zodat ie Daar wacht tot de register gelezen heeft zodat ie de juiste value pakt en genoeg tijd heeft
		// gehad, Hue Saturation Intensity
		// hoeveelheid herhalingen ook kunnen laten aanpassen, nieuwe functie daarvoor
		// verandering in twee if statements zodat de herhaling beter klopt, herhaling misschien buiten deze functie?
		if(Blue_value < Green_value - minusvalue * yellow_multiplier && Blue_value < Red_value - minusvalue * 3 && Red_value > Green_value - minusvalue * yellow_multiplier * 2 - minusvalue){
			herhalingKleur += 1;
			if(kleurCode != 4){
				herhalingKleur = 1;
				kleurCode = 4;
			}
			if(herhalingKleur >= hoeveelheidHerhalingen && kleurCode == 4){
				hwlib::cout << "Kleur is geel!\n";
				// geel
				return 4;
			}
		}else if((Red_value - minusvalue) > Green_value && (Red_value - minusvalue) > Blue_value){
			herhalingKleur += 1;
			if(kleurCode != 1){
				herhalingKleur = 1;
				kleurCode = 1;
			}if(herhalingKleur >= hoeveelheidHerhalingen && kleurCode == 1){
				hwlib::cout << "Kleur is rood!\n";
				// rood
				return 1;
			}
		}else if((Blue_value - minusvalue) > Green_value && Red_value < (Blue_value - minusvalue)){
			herhalingKleur += 1;
			if(kleurCode != 2){
				herhalingKleur = 1;
				kleurCode = 2;
			}if(herhalingKleur >= hoeveelheidHerhalingen && kleurCode == 2){
				hwlib::cout << "Kleur is blauw!\n";
				// blauw
				return 2;
			}
		}else if(Blue_value < (Green_value - minusvalue) && Red_value < (Green_value - minusvalue)){
			herhalingKleur += 1;
			if(kleurCode != 3){
				herhalingKleur = 1;
				kleurCode = 3;
			}if(herhalingKleur >= hoeveelheidHerhalingen && kleurCode == 3){
				// groen
				hwlib::cout << "Kleur is groen!\n";
				return 3;
			}
		}
	hwlib::wait_ms(RGBC_Timing_Register_wait_time);
	}
//	writeData(enable_Register, 0);
	return 0;
}
