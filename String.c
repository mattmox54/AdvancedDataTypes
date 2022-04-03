#include "AdvancedDataTypes.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//=================================================================================================
//DEFINES & CONSTANTS
//=================================================================================================

//=================================================================================================
//FUNCTION PROTOTYPES
//=================================================================================================
int __cdecl String_Set(String self, const char * value);
char * __cdecl String_Get(String self);
int __cdecl String_Append(String self, const char * value);
DynamicArray __cdecl String_Split(String self, char deliminator);
int __cdecl String_ToUpper(String self);
int __cdecl String_ToLower(String self);
int __cdecl String_Compare(String self, String compareStr);
String __cdecl String_Copy(String self);
int __cdecl String_RemoveWhiteSpace(String self);

//=================================================================================================
//GLOBAL VARIABLES
//=================================================================================================

//=================================================================================================
//=================================================================================================
//=================================================================================================

String __cdecl NEW_String(const char * initVal){
	String str = NULL;
	
	str = (String)malloc(sizeof(struct __String));
	if(initVal!=NULL){
		str->len = strlen(initVal);
		str->_buffer = (char*)malloc(sizeof(char)*(str->len+1));
		strcpy(str->_buffer, initVal); 
	}else{
		str->_buffer = NULL;
		str->len = 0;
	}
	
	str->append = String_Append;
	str->compare = String_Compare;
	str->copy = String_Copy;
	str->get = String_Get;
	str->set = String_Set;
	str->split = String_Split;
	str->toLower = String_ToLower;
	str->toUpper = String_ToUpper;
	str->removeWhiteSpace = String_RemoveWhiteSpace;

	return str;
}

void __cdecl FREE_String(String str){
	
	if(str!=NULL){
		if(str->_buffer!=NULL){
			free(str->_buffer);	
		}
		
		free(str);
	}
	
	return;
}

//=================================================================================================
//=================================================================================================
//=================================================================================================

int __cdecl String_Set(String self, const char * value){
	char* tmpBuffer = NULL;

	if(self==NULL)
		return -1;
	
	self->len = strlen(value);
	tmpBuffer = (char*)realloc(self->_buffer, sizeof(char)*(self->len+1));
	if (tmpBuffer == NULL) {
		return -1;
	}
	self->_buffer = tmpBuffer;
	strcpy(self->_buffer, value);
	
	return 0;
}

char * __cdecl String_Get(String self){
	
	if(self==NULL)
		return NULL;
	
	return self->_buffer;
}

int __cdecl String_Append(String self, const char * value){
	char* tmpBuffer = NULL;

	if(self==NULL)
		return -1;
	
	self->len += strlen(value);
	tmpBuffer = (char*)realloc(self->_buffer, sizeof(char)*(self->len+1));
	if (tmpBuffer == NULL) {
		return -2;
	}
	self->_buffer = tmpBuffer;
	strcat(self->_buffer, value);
	
	return 0;
}

DynamicArray __cdecl String_Split(String self, char deliminator){
	char * start = self->_buffer;
	char * end = NULL;
	char * splitVal = NULL;
	DynamicArray array = NULL;
	
	if(self==NULL)
		return NULL;
	
	array = NEW_DynamicArray(ADVDT_STR);            
	
	while(start!=NULL){
		end = strchr(start, deliminator);
		if(end==NULL){
			//no more deliminators, just append remainder
			array->append(array, start);
			start=NULL;
		}else{
			splitVal = (char*)malloc(sizeof(char)*(end-start+1));
			memcpy(splitVal, start, end-start);
			splitVal[end-start] = 0; //null terminator
			array->append(array, splitVal);
			start = end+1;
			free(splitVal);
			splitVal = NULL;
		}	
	}
	
	return array;
}

int __cdecl String_ToUpper(String self){
	
	if(self==NULL)
		return -1;
	
	for(int i=0; i<self->len; i++){
		self->_buffer[i] = (char)toupper(self->_buffer[i]);	
	}
	
	return 0;
}

int __cdecl String_ToLower(String self){
	if(self==NULL)
		return -1;
	
	for(int i=0; i<self->len; i++){
		self->_buffer[i] = (char)tolower(self->_buffer[i]);	
	}
	
	return 0;	
}

int __cdecl String_Compare(String self, String compareStr){
	
	if(self==NULL || compareStr==NULL)
		return -1;
	
	if(self->_buffer==NULL){
		if(compareStr->_buffer==NULL){
			return 0; //both NULL so return 0 to be consistent with strcmp	
		}else{
			return -1; //self==NULL is less than non NULL value of compare string so return negative number	
		}
	}else if(compareStr->_buffer==NULL){
		return 1; //self== not NULL is greater than NULL value of compare string so return postive number	
	}

	return strcmp(self->_buffer, compareStr->_buffer);
}

String __cdecl String_Copy(String self){
	String newStr = NEW_String(self->_buffer);
	
	return newStr;
}

int __cdecl String_RemoveWhiteSpace(String self){
	char * bufferCopy = NULL;
	int copyIndex = 0;
	
	if(self==NULL)
		return -1;
	
	bufferCopy = (char*)malloc(sizeof(char)*(self->len+1));
	
	for(int i=0; i<self->len; i++){
		if(self->_buffer[i]>=33){
			//ascii chars less than 33 can be considered white space
			bufferCopy[copyIndex] = self->_buffer[i];
			copyIndex++;
		}
	}
	
	free(self->_buffer);
	self->len = copyIndex;
	bufferCopy[copyIndex] = 0;
	self->_buffer = bufferCopy;
	
	return 0;
}
