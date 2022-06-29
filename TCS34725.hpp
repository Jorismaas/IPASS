/****************************************************************************************************************************
*	auther :: Joris Maas                                                                                                                                           
*	File :: TCS34725.hpp                                                                                                                                   
*	Date :: 30/06/2022                                                                                                                                                      
*	copyright :: Joris Maas 2022
*                                                    
*  Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at 
* http://www.boost.org/LICENSE_1_0.txt)
*******************************************************************************************************************************/


#ifndef TCS34725_HPP
#define TCS34725_HPP
	/// @file

#include "hwlib.hpp"
#include "kleursensor.hpp"


	/// \brief
	/// Kleursensor klasse TCS34725
	/// \details
	/// Een class library geschreven voor de kleursensor TCS34725 die ook gebruikt kan worden onder de klas Kleursensor.
	/// Deze class kan op verschillende gevoeligheids standen worden gezet.
	/// Ook is deze class instaat om kleuren uit te lezen maar is vooral geschreven om gebruik van te maken bij de klasse Simon.
	/// Zou met extra configureren meer kleuren waardes kunnen teruggeven inplaats van een van vier kleuren terug te geven.
class TCS34725 : public Kleursensor{
//public:
private:
// address voor communicatie
uint8_t slaveAddress = 0x29;


enum RegisterAddresses : uint8_t{
	// alle kleuren
	clearLow = 0x14,
	clearHigh = 0x15,
	redLow = 0x16,
	redHigh = 0x17,
	greenLow = 0x18,
	greenHigh = 0x19,
	blueLow = 0x1A,
	blueHigh = 0x1B,
	
	
	enable_Register = 0x00,
	RGBCtimingRegister = 0x01,
	control_Register = 0x0F,
	ID_Register = 0x12
};

enum Enable_Register : uint8_t{
	Enable_address = 0x00,
	PON = 0x01,
	AEN = 0x02,
	WEN = 0x08,
	AIEN = 0x10,
};

enum RGBC_Timing_Register : uint8_t{
	RGBC_Timing_address = 0x01,
	stand1 = 0x00,
	stand2 = 0xC0,
	stand3 = 0xD5,
	stand4 = 0xF6,
	stand5 = 0xFF	
};

enum Control_Register : uint8_t{
	Control_address = 0x0F,
	x1gain = 0x00,
	x4gain = 0x01,
	x16gain = 0x02,
	x60gain = 0x03
};

// geeft aan dat het om een command gaat 0x80 (128) is het laatste bitje in een 8bit
uint8_t commandBitje = 0x80;


hwlib::i2c_bus_bit_banged_scl_sda &i2c_bus;
int controlRegisterSet;
int RGBCTimingRegisterGevoeligheid;
int integ_cycles;
int difference_multiplier = 6;
int RGBC_Timing_Register_wait_time = 0;
int hoeveelheidHerhalingen = 5;
	
public:
	/// \brief
	/// Constructor
	/// \details
	/// Voor het meegeven van de juiste bus en het instellen van de gevoeligheid van de chip.
	TCS34725(hwlib::i2c_bus_bit_banged_scl_sda &i2c_bus, int controlRegisterSet, int RGBCTimingRegisterGevoeligheid);
	
	/// \brief
	/// Default Constructor
	/// \details
	/// Geeft de standaard gebruikte values van de controlregister en timing register mee.
	TCS34725(hwlib::i2c_bus_bit_banged_scl_sda &i2c_bus);
	
	/// \brief
	/// Hulp write
	/// \details
	/// mini write functie om het process van schrijven te verkleinen in minder lines.
	/// Deze functie kan herbruikt worden zodat deze basis code niet herhaald hoeft te worden.
	/// write( locatie van het register dat je wilt gebruiken bijv 0x00 )
	void write(uint8_t location);
	
	/// \brief
	/// Read functie
	/// \details
	/// Gebruikt om te lezen wat de chip verteld aan de master.
	/// Alles wat wordt uitgelezen wordt in bits teruggegeven en gereturned.
	/// in dit geval geef je readData de locatie mee van een register en returned hij een databyte
	uint8_t readData(uint8_t location);
	
	
	/// \brief
	/// Herhaling setter
	/// \details
	/// Verander hoevaak een kleur herhaald moet worden voordat de colourRead deze kleur returned.
	/// Default value van het aantal herhalingen staat op 5. Hoe lager je hem set hoe sneller je een return krijgt maar hoe meer kans dat de gereturnde kleur minder accuraat is.
	void setHerhalingAccuracy(int nummer);
	
	
	/// \brief
	/// Connection check
	/// \details
	/// Checkt of er een goede connectie met de chip is.
	/// Als er met de juiste chip naar het ID register geschreven wordt dan zal deze chip een waarde van 68 returnen.
	/// Als deze waarde dus wordt teruggegeven nadat je schrijft naar de chip dan weet je dat er een goede connectie is met de chip
	/// De boolean waarde bij 68 is True en anders False, dit wordt gereturned
	bool checkConnection() override;
	
	/// \brief
	/// I2C write functie
	/// \details
	/// De main schrijf functie gebruikt om te communiceren met de chip.
	/// Als eerst wordt met de i2c_bus geschreven naar het juiste adress van de chip.
	/// Daarna wordt er een commando geschreven waarbij het command bit van belang is zodat deze chip weet dat het een command is.
	/// Daarna kan het stuk data dat je wilt schrijven, geschreven worden naar de chip.
	void writeData(uint8_t location, uint8_t databyte);
	
	/// \brief
	/// Set de multiplier van de vergelijker
	/// \details
	/// Deze functie bepaalt hoe groot de multiplier moet zijn van de keersom die ervoor verzorgt dat de resultaten die
	/// de kleuren lezen nogsteeds even hoog zullen zijn zodat hij nogsteeds de juiste kleur zal teruggeven ookal verschilt de sensitiviteit of gain van de chip.
	int setMultiplier();
	
	/// \brief
	/// Set de multiplier van het control register
	/// \details
	/// Zet het control register op één van zijn vier standen 1x, 4x, 16x, 60x, bij het invoeren van 1 wordt het 1x, bij 2 wordt de gain 4x en zo door.
	void gainMultiplierSet(const int sensitiviteit);
	
	/// \brief
	/// RGBC gevoeligheid instellen
	/// \details
	/// Stuurt een write naar het RGBC Timing Register met een waarde waarmee bepaald wordt hoeveel tijd
	/// er voor accuracy wordt opgeofferd. Zet de stand van het register, de wachttijd tussen reads en de
	/// integ_cycles
	///
	/// Er zijn 5 standen die ook de tijd significant beinvloeden. Hoe meer accuraat je wilt dat het resultaat
	/// is des te langer het duurt om de registers vol te zetten en voordat ze dus leesbaar zijn.
	void RGBCGevoeligheidsset(int RGBCTimingRegisterGevoeligheid);
	
	/// \brief
	/// Initializer
	/// \details
	/// Voert alle benodigheden uit om het product klaar te maken voor gebruik.
	/// Denk hierbij aan de registers die te maken hebben met het resultaat dat wordt meegegeven wanneer je dit
	/// opvraagd.
	/// Als je iets wilt aanpassen aan de chip kun je dit vervolgens initializeren zodat alle veranderingen worden aangeroepen in de chip
	void initializer() override;
	
	/// \brief
	/// Optellen highs and low
	/// \details
	/// Je krijgt bij het lezen van de kleuren twee registers aan data terug. De high, de hoogste 8 bits 
	/// en de low, de laagste 8 bits.
	/// In deze functie worden deze twee values bij elkaar in 1 cijfer teruggegeven
	uint16_t optellenHighAndLow(uint8_t high, uint8_t low);
	
	/// \brief
	/// Kleuren lezen
	/// \details
	/// Een functie die de waardes van kleuren opleest, bij elkaar laat optellen en dit returned voor de colourRead functie.
	/// Er wordt een kleur meegegeven zodat de functie weet van welke waarde hij moet lezen en teruggeven.
	uint16_t value_Colours(const unsigned int kleurSelect);
	
	/// \brief
	/// Colour reader
	/// \details
	/// Bepaalt welke kleur hij zal returnen gebaseerd op de gegeven waardes.
	/// Returned alleen een kleur als hij deze 5 keer heeft gelezen zodat hij meer accuraat kan zeggen of dit echt de juiste kleur is.
	/// Hoe hoger het aantal keer is dat hij het zou moeten lezen des te langer het process maar hoe meer accuraat het resultaat.
	///
	/// returned rood als 1, blauw als 2, groen als 3, geel als 4
	int colourRead() override;
};


#endif // TCS34725_HPP