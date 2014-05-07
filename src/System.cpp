//
//  System.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-22.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include "System.h"
#include <cstring>
#include <cstdio>
#include "Constant.h"
System* System::_instance = NULL;
System* System::sharedSystem() {
    if ( _instance == NULL ) {
        _instance = new System();
    }
    return _instance;
}

System::System() {
    _dataManager = new dataManager();
}

int System::SystemMain( int argc, const char *argv[] ) {
    switch (argc) {
        case 2:
            if ( strcmp( argv[1], JOIN_OPERATION_NAME) == 0 ) {
                //db join
            }
            break;
            
        case 3:
            if ( strcmp( argv[1],  LOAD_DATA_OPERATION_NAME) == 0 ) {
                // db load filename
                sharedSystem()->_dataManager->loadData( argv[2] );
            } else if ( strcmp(argv[1], QUERY_OPERATION_NAME) == 0 ) {
                sharedSystem()->_dataManager->queryInTable(argv[2]);
            }
            break;
        
        case 4:
            if ( strcmp(argv[1], COMPRESS_OPERATION_NAME) == 0 ) {
                //db compress tableName column
                sharedSystem()->_dataManager->compressData(argv[2], argv[3]);
            }
            break;
         printf( "%d of arguments input!\n", argc );
        //crash the application for bad input
    }
    return 0;
}

