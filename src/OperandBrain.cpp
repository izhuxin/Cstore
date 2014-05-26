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
#include "OperandBrain.h"
#include "pagesManager.h"
#include "Constant.h"
#include <typeinfo>

OperandBrain::OperandBrain() {
    _pageManager = pagesManager::sharedManager();
    _sortBrain = new SortBrain();
}

OperandBrain::~OperandBrain() {
    delete _sortBrain;
}

void OperandBrain::loadData( const char* _fileName ) {
    char filePath[15] = {};
    sprintf( filePath, "../%s.tbl", _fileName );
    time_t start_time, end_time;
    start_time = time( NULL );
    FILE *fptr = fopen( filePath, "rb" );
    if ( fptr == NULL ) {
        printf( "%s can not be open!", filePath );
    } else {
        if ( strcmp( _fileName, "orders" ) == 0 ) {
            char strLine[ORDER_ENTIRE_ENTRY_MAXSIZE] = {};
            //tempHelper variable
            int primaryKey = 0;
            char *tempArr = new char[sizeof(int) + sizeof(double)];
            ORDERDATA tempData;
            
            //pageIndex counters
            size_t pageIndexs[4] = {0, 1, 2, 3};
            
            //file pointer
            FILE *orderKeyFptr = fopen( "orderKey.dat", "wb" );
            FILE *custKeyFptr = fopen( "custKey.dat", "wb" );
            FILE *totalPriceFptr = fopen( "totalPrice.dat", "wb" );
            FILE *shipPriorityFptr = fopen( "shipPriority.dat", "wb" );
            while ( !feof( fptr ) ) {
                //read one line of file to memory
                fgets( strLine, ORDER_ENTIRE_ENTRY_MAXSIZE, fptr );
                //split into properities
                char *currentProperity = strtok(strLine, "|");
                //current track variable
                size_t currentCondition = 0;
                size_t maxCondition = O_SHIPPRIORITY_CONDITION;
                
                while ( currentProperity != NULL &&
                        currentCondition <= maxCondition ) {
                    //while not the end of the line
                    switch ( currentCondition ) {                    // TODO: clear the switch code

                    case O_ORDERKEY_CONDITION:
                        primaryKey = atoi( currentProperity );
                        memcpy(tempArr, &primaryKey, sizeof(int));
                        while ( !_pageManager->insertData((char*)&primaryKey, sizeof(int),pageIndexs[0] ) ) {  //if insert fail
                            //write the page to file
                            _pageManager->writePageAtIndexToFile( pageIndexs[0],orderKeyFptr );
                            //clear page
                            _pageManager->clearPageAtIndex( pageIndexs[0] );
                            //re-insert
                        }
                        break;
                        
                    case O_CUSTKEY_CONDITION:
                        tempData.O_CUSTKEY = atoi(currentProperity);
                        memcpy(tempArr + sizeof(int), &tempData.O_CUSTKEY, sizeof(int));
                        while ( !_pageManager->insertData(tempArr, sizeof(int) * 2, pageIndexs[1]) ) {  //if insert fail
                            //write the page to file
                            _pageManager->writePageAtIndexToFile(pageIndexs[1], custKeyFptr);
                            //clear page
                            _pageManager->clearPageAtIndex(pageIndexs[1]);
                            //while loop, re-insert
                        }
                        break;
                        
                    case O_TOTAL_PRICE_CONDITION:
                        tempData.O_TOTALPRICE = atof(currentProperity);
                        memcpy(tempArr + sizeof(int), &tempData.O_TOTALPRICE, sizeof(double));
                        if ( !_pageManager->insertData(tempArr, sizeof(int) + sizeof(double), pageIndexs[2]) ) {
                            _pageManager->writePageAtIndexToFile(pageIndexs[2], totalPriceFptr);
                            _pageManager->clearPageAtIndex(pageIndexs[2]);
                        }
                        break;
                        
                    case O_SHIPPRIORITY_CONDITION:
                        tempData.O_SHIPPRIORITY = shipPriorityMap(currentProperity);
                        memcpy(tempArr + sizeof(int), &tempData.O_SHIPPRIORITY, sizeof(int));
                        if ( !_pageManager->insertData(tempArr, sizeof(int) + sizeof(int), pageIndexs[3]) ) {
                            _pageManager->writePageAtIndexToFile(pageIndexs[3], shipPriorityFptr);
                            _pageManager->clearPageAtIndex(pageIndexs[3]);
                        }
                        break;
                    default:
                        break;
                    }
                    currentProperity = strtok( NULL, "|" );
                    currentCondition++;
                }
            }
            //end while(!feof)
            //printf("end while\n");
            
            //wrtie pages to files
            _pageManager->writePageAtIndexToFile(pageIndexs[0], orderKeyFptr);
            _pageManager->writePageAtIndexToFile(pageIndexs[1], custKeyFptr);
            _pageManager->writePageAtIndexToFile(pageIndexs[2], totalPriceFptr);
            _pageManager->writePageAtIndexToFile(pageIndexs[3], shipPriorityFptr);
            
            //clear pages
            for ( int i = 0; i < 4; i++ ) {
                _pageManager->clearPageAtIndex(pageIndexs[i]);
            }
            
            //close file
            fclose(orderKeyFptr);
            fclose(custKeyFptr);
            fclose(totalPriceFptr);
            fclose(shipPriorityFptr);
            
            //delete temp variable
            free(tempArr);
        } else if ( strcmp(_fileName, "customer") == 0 ) {
            
        }
    }
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    //I think it takes not loger than one day :-)
    printf( "\nDone!It takes %dhours and %dminiutes and %dseconds to load %s\n",hour, min, sec, _fileName );
}

void OperandBrain::queryInTable( const char *tableName ) {
    if ( strcmp(tableName, "orders") == 0 ) {
        //temp variables
        int orderKey, custKey = -1;
        double totalPrice = -1;
        SHIPPRIORITY priority = NONE;
        
        while( scanf( "%d", &orderKey ) != EOF ) {
            //time counters
            time_t start_time, end_time;
            start_time = time(NULL);
            
            bool found = false;
            
            //open files
            FILE *orderKeyFptr = fopen( orderKeyFileName, "rb" );
            FILE *custKeyFptr = fopen( custKeyFileName, "rb" );
            FILE *totalPriceFptr = fopen( totalPriceFileName, "rb" );
            FILE *shipPriorityFptr = fopen( shipPriorityFileName, "rb");
            
            //search in orderKeyFile with orderKey
            //read one page of data from file
            _pageManager->readFileToPageAtIndex( orderKeyFptr, 0 );
            while ( !feof( orderKeyFptr ) ) {
                //search key in page
                if ( !_pageManager->searchKeyInPageAtIndex( (char*)&orderKey, sizeof(int), (char*)&orderKey, 0, 0 ) ) {
                    // if not found, read next page of data from file
                    _pageManager->readFileToPageAtIndex( orderKeyFptr, 0 );
                } else {
                    found = true;
                    break;
                }
            }
            
            if ( found ) {  //if find orderKey, it means the tuple can be found by the orderKey
                //find relative custkey
                //read one page of data from custkey file
                _pageManager->readFileToPageAtIndex(custKeyFptr, 0);
                while ( !feof( custKeyFptr ) ) {
                    //search key in page
                    if ( !_pageManager->searchKeyInPageAtIndex((char*)&orderKey, sizeof(int), (char*)&custKey, sizeof(int), 0) ) {
                        // if not found, read next page of data from file
                        _pageManager->readFileToPageAtIndex(custKeyFptr, 0);
                    } else {
                        break;
                    }
                }
                
                //find relative totalPrice
                _pageManager->readFileToPageAtIndex( totalPriceFptr, 0 );
                while ( !feof( totalPriceFptr ) ) {
                    if ( !_pageManager->searchKeyInPageAtIndex( (char*)&orderKey, sizeof(int), (char*)&totalPrice, sizeof(double), 0 ) ) {
                        _pageManager->readFileToPageAtIndex( totalPriceFptr, 0 );
                    } else {
                        break;
                    }
                }
                
                //find relative shipPriority
                _pageManager->readFileToPageAtIndex(shipPriorityFptr, 0);
                while ( !feof( shipPriorityFptr ) ) {
                    if ( !_pageManager->searchKeyInPageAtIndex( (char*)&orderKey, sizeof(int), (char*)&priority, sizeof(int), 0) ) {
                        _pageManager->readFileToPageAtIndex( shipPriorityFptr, 0 );
                    } else {
                        break;
                    }
                }
                //print the result
                printf( "%-4d|%.2f|%s\n",custKey, totalPrice, priorityNameMap(priority) );

            } else {    //can not find orderKey, the query fail
                printf( "%d is not found!\n", orderKey );
            }
            
            // TODO: give an accurate time
            end_time = time(NULL);
            unsigned long long dur = end_time - start_time;
            int sec = dur % 60;
            int min = ( ( dur - sec ) / 60 ) % 60;
            int hour = (int)( dur - sec - min * 60 ) /  3600;
            
            //clear page
            _pageManager->clearPageAtIndex(0);
            
            //close the file
            fclose( orderKeyFptr );
            fclose( custKeyFptr );
            fclose( totalPriceFptr );
            fclose( shipPriorityFptr );
            
            printf( "It takes %dhours and %dminiutes and %dseconds\n",hour, min, sec);
        }
    }
    else {
        printf( "Table %s not in the system!\n", tableName );
    }
}

void OperandBrain::compressData( const char *tableName, const char* column ) {
    time_t start_time, end_time;
    start_time = time(NULL);
    printf("Start externSort %s table column%s...\n", tableName, column);
    printf("It may take you serveral seconds, please wait...\n");
    printf("%s table column%s has been sorted, start to compress...\n", tableName, column);
    //double rate = _pageManager->compress();
    double rate = 0.0;
    if ( strcmp( tableName, "orders" ) == 0 ) {
        if ( strcmp(column, "1" ) == 0 ) {
            //orders.custKey
            
            //extern sort
            _sortBrain->externSort(custKeyFileName, sortedCustKeyFileName);
            
            //begin to compress
            //open files
            FILE *sortedFptr = fopen( sortedCustKeyFileName, "rb" );
            FILE *outputFptr = fopen( compressCustKeyFileName, "wb" );
            
            //read NUMBER_OF_PAGE - 2 pages of data to pages
            int readCount = _pageManager->readFileToPages( sortedFptr, 0, NUMBER_OF_PAGE - 2 );
            while ( readCount == NUMBER_OF_PAGE - 1 ) {
                //while not end of file
                
                //compress thest pages data, and save to file
                _pageManager->compressPagesToFile( outputFptr, 0, NUMBER_OF_PAGE - 2 );
                
                //read next pages
                readCount = _pageManager->readFileToPages( sortedFptr, 0, NUMBER_OF_PAGE - 2 );
            }
            
            //compress remain pages
            _pageManager->compressPagesToFile( outputFptr, 0, readCount );
            
            //clear pages
            _pageManager->clearPages( 0, NUMBER_OF_PAGE - 1 );
            
            //compute compress rate
            fseek( sortedFptr , 0, SEEK_END );
            long beforeSize = ftell( sortedFptr );
            fclose( sortedFptr );
            
            fseek( outputFptr, 0, SEEK_END );
            long compressedSize = ftell( outputFptr );
            fclose( outputFptr );
            
            rate = 1.0 * compressedSize / ( beforeSize / 2.0 );
        }
    }
    
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    printf("Compress done!\nIt takes %dhours and %dminiutes and %dseconds\nCompress rate is: %lf\n", hour, min,sec, rate);
}

