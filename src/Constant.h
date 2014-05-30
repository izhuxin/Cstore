//
//  Constant.h
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-26.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#ifndef jgC_StoreDB_Constant_h
#define jgC_StoreDB_Constant_h

/* Operation name definition */
#define JOIN_OPERATION_NAME "join"
#define LOAD_DATA_OPERATION_NAME "load"
#define QUERY_OPERATION_NAME "retrieve"
#define COMPRESS_OPERATION_NAME "compress"

#define ENTIRE_ENTRY_MAXSIZE 300 /* Size of one line */

/* Columns of order table need to be loaded into system */
#define O_ORDERKEY_CONDITION 0
#define O_CUSTKEY_CONDITION 1
#define O_TOTAL_PRICE_CONDITION 3
#define O_SHIPPRIORITY_CONDITION 5

/* Columns of customer table need to be loaded into system */
#define C_CUSTKEY_CONDITION 0

#define NUMBER_OF_PAGE 128  /* Size of PAGES(*PAGE_SIZE bytes) */
#define PAGE_SIZE 4 * 1024  /* Size per page */

/* Files names */
#define orderKeyFileName "orderKey.dat"
#define custKeyFileName "custKey.dat"
#define totalPriceFileName "totalPrice.dat"
#define shipPriorityFileName "shipPriority.dat"
#define sortedCustKeyFileName "sortedCustKey.dat"
#define compressCustKeyFileName "compressedCustKey.dat"
#define C_CustKeyFileName "C_CustKey.dat"

/* Enum for ship-priority in order table */
typedef enum SHIPPRIORITY {
    NONE = 0,
    URGENT,
    HIGH,
    MEDIUM,
    NOT_SPECIFIED,
    LOW
}SHIPPRIORITY;

/* Map functions */
inline const char* priorityNameMap( SHIPPRIORITY priority ) {
    static char nameMap[5][20] = { "1-URGENT", "2-HIGH", "3-MEDIUM", "4-NOT SPECIFIED", "5-LOW" };
    return nameMap[priority - 1];
}

inline SHIPPRIORITY shipPriorityMap( const char* name ) {
    switch ( name[0] ) {
        case '1':
            return URGENT;
        case '2':
            return HIGH;
        case '3':
            return MEDIUM;
        case '4':
            return NOT_SPECIFIED;
        case '5':
            return LOW;
    }
    return NONE;
}


/* Union for entry in order table */
typedef union ORDERDATA {
    int O_ORDERKEY;
    int O_CUSTKEY;
    double O_TOTALPRICE;
    SHIPPRIORITY O_SHIPPRIORITY;
}ORDERDATA;

#endif
