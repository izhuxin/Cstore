//
//  OperandBrain.cpp
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

OperandBrain::OperandBrain() {
    _pageManager = pagesManager::sharedManager();
    _sortBrain = new SortBrain();
}

OperandBrain::~OperandBrain() {
    delete _sortBrain;
}

void OperandBrain::loadData( const char* _fileName ) {
    char filePath[20] = {};
    sprintf( filePath, "../%s.tbl", _fileName );
    time_t start_time, end_time;
    start_time = time( NULL );
    FILE *fptr = fopen( filePath, "rb" );
    char strLine[ENTIRE_ENTRY_MAXSIZE] = {};

    if ( fptr == NULL ) {
        printf( "%s can not be open!", filePath );
    } else {
        if ( strcmp( _fileName, "orders" ) == 0 ) {
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
                fgets( strLine, ENTIRE_ENTRY_MAXSIZE, fptr );
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
                        while ( !_pageManager->insertData(tempArr, sizeof(int) + sizeof(double), pageIndexs[2]) ) {
                            _pageManager->writePageAtIndexToFile(pageIndexs[2], totalPriceFptr);
                            _pageManager->clearPageAtIndex(pageIndexs[2]);
                        }
                        break;
                        
                    case O_SHIPPRIORITY_CONDITION:
                        tempData.O_SHIPPRIORITY = shipPriorityMap(currentProperity);
                        memcpy(tempArr + sizeof(int), &tempData.O_SHIPPRIORITY, sizeof(int));
                        while ( !_pageManager->insertData(tempArr, sizeof(int) + sizeof(int), pageIndexs[3]) ) {
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
            
            //wrtie pages to files
            _pageManager->writePageAtIndexToFile( pageIndexs[0], orderKeyFptr );
            _pageManager->writePageAtIndexToFile( pageIndexs[1], custKeyFptr );
            _pageManager->writePageAtIndexToFile( pageIndexs[2], totalPriceFptr );
            _pageManager->writePageAtIndexToFile( pageIndexs[3], shipPriorityFptr );
            
            //clear pages
            _pageManager->clearPages( 0, NUMBER_OF_PAGE - 1 );
            //close file
            fclose( orderKeyFptr );
            fclose( custKeyFptr );
            fclose( totalPriceFptr );
            fclose( shipPriorityFptr );

            //delete temp variable
            delete [] tempArr;
            
            
        } else if ( strcmp( _fileName, "customer" ) == 0 ) {
            int custKey;
            
            FILE *custKeyFptr = fopen( C_CustKeyFileName, "wb" );
            while ( !feof( fptr ) ) {
                //read one line of file to memory
                fgets( strLine, ENTIRE_ENTRY_MAXSIZE, fptr );
                //split into properities
                char *currentProperity = strtok( strLine, "|" );
                //current track variable
                size_t currentCondition = 0;
                size_t maxCondition = C_CUSTKEY_CONDITION;
                
                while ( currentProperity != NULL &&
                       currentCondition <= maxCondition ) {
                    switch ( currentCondition ) {
                        case C_CUSTKEY_CONDITION:
                            custKey = atoi( currentProperity );
                            
                            while ( !_pageManager->insertData( (char*)&custKey, sizeof(int), 0 ) ) {
                                _pageManager->writePageAtIndexToFile( 0, custKeyFptr );
                                _pageManager->clearPageAtIndex( 0 );
                            }
                            break;
                            
                        default:
                            break;
                    }
                    currentProperity = strtok( NULL, "|" );
                    currentCondition++;
                }
            }
            _pageManager->writePageAtIndexToFile( 0 , custKeyFptr );
            
            _pageManager->clearPageAtIndex( 0 );
            
            fclose( custKeyFptr );
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
    if ( strcmp( tableName, "orders" ) == 0 ) {
        //temp variables
        int orderKey, custKey = -1;
        double totalPrice = -1;
        SHIPPRIORITY priority = NONE;
        
        while( scanf( "%d", &orderKey ) != EOF ) {
            //time counters
            time_t start_time, end_time;
            start_time = time( NULL );
            
            bool found = false;
            
            //open files
            FILE *orderKeyFptr = fopen( orderKeyFileName, "rb" );
            if ( orderKeyFptr == NULL ) {
                printf( "%s can not be open!Do you forget to load relative data?",orderKeyFileName );
                return;
            }
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
                printf( "|%-10d|%-10d|%.2f|%s|\n",orderKey,custKey, totalPrice, priorityNameMap(priority) );

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
    //double rate = _pageManager->compress();
    double rate = 0.0;
    if ( strcmp( tableName, "orders" ) == 0 ) {
        if ( strcmp(column, "1" ) == 0 ) {
            //orders.custKey
            
            //extern sort
            _sortBrain->externSort( custKeyFileName, sortedCustKeyFileName );
            printf("%s table column%s has been sorted, start to compress...\n", tableName, column);

            //begin to compress
            //open files
            FILE *sortedFptr = fopen( sortedCustKeyFileName, "rb" );
            FILE *outputFptr = fopen( compressCustKeyFileName, "wb" );
            
            int currentCustKey;
            int nextCustKey;
            int offset = sizeof(int);
            int count = 1;
            
            //read first page from sorted file
            _pageManager->readFileToPageAtIndex( sortedFptr, 0 );
            //read first custKey from page
            _pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&currentCustKey, sizeof(int), offset, 0 );
            offset += 8;
            
            bool end = false;
            
            while ( true ) {     //while soted file have not been read
                //read a custKey from page
                while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&nextCustKey, sizeof(int), offset, 0 ) ) {
                    if ( feof( sortedFptr ) ) {     //if is end of file and page have reed
                        end = true;        //mark end
                        break;
                    } else {
                        _pageManager->readFileToPageAtIndex( sortedFptr, 0 );   //else, read a new page
                        offset = 4;
                    }
                }
                offset += 8;                    //offset point to next entry
                
                if ( end ) break;      //if end, break while loop
                
                //if two custKey is same
                if ( currentCustKey == nextCustKey ) {
                    count++;
                } else {        //else insert <key><length> to output
                   bool success =  _pageManager->insertData( (char*)&currentCustKey, sizeof(int), 1 ) &&
                    _pageManager->insertData( (char*)&count, sizeof(int), 1 );
                    if ( !success ) {   //if insert fail
                        _pageManager->writePageAtIndexToFile( 1, outputFptr );  //write page
                        _pageManager->clearPageAtIndex( 1 );    //clear output page
                        _pageManager->insertData( (char*)&currentCustKey, sizeof(int), 1 ); //reinsert
                        _pageManager->insertData( (char*)&count, sizeof(int), 1 );
                    }
                    currentCustKey = nextCustKey;   //find next
                    count = 1;
                }
            }
            //write remain page
            _pageManager->insertData( (char*)&currentCustKey, sizeof(int), 1 );
            _pageManager->insertData( (char*)&count, sizeof(int), 1 );
            _pageManager->writePageAtIndexToFile( 1, outputFptr );
            _pageManager->clearPageAtIndex( 1 );
            
            //compute compress rate
            fseek( sortedFptr , 0, SEEK_END );
            long beforeSize = ftell( sortedFptr );
            fclose( sortedFptr );
            
            fseek( outputFptr, 0, SEEK_END );
            long compressedSize = ftell( outputFptr );
            fclose( outputFptr );
            
            rate = compressedSize / ( beforeSize / 2.0 );

        }
    }
    
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    printf( "Compress done!\nIt takes %dhours and %dminiutes and %dseconds\nCompress rate is: %lf\n", hour, min,sec, rate );
}

void OperandBrain::join() {
    time_t start_time, end_time;
    start_time = time(NULL);

    //file pointers
    FILE *cCustKeyFptr = fopen( C_CustKeyFileName, "rb" );
    FILE *oCustKeyFptr = fopen( sortedCustKeyFileName, "rb" );
    FILE *compressFptr = fopen( compressCustKeyFileName, "rb" );
    if ( oCustKeyFptr == NULL || cCustKeyFptr == NULL || compressFptr == NULL ) {
        printf( "File can not be oepned!Do you forget to load data and compress it?" );
        return;
    }
    //read first pages
    _pageManager->readFileToPageAtIndex( cCustKeyFptr, 0 );
    _pageManager->readFileToPageAtIndex( compressFptr, 1 );
    _pageManager->readFileToPageAtIndex( oCustKeyFptr, 2 );
    
    //helper temp variables
    int o_custKey = -1;
    int o_custKeyCount = -1;
    int c_custKey = -1;
    int orderKey = -1;
    
    int offSets[3] = { 0 };
    bool end = false;
    printf( "-----------------------\n" );
    printf( "|CustKey   |OrderKey  |\n" );
    printf( "-----------------------\n" );
    
    while ( true ) {
        //read a custKey from compressed page
        while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&o_custKey, sizeof(int), offSets[1], 1 )
               || !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&o_custKeyCount, sizeof(int), offSets[1] + sizeof(int), 1 ) ) {
            //read fail
            //read another page
            if ( feof( compressFptr ) ) {
                //end
                end =  true;
                break;
            } else {
                _pageManager->readFileToPageAtIndex( compressFptr, 1 );
                offSets[1] = 0;
            }
            //while loops, re-readProperty
        }
        if ( end ) break;
        
        offSets[1] += sizeof(int) * 2;
        
        //read a custKey from customer page
        while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&c_custKey, sizeof(int), offSets[0], 0 ) ) {
                //read fail
                //read another page
            if ( feof( cCustKeyFptr ) ) {
                break;
            } else {
                _pageManager->readFileToPageAtIndex( cCustKeyFptr, 0 );
                offSets[0] = 0;
            }
        }
        offSets[0] += sizeof(int);

        //while two custKeys is not same
        while ( c_custKey != o_custKey ) {
            //because all o_custKey can be find in c_custKey
            //just read next custKey from customer page
            while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&c_custKey, sizeof(int), offSets[0], 0 ) ) {
                //read fail
                //read another page
                if ( feof( cCustKeyFptr ) ) {
                    break;
                } else {
                    _pageManager->readFileToPageAtIndex( cCustKeyFptr, 0 );
                    offSets[0] = 0;
                }
            }
            offSets[0] += sizeof(int);
        }
        
            //now, c_custKey == o_custKey
        for ( int i = 0; i < o_custKeyCount; i++ ) {    //print result
            while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&orderKey, sizeof(int), offSets[2], 2 ) ) {
                _pageManager->readFileToPageAtIndex( oCustKeyFptr, 2 );
                offSets[2] = 0;
            }
            offSets[2] += sizeof(int) * 2;
            printf( "|%-10d|%-10d|\n",o_custKey, orderKey );
        }
    }
    printf( "-----------------------\n" );    
    end_time = time(NULL);
    unsigned long long dur = end_time - start_time;
    int sec = dur % 60;
    int min = ( ( dur - sec ) / 60 ) % 60;
    int hour = (int)( dur - sec - min * 60 ) /  3600;
    printf( "Join done!\nIt takes %dhours and %dminiutes and %dseconds\n", hour, min,sec );

}
long OperandBrain::count() {
    
    FILE *compressedFptr = fopen( compressCustKeyFileName, "rb" );
    if ( compressedFptr == NULL ) {
        printf( "%s can not be open!Do you forget to compress it before count?\n",compressCustKeyFileName );
        return 0;
    }
    //helper variables
    long result = 0;    //length sum
    int length; //current length
    
    int offset = 4;     //keep track of page
    bool end = false;
    
    //read first page from file
    _pageManager->readFileToPageAtIndex( compressedFptr, 0 );
    while ( !end ) {
        //read a length from page
        while ( !_pageManager->readPropertyFromPageAtIndexWithOffSet( (char*)&length, sizeof(int), offset, 0 ) ) {  //if read fail
            if ( feof( compressedFptr ) ) {     //if end of file
                end = true;                     //can not read a new page, end of the count
                break;
            } else {
                //read a new page
                _pageManager->readFileToPageAtIndex( compressedFptr, 0 );
                offset = 4;
            }
        }
        
        if ( end ) break;   //if end, return the result
        result += length;   //add length to result
        offset += 8;        //offset point to next entry
    }
    
    //close file
    fclose( compressedFptr );
    return result;
}
