/****************************************************************************************************************************
*	auther :: Joris Maas                                                                                                                                           
*	File :: kleursensor.hpp                                                                                                                                   
*	Date :: 30/06/2022                                                                                                                                                      
*	copyright :: Joris Maas 2022
*                                               
*  Distributed under the Boost Software License, Version 1.0.
* (See accompanying file LICENSE_1_0.txt or copy at 
* http://www.boost.org/LICENSE_1_0.txt)
*******************************************************************************************************************************/


#ifndef KLEURSENSOR_HPP
#define KLEURSENSOR_HPP

#include "hwlib.hpp"
	/// @file

	/// \brief
	/// Abstracte Kleursensor class
	/// \details
	/// Kleursensor klasse die gebruikt maakt van een paar simpele functies die doorgegeven kunnen worden en gebruikt kunnen worden voor verschillende doeleinden zodat meerdere chips gebruikt kunnen worden
class Kleursensor{
private:
public:

	/// \brief
	/// Abstracte check connection
	/// \details
	/// Laat zien of je connectie hebt met de door jou gebruikte chip
virtual bool checkConnection() = 0;

	/// \brief
	/// Abstracte colour read
	/// \details
	/// Geeft een int waarde die je kan binden aan bepaalde kleuren
virtual int colourRead() = 0;

	/// \brief
	/// Abstracte initializer
	/// \details
	/// Kan gebruikt worden om je chip te voorzien van alle benodigde functies die moeten worden uitgevoerd.
virtual void initializer() = 0;

	
};


#endif // KLEURSENSOR_HPP