//
//  FileManager.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-26.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include "FileManager.h"
#include "Constant.h"

fileManager* fileManager::_instance = NULL;

fileManager* fileManager::sharedManager() {
    if ( _instance == NULL ) {
        _instance = new fileManager();
    }
    return _instance;
}

fileManager::fileManager() {
    ordKeyFiptr = fopen( fileNameMap(__ORDER_KEY_FILE__), "ab+" );
    custKeyFiptr = fopen( fileNameMap(__CUST_KEY_FILE__), "ab+" );
    totPriFiptr = fopen( fileNameMap(__TOTAL_PRICE_FILE__), "ab+" );
    shiProFiptr = fopen ( fileNameMap(__SHIP_PRIORITY_FILE__), "ab+" );
    emptyFiptr = NULL;
}

fileManager::~fileManager() {
    //delete _instance;
    fclose(ordKeyFiptr);
    fclose(custKeyFiptr);
    fclose(totPriFiptr);
    fclose(shiProFiptr);
}

bool fileManager::writeDataToFile( void *pageData,size_t size,  fileName _fileName ) {
    size_t writedSize = fwrite( pageData, sizeof(char), size, filePtrMap(_fileName) );
    //printf( "write %zu/%zu bytes to file\n", writedSize, size );
    return writedSize != size;
}

bool fileManager::readFileToData(void *data, size_t size, fileName _fileName, bool restart) {
    FILE *fiptr = filePtrMap(_fileName);
    //1
    //printf( "File Offset: %ld\n", ftell(fiptr) );
    if ( restart ) {
        //fclose(fiptr);
        fiptr = fopen(fileNameMap(_fileName), "rb");
    }
    fread( data, sizeof(char), size, fiptr );
    if( feof( fiptr ) ) {
        fclose( fiptr );
        return true;
    }
    return false;
}
