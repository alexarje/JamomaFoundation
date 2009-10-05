/* 
 * TTBlue (Linked) List Class
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#include "TTList.h"
#include "TTObject.h"
#include "TTEnvironment.h"


/****************************************************************************************************/

TTList::TTList()
	:threadProtection(YES)
{
	mutex = new TTMutex(false);
}


TTList::~TTList()
{
	delete mutex;
}


TTUInt32 TTList::getSize()
{
	return theList.size();
}
	

TTValue& TTList::getHead()
{
	return *theList.front();
}


TTValue& TTList::getTail()
{
	return *theList.back();
}


TTErr TTList::getIndex(TTUInt32 index, TTValue& returnedValue)
{
	TTErr		err = kTTErrValueNotFound;
	TTUInt32	i=0;
	
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		if (i==index) {
			err = kTTErrNone;
			returnedValue = **iter;
			break;
		}
		i++;
	}
	unlock();
	
	return err;
}


void TTList::append(const TTValue& newValue)
{
	lock();
	theList.insert(theList.end(), (TTValue*)&newValue);
	unlock();
}

void TTList::appendUnique(const TTValuePtr newValue)
{
	TTErr	err;
	TTValue	foundValue;
	
	err = findEquals(*newValue, foundValue);
	if(err == kTTErrValueNotFound)
		append(*newValue);
}


void TTList::merge(TTList& newList)
{
	lock();
	theList.merge(newList.theList);
	unlock();
}


TTErr TTList::findEquals(const TTValue& valueToCompareAgainst, TTValue& foundValue)
{
	TTErr err = kTTErrValueNotFound;
	
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		if((**iter) == valueToCompareAgainst){
			foundValue = **iter;
			err = kTTErrNone;
			break;
		}
	}
	unlock();
	
	return err;
}


void TTList::remove(const TTValue& value)
{
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		if((**iter) == value){
			theList.remove(*iter);
			break;
		}
	}
	unlock();
}


void TTList::clear()
{
	lock();
	theList.clear();
	unlock();
}


void TTList::free()
{
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		TTValuePtr v = *iter;
		if (v->getType() == kTypeObject) {
			TTObjectPtr o = NULL;
			v->get(0, &o);
			TTObjectRelease(&o);
		}
		delete *iter;
	}	
	theList.clear();
	unlock();
}


void TTList::assignToValue(TTValue& value)
{
	TTListIter	iter;
	
	value.clear();
	
	lock();
	for(iter = theList.begin(); iter != theList.end(); iter++){
		value.append(*iter);
	}
	unlock();	
}


TTErr TTList::iterate(const TTObjectPtr target, const TTSymbolPtr messageName)
{
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		target->sendMessage(messageName, **iter);
	}
	unlock();
	return kTTErrNone;	
}


TTErr TTList::iterateObjectsSendingMessage(const TTSymbolPtr messageName)
{
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		TTObjectPtr obj = NULL;
		
		(*iter)->get(0, &obj);
		if(obj && obj->valid)
			obj->sendMessage(messageName);
	}
	unlock();
	return kTTErrNone;
}


TTErr TTList::iterateObjectsSendingMessage(const TTSymbolPtr messageName, TTValue& aValue)
{
	lock();
	for(TTListIter iter = theList.begin(); iter != theList.end(); iter++){
		TTObjectPtr obj = NULL;
		
		(*iter)->get(0, &obj);
		if(obj && obj->valid)
			obj->sendMessage(messageName, aValue);
	}
	unlock();
	return kTTErrNone;
}


void TTList::lock()
{
	if(threadProtection)
		mutex->lock();
}

void TTList::unlock()
{
	if(threadProtection)
		mutex->unlock();
}

