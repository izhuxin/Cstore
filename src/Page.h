//
//  Page.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-5-8.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __jgC_StoreDB__Page__
#define __jgC_StoreDB__Page__

#include <iostream>
#include "Constant.h"

class Page {
private:
    char data[PAGE_SIZE];
    size_t offSet;
    
public:
    Page();
    
    ~Page();
    
    /**
     *  copy data to page memory
     *
     *  @param data pointer to the data to insert
     *  @param size size of the data to insert
     *
     *  @return return false if the page has been full, otherwise, return true
     */
    bool insertDataToPage( char *data_, size_t size );
    
    /**
     *  clear the content of the page
     */
    void clearPage();
    
    /**
     *  write and read content between file and page
     *
     *  @param fptr the file pointer
     */
    void writePageToFile( FILE *fptr );
    void readFileToPage( FILE *fptr );
    
    /**
     *  search in page with key
     *
     *  @param key the start address of the key to search in page
     *
     *  @param _size the size of the key
     *
     *  @return true for found, false for unfound
     */
    bool searchInPage( const char *key, size_t keySize, char* & outputEntry,  size_t entrySize );
    
    void compressToPage( Page *outputPage, int keySize, int entrySize, FILE *fptr );
    
    inline bool isFull() { return offSet >= PAGE_SIZE; }
    inline bool isEmpty() { return offSet <= 0; }
    /**
     *  getters and setters for the private member
     *
     */
    inline char *getData() { return data; }
    inline size_t getOffset() { return offSet; }
    inline void setOffset( int offset ) { offSet = offset; }
};

#endif /* defined(__jgC_StoreDB__Page__) */
