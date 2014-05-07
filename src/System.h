//
//  System.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-22.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef __jgC_StoreDB__jgDBSystem__
#define __jgC_StoreDB__jgDBSystem__

#include "dataManager.h"

class System {
private:
    static System *_instance;
    System();
    dataManager *_dataManager;
public:
    static System *sharedSystem();
    int SystemMain( int argc, const char *argv[] );
    
};

#endif /* defined(__jgC_StoreDB__jgDBSystem__) */
