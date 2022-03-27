#include "AdvancedDataTypes.h"
#include <stddef.h>

//=================================================================================================
//DEFINES & CONSTANTS
//=================================================================================================

typedef struct __Node {
	void * data;
	struct __Node* next;
	struct __Node* prev;
	int id;
} *Node;

typedef struct __PrivateData {
	Node head; 	//first node in list
	Node tail;	//last node in list
	int count; 	//used to generate IDs
} *PrivateData;

enum ERROR_CODES{
	ERR_NONE = 0,
	ERR_INVALID_LIST = -999,
	ERR_UNKNOWN_ID,
};

//=================================================================================================
//FUNCTION PROTOTYPES
//=================================================================================================
static int __cdecl Append(LinkedList self, void* data);	
static int __cdecl Insert(LinkedList self, void* data, int method, int id); 
static int __cdecl Delete(LinkedList self, int id);
static int __cdecl Get(LinkedList self, int id, void ** data);
static int __cdecl Apply(LinkedList self, void (*func)(void* data));
static int __cdecl Stringify(LinkedList self, char string[]);
static Node __cdecl GetNodeFromID(PrivateData pd, int id);

//=================================================================================================
//GLOBAL VARIABLES
//=================================================================================================

//=================================================================================================
//=================================================================================================
//=================================================================================================

LinkedList __cdecl NEW_LinkedList(void){
	LinkedList list = malloc(sizeof(struct __LinkedList));
	PrivateData pd = malloc(sizeof(struct __PrivateData));
	
	if(list==NULL || pd==NULL)
		goto CLEANUP;
	
	list->append = Append;
	list->delete = Delete;
	list->get = Get;
	list->insert = Insert;
	list->apply = Apply;
	list->stringify = Stringify;
	list->_private = (void*)pd;
	
	pd->count = 0;
	pd->head = NULL;
	pd->tail = NULL;
	
	return list;
				   
CLEANUP:
	if(list!=NULL)
		free(list);
	
	if(pd!=NULL)
		free(pd);
				 
	return NULL;
}

void __cdecl FREE_LinkedList(LinkedList list){
	PrivateData pd = NULL;
	Node currentNode = NULL;
	Node nextNode = NULL; 
	
	if(list!=NULL){
	
		pd = (PrivateData)list->_private;
	
		if(pd!=NULL){  
			
			currentNode = pd->head;
	
			while(currentNode!=NULL){
				nextNode = currentNode->next;
				free(currentNode);
				currentNode = nextNode;
			}
			
			free(pd);
			pd = NULL;
		}
		
		free(list);
		list = NULL;
		
	}
	
	return;
}

void GetLinkedListErrorStr(int error, char errorStr[], int errorStrLen){

	switch(error){
		case ERR_INVALID_LIST:
			strncpy(errorStr, "Invalid List", errorStrLen);
			break;
		case ERR_UNKNOWN_ID:
			strncpy(errorStr, "Unknown ID", errorStrLen);
			break;
		default:
			strncpy(errorStr, "Unknown Error", errorStrLen);
	}
	
	errorStr[errorStrLen] = 0; //make sure there is a nul terminator       
	
	return;
}

//=================================================================================================
//=================================================================================================
//=================================================================================================

static int __cdecl Append(LinkedList self, void* data){
	int error = 0;
	
	error = Insert(self, data, LL_INSERT_AFTER, -1);
	
	return error;
}

static int __cdecl Insert(LinkedList self, void* data, int method, int id){
	PrivateData pd = NULL; 
	Node insertNode = NULL; //node to insert in list
	Node idNode = NULL;  	//node that matches id
	Node nextNode = NULL;   //holder for idNode->next while inserting insertNode
	Node prevNode = NULL;   //holder for idNode->prev while inserting insertNode       
	
	if(self==NULL || self->_private==NULL)
		return ERR_INVALID_LIST;
	
	pd = self->_private;
	
	if(pd->head==NULL && pd->tail==NULL){
		//list empty
		insertNode = malloc(sizeof(struct __Node));
	
		insertNode->id = ++pd->count;
		insertNode->data = data;
		insertNode->next = NULL;
		insertNode->prev = NULL;
		
		pd->head = insertNode;
		pd->tail = insertNode;
	}else{
		idNode = GetNodeFromID(pd, id);
		if(idNode==NULL){  
			return ERR_UNKNOWN_ID;
		}
	
		insertNode = malloc(sizeof(struct __Node));
	
		insertNode->id = ++pd->count;
		insertNode->data = data;
	
		if(method==LL_INSERT_BEFORE){
			//BEFORE
			prevNode = idNode->prev;
			idNode->prev = insertNode;
			insertNode->next = idNode;
			insertNode->prev = prevNode;
			if(prevNode!=NULL)
				prevNode->next = insertNode;
			if(idNode==pd->head)
				pd->head = insertNode; //inserting before current head so update head value
		}else{
			//AFTER
			nextNode = idNode->next;
			idNode->next = insertNode;
			insertNode->next = nextNode;
			insertNode->prev = idNode;
			if(nextNode!=NULL)
				nextNode->prev = insertNode;
			if(idNode==pd->tail)
				pd->tail = insertNode; //inserting after current tail so update tail value
		}
	}
	
	return insertNode->id;
}

static int __cdecl Delete(LinkedList self, int id){
	PrivateData pd = NULL; 
	Node delNode = NULL; 	//node to delete from list
	//Node nextNode = NULL;   //holder for idNode->next while deleting delNode
	
	if(self==NULL || self->_private==NULL)
		return ERR_INVALID_LIST;
	
	pd = self->_private;
	
	delNode = GetNodeFromID(pd, id);
	if(delNode==NULL){    
		return ERR_UNKNOWN_ID;
	}
	
	//update previous node's next to delNode->next
	if(delNode->prev!=NULL)
		delNode->prev->next = delNode->next;
	
	//update next node's prev to delNode->prev
	if(delNode->next!=NULL)
		delNode->next->prev = delNode->prev;
	
	if(delNode==pd->head)
		pd->head = delNode->next;
	
	if(delNode==pd->tail)
		pd->tail = delNode->prev;
	
	free(delNode);
	delNode = NULL;
	
	return 0;
}

static int __cdecl Get(LinkedList self, int id, void ** data){
	PrivateData pd = NULL;  
	Node idNode = NULL;
	
	*data = NULL;
	
	if(self==NULL || self->_private==NULL)
		return ERR_INVALID_LIST;
	
	pd = self->_private;
	
	idNode = GetNodeFromID(pd, id);
	if(idNode==NULL){   
		return ERR_UNKNOWN_ID;
	}
	
	*data = idNode->data;
	
	return 0;
}

static int __cdecl Apply(LinkedList self, void (*func)(void* data)){
	PrivateData pd = NULL; 
	Node currentNode = NULL;
	
	if(self==NULL || self->_private==NULL)
		return ERR_INVALID_LIST;
	
	pd = self->_private;
	
	currentNode = pd->head;
	while(currentNode!=NULL){
		func(currentNode->data);
		currentNode = currentNode->next;
	}
	
	return 0;
}

static int __cdecl Stringify(LinkedList self, char string[]){
	PrivateData pd = NULL; 
	Node currentNode = NULL; 
	int len = 0;
	
	if(self==NULL || self->_private==NULL)
		return ERR_INVALID_LIST;
	
	pd = self->_private;
	
	strcpy(string, "");
	
	currentNode = pd->head;
	while(currentNode!=NULL){
		sprintf(string, "%s%X->", string, currentNode);
		currentNode = currentNode->next;
	}
	
	len = strlen(string);
	if(len<2){
		//empty list
		strcpy(string, "NULL");
	}else{
		string[len-2] = 0; //replace last arrow with nul terminator   	
	}
	
	
	return 0;
}

static Node __cdecl GetNodeFromID(PrivateData pd, int id){
	Node currentNode = pd->head;
	
	if(id==LL_TAIL_ID){
		//add to end of list
		currentNode = pd->tail;
	}else if(id==LL_HEAD_ID){
		//add to start of list
		currentNode = pd->head;	
	}else{
		currentNode = pd->head;
		while(currentNode!=NULL && currentNode->id!=id){
			currentNode = currentNode->next;
		}
	}
	
	return currentNode;
}
