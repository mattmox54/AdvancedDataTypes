#ifndef __AdvancedDataTypes_H__
#define __AdvancedDataTypes_H__

enum ADVDT_DATA_TYPES{
	ADVDT_NONE = 0,
	ADVDT_INT,
	ADVDT_DBL,
	ADVDT_STR,
	ADVDT_PTR
};

//=================================================================================================
//DYNAMIC ARRAY
//=================================================================================================

typedef struct __DynamicArray {
	int dataType;
	int len;
	int (*append)(struct __DynamicArray* self, ...);
	int (*clear)(struct __DynamicArray* self);
	struct __DynamicArray* (*copy)(struct __DynamicArray* self);
	int (*delete)(struct __DynamicArray* self, int index); 
	//int (*extend)(struct __DynamicArray* self, struct __DynamicArray* extendArray);     
	int (*get)(struct __DynamicArray* self, int index, void * value); 
	//int (*index)(struct __DynamicArray* self, ...); //get index from value         
	int (*insert)(struct __DynamicArray* self, int index, ...);
	int (*set)(struct __DynamicArray* self, int index, ...);     
	//struct __DynamicArray* (*slice)(struct __DynamicArray* self, int startIndex, int endIndex);
	//int (*sort)(struct __DynamicArray * self)
	int (*stringify)(struct __DynamicArray* self, char string[]);  
	void * _array;
} *DynamicArray;

DynamicArray NEW_DynamicArray(int DataType);
void FREE_DynamicArray(DynamicArray array);

//=================================================================================================
//Linked List
//=================================================================================================

#define LL_HEAD_ID ((int)0)
#define LL_TAIL_ID ((int)-1)
#define LL_INSERT_AFTER ((int)0)
#define LL_INSERT_BEFORE ((int)1)

typedef struct __LinkedList{
	int (*append)(struct __LinkedList* self, void* data);
	int (*apply)(struct __LinkedList* self, void (*)(void* data)); 
	int (*delete)(struct __LinkedList* self, int id);                
	int (*get)(struct __LinkedList* self, int id, void** data);      
	int (*insert)(struct __LinkedList* self, void* data, int method, int id);
	int (*stringify)(struct __LinkedList* self, char string[]);
	void * _private;
} *LinkedList;

LinkedList NEW_LinkedList(void);
void FREE_LinkedList(LinkedList list);
void GetLinkedListErrorStr(int error, char errorStr[], int errorStrLen);

//=================================================================================================
//STRING
//=================================================================================================

typedef struct __String{
	char * _buffer;
	int len;
	int (*append)(struct __String* self, const char * value); 
	int (*compare)(struct __String* self, struct __String* compareStr); 
	struct __String* (*copy)(struct __String* self);   
	char* (*get)(struct __String* self); 
	int (*removeWhiteSpace)(struct __String* self);
	int (*set)(struct __String* self, const char * value);
	DynamicArray (*split)(struct __String* self, char deliminator);
	int (*toUpper)(struct __String* self);
	int (*toLower)(struct __String* self);
} *String;

String NEW_String(const char * initVal);
void FREE_String(String str);

//=================================================================================================
//DICTIONARY
//=================================================================================================

typedef struct __Dict{
	void * _dict;
	int _len;
	int (*get)(struct __Dict* self, const char * key, void * value);
	int (*delete)(struct __Dict* self, const char * key);  
	//DynamicArray __cdecl (*keys)(struct __Dict* self);  
	int (*set)(struct __Dict* self, const char * key, int dataType, ...); 
	int (*stringify)(struct __Dict* self, char string[]);
} *Dict;

Dict NEW_Dict(int maxEntries);
void FREE_Dict(Dict dict);
void GetDictionaryErrorStr(int error, char errorStr[], int errorStrLen);

#endif /* ndef __AdvancedDataTypes_H__ */  
