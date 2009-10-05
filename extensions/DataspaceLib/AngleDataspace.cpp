/* 
 * Jamoma DataspaceLib: AngleDataspace.cpp
 * Copyright © 2007, Nils Peters
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#include "AngleDataspace.h"

/***********************************************************************************************/
// rad is also the neutral format

#define thisTTClass			RadianUnit
#define thisTTClassName		"unit.radian"
#define thisTTClassTags		"dataspace, angle"

TT_DATASPACEUNIT_CONSTRUCTOR{;}
RadianUnit::~RadianUnit(){;}


void RadianUnit::convertToNeutral(const TTValue& input, TTValue& output)
{
	output = input;
}

void RadianUnit::convertFromNeutral(const TTValue& input, TTValue& output)
{
	output = input;
}


#undef thisTTClass
#undef thisTTClassName
#undef thisTTClassTags

/***********************************************************************************************/

#define thisTTClass			DegreeUnit
#define thisTTClassName		"unit.degree"
#define thisTTClassTags		"dataspace, angle"

TT_DATASPACEUNIT_CONSTRUCTOR{;}
DegreeUnit::~DegreeUnit(){;}


void DegreeUnit::convertToNeutral(const TTValue& input, TTValue& output)
{
	output = TTFloat64(input) * kDegreesToRadians;
}

void DegreeUnit::convertFromNeutral(const TTValue& input, TTValue& output)
{
	output = TTFloat64(input) * kRadiansToDegrees;
}


#undef thisTTClass
#undef thisTTClassName
#undef thisTTClassTags

/***********************************************************************************************/

#define thisTTClass			AngleDataspace
#define thisTTClassName		"dataspace.angle"
#define thisTTClassTags		"dataspace, angle"

TT_DATASPACELIB_CONSTRUCTOR
{	
	// Create one of each kind of unit, and cache them in a hash
	registerUnit(TT("unit.radian"),	TT("radian"));
	registerUnit(TT("unit.radian"),	TT("rad"));
	registerUnit(TT("unit.degree"),	TT("degree"));
	registerUnit(TT("unit.degree"),	TT("deg"));
	
	// Set our neutral unit (the unit through which all conversions are made)
	neutralUnit = TT("radian");
	
	// Now that the cache is created, we can create a set of default units
	setInputUnit(neutralUnit);
	setOutputUnit(neutralUnit);
}


AngleDataspace::~AngleDataspace()
{
	;
}

#undef thisTTClass
#undef thisTTClassName
#undef thisTTClassTags

