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
    if ( size + offSet <= PAGE_SIZE ) {
        memcpy(data+offSet, data_, size);
        offSet += size;
        return true;
    } else {
        return false;
    }
}

void Page::clearPage() {
    memset( data, 0, PAGE_SIZE );
    offSet = 0;
}

void Page::writePageToFile( FILE *fptr ) {
    fwrite( data, sizeof(char), (size_t)offSet, fptr );
}

void Page::readFileToPage( FILE *fptr ) {
    offSet = (int)fread( data, sizeof(char), PAGE_SIZE, fptr );
}

bool Page::searchInPage( const char *key, size_t keySize, char* & outputEntry, size_t entrySize ) {
    char *currentKey = new char[keySize];
    
    bool found = false;
    int currentOffset = 0;
    
    while( currentOffset < offSet )  {
        memcpy( currentKey, data + currentOffset, keySize );
        if ( memcmp( key, currentKey, keySize ) != 0 ) {
            currentOffset += entrySize;
        } else {
            found = true;
            memcpy( outputEntry, data + currentOffset, entrySize );
            break;
        }
    }
    
    delete []currentKey;
    return found;
}


bool Page::readPropertyWithOffSet( char *property, size_t proertySize, int _offSet ) {
    if ( _offSet + (int)proertySize > offSet ) {
        return false;
    } else {
        memcpy(property, data + _offSet, proertySize );
        return true;
    }
}
