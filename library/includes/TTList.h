/* 
 * TTBlue (Linked) List Class
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#ifndef __TT_LIST_H__
#define __TT_LIST_H__

#include "TTElement.h"
#include "TTValue.h"
#include "TTMutex.h"
#include <list>
using namespace std;

class TTObject;
typedef TTObject* TTObjectPtr;
typedef list<TTValue*>::iterator	TTListIter;

/****************************************************************************************************/
// Class Specification

class TTFOUNDATION_EXPORT TTList : TTElement {
private:
	TTBoolean	threadProtection;	///< Use thread safety mechanisms.  Only disable this if you are certain that you will be calling from a single thread.
	TTMutex*	mutex;

	#ifdef TT_PLATFORM_WIN
	#pragma warning(disable:4251)
	#endif
	std::list<TTValue*>	theList;
	
	void lock();
	void unlock();
	
public:
	TTList();
	virtual ~TTList();

	/** Determine the number of values in the list.
		@return	The count of the values in the list.	*/
	TTUInt32 getSize();
	
	/** Return the first value in the list. 
		@return	The first value in the list.			*/
	TTValue& getHead();
	
	/** Return the last value in the list.
		@return	The last value in the list.				*/
	TTValue& getTail();
	
	/**	Return a value by it's location in the list.	*/
	TTErr getIndex(TTUInt32 index, TTValue& returnedValue);
	
	/** Appends a value to the list. 
		@param	newValue	The value to add to the list.  */
	void append(const TTValue& newValue);
	
	/**	If we don't define a version of this function that takes a pointer, 
		then when a pointer is provided a new temporary TTValue is created to provide the reference
		and then when the temporary is deleted we end up with a corrupt entry in the linked list.		*/
	void append(const TTValuePtr newValue)
	{
		append(*newValue);
	}
	
	void appendUnique(const TTValuePtr newValue);
	
	/** Appends a list to the list.  
		@param	newList	The list to add to the list.  */
	void merge(TTList& newList);
	
	/** Find a value in the list that is equal to a value passed-in. */
	TTErr findEquals(const TTValue& valueToCompareAgainst, TTValue& foundValue);
	
	/** Remove the specified value. 
		This doesn't change the value or free it, it just removed the pointer to it from the list.
		@param	The value to remove.					*/
	void remove(const TTValue& value);
	
	/** Remove all values from the list					*/
	void clear();
	
	/**	Remove all values from the list and free them.	*/
	void free();

	/**	Assign the contents of the list to a value as an array.	*/
	void assignToValue(TTValue& value);
	
	/**	Traverse the entire list, sending each item of the list to a specified object with the specified message.	*/
	TTErr iterate(const TTObjectPtr target, const TTSymbolPtr messageName);
	
	/**	Traverse the entire list, and if the item in the list is an object, then send it the specified message.		*/
	TTErr iterateObjectsSendingMessage(const TTSymbolPtr messageName);
	TTErr iterateObjectsSendingMessage(const TTSymbolPtr messageName, TTValue& aValue);
	
	
	void setThreadProtection(TTBoolean newThreadProtection)
	{	
		threadProtection = newThreadProtection;
	}
};


typedef TTList* TTListPtr;


#endif // __TT_LIST_H__

