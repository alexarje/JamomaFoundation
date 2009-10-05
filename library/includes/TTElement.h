/*
 * TTBlue Base Class
 * Copyright © 2008, Timothy Place
 *
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef __TT_ELEMENT_H__
#define __TT_ELEMENT_H__

// Platform Sniffing
// Ideally the platform would already be set with a -D option to gcc...
#ifndef TT_PLATFORM_LINUX
	#ifndef TT_PLATFORM_WIN
		#ifdef WIN_VERSION
			#define TT_PLATFORM_WIN
		#else
			#define TT_PLATFORM_MAC
		#endif
	#endif
#endif

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

#ifdef TT_PLATFORM_LINUX
#include <stdarg.h>
#include <string.h>
#endif

#ifdef TT_PLATFORM_WIN
	#include "windows.h"
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	#define snprintf _snprintf
	#ifdef TTFOUNDATION_EXPORTS
		#define TTFOUNDATION_EXPORT __declspec(dllexport)
	#else
		#ifdef TTSTATIC
			#define TTFOUNDATION_EXPORT
		#else
			#define TTFOUNDATION_EXPORT __declspec(dllimport)
		#endif
	#endif // _DLL_EXPORT

#else // TT_PLATFORM_MAC
	#ifdef TTFOUNDATION_EXPORTS
		#define TTFOUNDATION_EXPORT __attribute__((visibility("default")))
	#else
		#define TTFOUNDATION_EXPORT
	#endif
#endif


#ifndef NO
#define NO 0
#endif

#ifndef YES
#define YES 1
#endif


#ifdef TT_DEBUG
#define TT_ENABLE_ASSERTIONS
#endif

#ifdef TT_ENABLE_ASSERTIONS
#define TT_ASSERT(name, result) \
				if(!result){\
					char* nullPtr = 0;\
					TTLogError("%s:%ld ASSERTION %s FAILED\n", __FILE__, __LINE__, #name );\
					*nullPtr = 1;\
				}
#else
#define TT_ASSERT(name, result) ((void)(0));
#endif


/****************************************************************************************************/
// Type Definitions

// Note http://developer.apple.com/mac/library/documentation/Darwin/Conceptual/64bitPorting/MakingCode64-BitClean/MakingCode64-BitClean.html#//apple_ref/doc/uid/TP40001064-CH226-SW2

typedef bool				TTBoolean;			// same as Boolean on the Mac
typedef char*				TTCString;
typedef const char*			TTImmutableCString;
typedef std::string			TTString;

typedef signed char			TTInt8;
typedef unsigned char		TTUInt8;
typedef signed short		TTInt16;
typedef unsigned short		TTUInt16;

#ifdef __LP64__		// Mac 64-bit
	typedef signed int			TTInt32;
	typedef unsigned int		TTUInt32;
#else				// Mac 32-bit, Win32 32-bit
	typedef signed long			TTInt32;
	typedef unsigned long		TTUInt32;
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
	typedef __int64				TTInt64;
	typedef unsigned __int64	TTUInt64;
#else
	#if defined(__LP64__)	// Mac 64-bit
		typedef signed long			TTInt64;
		typedef unsigned long		TTUInt64;
	#else // Max 32-bit
		typedef signed long long	TTInt64;
		typedef unsigned long long	TTUInt64;
	#endif
#endif

typedef float				TTFloat32;
typedef double				TTFloat64;

/** A value representing a single audio sample.  TTSampleValue should be used any place a sample value is what the value represents.  This will enable us to change the type in the future if needed.  For example, to use 64-bit floats. */
typedef TTFloat64			TTSampleValue;
/** A TTSampleVector is simply a pointer to the first of an array of TTSampleValues. */
typedef TTSampleValue*		TTSampleVector;

/** An integer that is the same size as a pointer.	*/
typedef long				TTPtrSizedInt;				// this works for both 32 and 64 bit code on the Mac

/** A generic pointer. */
typedef void*				TTPtr;


/**	A simple/generic function pointer with no args.	*/
typedef void (*TTFunctionPtr)();

/**	A simple/generic function pointer with one arg.	*/
typedef void (*TTFunctionWithArgPtr)(TTPtr);

/**	A simple/generic function pointer with one generic pointer (baton) and one TTValueRef.	*/
class TTValue;
typedef void (*TTFunctionWithBatonAndValue)(TTPtr, TTValue&);


/****************************************************************************************************/

/**	TTBlue Data Types
 *	Enumeration of data types used through out TTBlue, including the TTValue class and declaring the types of
 *	TTAttribute objects.																			*/
enum TTDataType{
	kTypeNone = 0,
	kTypeFloat32,
	kTypeFloat64,
	kTypeInt8,
	kTypeUInt8,
	kTypeInt16,
	kTypeUInt16,
	kTypeInt32,
	kTypeUInt32,
	kTypeInt64,
	kTypeUInt64,
	kTypeBoolean,
	kTypeSymbol,
	kTypeObject,
	kTypePointer,
	kTypeString,
	kTypeLocalValue,	///< This is a special type used by TTAttribute to indicate that a value is a TTValue and is locally maintained.
	kNumTTDataTypes
};


class TTSymbol;		// forward declaration
class TTDataInfo;
typedef TTDataInfo* TTDataInfoPtr;

/**	An array, indexed by values from TTDataType, containing information about those data types.	*/
extern TTFOUNDATION_EXPORT TTDataInfoPtr	ttDataTypeInfo[kNumTTDataTypes];


class TTFOUNDATION_EXPORT TTDataInfo {
public:
	TTSymbol*	name;			///< The name of the type as a symbol, e.g. float32, float64, etc.
	TTBoolean	isNumerical;	///< Is this type numeric?
	TTInt8		bitdepth;		///< Negative numbers indicate dynamic or unknown bitdepth.

	static TTDataInfoPtr getInfoForType(TTDataType type)
	{
		return ttDataTypeInfo[type];
	}

	static TTBoolean getIsNumerical(TTDataType type)
	{
		return ttDataTypeInfo[type]->isNumerical;
	}

	static void addDataInfoForType(TTDataType type);

};



/**	TTBlue Error Codes
 *	Enumeration of error codes that might be returned by any of the TTBlue functions and methods.	*/
enum TTErr {
	kTTErrNone = 0,			///< No Error.
	kTTErrGeneric,			///< Something went wrong, but what exactly is not known.  Typically used for context-specific problems.
	kTTErrAllocFailed,		///< Couldn't get memory.
	kTTErrFreeFailed,		///< Couldn't free memory.
	kTTErrInvalidType,		///< Bad DataType for the context.
	kTTErrInvalidAttribute,	///< Bad Attribute specified.
	kTTErrInvalidValue,		///< An inappropriate value was specified for an attribute or variable.
	kTTErrWrongNumValues,	///< The wrong number of values were passed to a method or attribute.
	kTTErrMethodNotFound,	///< Method not found.  Typically returned by the TTObject::sendMessage() function.
	kTTErrValueNotFound,	///< A value was not found when doing a look up for it (in a TTHash, TTList, or other class).
	kTTErrBadChannelConfig,	///< An invalid number of audio channels for a given context was encountered.
	kTTErrReadOnly			///< Attempted a write to a read-only entity.
};


/****************************************************************************************************/
// Class Specifications


/**	A TTBlue exception is thown with this object. */
class TTFOUNDATION_EXPORT TTException {
	TTImmutableCString	reason;
public:
	TTException(TTImmutableCString aReason)
	: reason(aReason)
	{}
};


/**	The required base-class from which all TTBlue objects must inherit.
 	This object is the primary base-class for all TTBlue objects, including TTObject.
 	It does not define any real functionality.
	Instead it provides a way to group and work polymorphically with any class in TTBlue,
	including both TTValue and TTObject.													*/
class TTFOUNDATION_EXPORT TTElement {
public:
	TTElement();			///< Constructor.
	virtual ~TTElement();	///< Destructor.
};


TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupEqualPower[];			///< Equal Power lookup table
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupHalfPaddedwWelch[];		///< 256 point window table (the first half of it)
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupQuarterSine[];			///< Quarter Sine lookup table
TTFOUNDATION_EXPORT extern const TTFloat64 kTTPi;							///< pre-calculated value of pi
TTFOUNDATION_EXPORT extern const TTFloat64 kTTTwoPi;						///< pre-calculated value of pi * 2
TTFOUNDATION_EXPORT extern const TTFloat64 kTTAntiDenormalValue;			///< constant used by the ttantidenormal function
TTFOUNDATION_EXPORT extern const TTFloat64 kTTSqrt2;						///< pre-calculated square-root of 2


/** Platform and host independent method for posting messages. */
void TTFOUNDATION_EXPORT TTLogMessage(TTImmutableCString message, ...);

/** Platform and host independent method for posting messages. */
void TTFOUNDATION_EXPORT TTLogWarning(TTImmutableCString message, ...);

/** Platform and host independent method for posting errors. */
void TTFOUNDATION_EXPORT TTLogError(TTImmutableCString message, ...);

/** Platform and host independent method for posting messages only when debugging is enabled in the environment. */
void TTFOUNDATION_EXPORT TTLogDebug(TTImmutableCString message, ...);


/** Filter out denormaled values, which can make processing extremely slow when they are present. */
template<class T>
static T TTAntiDenormal(T value)
{
#ifndef TT_DISABLE_DENORMAL_FIX
	value += kTTAntiDenormalValue;
	value -= kTTAntiDenormalValue;
#endif
	return(value);
}

/** A fast routine for clipping a number to a range.  This routine does not use branching. */
template<class T>
static T TTClip(T value, T low_bound, T high_bound)
{
//	TODO: need to benchmark this again now that we are doing this additional casting to a double before calling fabs().
//	TODO: is there a way in a template to find out the actual type of the input so that we can handle unsigned types differently from signed types?
//	CHANGED: on the Mac at least, a call to fabs() on an unsigned type like TTUInt32 will always return zero, thus making this template return bogus results [TAP]
//	#ifdef TT_PLATFORM_MAC
//		value = T(((fabs(value - low_bound)) + (low_bound + high_bound)) - fabs(value - high_bound));
//	#else	// VC++ gens an ERROR because of the ambiguous call to fabs().  This is annoying...
		value = T(((fabs(value - double(low_bound))) + (low_bound + high_bound)) - fabs(value - double(high_bound)));
//	#endif
	value /= 2;		// relying on the compiler to optimize this, chosen to reduce compiler errors in Xcode
	return value;
}

/** A fast routine for clipping a number to a maximum range.  The bottom end of the range is not checked.  This routine does not use branching. */
template<class T>
static T TTLimitMax(T value, T high_bound)
{
	value = high_bound - value;
	#ifdef TT_PLATFORM_MAC
		value += fabs(value);
	#else
		value = T(value + fabs((double)value));
	#endif
	value = T(value * 0.5);
	value = high_bound - value;
	return value;
}

/** A fast routine for clipping a number on it's low range.  The high end of the range is not checked.
	This routine does not use branching. */
template<class T>
static T TTLimitMin(T value, T low_bound)
{
	value -= low_bound;
	#ifdef TT_PLATFORM_MAC
		value += fabs(value);
	#else
		value = T(value + fabs((double)value));
	#endif
	value = T(value * 0.5);
	value = T(value + low_bound);
	return value;
}

/** A fast routine for wrapping around the range once.  This is faster than doing an expensive module, where you know the range of the input
 	will not equal or exceed twice the range. */
template<class T>
static T TTOneWrap(T value, T low_bound, T high_bound)
{
	if((value >= low_bound) && (value < high_bound))
		return value;
	else if(value >= high_bound)
		return((low_bound - 1) + (value - high_bound));
	else
		return((high_bound + 1) - (low_bound - value));
}
/** this routine wrapping around the range as much as necessary, Nils Peters, Nov. 2008 */
template<class T>
static T TTInfWrap(T value, T low_bound, T high_bound)
{
	if((value >= low_bound) && (value < high_bound))
		return value; //nothing to wrap
	/* let's wrap it */
	else if (value - low_bound >= 0)
		return(fmod((double)value  - low_bound, fabs((double)low_bound - high_bound)) + low_bound);
	else
		return(-1.0 * fmod(-1.0 * (value  - low_bound), fabs((double)low_bound - high_bound)) + high_bound);
}

/** this routine folds numbers into the data range, Nils Peters, Nov. 2008 */
template<class T>
static T TTFold(T value, T low_bound, T high_bound)
{
	double foldRange;

	if((value >= low_bound) && (value <= high_bound))
		return value; //nothing to fold
	else{
		foldRange = 2 * fabs((double)low_bound - high_bound);
#ifdef TT_PLATFORM_WIN
		// The standard remainder() function is not present on Windows, so we do it ourselves.
		double	v = value - low_bound;
		double	d = v / foldRange;
		long	n = TTRound(d);
		double	r = v - n * foldRange;
		return fabs(r);
#else
		return fabs(remainder(value - low_bound, foldRange)) + low_bound;
#endif
	}
}



/** A utility for scaling one range of values onto another range of values. */
template<class T>
static T TTScale(T value, T inlow, T inhigh, T outlow, T outhigh)
{
	double inscale, outdiff;

	inscale = 1 / (inhigh - inlow);
	outdiff = outhigh - outlow;

	value = (value - inlow) * inscale;
	value = (value * outdiff) + outlow;
	return(value);
}

/** Rounding utility. */
template<class T>
static TTInt32 TTRound(T value)
{
	if(value > 0)
		return((long)(value + 0.5));
	else
		return((long)(value - 0.5));
}


#endif // __TT_ELEMENT_H__

