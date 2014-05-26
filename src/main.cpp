//
//  main.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-21.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//


#include "OperandBrain.h"

int main(int argc, const char * argv[])
{
    // insert code here...
    OperandBrain operandBrain;
    switch (argc) {
        case 2:
            if ( strcmp( argv[1], JOIN_OPERATION_NAME) == 0 ) {
                //db join
            }
            break;
            
        case 3:
            if ( strcmp( argv[1],  LOAD_DATA_OPERATION_NAME) == 0 ) {
                // db load filename
                operandBrain.loadData( argv[2] );
            } else if ( strcmp(argv[1], QUERY_OPERATION_NAME) == 0 ) {
                operandBrain.queryInTable(argv[2]);
            }
            break;
            
        case 4:
            if ( strcmp(argv[1], COMPRESS_OPERATION_NAME) == 0 ) {
                //db compress tableName column
                operandBrain.compressData(argv[2], argv[3]);
            }
            break;
            
        default:
            printf( "%d of arguments input!\n", argc );
            //crash the application for bad input
    }
    return 0;
}
