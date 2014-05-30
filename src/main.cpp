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
    OperandBrain _operandBrain;
    switch (argc) {
        case 2:
            if ( strcmp( argv[1], JOIN_OPERATION_NAME) == 0 ) {
                //db join
                _operandBrain.join();
            }
            break;
            
        case 3:
            if ( strcmp( argv[1],  LOAD_DATA_OPERATION_NAME ) == 0 ) {
                // db load filename
                _operandBrain.loadData( argv[2] );
            } else if ( strcmp(argv[1], QUERY_OPERATION_NAME ) == 0 ) {
                _operandBrain.queryInTable( argv[2] );
            }
            break;
            
        case 4:
            if ( strcmp(argv[1], COMPRESS_OPERATION_NAME) == 0 ) {
                //db compress tableName column
                _operandBrain.compressData(argv[2], argv[3]);
            }
            break;
            
        default:
            printf( "%d of arguments input!\n", argc );
            break;
            //crash the application for bad input
    }
    return 0;
}
