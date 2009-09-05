/* 
 * TTBlue Class for representing Values
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#include "TTValue.h"
#include "TTSymbolTable.h"
#include "TTObject.h"


/****************************************************************************************************/

TTValue::TTValue()
	: type(NULL), data(NULL), numValues(0)
{
	clear();
}

TTValue::TTValue(const TTFloat32 initialValue)
{
	init();
	data->float32 = initialValue;
	*type = kTypeFloat32;
}

TTValue::TTValue(const TTFloat64 initialValue)
{
	init();
	data->float64 = initialValue;
	*type = kTypeFloat64;
}

TTValue::TTValue(const TTInt8 initialValue)
{
	init();
	data->int8 = initialValue;
	*type = kTypeInt8;
}

TTValue::TTValue(const TTUInt8 initialValue)
{
	init();
	data->uint8 = initialValue;
	*type = kTypeUInt8;
}

TTValue::TTValue(const TTInt16 initialValue)
{
	init();
	data->int16 = initialValue;
	*type = kTypeInt16;
}

TTValue::TTValue(const TTUInt16 initialValue)
{
	init();
	data->uint16 = initialValue;
	*type = kTypeUInt16;
}

# if 1
	TTValue::TTValue(const int initialValue)
#else
	TTValue::TTValue(const TTInt32 initialValue)
#endif	
//TTValue::TTValue(const TTInt32 initialValue)
{
	init();
	data->int32 = initialValue;
	*type = kTypeInt32;
}

TTValue::TTValue(const TTUInt32 initialValue)
{
	init();
	data->uint32 = initialValue;
	*type = kTypeUInt32;
}

TTValue::TTValue(const TTInt64 initialValue)
{
	init();
	data->int64 = initialValue;
	*type = kTypeInt64;
}

TTValue::TTValue(const TTUInt64 initialValue)
{
	init();
	data->uint64 = initialValue;
	*type = kTypeUInt64;
}

TTValue::TTValue(const TTBoolean initialValue)
{
	init();
	data->boolean = initialValue;
	*type = kTypeBoolean;
}

TTValue::TTValue(const TTSymbolPtr initialValue)
{
	init();
	data->sym = initialValue;
	*type = kTypeSymbol;
}

TTValue::TTValue(const TTString& initialValue)
{
	init();
	data->stringPtr = new TTString(initialValue);
	*type = kTypeString;
	stringsPresent = true;
}

TTValue::TTValue(const TTObject& initialValue)
{
	init();
	data->object = (TTObjectPtr)&initialValue;
	*type = kTypeObject;
}

TTValue::TTValue(const TTValue& obj)
{
	numValues = obj.numValues;
 	type = new TTDataType[numValues];
	data = new DataValue[numValues];
	memcpy(type, obj.type, sizeof(TTDataType) * numValues);
	memcpy(data, obj.data, sizeof(DataValue) * numValues);
}

TTValue::TTValue(const TTPtr initialValue)
{
	init();
	data->ptr = initialValue;
	*type = kTypePointer;
}

TTValue::TTValue(const TTFloat64 initialValue1, const TTFloat64 initialValue2)
{
	init();
	setSize(2);
	set(0, initialValue1);
	set(1, initialValue2);
}


TTValue::~TTValue()
{
	if(stringsPresent){
		for(TTUInt16 i=0; i<numValues; i++){
			if(type[i] == kTypeString)
				delete data[i].stringPtr;
		}
	}
	delete [] type;
	delete [] data;
}


void TTValue::clear()
{
	if(stringsPresent){
		for(TTUInt16 i=0; i<numValues; i++){
			if(type[i] == kTypeString)
				delete data[i].stringPtr;
		}
		stringsPresent = false;
	}
	delete [] data;
	delete [] type;

	init();
	data->float64 = 0.0;
	*type = kTypeNone;
	numValues = 0;		// Important: We want TTValue to contain zero values so we can build an array dynamically with the append() method [TAP]
}


TTDataType TTValue::getType() const
{
	return *type;
}


TTDataType TTValue::getType(const TTUInt16 index) const
{
	return type[index];
}


TTUInt16 TTValue::getSize() const
{
	return numValues;
}


void TTValue::init()
{
	type = new TTDataType;
	data = new DataValue;
	numValues = 1;
	stringsPresent = false;
}


void TTValue::setType(const TTDataType arg)
{
	*type = arg;
	if(arg == kTypeString)
		stringsPresent = true;
}


void TTValue::setSize(const TTUInt16 arg)
{
	if(arg != numValues){
		TTUInt16	safeNumValues = (arg < numValues) ? arg : numValues;	// The safe number of values that can be copied.
		
		numValues = arg;
		TTDataType* t = new TTDataType[numValues];
		DataValue* d = new DataValue[numValues];
		if(safeNumValues){
			memcpy(t, type, sizeof(TTDataType) * safeNumValues);
			memcpy(d, data, sizeof(TTValue::DataValue) * safeNumValues);
		}
		delete [] type;
		delete [] data;
		type = t;
		data = d;
	}
}


void TTValue::copy(const TTValue& obj)
{
	numValues = obj.numValues;
	TTDataType* t = new TTDataType[numValues];
	DataValue* d = new DataValue[numValues];		
	memcpy(t, obj.type, sizeof(TTDataType) * numValues);
	memcpy(d, obj.data, sizeof(DataValue) * numValues);
	delete [] type;
	delete [] data;
	type = t;
	data = d;
}


TTValue& TTValue::operator = (const TTValue &newValue)
{
	if(this != &newValue)
		copy(newValue);
	
	return *this;
}	


// FLOAT32
TTValue& TTValue::operator = (TTFloat32 value) 
{
	setSize(1);
	*type = kTypeFloat32;
	data->float32 = value;
	return *this;
}

TTValue::operator TTFloat32() const
{
	if(*type == kTypeFloat32)
		return data->float32;
	else{
		TTUInt16 index = 0;
		TTFloat32 value;
		CONVERT(TTFloat32);
		return value;
	}
}


// FLOAT64
TTValue& TTValue::operator = (TTFloat64 value)
{
	setSize(1);
	*type = kTypeFloat64;
	data->float64 = value;
	return *this;
}

TTValue::operator TTFloat64() const
{
	if(*type == kTypeFloat64)
		return data->float64;
	else{
		TTUInt16 index = 0;
		TTFloat64 value;
		CONVERT(TTFloat64)
		return value;
	}
}


// INT8
TTValue& TTValue::operator = (TTInt8 value)
{
	setSize(1);
	*type = kTypeInt8;
	data->int8 = value;
	return *this;
}

TTValue::operator TTInt8() const
{
	if(*type == kTypeInt8)
		return data->int8;
	else{
		TTUInt16 index = 0;
		TTInt8 value;
		CONVERT(TTInt8)
		return value;
	}
}


// UINT8
TTValue& TTValue::operator = (TTUInt8 value)
{
	setSize(1);
	*type = kTypeUInt8;
	data->uint8 = value;
	return *this;
}

TTValue::operator TTUInt8() const
{
	if(*type == kTypeUInt8)
		return data->uint8;
	else{
		TTUInt16 index = 0;
		TTUInt8 value;
		CONVERT(TTUInt8)
		return value;
	}
}


// INT16
TTValue& TTValue::operator = (TTInt16 value)
{
	setSize(1);
	*type = kTypeInt16;
	data->int16 = value;
	return *this;
}

TTValue::operator TTInt16() const
{
	if(*type == kTypeInt16)
		return data->int16;
	else{
		TTUInt16 index = 0;
		TTInt16 value;
		CONVERT(TTInt16)
		return value;
	}
}


// UINT16
TTValue& TTValue::operator = (TTUInt16 value)
{
	setSize(1);
	*type = kTypeUInt16;
	data->uint16 = value;
	return *this;
}

TTValue::operator TTUInt16() const
{
	if(*type == kTypeUInt16)
		return data->uint16;
	else{
		TTUInt16 index = 0;
		TTUInt16 value;
		CONVERT(TTUInt16)
		return value;
	}
}


// INT32
TTValue& TTValue::operator = (TTInt32 value)
{
	setSize(1);
	*type = kTypeInt32;
	data->int32 = value;
	return *this;
}

TTValue::operator TTInt32() const
{
	if(*type == kTypeInt32)
		return data->int32;
	else{
		TTUInt16 index = 0;
		TTInt32 value;
		CONVERT(TTInt32)
		return value;
	}
}


// UINT32
TTValue& TTValue::operator = (TTUInt32 value)
{
	setSize(1);
	*type = kTypeUInt32;
	data->uint32 = value;
	return *this;
}

TTValue::operator TTUInt32() const
{
	if(*type == kTypeUInt32)
		return data->uint32;
	else{
		TTUInt16 index = 0;
		TTUInt32 value;
		CONVERT(TTUInt32)
		return value;
	}
}


// INT64
TTValue& TTValue::operator = (TTInt64 value)
{
	setSize(1);
	*type = kTypeInt64;
	data->int64 = value;
	return *this;
}

TTValue::operator TTInt64() const
{
	if(*type == kTypeInt64)
		return data->int64;
	else{
		TTUInt16 index = 0;
		TTInt64 value;
		CONVERT(TTInt64)
		return value;
	}
}


// UINT64
TTValue& TTValue::operator = (TTUInt64 value)
{
	setSize(1);
	*type = kTypeUInt64;
	data->uint64 = value;
	return *this;
}

TTValue::operator TTUInt64() const
{
	if(*type == kTypeUInt64)
		return data->uint64;
	else{
		TTUInt16 index = 0;
		TTUInt64 value;
		CONVERT(TTUInt64)
		return value;
	}
}


// BOOLEAN
TTValue& TTValue::operator = (TTBoolean value)
{
	setSize(1);
	*type = kTypeBoolean;
	data->boolean = value;
	return *this;
}

TTValue::operator TTBoolean() const
{
	if(*type == kTypeBoolean)
		return data->boolean;
	else{
		TTUInt16 index = 0;
		TTBoolean value;
		CONVERT(TTBoolean)
		return value;
	}
}


// SYMBOL
TTValue& TTValue::operator = (TTSymbol* value)
{
	setSize(1);
	if((TTSymbol*)this != value) {
		*type = kTypeSymbol;
		data->sym = value;
	}
	return *this;
}

TTValue::operator TTSymbol*() const
{
	if(*type == kTypeSymbol)
		return data->sym;
	else{
		return TT("");
	}
}


// STRING
TTValue& TTValue::operator = (TTString& value)
{
	setSize(1);

	if(!stringsPresent && *type != kTypeString)
		data->stringPtr = new TTString;
	*type = kTypeString;
	stringsPresent = true;
	*data->stringPtr = value;
	return *this;
}

TTValue::operator TTString&() const
{
	TT_ASSERT(ttvalue_cast_to_string_ref, *type == kTypeString);

	if(*type == kTypeString)
		return *data->stringPtr;
	else
		return *(new TTString(""));
		// TODO: This will cause a memory leak if there is an error, right?
}


// OBJECT
TTValue& TTValue::operator = (TTObject& value)
{
	setSize(1);
	*type = kTypeObject;
	data->object = &value;
	return *this;
}

TTValue::operator TTObject&() const
{
	TT_ASSERT(ttvalue_cast_to_object_ref, *type == kTypeObject);

	if(*type == kTypeObject)
		return *data->object;
	else{
		// TODO: This is an error, not sure what to do...
		return *data->object;
	}
}

TTValue& TTValue::operator = (TTObject* value)
{
	setSize(1);
	*type = kTypeObject;
	data->object = value;
	return *this;
}

TTValue::operator TTObject*() const
{
	TT_ASSERT(ttvalue_cast_to_object_ptr, *type == kTypeObject);
	
	if(*type == kTypeObject)
		return data->object;
	else
		return NULL;
}


// POINTER
TTValue& TTValue::operator = (TTPtr value)
{
	setSize(1);
	if((TTSymbol*)this != value) {
		*type = kTypePointer;
		data->ptr = value;
	}
	return *this;
}

TTValue::operator TTPtr() const
{
	if(*type == kTypePointer)
		return data->ptr;
	else
		return NULL;
}




void TTValue::set(const TTUInt16 index, const TTFloat32 newValue)
{
	type[index] = kTypeFloat32;
	data[index].float32 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTFloat64 newValue)
{
	type[index] = kTypeFloat64;
	data[index].float64 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTInt8 newValue)
{
	type[index] = kTypeInt8;
	data[index].int8 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTUInt8 newValue)
{
	type[index] = kTypeUInt8;
	data[index].uint8 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTInt16 newValue)
{
	type[index] = kTypeInt16;
	data[index].int16 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTUInt16 newValue)
{
	type[index] = kTypeUInt16;
	data[index].uint16 = newValue;
}

# if 1
	void TTValue::set(const TTUInt16 index, const int newValue)
#else
	void TTValue::set(const TTUInt16 index, const TTInt32 newValue)
#endif
//void TTValue::set(const TTUInt16 index, const TTInt32 newValue)
{
	type[index] = kTypeInt32;
	data[index].int32 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTUInt32 newValue)
{
	type[index] = kTypeUInt32;
	data[index].uint32 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTInt64 newValue)
{
	type[index] = kTypeInt64;
	data[index].int64 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTUInt64 newValue)
{
	type[index] = kTypeUInt64;
	data[index].uint64 = newValue;
}

void TTValue::set(const TTUInt16 index, const TTBoolean newValue)
{
	type[index] = kTypeBoolean;
	data[index].boolean = newValue;
}

void TTValue::set(const TTUInt16 index, const TTSymbol* newValue)
{
	type[index] = kTypeSymbol;
	data[index].sym = (TTSymbol*)newValue;
}

void TTValue::set(const TTUInt16 index, const TTObject& newValue)
{
	type[index] = kTypeObject;
	data[index].object = (TTObject*)&newValue;
}

void TTValue::set(const TTUInt16 index, const TTString& newValue)
{
	if(!stringsPresent || type[index] != kTypeString){
		type[index] = kTypeString;
		stringsPresent = true;
		data[index].stringPtr = new TTString(newValue);
	}
	else
		*data[index].stringPtr = newValue;
}

void TTValue::set(const TTUInt16 index, const TTPtr newValue)
{
	type[index] = kTypePointer;
	data[index].ptr = newValue;
}


// THESE FUNCTIONS CURRENTLY DO NO TYPE OR BOUNDS CHECKING !!!
// Should an error be returned on failure?
void TTValue::get(const TTUInt16 index, TTFloat32 &value) const
{
	if(type[index] == kTypeFloat32)
		value = (data+index)->float32;
	else
		CONVERT(TTFloat32)
}

void TTValue::get(const TTUInt16 index, TTFloat64 &value) const
{
	if(type[index] == kTypeFloat64)
		value = (data+index)->float64;
	else
		CONVERT(TTFloat64)
}

void TTValue::get(const TTUInt16 index, TTInt8 &value) const
{
	if(type[index] == kTypeInt8)
		value = (data+index)->int8;
	else
		CONVERT(TTInt8)
}

void TTValue::get(const TTUInt16 index, TTUInt8 &value) const
{
	if(type[index] == kTypeUInt8)
		value = (data+index)->uint8;
	else
		CONVERT(TTUInt8)
}

void TTValue::get(const TTUInt16 index, TTInt16 &value) const
{
	if(type[index] == kTypeInt16)
		value = (data+index)->int16;
	else
		CONVERT(TTInt16)
}

void TTValue::get(const TTUInt16 index, TTUInt16 &value) const
{
	if(type[index] == kTypeUInt16)
		value = (data+index)->uint16;
	else
		CONVERT(TTUInt16)
}

void TTValue::get(const TTUInt16 index, TTInt32 &value) const
{
	if(type[index] == kTypeInt32)
		value = (data+index)->int32;
	else
		CONVERT(TTInt32)
}

void TTValue::get(const TTUInt16 index, TTUInt32 &value) const
{
	if(type[index] == kTypeUInt32)
		value = (data+index)->uint32;
	else
		CONVERT(TTUInt32)
}

void TTValue::get(const TTUInt16 index, TTInt64 &value) const
{
	if(type[index] == kTypeInt64)
		value = (data+index)->int64;
	else
		CONVERT(TTInt64)
}

void TTValue::get(const TTUInt16 index, TTUInt64 &value) const
{
	if(type[index] == kTypeUInt64)
		value = (data+index)->uint64;
	else
		CONVERT(TTUInt64)
}

void TTValue::get(const TTUInt16 index, TTBoolean &value) const
{
	if(type[index] == kTypeBoolean)
		value = (data+index)->boolean;
	else
		CONVERT(TTBoolean)
}

void TTValue::get(const TTUInt16 index, TTSymbol** value) const
{
	if(type[index] == kTypeSymbol)
		*value = (data+index)->sym;
}

void TTValue::get(const TTUInt16 index, TTString& value) const
{
	if(type[index] == kTypeString)
		value = *(data+index)->stringPtr;
}

void TTValue::get(const TTUInt16 index, TTObject &value) const
{
	if(type[index] == kTypeObject)
		value = *(data+index)->object;
}

void TTValue::get(const TTUInt16 index, TTObject** value) const
{
	if(type[index] == kTypeObject)
		*value = (data+index)->object;
}

void TTValue::get(const TTUInt16 index, TTPtr* value) const
{
	if(type[index] == kTypePointer)
		*value = (data+index)->ptr;
}


void TTValue::append(const TTFloat32 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTFloat64 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTInt8 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTUInt8 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTInt16 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTUInt16 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

//void TTValue::append(const TTInt32 newValue)
void TTValue::append(const int newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTUInt32 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTInt64 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTUInt64 newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTBoolean newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTSymbol* newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTString& newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTObject& newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTPtr newValue)
{
	setSize(numValues + 1);
	set(numValues-1, newValue);
}

void TTValue::append(const TTValue* newValue)
{
	setSize(numValues + 1);
	data[numValues-1] = newValue->data[0];
	type[numValues-1] = newValue->type[0];
}



TTErr TTValue::transformCSVStringToSymbolArray()
{
	TTString	str;
	char*		cStr;
	char*		current;
	
	if(*type != kTypeString)
		return kTTErrInvalidType;
	
	str = *data->stringPtr;
	clear();
	
	cStr = new char[str.size()+1];
	strncpy(cStr, str.c_str(), str.size()+1);
	
	current = strrchr(cStr, ',');
	while(current){
		*current = 0;
		current++;
		
		// Do some basic whitespace stripping from the ends
		while(*current == ' ')
			current++;
		while(current[strlen(current)-1] == ' ')
			current[strlen(current)-1] = 0;
			
		append(TT(current));
		current = strrchr(cStr, ',');
	}
	append(TT(cStr));
	delete[] cStr;
	return kTTErrNone;
}


TTErr TTValue::parseFromString(TTImmutableCString parseString)
{
#error needs implementation

	TTString	str(parseString);	// TODO: make sure this does a copy so we don't corrupt parseString
	
	clear();
	
	// TODO: parse the string, calling append() as we go...
	
}

