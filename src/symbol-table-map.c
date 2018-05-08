//
// symbol-table-map.c
//
// implements an interpreter
// one side returns a token, given a string
// the other side returns a string, given a pointer to a token in the trie
// (assumes the tokens are within a range)
// returns -1 if no match for string
//

// end of file
#define EOFILE (char)0x80
// end of data (no more follows) -- like final string of 0 length
// token follows
#define EOD (char)0x81
// possible end of string (token follows)
#define EOS (char)0x00
// second char in final string (so have 2 chars) -- dummy char
// Dummy In Final String
#define DIFS (char)0x01

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "symbol-table-map.h"

// finds a token number given a string
int findToken(char* trie, char *str)
{
    int strNdx = 0;
    int strLen = 0;
    bool searching = true; // keep searching until knows not found or found

    if( (NULL == str) || (NULL == trie) ) {
        return -1;
    }

    strLen = strlen(str);
    while (searching)
    {
        char thisChar = *trie++;
        if (thisChar >= 0) // not control
        {
            // must be either branch or final string
            char nextChar = *trie++;
            if (nextChar < 0) // branch
            {
                // see if matches char in string
                if (str[strNdx] == thisChar) // matches, hop over
                {
                    ++strNdx; // used up this char
                    ++trie; // hop 2nd char of branch, now hopped over
                    continue;
                }
                else if (str[strNdx] > thisChar) // keep searching, take branch
                {
                    trie += ((unsigned int)(nextChar & 0x7F) << 8) + ((unsigned char)*trie) - 1;
                }
                else // not found
                {
                    searching = false;
                    continue;
                }
            }
            else // final string
            {
                // first two chars are this char and next char
                if (thisChar == EOS) // string ends here
                {
                    if (strNdx == strLen) // match found
                    {
                        unsigned char hiByte = (unsigned char)nextChar & 0x7F;
                        unsigned char loByte = (unsigned char)*trie;
                        return ((int)hiByte << 8) + loByte;
                    }
                    else
                    {
                        ++trie; // hop token
                        continue;
                    }
                }
                else if (nextChar == DIFS) // only one char in final string
                {
                    if (str[strNdx] == thisChar && (strNdx + 1) == strLen) // match found 
                    {
                        unsigned char hiByte = (unsigned char)*trie++ & 0x7F;
                        unsigned char loByte = (unsigned char)*trie;
                        return ((int)hiByte << 8) + loByte;
                    }
                    else
                    {
                        searching = false;
                        continue;
                    }
                }
                else // final string with two or more letters
                {
                    char curChar;
                    trie -= 2; // now pointing to start of final string
                    curChar = *trie++;
                    while (strNdx < strLen && (str[strNdx] == curChar || curChar == EOS))
                    {
                        if (curChar == EOS) // found token, but no match yet
                        {
                            trie += 2; // skip token
                            curChar = *trie++; // get next char in final string
                        }
                        else
                        {
                            ++strNdx;
                            curChar = *trie++;
                        }
                    }
                    // for match, strNdx must equal strLen
                    // and curChar must be control char (signalling token)
                    if (strNdx == strLen && ((curChar & 0x80) == 0x80 || curChar == EOS)) // found it
                    {
                        unsigned char hiByte;
                        unsigned char loByte;
                        if (curChar == EOS)
                        {
                            curChar = *trie++; // match, so skip EOS to find token
                        }
                        hiByte = (unsigned char)curChar & 0x7F;
                        loByte = (unsigned char)*trie;
                        return ((int)hiByte << 8) + loByte;
                    }
                    else
                    {
                        searching = false;
                        continue;
                    }
                }
            }
        }
        else // is control char (must be slide count or end of data (0x81) or EOFILE (0x80))
        {
            // only have thisChar, and now pointing after it
            if (thisChar == EOFILE) // end of file
            {
                searching = false;
                continue; // got to EOFILE without finding match
            }
            else if (thisChar == EOD) // end of data (like final string of 0 length)
            {
                if (strNdx == strLen) // match found
                {
                    unsigned char hiByte = (unsigned char)*trie++ & 0x7F;
                    unsigned char loByte = (unsigned char)*trie;
                    return ((int)hiByte << 8) + loByte;
                }
                else
                {
                    searching = false;
                    continue;
                }
            }
            else // slide count
            {
                int slideCount = (int)(thisChar & 0x7F);
                char curChar = *trie++;
                while (strNdx < strLen 
                        && slideCount > 1
                        && (str[strNdx] == curChar || curChar == EOS))
                {
                    if (curChar == EOS) // found token, but no match yet
                    {
                        trie += 2; // skip token
                        curChar = *trie++; // get next char in final string
                    }
                    else
                    {
                        ++strNdx;
                        curChar = *trie++;
                        --slideCount;
                    }
                }
                if (strNdx == strLen) // end of string in middle of slide
                {
                    if (curChar == EOS) // ok, since token here
                    {
                        unsigned char hiByte = (unsigned char)*trie++ & 0x7F;
                        unsigned char loByte = (unsigned char)*trie;
                        return ((int)hiByte << 8) + loByte;
                    }
                    else
                    {
                        searching = false;
                        continue;
                    }
                }
                else if (slideCount == 1) // on last char of slide, treat like branch
                {
                    if (curChar == EOS) // found token, but no match yet
                    {
                        trie += 2; // skip token (from first byte of token)
                    }
                    else
                    {
                        --trie; // move ptr back to final character
                    }
                    continue;
                }
                else  // mismatch when shouldn't be
                {
                    searching = false;
                    continue;
                }
            }
        }
    }
    return -1; // error
}

static void tokenFound(char* trie, int token, char* retString, char *pStr)
{
    unsigned char hiByte = (unsigned char)*trie++ & 0x7F;
    unsigned char loByte = (unsigned char)*trie;
    unsigned int tokenValue = ((int)hiByte << 8) + loByte;
    if (tokenValue == (unsigned) token)
    {
        *pStr = 0; // terminate string (FOUND)
        return;
    }
    else // internal err
    {
        *retString = 0; // null string
        return;
    }
}

// finds a string given a token
// pTokens are offsets to the end tokens, the 10th offset points to the 10th token in the trie
// numTokens is the number of pTokens
// token is the token we are looking for (must be <= numTokens)
// retString should have at least space for 256 chars (or longest string, if greater than that)
void findString(char* trie, unsigned short* pTokens, int numTokens, int token, char* retString)
{
    char *pStr = retString;
    char *pTrie = trie;
    char *pTrieFound;  // the location where the token is to be found
    if (token < 0 || token > numTokens)
    {
        *retString = 0;
        return;
    }
    pTrieFound = trie + pTokens[token];
    while (pTrie < pTrieFound) // loops until hit token, then returns with string in retString
    {
        char thisChar = *pTrie++;
        
        if (thisChar < 0) // control char (must be EOFILE, EOD, or slide count)
        {
            if (thisChar == EOFILE) // end of file hit without finding token (internal err)
            {
                *retString = 0;
                break;
            }
            else if (thisChar == EOD) // end of data (like final string of length 0)
            {
                tokenFound(pTrie, token, retString, pStr);
                return;
            }
            else // slide count
            {
                int slideCount = (int)(thisChar & 0x7F);
                char curChar = *pTrie++;
                while (pTrie < pTrieFound && --slideCount > 0)
                {
                    if (curChar == EOS)
                    {
                        pTrie += 2; // skip token (not it since pTrie < pTrieFound)
                        curChar = *pTrie++;
                        ++slideCount; // restore count
                        continue;
                    }
                    *pStr++ = curChar; // add to string
                    curChar = *pTrie++;
                }
                if (pTrie == pTrieFound) // found token in EOS
                {
                    tokenFound(pTrie, token, retString, pStr);
                    return;
                }
                else if  (curChar == EOS) // last char on EOS
                {
                    pTrie += 2; // skip token
                }
                else // normal last char
                {
                    // on last char of slide, still not found, treat like branch
                    --pTrie; // move back to last char of slide to make like a branch
                }
                continue;
            }
        }
        else // ASCII char (or EOS) -- must be branch or final string
        {
            char curChar = *pTrie++;
            if (curChar > 0) // final string
            {
                *pStr++ = thisChar;
                while (pTrie < pTrieFound && curChar >= 0)
                {
                    if (curChar == EOS)
                    {
                        pTrie += 2; // skip token (not it since pTrie < pTrieFound)
                        curChar = *pTrie++;
                        continue;
                    }
                    *pStr++ = curChar; // add to string
                    curChar = *pTrie++;
                }
                if (pTrie == pTrieFound) // at token after EOS or end of final string
                {
                    if (curChar != DIFS)
                    {
                        *pStr++ = curChar;
                    }
                    tokenFound(pTrie, token, retString, pStr);
                    return;
                }
                else // not found in final string
                {
                    *retString = 0; // null string
                    return;
                }
            }
            else // branch
            {
                unsigned char loByteBranch = *pTrie++; // now positioned after branch
                int branchLoc = ((int)(curChar & 0x7F) << 8) + loByteBranch;
                if ((pTrie + branchLoc) <= pTrieFound) // not down this branch, so take it
                {
                    pTrie += branchLoc - 2; // back up to start of branch, then add
                }
                else {
                    // down this branch, add char to string
                    *pStr++ = thisChar;
                }
                continue;
            }
        }
    }
    *retString = 0; // failure
}


