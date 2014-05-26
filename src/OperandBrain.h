//
//  dataManager.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-22.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __jgC_StoreDB__OperandBrain__
#define __jgC_StoreDB__OperandBrain__
#include "pagesManager.h"
#include "SortBrain.h"
class OperandBrain {
private:
    pagesManager *_pageManager;
    SortBrain *_sortBrain;
    void externSort( const char* tableName, const char* column );
    int pagesMemorySort( const char* inputFile );
    void mergeSortTempFiles( int count, const char* outputFile );
    
public:
    OperandBrain();
    ~OperandBrain();
    void queryInTable( const char *tableName );
    void loadData( const char *_fileName );
    void compressData( const char *tableName, const char* column );

};

#endif