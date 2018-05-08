//
// symbol-table-map.h
//
// 
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
// include a symbol table

#ifdef __cplusplus
extern "C" {
#endif

// finds a token given a string
// returns -1 if not found
int findToken(char* trie, char *str);

// finds a string given a token
void findString(char* trie, unsigned short* pTokens, int numTokens, int token, char* retString);

void test();

#ifdef __cplusplus
}
#endif

#endif

