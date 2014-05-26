//
//  SortBrain.h
//  CStore
//
//  Created by Jeason on 14-5-26.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __CStore__SortBrain__
#define __CStore__SortBrain__

#include "pagesManager.h"

class SortBrain {
private:
    pagesManager *_pageManager;
    int pagesMemorySort( const char* inputFile );
    void mergeSortTempFiles( int count, const char* outputFile );

public:
    SortBrain();
    ~SortBrain();
    void externSort( const char *inputFileName, const char* outputFileName );
};

#endif /* defined(__CStore__SortBrain__) */
