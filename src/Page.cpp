//
//  Page.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-5-8.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include "Page.h"
#include "Constant.h"
#include <cstring>
#include <cstdio>

Page::Page() {
    offSet = 0;
}

Page::~Page() {
}

bool Page::insertDataToPage(char *data_, size_t size) {
    if ( size + offSet <= PAGE_SIZE ) {     //if the page is not full
        memcpy(data+offSet, data_, size);   //copy data to page
        offSet += size;
        return true;
    } else {                                //else, insert fail
        return false;
    }
}

void Page::clearPage() {
    memset( data, 0, PAGE_SIZE );   //clear page content
    offSet = 0;                     //clear pointer
}

void Page::writePageToFile( FILE *fptr ) {
    offSet = fwrite( data, sizeof(char), offSet, fptr );
}

void Page::readFileToPage( FILE *fptr ) {
    offSet = fread( data, sizeof(char), PAGE_SIZE, fptr );
}

bool Page::searchInPage( const char *key, size_t keySize, char* & outputEntry, size_t entrySize ) {
    //helper variables
    char *currentKey = new char[keySize];
    bool found = false;
    int currentOffset = 0;
    
    while( currentOffset < offSet )  {  //while not end of page
        memcpy( currentKey, data + currentOffset, keySize );    //read a key
        if ( memcmp( key, currentKey, keySize ) != 0 ) {    //if not same
            currentOffset += entrySize;                     //point to next
        } else {    //else, found
            found = true;   //return value
            
            //copy to output
            memcpy( outputEntry, data + currentOffset, entrySize );            break;
        }
    }
    
    delete []currentKey;
    return found;
}

void Page::compressToPage( Page *outputPage, int keySize, int entrySize, FILE *fptr ) {
    //compress current page to output
    char *currentEntry = new char[keySize]; //read an entry form current page
    memcpy( currentEntry, data + keySize, entrySize );
    
    char *nextEntry = new char[entrySize];
    int currentCount = 1;
    
    for ( int i = 1; i < offSet / (entrySize+keySize); i++ ) {   //not end of page
        //read next entry
        memcpy( nextEntry, data + keySize + ( entrySize+keySize) * i, entrySize );
        
        if ( memcmp( currentEntry, nextEntry, entrySize ) == 0 ) {  //if they are same
            currentCount++;
        } else {
            //if not
            //write to output
            outputPage->insertDataToPage( currentEntry, entrySize );
            outputPage->insertDataToPage( (char*)&currentCount, sizeof(int) );
            //currentEntry = nextEntry
            memcpy( currentEntry, nextEntry, entrySize );
            currentCount = 1;
            
            //if output is full,write to file
            if ( outputPage->isFull() ) {
                outputPage->writePageToFile( fptr );
                outputPage->clearPage();
            }
        }
    }
    delete []currentEntry;
    delete []nextEntry;
}

bool Page::readPropertyWithOffSet( char *property, size_t proertySize, int _offSet ) {
    if ( _offSet + (int)proertySize > offSet ) {
        return false;
    } else {
        memcpy(property, data + _offSet, proertySize );
        return true;
    }
}
