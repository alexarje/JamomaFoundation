/* 
 * TTBlue Hash Table Class
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#ifndef __TT_HASH_H__
#define __TT_HASH_H__

#include "TTElement.h"
#include "TTValue.h"
#if defined( TT_PLATFORM_MAC ) || defined ( TT_PLATFORM_IPHONE )
#include <ext/hash_map>
using namespace __gnu_cxx;
#elif TT_PLATFORM_LINUX
#include <map>
#else // TT_PLATFORM_WIN
#include <hash_map>
using namespace stdext;	// Visual Studio 2008 puts the hash_map in this namespace
#endif

/**	A type that contains a key and a value. */
typedef pair<TTPtrSizedInt,TTValue>			TTKeyVal;
#ifdef TT_PLATFORM_LINUX
typedef map<TTPtrSizedInt,TTValue> TTHashMap;
#else
typedef hash_map<TTPtrSizedInt,TTValue>		TTHashMap;
#endif
typedef TTHashMap::const_iterator			TTHashMapIter;


/****************************************************************************************************/
// Class Specification

/**	The standard TTBlue hash table.  
	Maintains a collection of TTValue objects indexed by TTSymbol pointers.
	The TTValue objects can themselves include TTSymbol pointers, TTObjects, simple values, polymorphic arrays, etc.
 
	TTHash is implemented as a light wrapper class around the STL hash_map templates.
	While we could inherit from hash_map and then override the appropriate things, there are a few reasons to just contain the hash_map and wrap it.
	- For example, we may later make TTHash inherit from TTObject and want to address it with messages.
	- This approach isolates coders from having to worry about an template craziness
	- This approach offers the possibility, should the need ever arise, to change the underlying implementation away from STL.
 
 
*/
class TTFOUNDATION_EXPORT TTHash : TTElement {
private:
//	#ifdef TT_PLATFORM_WIN
//	#pragma warning(disable:4251)
//	#endif
	TTHashMap	hashMap;
	
public:
	TTHash();
	virtual ~TTHash();

	/** Insert an item into the hash table. */
	TTErr append(const TTSymbolPtr key, const TTValue& value);
	
	/** Find the value for the given key. */
	TTErr lookup(const TTSymbolPtr key, TTValue& value);
	
	/** Remove an item from the hash table. */
	TTErr remove(const TTSymbolPtr key);
	
	/** Remove all items from the hash table. */
	TTErr clear();
	
	/** Get an array of all of the keys for the hash table. */
	TTErr getKeys(TTValue& hashKeys);
	
	/** Return the number of keys in the hash table. */
	TTUInt32 getSize();
	
	/** Return true if the hash has nothing stored in it. */
	TTBoolean isEmpty();
};


typedef TTHash* TTHashPtr;


#endif // __TT_HASH_H__

