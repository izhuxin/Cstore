//
//  OperandBrain.h
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
    //memory management
    pagesManager *_pageManager;
    
    //extern sort helper
    SortBrain *_sortBrain;
    
public:
    OperandBrain();
    
    ~OperandBrain();
    
    /* operands the system support */
    void queryInTable( const char *tableName );
    
    void loadData( const char *_fileName );
    
    void compressData( const char *tableName, const char* column );
    
    void join();
    
    long count();
};

#endif