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
    int offSet;
    
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
    bool searchInPage( void *key, size_t _size );
    
    /**
     *  getters and setters for the private member
     *
     */
    char *getData();
    
    int getOffset();
    void setOffset( int offset );
};

#endif /* defined(__jgC_StoreDB__Page__) */
