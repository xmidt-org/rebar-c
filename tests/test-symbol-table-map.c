/**
 *  Copyright 2010-2016 Comcast Cable Communications Management, LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdbool.h>

#include "../src/symbol-table-map.h"
#include "trie.h"

// assumed in token order (first is token 0, second token 1, etc)
// MUST correspond with trie in trie.h
char* test_strings[] = {
    "TELEPHONE",
    "ZIPCODE",
    "ADDRESSING",
    "NEXT-ADDRESS",
    "NEXT-OF-KIN",
    "ADDRESS-CHANGE",
    "NAME",
    "ADDRESS",
    "TO"
};

// test using trie in trie.h
void test(void)
{
    char returned_string[255];
    // get how many symbols there are
    int symbol_cnt = 0;
    int i;

    printf("entered test");

    while (trie_offsets[symbol_cnt] != 0)
    {
        ++symbol_cnt;
    }
    // search for each symbol in the trie
    for (i = 0; i < symbol_cnt; i++)
    {
        if (i != findToken(trie, test_strings[i]))
        {
            printf("did not find token ");
        }
        findString(trie, trie_offsets, symbol_cnt, i, returned_string);
        if (strcmp(test_strings[i], returned_string) != 0)
        {
            printf("did not find string ");
        }
    }
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    test();  
    return 0;
}
