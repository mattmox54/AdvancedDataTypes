#include "AdvancedDataTypes.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//=================================================================================================
//DEFINES & CONSTANTS
//=================================================================================================
#define APPEND_VA_ARG(s, dtype) 									\
	do{																\
		void* _s = NULL;											\
		_s = (dtype*)realloc(s->_array, sizeof(dtype)*(++s->len));	\
		if(_s==NULL)												\
			return -2;												\
		s->_array = _s;												\
		((dtype*)s->_array)[s->len-1] = va_arg(valist, dtype);		\
	}while(0)


//=================================================================================================
//FUNCTION PROTOTYPES
//=================================================================================================
int DA_Append(DynamicArray self, ...);
DynamicArray DA_Copy(DynamicArray self);
int DA_GetIndex(DynamicArray self, int Index, void * value);
int DA_SetIndex(DynamicArray self, int Index, ...);
int DA_Insert(DynamicArray self, int Index, ...);
int DA_Delete(DynamicArray self, int Index);
int DA_Clear(DynamicArray self);
int DA_Stringify(DynamicArray self, char string[]);

//=================================================================================================
//GLOBAL VARIABLES
//=================================================================================================

//=================================================================================================
//=================================================================================================
//=================================================================================================

DynamicArray NEW_DynamicArray(int DataType){
	DynamicArray self = (DynamicArray)malloc(sizeof(struct __DynamicArray));
	if(self==NULL)
		return NULL;
	
	self->dataType = DataType;
	self->len = 0;
	self->_array = NULL;
	
	self->append = DA_Append;
	self->get = DA_GetIndex; 
	self->set = DA_SetIndex;
	self->copy = DA_Copy;
	self->insert = DA_Insert;
	self->delete = DA_Delete;
	self->clear = DA_Clear;
	self->stringify = DA_Stringify;
	
	return self;
}

void FREE_DynamicArray(DynamicArray array){

	if(array==NULL)
		return;
	
	if(array->_array!=NULL){
		if(array->dataType==ADVDT_STR){
			for(int i=0; i<array->len; i++){
				free(((char**)array->_array)[i]);	
			}
		}
		
		free(array->_array);
	}
	
	free(array);
	
	return;
}

//=================================================================================================
//=================================================================================================
//=================================================================================================

int DA_Append(DynamicArray self, ...){
	va_list valist;
	char * strTemp = NULL;
	void* tmpArray = NULL;
	
	if(self==NULL)
		return -99;
	
	va_start(valist, 1);
	
	switch(self->dataType){
		case ADVDT_INT:
			APPEND_VA_ARG(self, int);
			break;
		case ADVDT_DBL:
			APPEND_VA_ARG(self, double);
			break;
		case ADVDT_STR:
			strTemp = va_arg(valist, char*); //get value to add
			tmpArray = (char*)realloc(self->_array, sizeof(char*)*(++self->len)); //resize array
			if (tmpArray == NULL) {
				return -2;
			}
			self->_array = tmpArray;
			((char**)self->_array)[self->len-1] = malloc(sizeof(char)*(strlen(strTemp)+1)); //create memory for new value
			strcpy(((char**)self->_array)[self->len-1], strTemp); //set value
			break;
		case ADVDT_PTR:
			APPEND_VA_ARG(self, void*);
			break;
		default:
			va_end(valist);
			return -1;
	}
	
	va_end(valist);    
	
	return 0;
}

DynamicArray DA_Copy(DynamicArray self){
	DynamicArray newArray = NULL;
	
	if(self==NULL)
		return NULL;
	
	newArray = (DynamicArray)malloc(sizeof(struct __DynamicArray));
	if(self==NULL)
		return NULL;
	
	newArray = self; //intialize from self, still need to copy _array
	
	switch(newArray->dataType){
		case ADVDT_INT:
			newArray->_array = (int*)malloc(sizeof(int)*newArray->len);
			memcpy(newArray->_array, self->_array, sizeof(int)*newArray->len);
			break;
		case ADVDT_DBL:
			newArray->_array = (double*)malloc(sizeof(double)*newArray->len);
			memcpy(newArray->_array, self->_array, sizeof(double)*newArray->len);
			break;
		case ADVDT_STR:
			newArray->_array = (char**)malloc(sizeof(char*)*newArray->len);
			for(int i=0; i<newArray->len; i++){
				((char**)newArray->_array)[i] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[i])+1));
				strcpy(((char**)newArray->_array)[i], ((char**)self->_array)[i]);
			}
			break;
		case ADVDT_PTR:
			newArray->_array = malloc(sizeof(void*)*newArray->len);
			memcpy(newArray->_array, self->_array, sizeof(void*)*newArray->len);
			break;
	}
	
	return newArray;
}

int DA_GetIndex(DynamicArray self, int Index, void * value){

	if(self==NULL)
		return -99;
	
	if(Index>=self->len)
		return -98;
	
	if(Index<0){
		Index = self->len+Index; //if Index is negative, it is the index starting from the end
		if(Index<0)
			return -97;
	}
	
	switch(self->dataType){
		case ADVDT_INT:
			*(int*)value = ((int*)self->_array)[Index];
			break;
		case ADVDT_DBL:
			*(double*)value = ((double*)self->_array)[Index];       
			break;
		case ADVDT_STR:
			strcpy((char*)value, ((char**)self->_array)[Index]);
			break;
		case ADVDT_PTR:
			value = ((void**)self->_array)[Index];
			break;
		default:
			return -1;		
	}
	
	return 0;
}

int DA_SetIndex(DynamicArray self, int Index, ...){
	va_list valist;
	
	if(self==NULL)
		return -99;
	
	if(Index>=self->len)
		return -98;
	
	if(Index<0){
		Index = self->len+Index; //if Index is negative, it is the index starting from the end
		if(Index<0)
			return -97;
	}
	
	va_start(valist, 1);
	
	switch(self->dataType){
		case ADVDT_INT:
			((int*)self->_array)[Index] = va_arg(valist, int);
			break;
		case ADVDT_DBL:
			((double*)self->_array)[Index] = va_arg(valist, double);      
			break;
		case ADVDT_STR:
			strcpy(((char**)self->_array)[Index], va_arg(valist, char*));
			break;
		case ADVDT_PTR:
			((void**)self->_array)[Index] = va_arg(valist, void*);    
			break;
		default:
			va_end(valist);
			return -1;
	}
	
	va_end(valist);    
	
	return 0;
}

int DA_Insert(DynamicArray self, int Index, ...){
	va_list valist;
	void * _array = NULL;
	
	if(self==NULL)
		return -99;
	
	if(Index>=self->len)
		return -98;
	
	if(Index<0){
		Index = self->len+Index; //if Index is negative, it is the index starting from the end
		if(Index<0)
			return -97;
	}
	
	va_start(valist, 1);
	
	switch(self->dataType){
		case ADVDT_INT:
			_array = (int*)malloc(sizeof(int)*(self->len+1));
			if(Index!=0) //nothing to copy before Index if insert at beginning
				memcpy(_array, self->_array, sizeof(int)*Index);
			((int*)_array)[Index] = va_arg(valist, int);
			memcpy(&((int*)_array)[Index+1], &((int*)self->_array)[Index], sizeof(int)*(self->len-Index));
			self->len++;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_DBL:
			_array = (double*)malloc(sizeof(double)*(self->len+1));
			if(Index!=0) //nothing to copy before Index if insert at beginning             
				memcpy(_array, self->_array, sizeof(double)*Index);
			((double*)_array)[Index] = va_arg(valist, double); 
			memcpy(&((double*)_array)[Index+1], &((double*)self->_array)[Index], sizeof(double)*(self->len-Index));
			self->len++;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_STR:
			_array = (char**)malloc(sizeof(char*)*(self->len+1));  
			for(int i=0; i<Index; i++){
				((char**)_array)[i] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[i])+1));
				strcpy(((char**)_array)[i], ((char**)self->_array)[i]);
				free(((char**)self->_array)[i]);    
			}
			((char**)_array)[Index] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[Index])+1));
			strcpy(((char**)_array)[Index], ((char**)self->_array)[Index]);
			for(int i=Index; i<self->len; i++){
				((char**)_array)[i+1] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[i])+1));
				strcpy(((char**)_array)[i+1], ((char**)self->_array)[i]);
				free(((char**)self->_array)[i]);    
			}
			self->len++;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_PTR:
			_array = malloc(sizeof(void*)*(self->len+1));
			if(Index!=0) //nothing to copy before Index if insert at beginning             
				memcpy(_array, self->_array, sizeof(void*)*Index);
			((void**)_array)[Index] = va_arg(valist, void*); 
			memcpy(&((void**)_array)[Index+1], &((void**)self->_array)[Index], sizeof(void*)*(self->len-Index));
			self->len++;
			free(self->_array);
			self->_array = _array;
			break;
		default:
			va_end(valist);
			return -1;
	}
	
	va_end(valist);    
	
	return 0;	
}

int DA_Delete(DynamicArray self, int Index){
	void * _array = NULL;
	
	if(self==NULL)
		return -99;
	
	if(Index>=self->len)
		return -98;
	
	if(Index<0){
		Index = self->len+Index; //if Index is negative, it is the index starting from the end
		if(Index<0)
			return -97;
	}
	
	switch(self->dataType){
		case ADVDT_INT:
			_array = (int*)malloc(sizeof(int)*(self->len-1));
			if(Index!=0) //nothing to copy before Index if insert at beginning
				memcpy(_array, self->_array, sizeof(int)*Index);
			memcpy(&((int*)_array)[Index], &((int*)self->_array)[Index+1], sizeof(int)*(self->len-Index-1));
			self->len--;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_DBL:
			_array = (double*)malloc(sizeof(double)*(self->len-1));
			if(Index!=0) //nothing to copy before Index if insert at beginning             
				memcpy(_array, self->_array, sizeof(double)*Index);
			memcpy(&((double*)_array)[Index], &((double*)self->_array)[Index+1], sizeof(double)*(self->len-Index-1));
			self->len--;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_STR:
			_array = (char**)malloc(sizeof(char*)*(self->len-1));  
			for(int i=0; i<Index; i++){
				((char**)_array)[i] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[i])+1));
				strcpy(((char**)_array)[i], ((char**)self->_array)[i]);
				free(((char**)self->_array)[i]);
			}
			for(int i=Index; i<self->len-1; i++){
				((char**)_array)[i] = (char*)malloc(sizeof(char)*(strlen(((char**)self->_array)[i+1])+1));
				strcpy(((char**)_array)[i], ((char**)self->_array)[i+1]);
				free(((char**)self->_array)[i+1]);    
			}
			self->len--;
			free(self->_array);
			self->_array = _array;
			break;
		case ADVDT_PTR:
			_array = malloc(sizeof(void*)*(self->len-1));
			if(Index!=0) //nothing to copy before Index if insert at beginning             
				memcpy(_array, self->_array, sizeof(void*)*Index);
			memcpy(&((void**)_array)[Index], &((void**)self->_array)[Index+1], sizeof(void*)*(self->len-Index-1));
			self->len--;
			free(self->_array);
			self->_array = _array;
			break;
		default:
			return -1;
	}
	
	return 0;
}

int DA_Clear(DynamicArray self){
	
	if(self==NULL)
		return -99;
	
	switch(self->dataType){
		case ADVDT_INT:
		case ADVDT_DBL:
		case ADVDT_PTR:
			if(self->_array!=NULL){
				free(self->_array);
				self->_array = NULL;
				self->len = 0;
			}
			break;
		case ADVDT_STR:
			if(self->_array!=NULL){
				for(int i=0; i<self->len; i++){
					if(((char**)self->_array)[i]!=NULL)
						free(((char**)self->_array)[i]);
				}
				free(self->_array);
				self->_array = NULL;
				self->len = 0;
			}
			break;
		default:
			return -1;
	}
	
	return 0;
}

int DA_Stringify(DynamicArray self, char string[]){
	int len = 0;
	
	if(self==NULL)
		return -99;
	
	strcpy(string, "[");
	
	for(int i=0; i<self->len; i++){
	
		switch(self->dataType){
			case ADVDT_INT:
				sprintf(string, "%s%d, ", string, ((int*)self->_array)[i]);
				break;
			case ADVDT_DBL:
				sprintf(string, "%s%f, ", string, ((double*)self->_array)[i]);
				break;
			case ADVDT_PTR:
				sprintf(string, "%s0x%X, ", string, ((void**)self->_array)[i]);
				break;
			case ADVDT_STR:
				sprintf(string, "%s\"%s\", ", string, ((char**)self->_array)[i]);
				break;
			default:
				return -1;
		}
		
	}
	
	len = strlen(string);
	if(len<2){
		//empty list
		strcpy(string, "[]");
	}else{
		string[len-2] = ']'; //overwrite last comma with close bracket
		string[len-1] = 0;  //overwrite last space with nul terminator
	}
	
	return 0;
	
}
