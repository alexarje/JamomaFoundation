/*
 * TTFoundation Limiting and Constraining Utilities
 * Copyright © 2009, Timothy Place and Tristan Matthews
 *
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef __TT_LIMITS_H__
#define __TT_LIMITS_H__


/** Filter out denormaled values, which can make processing extremely slow when present. 
	@seealso	TTZeroDenormal
 */
template<class T>
static T TTAntiDenormal(const T input)
{
	T output = input;
	TTZeroDenormal(output);
	return output;
	
	// This function used to be implemented using the following algorithm:
	// 
	// value += kTTAntiDenormalValue;
	// value -= kTTAntiDenormalValue;
	//
	// That algorithm has the advantage of not branching.  On the PPC this yielded a dramatic performance improvement.
	// When benchmarked on Intel processors, however, it was significantly slightly slower (roughly 50%) than the more traditional
	// branching version, which is now implemented in the TTZeroDenormal() function.
}


/** Filter out denormaled values, which can make processing extremely slow when present.  Calculation is performed in-place.
 @seealso	TTZeroDenormal
 */
template<class T>
static void TTZeroDenormal(T& value)
{
#ifndef TT_DISABLE_DENORMAL_FIX
#ifdef TT_PLATFORM_WIN
	// MSVC is not standards-compliant, which includes lack of support for C99's fpclassify()
	value += kTTAntiDenormalValue;
	value -= kTTAntiDenormalValue;
#else // a good platform
	if (!isnormal(value))
		value = 0;
#endif
#endif
}


/** Constrain a number to within a range. 
	@seealso	TTLimit()
 */
template<class T>
static T TTClip(const T input, const T lowBound, const T highBound)
{
	T output = input;
	TTLimit(output, lowBound, highBound);
	return output;

	// This function used to be implemented using the following algorithm:
	// 
	// value = T(((fabs(value - double(low_bound))) + (low_bound + high_bound)) - fabs(value - double(high_bound)));
	// value /= 2;		// relying on the compiler to optimize this, chosen to reduce compiler errors in Xcode
	// return value;
	//
	// That algorithm has the advantage of not branching.  On the PPC this yielded a dramatic performance improvement.
	// When benchmarked on Intel processors, however, it was actually very slightly slower than the more traditional
	// branching version, which is now implemented in the TTLimit() function.
}


/** Constrain a number to within a range.  Calculation is performed in-place. 
	@seealso	TTClip()
 */
template<class T>
static void TTLimit(T& value, const T lowBound, const T highBound)
{
	if (value < lowBound)
		value = lowBound;
	else if (value > highBound)
		value = highBound;
}


/** A fast routine for clipping a number to a maximum range.  The bottom end of the range is not checked.  This routine does not use branching. */
template<class T>
static void TTLimitMax(T value, const T highBound)
{
	if (value > highBound)
		value = highBound;

	// old algorithm -- see comments in TTClip() for details
	// value = high_bound - value;
	// #ifdef TT_PLATFORM_MAC
	//	value += fabs(value);
	// #else
	//	value = T(value + fabs((double)value));
	// #endif
	// value = T(value * 0.5);
	// value = high_bound - value;
	// return value;
}


/** A fast routine for clipping a number on it's low range.  The high end of the range is not checked.
	This routine does not use branching. */
template<class T>
static void TTLimitMin(T value, const T lowBound)
{
	if (value < lowBound)
		value = lowBound;

	// old algorithm -- see comments in TTClip() for details
	// value -= low_bound;
	// #ifdef TT_PLATFORM_MAC
	//	value += fabs(value);
	// #else
	//	value = T(value + fabs((double)value));
	// #endif
	// value = T(value * 0.5);
	// value = T(value + low_bound);
	// return value;
}


/** A fast routine for wrapping around the range once.  This is faster than doing an expensive module, where you know the range of the input
 	will not equal or exceed twice the range. */
template<class T>
static T TTOneWrap(T value, T low_bound, T high_bound)
{
	if ((value >= low_bound) && (value < high_bound))
		return value;
	else if (value >= high_bound)
		return((low_bound - 1) + (value - high_bound));
	else
		return((high_bound + 1) - (low_bound - value));
}
/** this routine wrapping around the range as much as necessary, Nils Peters, Nov. 2008 */
template<class T>
static T TTInfWrap(T value, T low_bound, T high_bound)
{
	if ((value >= low_bound) && (value < high_bound))
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

	if ((value >= low_bound) && (value <= high_bound))
		return value; //nothing to fold
	else {
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
	if (value > 0)
		return((long)(value + 0.5));
	else
		return((long)(value - 0.5));
}




#if 0
#pragma mark -
#pragma mark - new code from Tristan
#endif

//#include <cassert>
//#include <cmath>
//#include <iostream>
//#include <algorithm>
	
template <class T>
T limitMin(T value, T low_bound)
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

// template specialization for the unsigned case in limitMin, as per http://redmine.jamoma.org/issues/show/300
// otherwise the algorithm can wrap around (below zero) and produce unexpected results

// at least on the mac, this specialization produces a confict at link time -- tap
template <class T>
TTUInt8 limitMin(TTUInt8 value, TTUInt8 low_bound)
{
    value -= std::min(low_bound, value); // so 0 at lowest
#ifdef TT_PLATFORM_MAC
    value += fabs(value);
#else
    value = TTUInt8(value + fabs((double)value));
#endif
    value = TTUInt8(value * 0.5);
    value = TTUInt8(value + low_bound);
    return value;
}

template <class T>
TTUInt16 limitMin(TTUInt16 value, TTUInt16 low_bound)
{
    value -= std::min(low_bound, value); // so 0 at lowest
#ifdef TT_PLATFORM_MAC
    value += fabs(value);
#else
    value = TTUInt16(value + fabs((double)value));
#endif
    value = TTUInt16(value * 0.5);
    value = TTUInt16(value + low_bound);
    return value;
}

template <class T>
TTUInt32 limitMin(TTUInt32 value, TTUInt32 low_bound)
{
    value -= std::min(low_bound, value); // so 0 at lowest
#ifdef TT_PLATFORM_MAC
    value += fabs(value);
#else
    value = TTUInt32(value + fabs((double)value));
#endif
 value = TTUInt32(value * 0.5);
 value = TTUInt32(value + low_bound);
 return value;
}
 
 
template <class T>	
TTUInt64 limitMin(TTUInt64 value, TTUInt64 low_bound)
{
	value -= std::min(low_bound, value); // so 0 at lowest
#ifdef TT_PLATFORM_MAC
	value += fabs(value);
#else
  value = TTUInt64(value + fabs((double)value));
#endif
	value = TTUInt64(value * 0.5);
	value = TTUInt64(value + low_bound);
	return value;
}


/*
int main(int argc, char* argv[])
{
	
	if (argc != 3)
	
	{
	
	    std::cout << "Usage: limitMin <value> <lowerBound>\n";
	
	    return 1;
	
	}
	
	
	TTUInt16 u, l;
	
	std::cout << "Enter a value:\n";
	
	u = atoi(argv[1]);
	
	std::cout << "u is " << u << std::endl;
	
	std::cout << "Enter a lower bound:\n";
	
	l = atoi(argv[2]);
	
	std::cout << "l is " << l << std::endl;
	
	
	long long iterations = 100000000LL;
	
	
	// execute a lot of times
	
	while (iterations--)
	
	    TTUInt16 v = limitMin<TTUInt16>(u, l);
	
	
	
	return 0;
	
}
*/







#endif // __TT_LIMITS_H__

