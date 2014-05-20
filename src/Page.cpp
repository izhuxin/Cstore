//
//  Page.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-5-8.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include "Page.h"
#include "Constant.h"

Page::Page() {
    offSet = 0;
}

Page::~Page() {

}

bool Page::insertDataToPage(char *data_, size_t size) {
    if ( size + offSet <= PAGE_SIZE ) {
        memcpy(data+offSet, data_, size);
        offSet += size;
    } else {
        return false;
    }
    
    return true;
}

void Page::clearPage() {
    offSet = 0;
}

void Page::writePageToFile( FILE *fptr ) {
    fwrite(data, sizeof(char), PAGE_SIZE, fptr);
}

void Page::readFileToPage( FILE *fptr ) {
    fread(data, sizeof(char), PAGE_SIZE, fptr);
}

bool Page::searchInPage( int key ) {
    int currentKey = 0;
    int currentOffset = offSet;
    while( currentOffset < PAGE_SIZE )  {
        memcpy( &currentKey, data+currentOffset, sizeof(int) );
        if ( key != currentKey ) {
            currentOffset += sizeof(int);
        } else {
            break;
        }
    }
    if ( key == currentKey ) return true;
    else return false;

}