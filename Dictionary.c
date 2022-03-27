#include "AdvancedDataTypes.h"
#include <stddef.h>
#include <stdarg.h>

//=================================================================================================
//DEFINES & CONSTANTS
//=================================================================================================

#define MAX_KEY_LEN 128

typedef struct __DictEntry{
	char key[MAX_KEY_LEN];
	int dType;
	void * value;
} DictEntry, *DictBuffer;

enum DICT_ERROR_CODES{
	ERR_NONE = 0,
	ERR_INVALID_DICT = -999,
	ERR_INVALID_INTERNAL_DICT,
	ERR_DICT_FULL,
	ERR_UNKNOWN_DTYPE,
	ERR_UNKNOWN_KEY
};

//=================================================================================================
//FUNCTION PROTOTYPES
//=================================================================================================
int Dict_Set(Dict self, const char * key, int dataType, ...);
int Dict_Get(Dict self, const char * key, void * value);
int Dict_Delete(Dict self, const char * key);
int Dict_Stringify(Dict self, char string[]);
static unsigned long Hash(unsigned char *str);

//=================================================================================================
//GLOBAL VARIABLES
//=================================================================================================

//=================================================================================================
//=================================================================================================
//=================================================================================================

Dict NEW_Dict(int maxEntries){
	Dict dict = malloc(sizeof(struct __Dict));
	DictEntry DictInitialVal = {
		.key = "",
		.dType = ADVDT_NONE,
		.value = NULL
	};
	
	dict->_dict = malloc(sizeof(DictEntry)*maxEntries);
	dict->_len = maxEntries;
	for(int i=0; i<maxEntries; i++){
		((DictBuffer)dict->_dict)[i] = DictInitialVal;	
	}
	
	dict->set = Dict_Set;
	dict->get = Dict_Get;
	dict->delete = Dict_Delete;
	dict->stringify = Dict_Stringify;
	
	return dict;
}

void FREE_Dict(Dict dict){
	DictBuffer __dict = NULL;  
	
	if(dict!=NULL){
		if(dict->_dict!=NULL){
			__dict = dict->_dict;
			for(int i=0; i<dict->_len; i++){
				if(__dict[i].value!=NULL && (__dict[i].dType==ADVDT_INT || __dict[i].dType==ADVDT_DBL || __dict[i].dType==ADVDT_STR))
					free(__dict[i].value);
			}
			free(dict->_dict);	
		}
		
		free(dict);
	}
	
	return;
}

void GetDictionaryErrorStr(int error, char errorStr[], int errorStrLen){
	
	switch(error){
		case ERR_NONE:
			strncpy(errorStr, "No Error", errorStrLen);
			break;
		case ERR_INVALID_DICT:
			strncpy(errorStr, "Uninitialized Dict Error", errorStrLen);
			break;
		case ERR_INVALID_INTERNAL_DICT:
			strncpy(errorStr, "Uninitialized Internal Dict Error", errorStrLen);
			break;
		case ERR_DICT_FULL:
			strncpy(errorStr, "Dict Full Error", errorStrLen);
			break;
		case ERR_UNKNOWN_DTYPE:
			strncpy(errorStr, "Unknown Data Type Error", errorStrLen);
			break;
		case ERR_UNKNOWN_KEY:
			strncpy(errorStr, "Unknown Key Error", errorStrLen);
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

int Dict_Set(Dict self, const char * key, int dataType, ...){
	unsigned long index = 0;
	char __key[MAX_KEY_LEN] = "";
	DictBuffer dict = NULL;
	int start_index = 0;
	va_list valist;
	char * tempStr = NULL;
	
	if(self==NULL)
		return ERR_INVALID_DICT;
	
	if(self->_dict==NULL)
		return ERR_INVALID_INTERNAL_DICT;
	
	dict = (DictBuffer)self->_dict;
	
	strncpy(__key, key, MAX_KEY_LEN);
	__key[MAX_KEY_LEN-1] = 0; //make sure NUL terminator is added
	
	index = Hash((unsigned char*)__key);
	index %= self->_len;//convert hash value to something within the limits of dictionary length
	start_index = index; //keep track of start index to detect if dict is full
	
	while(dict[index].key[0] != 0/*empty key*/ && strcmp(dict[index].key, __key)!=0/*keys don't match*/){  //find next empty key or index that __key==key
		index++;
		if(index>=self->_len){
			index = 0;
		}else if(index==start_index){
			return ERR_DICT_FULL; //dict full	
		}
	}
	
	strcpy(dict[index].key, __key);
	dict[index].dType = dataType;
	
	va_start(valist, 1);   
	
	switch(dict[index].dType){
		case ADVDT_INT:
			dict[index].value = malloc(sizeof(int));
			*(int*)dict[index].value = va_arg(valist, int); 
			//printf("DICT ADD: %s=%d\n", __key, *(int*)dict[index].value);
			break;
		case ADVDT_DBL:
			dict[index].value = malloc(sizeof(double));
			*(double*)dict[index].value = va_arg(valist, double);  
			//printf("DICT ADD: %s=%f\n", __key, *(double*)dict[index].value);    
			break;
		case ADVDT_STR:
			tempStr = va_arg(valist, char*);
			dict[index].value = malloc(sizeof(char)*(strlen(tempStr)+1));
			strcpy((char*)dict[index].value, tempStr);
			//printf("DICT ADD: %s=%s\n", __key, (char*)dict[index].value);     
			break;
		case ADVDT_PTR:
			dict[index].value = va_arg(valist, void*); 
			break;
		default:
			va_end(valist);  
			return ERR_UNKNOWN_DTYPE;		
	}
	
	va_end(valist);
	
	
	
	return 0;	
}

int Dict_Get(Dict self, const char * key, void * value){
	unsigned long index = 0;
	char __key[MAX_KEY_LEN] = "";
	DictBuffer dict = NULL;
	int start_index = 0;
	
	if(self==NULL)
		return ERR_INVALID_DICT;
	
	if(self->_dict==NULL)
		return ERR_INVALID_INTERNAL_DICT;
	
	dict = (DictBuffer)self->_dict;     
	
	strncpy(__key, key, MAX_KEY_LEN);
	__key[MAX_KEY_LEN-1] = 0; //make sure NUL terminator is added
	
	index = Hash((unsigned char*)__key);
	index %= self->_len;//convert hash value to something within the limits of dictionary length
	start_index = index; //keep track of start index to detect if dict is full

	while(strcmp(dict[index].key, __key)!=0/*keys don't match*/){
		index++;
		if(index>=self->_len){
			index = 0; //wrap to start of array
		}
		if(index==start_index){
			return ERR_DICT_FULL; //dict full	
		}else if(dict[index].key[0] == 0){
			//found empty entry
			return ERR_UNKNOWN_KEY; //key not found	
		}
	}
	
	switch(dict[index].dType){
		case ADVDT_INT:
			*(int*)value = *(int*)dict[index].value;
			break;
		case ADVDT_DBL:
			*(double*)value = *(double*)dict[index].value;  
			break;
		case ADVDT_STR:
			strcpy((char*)value, (char*)dict[index].value);
			break;
		case ADVDT_PTR:
			*(void**)value = dict[index].value;  
			break;
		default:
			return ERR_UNKNOWN_DTYPE;		
	}
	
	return 0;
}

int Dict_Delete(Dict self, const char * key){
	unsigned long index = 0;
	char __key[MAX_KEY_LEN] = "";
	DictBuffer dict = NULL;
	int start_index = 0;
	
	if(self==NULL)
		return ERR_INVALID_DICT;
	
	if(self->_dict==NULL)
		return ERR_INVALID_INTERNAL_DICT;
	
	dict = (DictBuffer)self->_dict;     
	
	strncpy(__key, key, MAX_KEY_LEN);
	__key[MAX_KEY_LEN-1] = 0; //make sure nul terminator is added
	
	index = Hash((unsigned char*)__key);
	index %= self->_len;//convert hash value to something within the limits of dictionary length
	start_index = index; //keep track of start index to allow while to exit if dict and full and key not in dict

	while(strcmp(dict[index].key, __key)!=0/*keys don't match*/){
		index++;
		if(index>=self->_len){
			index = 0; //wrap to start of array
		}
		if(index==start_index || dict[index].key[0] == 0){
			return 0; //key not in dict already
		}
	}	
	
	if(dict[index].value != NULL)
		free(dict[index].value);
	dict[index].value = NULL;
	dict[index].dType = ADVDT_NONE;
	strcpy(dict[index].key, "");
	
	return 1;
}

int Dict_Stringify(Dict self, char string[]){
	DictBuffer dict = NULL; 
	int len = 0;
	
	if(self==NULL)
		return ERR_INVALID_DICT;
	
	if(self->_dict==NULL)
		return ERR_INVALID_INTERNAL_DICT;
	
	dict = (DictBuffer)self->_dict;      
	
	strcpy(string, "{\n");
	
	for(int i=0; i<self->_len; i++){
		
		switch(dict[i].dType){
			case ADVDT_INT:
				sprintf(string, "%s\t%s:%d,\n", string, dict[i].key, *(int*)dict[i].value);
				break;
			case ADVDT_DBL:
				sprintf(string, "%s\t%s:%f,\n", string, dict[i].key, *(double*)dict[i].value);      
				break;
			case ADVDT_STR:
				sprintf(string, "%s\t%s:\"%s\",\n", string, dict[i].key, (char*)dict[i].value);     
				break;
			case ADVDT_PTR:
				sprintf(string, "%s\t%s:0x%X,\n", string, dict[i].key, dict[i].value);       
				break;
		}
	}
	
	len = strlen(string);
	if(len<3){
		//empty dict
		strcpy(string, "{}");
	}else{
		string[len-2] = '\n'; //replace last comma with new line
		string[len-1] = '}'; //replace last new line with close bracket
		string[len] = 0; //make sure there is a nul terminator
	}
	
	return 0;
}

static unsigned long Hash(unsigned char *str){
	//taken from https://stackoverflow.com/questions/7666509/hash-function-for-string
	//hash functions convert a string to an unsigned long in such a way as to minimize the chance of two different string producting the same number
    unsigned long hash = 5381;
    int c;

    while (0 != (c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
