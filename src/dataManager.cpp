//
//  dataManager.cpp
//  jgC-StoreDB
//
//  Created by Jeason on 14-4-22.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include <cstdio>  /* FILE scanf printf */
#include <cstring> /* strtok */
#include <cstdlib> /* atoi */
#include <ctime>  /* time_t time */
#include "dataManager.h"
#include "pagesManager.h"
#include "Constant.h"
#include <typeinfo>

dataManager::dataManager() {
    _pageManager = pagesManager::sharedManager();
}

void dataManager::loadData( const char* _fileName ) {
    char filePath[15] = {};
    sprintf( filePath, "../%s.tbl", _fileName );
    time_t start_time, end_time;
    start_time = time( NULL );
    FILE *fptr = fopen( filePath, "r" );
    if ( fptr == NULL ) {
        printf( "%s can not be open!", filePath );
    } else {
        if ( strcmp( _fileName, "orders" ) == 0 ) {
            char strLine[ORDER_ENTIRE_ENTRY_MAXSIZE] = {};
            while ( !feof( fptr ) ) {
                fgets( strLine, ORDER_ENTIRE_ENTRY_MAXSIZE, fptr );
                //printf( "%s\n", strLine );
                //split into properities
                int primaryKey = 0;

                char *currentProperity = strtok(strLine, "|");
                size_t currentCondition = 0;
                while ( currentProperity != NULL &&
                        currentCondition <= O_SHIPPRIORITY_CONDITION ) {
                    switch ( currentCondition ) {
                    case O_ORDERKEY_CONDITION:
                        primaryKey = atoi( currentProperity );
                        //printf( "%d", atoi( currentProperity ) );
                        _pageManager->insertOrderKey( primaryKey );
                        break;
                        
                    case O_CUSTKEY_CONDITION:
                        _pageManager->insertCustKey(primaryKey, atoi(currentProperity));
                        break;
                        
                    case O_TOTAL_PRICE_CONDITION:
                        _pageManager->insertTotalPrice(primaryKey, atof(currentProperity));
                        break;
                        
                    case O_SHIPPRIORITY_CONDITION:
                        _pageManager->insertShipPriority(primaryKey, shipPriorityMap(currentProperity));
                        break;
                    default:
                        break;
                    }
                    currentProperity = strtok( NULL, "|" );
                    currentCondition++;
                }
                //printf( "%s\n", strLine );
            }
            //end while(!feof)
            //printf("end while\n");
            _pageManager->writeAllPagesToFile();
        }
    }
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    //I think it takes not loger than one day :-)
    printf( "It takes %dhours and %dminiutes and %dseconds to load %s\n",hour, min, sec, _fileName );
}

void dataManager::queryInTable( const char *tableName ) {
    if ( strcmp(tableName, "orders") == 0 ) {
        int orderKey, custKey;
        double totalPrice;
        SHIPPRIORITY priority;
        //size_t outputSize =
        //char *arrayOutput = new char( sizeof(int) * 2 + sizeof(double) + sizeof(SHIPPRIORITY) );
        char arrayOutput[20] = {};
        //_pageManager->queryWithOrderKey( arrayOutput, 8 );
        while( scanf( "%d", &orderKey ) != EOF ) {
            time_t start_time, end_time;
            start_time = time(NULL);
            bool found = _pageManager->queryWithOrderKey( arrayOutput, orderKey );
            if ( found ) {
                memcpy( &custKey, arrayOutput+sizeof(int), sizeof(int) );
                memcpy( &totalPrice, arrayOutput+sizeof(int)*2, sizeof(double) );
                memcpy( &priority,
                        arrayOutput+sizeof(int)*2+sizeof(double),
                        sizeof(SHIPPRIORITY) );

                printf( "%-4d|%.2f|%s\n",custKey, totalPrice, priorityNameMap(priority) );
        
            } else {
                printf( "Not found!\n" );
            }
            end_time = time(NULL);
            unsigned long long dur = end_time - start_time;
            int sec = dur % 60;
            int min = ( ( dur - sec ) / 60 ) % 60;
            int hour = (int)( dur - sec - min * 60 ) /  3600;
            //I think it takes not loger than one day :-)
            printf( "It takes %dhours and %dminiutes and %dseconds\n",hour, min, sec);
        }
    }
}

void dataManager::compressData( const char *tableName, const char* column ) {
    time_t start_time, end_time;
    start_time = time(NULL);
    printf("Start externSort %s table column%s...\n", tableName, column);
    printf("It may take you serveral seconds, please wait...\n");
    externSort( tableName, column );
    printf("%s table column%s has been sorted, start to compress...\n", tableName, column);
    double rate = _pageManager->compress();
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    printf("Compress done!\nIt takes %dhours and %dminiutes and %dseconds\nCompress rate is: %lf\n", hour, min,sec, rate);
}
void dataManager::externSort(const char *tableName, const char* column) {
    if ( strcmp(tableName, "orders") == 0 ) {
        if ( strcmp(column, "1") == 0 ) {
            //orders.custKey
            int tempFileCount = pagesMemorySort( __CUST_KEY_FILE__, __INT__);
            mergeSortTempFiles( tempFileCount, __SORTED_CUST_KEY_FILE__ );
        }
    }
}

int dataManager::pagesMemorySort(fileName inputFile, propertyTypeName entryType) {
    int count = 0;
    bool f_eof = _pageManager->readPropertyToPages(inputFile, true);
    while ( !f_eof ) {
        count++;
        _pageManager->sortAllPages(entryType, count);
        f_eof = _pageManager->readPropertyToPages(inputFile, false);
    }
    _pageManager->sortAllPages(entryType, ++count);
    return count;
}

void dataManager::mergeSortTempFiles(int count, fileName outputFile) {
    //read temp[i] file to page[i]
    remove(fileNameMap( outputFile ));
    if ( count <= 0 ) return;
    FILE* *fileArray = new FILE *[count];
    for ( int i = 0 ; i < count; i++ ) {
        char fileName[15];
        sprintf(fileName, "temp%d.dat", i+1);
        fileArray[i] = fopen(fileName, "rb");
    }
    bool *fileHaveRead = new bool[count];
    memset(fileHaveRead, false, count);
    int readCount = 0;
    while ( readCount < count ) {
        for ( int i = 0 ; i < count; i++ ) {
            if ( fileHaveRead[i] )  continue;
            _pageManager->readFileToOnePage(fileArray[i]);
            if ( feof(fileArray[i]) ) {
                fileHaveRead[i] = true;
                fclose(fileArray[i]);
                readCount++;
            }
        }
        FILE *fptr = fopen(fileNameMap(outputFile), "ab+");
        _pageManager->megrePagesToFile(fptr);
        fclose(fptr);
    }
    for ( int i = 0 ; i < count; i++ ) {
        char fileName[15];
        sprintf(fileName, "temp%d.dat", i+1);
        remove( fileName );
    }
    free(fileArray);
}
