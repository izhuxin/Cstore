//
//  FileManager.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-26.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __jgC_StoreDB__FileManager__
#define __jgC_StoreDB__FileManager__
#include "Constant.h"
#include <stdio.h> //FILE
class fileManager {
private:
    static fileManager *_instance;
    fileManager();
    ~fileManager();
    FILE *ordKeyFiptr;
    FILE *custKeyFiptr;
    FILE *totPriFiptr;
    FILE *shiProFiptr;
    FILE *emptyFiptr;
    FILE *sortedCustKeyFiptr;
    inline FILE* &filePtrMap( fileName _fileName ){
        switch( _fileName ) {
        case __ORDER_KEY_FILE__:
            return ordKeyFiptr;

        case __CUST_KEY_FILE__:
            return custKeyFiptr;

        case __TOTAL_PRICE_FILE__:
            return totPriFiptr;

        case __SHIP_PRIORITY_FILE__:
            return shiProFiptr;
        default:
            return emptyFiptr;
        }
        return emptyFiptr;
    }

public:
    static fileManager* sharedManager();
    bool writeDataToFile( void *data, size_t size,  fileName _fileName );
    bool readFileToData( void *data, size_t size, fileName _fileName, bool restart );
    /*bool writeExtraInfoToFile( void *info, fileName _fileName );*/
};

#endif /* defined(__jgC_StoreDB__FileManager__) */
