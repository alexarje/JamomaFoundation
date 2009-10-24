/* 
 * TTNodeDirectory
 * Copyright © 2008, Théo de la Hogue & Tim Place
 * 
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html 
 */

#include "TTNodeDirectory.h"

#define thisTTClass			TTNodeDirectory
#define thisTTClassName		"TTNodeDirectory"
#define thisTTClassTags		"tree"

TTNodeDirectory::TTNodeDirectory(TTSymbolPtr newName):TTObject(kTTValNONE)
{
	TTBoolean *nodeCreated = new TTBoolean(false);
	
	// Set the name of the tree
	this->name = newName;
	
	// create a new directory
	this->directory = new TTHash();

	// create a root (OSC style)
	TTNodeCreate(S_SEPARATOR, TT("container"), NULL, &this->root, nodeCreated);
}

TTNodeDirectory::~TTNodeDirectory()
{
	// TODO : delete all the node of the directory then the directory
	// WARNING : if you destroy all the directory, the root will be destroyed too
	// so don't destroy it again !!!
	this->root->~TTNode();
}

#if 0
#pragma mark -
#pragma mark Static Methods
#endif

TTSymbolPtr	TTNodeDirectory::getName(){return this->name;}
TTNodePtr	TTNodeDirectory::getRoot(){return this->root;}
TTHashPtr	TTNodeDirectory::getDirectory(){return this->directory;}

TTErr TTNodeDirectory::setName(TTSymbolPtr aName)
{
	this->name = aName;
	return kTTErrNone;
}


TTErr TTNodeDirectory::getTTNodeForOSC(const char* oscAddress, TTNodePtr* returnedTTNode)
{
	return getTTNodeForOSC(TT((char*)oscAddress), returnedTTNode);
}

TTErr TTNodeDirectory::getTTNodeForOSC(TTSymbolPtr oscAddress, TTNodePtr* returnedTTNode)
{
	TTErr err;
	TTValue* found = new TTValue();
	
	if(directory){
		// look into the hashtab to check if the address exist in the tree
		err = this->directory->lookup(oscAddress,*found);

		// if this address doesn't exist
		if(err == kTTErrValueNotFound){
			return kTTErrGeneric;
		}
		else{
			found->get(0,(TTPtr*)returnedTTNode);
			return kTTErrNone;
		}
	}
	return kTTErrGeneric;
}

TTErr TTNodeDirectory::TTNodeCreate(TTSymbolPtr oscAddress, TTSymbolPtr newType, void *newObject, TTNodePtr *returnedTTNode, TTBoolean *newInstanceCreated)
{
	TTSymbolPtr oscAddress_parent, oscAddress_name, oscAddress_instance, oscAddress_propertie, newInstance, oscAddress_got;
	TTBoolean parent_created;
	TTValue* found;
	TTNodePtr newTTNode, n_found;
	TTErr err;

	// Split the OSC address in /parent/name.instance:/propertie
	err = splitOSCAddress(oscAddress,&oscAddress_parent,&oscAddress_name, &oscAddress_instance, &oscAddress_propertie);

	// if no error in the parsing of the OSC address
	if(err == kTTErrNone){

		// If there is a propertie part
		if(oscAddress_propertie != NO_PROPERTIE){

			// get the TTNode
			mergeOSCAddress(&oscAddress_got,oscAddress_parent,oscAddress_name,oscAddress_instance,NO_PROPERTIE);
			found = new TTValue();
			err = this->directory->lookup(oscAddress_got, *found);

			// if the TTNode doesn't exist
			if(err == kTTErrValueNotFound)
				return kTTErrGeneric;

			else{
				// get the TTNode at this address
				found->get(0,(TTPtr*)&n_found);
				n_found->addPropertie(oscAddress_propertie, NULL, NULL);  // TODO : advise the user that he is creating an attribut without any access (get and set) method

				return kTTErrNone;
			}
		}

		// is there a TTNode with this address in the tree ?
		found = new TTValue();
		err = this->directory->lookup(oscAddress, *found);

		// if it's the first at this address
		if(err == kTTErrValueNotFound){
			// keep the instance found in the OSC address
			newInstance = oscAddress_instance;
			*newInstanceCreated = false;
		}
		else{
			// this address already exists
			// get the TTNode at this address
			found->get(0,(TTPtr*)&n_found);

			// Autogenerate a new instance
			n_found->getParent()->generateInstance(n_found->getName(), &newInstance);
			*newInstanceCreated = true;
		}

		// CREATION OF A NEW TTNode
		///////////////////////////

		// 1. Create a new TTNode
		newTTNode = new TTNode(oscAddress_name, newInstance, newType, newObject, this);

		// 2. Ensure that the path to the new TTNode exists
		if(oscAddress_parent != NO_PARENT){

			// set his parent
			parent_created = false;
			newTTNode->setParent(oscAddress_parent, &parent_created);

			// add the new TTNode as a children of his parent
			newTTNode->getParent()->setChild(newTTNode);
		}
		else
			// the new TTNode is the root : no parent
			;

		// 3. Add the effective address (with the generated instance) to the global hashtab
		newTTNode->getOscAddress(&oscAddress_got);
		this->directory->append(oscAddress_got,TTValue(newTTNode));

		// 4. returned the new TTNode
		*returnedTTNode = newTTNode;

		return kTTErrNone;
	}
	return kTTErrGeneric;
}

TTErr TTNodeDirectory::TTNodeRemove(TTSymbolPtr oscAddress)
{
	return this->directory->remove(oscAddress);
}

TTErr TTNodeDirectory::Lookup(TTSymbolPtr oscAddress, TTList& returnedTTNodes, TTNodePtr *firstReturnedTTNode)
{
	TTSymbolPtr oscAddress_parent, oscAddress_name, oscAddress_instance, oscAddress_propertie, oscAddress_nopropertie;
	TTList lk_selection, lk_children;
	TTNodePtr n_r;
	TTErr err;

	// Make sure we are dealing with valid OSC input by looking for a leading slash
	if(oscAddress->getCString()[0]!= C_SEPARATOR)
		return kTTErrGeneric;
	
	// Split the address /parent/name.instance:propertie
	splitOSCAddress(oscAddress, &oscAddress_parent, &oscAddress_name, &oscAddress_instance, &oscAddress_propertie);

	// Is there a wild card ?
	if(strrchr(oscAddress->getCString(), C_WILDCARD)){

		// Here is a recursive call to the TTNodeDirectory Lookup to get all TTNodes at upper levels
		err = Lookup(oscAddress_parent, returnedTTNodes, firstReturnedTTNode);
			
		// for each returned TTNodes at upper levels
		// select all corresponding "name.instance" TTNodes
		// among the TTNode list.
		
		if(!returnedTTNodes.isEmpty()){
			
			// select all children of all parent nodes
			for(returnedTTNodes.begin(); returnedTTNodes.end(); returnedTTNodes.next()){
				
				returnedTTNodes.current().get(0, (TTPtr*)&n_r);
				n_r->getChildren(oscAddress_name, oscAddress_instance, lk_children);
				
				if(!lk_children.isEmpty())
					lk_selection.merge(lk_children);
			}
			
			// return the slection
			returnedTTNodes.clear();
			
			if(!lk_selection.isEmpty()){
				returnedTTNodes.merge(lk_selection);
				returnedTTNodes.getHead().get(0, (TTPtr*)&firstReturnedTTNode);
			}
		}
		
		return err;
	}
	// no wild card : do a lookup in the global hashtab 
	// with the /parent/node.instance part (no propertie)
	else{
		
		// be sure there is no propertie part
		if(oscAddress_propertie != NO_PROPERTIE)
			mergeOSCAddress(&oscAddress_nopropertie, oscAddress_parent, oscAddress_name, oscAddress_instance, NO_PROPERTIE);
		else
			oscAddress_nopropertie = oscAddress;
		
		// look into the hashtab
		err = getTTNodeForOSC(oscAddress_nopropertie, &n_r);
		
		// if the node exists
		if(err == kTTErrNone){
			returnedTTNodes.append(new TTValue((TTPtr)n_r));
			*firstReturnedTTNode = n_r;
		}
		
		return err;
	}
}

TTErr	TTNodeDirectory::LookingFor(TTListPtr whereToSearch, bool(testFunction)(TTNodePtr node, void*args), void *argument, TTList& returnedTTNodes, TTNodePtr *firstReturnedTTNode)
{
	TTList lk_children;
	TTNodePtr n_r, n_child;
	TTErr err;
	
	// if there are nodes from where to start
	if(!whereToSearch->isEmpty()){
		
		// Launch a recursive research below each given nodes
		for(whereToSearch->begin(); whereToSearch->end(); whereToSearch->next()){
			
			// get all children of the node
			whereToSearch->current().get(0, (TTPtr*)&n_r);
			n_r->getChildren(S_WILDCARD, S_WILDCARD, lk_children);
			
			// if there are children
			if(!lk_children.isEmpty()){
				
				// test each of them and add those which are ok
				for(lk_children.begin(); lk_children.end(); lk_children.next()){
					
					lk_children.current().get(0, (TTPtr*)&n_child);
					
					// test the child and fill the returnedTTNodes
					if(testFunction(n_child, argument))
						returnedTTNodes.append(new TTValue((TTPtr)n_child));
				}
				
				// continu the research below all children
				err = LookingFor(&lk_children, testFunction, argument, returnedTTNodes, firstReturnedTTNode);
				
				returnedTTNodes.getHead().get(0, (TTPtr*)&firstReturnedTTNode);
				
				if(err != kTTErrNone)
					return err;
			}
		}
		return kTTErrNone;
	}
	return kTTErrGeneric;
}

TTErr	TTNodeDirectory::IsThere(TTListPtr whereToSearch, bool(testFunction)(TTNodePtr node, void*args), void *argument, bool *isThere, TTNodePtr *firstTTNode)
{
	TTList lk_children;
	TTNodePtr n_r, n_child;
	TTErr err, err_look;
	
	// if there are nodes from where to start
	if(!whereToSearch->isEmpty()){
		
		// Launch a recursive research below each given nodes
		for(whereToSearch->begin(); whereToSearch->end(); whereToSearch->next()){
			
			// get all children of the node
			whereToSearch->current().get(0, (TTPtr*)&n_r);
			err = n_r->getChildren(S_WILDCARD, S_WILDCARD, lk_children);
			
			// if there are children
			if(err == kTTErrNone){
				
				// test each of them and add those which are ok
				for(lk_children.begin(); lk_children.end(); lk_children.next()){
					
					lk_children.current().get(0, (TTPtr*)&n_child);
					
					// test the child and fill the returnedTTNodes
					if(testFunction(n_child, argument)){
						(*isThere) = true;
						(*firstTTNode) = n_child;
						return kTTErrNone;
					}
					else
						(*isThere) = false;
				}
				
				// continu the research from all children
				err_look = IsThere(&lk_children, testFunction, argument, isThere, firstTTNode);
				
				if(err_look != kTTErrNone)
					return err_look;
				
				// if a node is found below, stop the research
				if((*isThere))
					return kTTErrNone;
			}
			else
				(*isThere) = false;
		}
		return kTTErrNone;
	}
	return kTTErrGeneric;
}

/***********************************************************************************
 *
 *		GLOBAL METHODS
 *
 ************************************************************************************/

TTErr splitOSCAddress(TTSymbolPtr oscAddress, TTSymbolPtr* returnedParentOscAdress, TTSymbolPtr* returnedTTNodeName, TTSymbolPtr* returnedTTNodeInstance, TTSymbolPtr* returnedTTNodePropertie)
{
	long len, pos;
	char *last_colon, *last_slash, *last_dot;
	char *propertie, *parent, *instance;
	char *to_split;

	// Make sure we are dealing with valid OSC input by looking for a leading slash
	if(oscAddress->getCString()[0]!= C_SEPARATOR)
		return kTTErrGeneric;

	to_split = (char *)malloc(sizeof(char)*(strlen(oscAddress->getCString())+1));
	strcpy(to_split,oscAddress->getCString());

	// find the last ':' in the OSCaddress
	// if exists, split the OSC address in an address part (to split) and an propertie part
	len = strlen(to_split);
	last_colon = strrchr(to_split, C_PROPERTIE);
	pos = (long)last_colon - (long)to_split;

	if(last_colon){
		propertie = (char *)malloc(sizeof(char)*(len - (pos+1)));
		strcpy(propertie,to_split + pos+1);
		*returnedTTNodePropertie = TT(propertie);

		to_split[pos] = NULL;	// split to keep only the address part
	}
	else
		*returnedTTNodePropertie = NO_PROPERTIE;
	
	// find the last '/' in the address part
	// if exists, split the address part in a TTNode part (to split) and a parent part
	len = strlen(to_split);
	last_slash = strrchr(to_split, C_SEPARATOR);
	pos = (long)last_slash - (long)to_split;

	if(last_slash){
		if(pos){ // In the root case pos == 0
			parent = (char *)malloc(sizeof(char)*(pos+1));
			strncpy(parent,to_split,pos);
			parent[pos] = NULL;
			*returnedParentOscAdress = TT(parent);
			to_split = last_slash+1;	// split to keep only the TTNode part
		}
		else{
			// Is it the root or a child of the root ?
			if(strlen(to_split) > 1){
				*returnedParentOscAdress = S_SEPARATOR;
				to_split = last_slash+1;	// split to keep only the TTNode part
			}
			else
				*returnedParentOscAdress = NO_PARENT;
		}
	}
	else
		*returnedParentOscAdress = NO_PARENT;

	// find the last '.' in the TTNode part
	// if exists, split the TTNode part in a name part and an instance part
	len = strlen(to_split);
	last_dot = strrchr(to_split,C_INSTANCE);
	pos = (long)last_dot - (long)to_split;

	if(last_dot > 0){
		instance = (char *)malloc(sizeof(char)*(len - (pos+1)));
		strcpy(instance,to_split + pos+1);
		*returnedTTNodeInstance = TT(instance);

		to_split[pos] = NULL;	// split to keep only the name part
	}
	else
		*returnedTTNodeInstance = NO_INSTANCE;

	// TODO : ??? (detect unusual characters in a TTNode name)
	if(strlen(to_split) > 0)
		*returnedTTNodeName = TT(to_split);
	else
		*returnedTTNodeName = NO_NAME;

	return kTTErrNone;
}

TTErr mergeOSCAddress(TTSymbolPtr *returnedOscAddress, TTSymbolPtr parent, TTSymbolPtr name, TTSymbolPtr instance, TTSymbolPtr propertie)
{
	TTString address;

	if(parent != NO_PARENT)
		address = parent->getCString();

	if(name != NO_NAME){
		address += S_SEPARATOR->getCString();
		address += name->getCString();
	}

	if(instance != NO_INSTANCE){
		address += S_INSTANCE->getCString();
		address += instance->getCString();
	}

	if(propertie != NO_PROPERTIE){
		address += S_PROPERTIE->getCString();
		address += propertie->getCString();
	}

	*returnedOscAddress = TT(address);

	return kTTErrNone;
}