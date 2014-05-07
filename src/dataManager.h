//
//  dataManager.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-22.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __jgC_StoreDB__dataManager__
#define __jgC_StoreDB__dataManager__
#include "pagesManager.h"
class dataManager {
private:
    pagesManager *_pageManager;
    void externSort( const char* tableName, const char* column );
    int pagesMemorySort( fileName inputFile, propertyTypeName entryType );
    void mergeSortTempFiles( int count, fileName outputFile );
public:
    dataManager();
    void queryInTable( const char *tableName );
    void loadData( const char *_fileName );
    void compressData( const char *tableName, const char* column );

};

#endif /* defined(__jgC_StoreDB__dataManager__) */
