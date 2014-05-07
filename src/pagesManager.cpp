//
//  pagesManager.cpp
#include "pagesManager.h"
#include "FileManager.h"
#include <cstdlib>

pagesManager* pagesManager::_instance = NULL;
static const int inset = NUMBER_OF_PAGE / NUMBER_OF_PRORITY;

pagesManager* pagesManager::sharedManager() {
    if ( _instance == NULL ) {
        _instance = new pagesManager();
    }
    return _instance;
}

pagesManager::pagesManager() {
    pages = new page[NUMBER_OF_PAGE]();
    orderKeyIndex = 0;
    custKeyIndex = orderKeyIndex + inset;
    totalPirceIndex = orderKeyIndex + 2 * inset;
    priorityIndex = orderKeyIndex + 3 * inset;
    pageIndex = 0;
}

pagesManager::~pagesManager() {
    delete []pages;
}

void pagesManager::insertOrderKey( int orderKey ) {
    orderKeyIndex = insertInPagesAtIndex(orderKeyIndex, &orderKey, sizeof(orderKey));
    if ( orderKeyIndex == -1 ) {
        //printf( "write order kes to file" );
        orderKeyIndex = 0;
        writePagesToFile(0, inset - 1, __ORDER_KEY_FILE__);
        return insertOrderKey( orderKey );
    }
}

void pagesManager::insertCustKey( int orderKey, int custKey ) {
    int tempArr[2] = { orderKey, custKey };
    //printf( "temp Arr size is :%u\n", sizeof( tempArr ) );
    custKeyIndex = insertInPagesAtIndex(custKeyIndex, tempArr, sizeof(tempArr));
    if ( custKeyIndex == -1 ) {
        custKeyIndex = inset;
        writePagesToFile(inset, inset*2-1, __CUST_KEY_FILE__);
        return insertCustKey( orderKey,custKey );
    }
}

void pagesManager::insertTotalPrice( int orderKey, double totalPrice ) {
    char *tempArr = new char[sizeof(int)+sizeof(double)];
    memcpy( tempArr, &orderKey, sizeof(int) );
    memcpy( tempArr + sizeof(int), &totalPrice, sizeof(double) );
    totalPirceIndex = insertInPagesAtIndex(totalPirceIndex,
                                           tempArr,
                                           sizeof(int) + sizeof(double));
    if ( totalPirceIndex == -1 ) {
        totalPirceIndex =  2 * inset;
        writePagesToFile(inset*2, inset*3-1, __TOTAL_PRICE_FILE__);
        return insertTotalPrice( orderKey, totalPrice );
    }
}

void pagesManager::insertShipPriority( int orderKey, SHIPPRIORITY priority ) {
    //printf( "size of priority is :%u\n", sizeof( priority ) );
    int tempArr[2] = { orderKey, priority };
    priorityIndex = insertInPagesAtIndex(priorityIndex, &tempArr, sizeof(tempArr));
    if ( priorityIndex == -1 ) {
        priorityIndex = 3 * inset;
        writePagesToFile(inset*3, inset*4-1, __SHIP_PRIORITY_FILE__);
        return insertShipPriority( orderKey, priority );
    }
}

int pagesManager::insertInPagesAtIndex( int index, const void *entry, size_t size ) {
    if ( index >= NUMBER_OF_PAGE || index < 0 ) {
        return -1;
    }
    page *currentPage = &pages[index];
    if ( currentPage->offSet + size > PAGE_SIZE ) {
        index++;
        if ( index >= NUMBER_OF_PAGE || index < 0 ) {
            return -1;
        }
        currentPage = &pages[index];
        if ( currentPage->offSet != 0 ) {
            //no empty memory exist, insert false
            return -1;
        }
    }
    memcpy(currentPage->data + currentPage->offSet, entry, size);
    currentPage->offSet += size;
    return index;
}

bool pagesManager::writePagesToFile(int low, int high, fileName _fileName) {
    //printf( "write pages[%d] to pages[%d] to file\n", low, high );
    for ( int index = low ; index <= high; index++ ) {
        bool nonFullPage = fileManager::sharedManager()->writeDataToFile( pages[index].data, PAGE_SIZE,  _fileName );
        if ( nonFullPage && index == high  ) {
            // TODO:handle last page
            return fileManager::sharedManager()->writeDataToFile( &pages[index].offSet, sizeof(int), _fileName );
        }
        pages[index].offSet = 0;
    }

    return false;
}

int pagesManager::writeAllPagesToFile() {
    int count = 0;
    for ( int i = 0; i < NUMBER_OF_PAGE; i++ ) {
        //printf( "pages[%d]'s offSet is:%d\n", i, pages[i].offSet );
        if ( pages[i].offSet != 0 ) {
            count++;
            if ( i < inset ) {
                writePagesToFile(i, i,__ORDER_KEY_FILE__ );
            } else if ( i < 2 * inset ) {
                writePagesToFile(i, i, __CUST_KEY_FILE__ );
            } else if ( i < 3 * inset ) {
                writePagesToFile(i, i, __TOTAL_PRICE_FILE__ );
            } else {
                writePagesToFile( i, i, __SHIP_PRIORITY_FILE__ );
            }
        }
    }
    return count;
}

int pagesManager::searchInFile( int key, int pageIndex, fileName _fileName ) {
    int OffSet = -1;
    bool f_eof =
            fileManager::sharedManager()->readFileToData(pages[pageIndex].data,
                                                         PAGE_SIZE,
                                                         _fileName,
                                                         true);
    while( !f_eof ) {
        size_t steps[4] = { sizeof(int), sizeof(int)*2, sizeof(int)+sizeof(double), sizeof(int)+sizeof(SHIPPRIORITY) };
        OffSet = searchInPageAtIndex( pageIndex, key, steps[_fileName] );
        if ( OffSet == -1 ) {
            f_eof =
                    fileManager::sharedManager()->readFileToData( pages[pageIndex].data,
                                                                  PAGE_SIZE,
                                                                  _fileName,
                                                                  false );
        } else {
            break;
        }
    }
    return OffSet;
}

bool pagesManager::queryWithOrderKey(char *arrayOutput, int orderKey) {
    //read orderKey.dat
    int count = 0;
    int OffSet = searchInFile( orderKey, 0, __ORDER_KEY_FILE__ );
    if ( OffSet == -1 ) return false;
    memcpy( arrayOutput, pages[0].data+OffSet, sizeof(int) );
    count += sizeof(int);

    OffSet = searchInFile( orderKey,0, __CUST_KEY_FILE__ );
    if ( OffSet == -1 ) return false;
    memcpy( arrayOutput+count, pages[0].data+OffSet+sizeof(int), sizeof(int) );
    count += sizeof(int);

    OffSet = searchInFile( orderKey, 0, __TOTAL_PRICE_FILE__ );
    if ( OffSet == -1 ) return false;
    memcpy( arrayOutput+count, pages[0].data+OffSet+sizeof(int), sizeof(double) );
    count += sizeof(double);

    OffSet = searchInFile( orderKey, 0, __SHIP_PRIORITY_FILE__ );
    if ( OffSet == -1 ) return false;
    memcpy( arrayOutput+count, pages[0].data+OffSet+sizeof(int), sizeof(SHIPPRIORITY) );
    count += sizeof(SHIPPRIORITY);
    //fclose();
    return true;
}

int pagesManager::searchInPageAtIndex(int index, int key, size_t step) {
    int primaryKey = -1;
    page currentPage = pages[index];
    currentPage.offSet = 0;
    while( currentPage.offSet < PAGE_SIZE )  {
        memcpy( &primaryKey, currentPage.data+currentPage.offSet, sizeof(int) );
        if ( key != primaryKey ) {
            currentPage.offSet += step;
        } else {
            break;
        }
    }
    if ( key == primaryKey ) return currentPage.offSet;
    else return -1;
}

bool pagesManager::readPropertyToPages( fileName _fileName, bool restart ) {
    //printf("read %s to pages[%d]..\n", fileNameMap(_fileName), pageIndex);
    bool f_eof = fileManager::sharedManager()->readFileToData(pages[pageIndex++].data, PAGE_SIZE, _fileName, restart);
    while ( !f_eof ) {
        if ( pageIndex >= NUMBER_OF_PAGE-1 ) {
            //unread file remain
            //remain one page to output
            return false;
        } else {
            //printf("read %s to pages[%d]..\n", fileNameMap(_fileName), pageIndex);
            f_eof = fileManager::sharedManager()->readFileToData(pages[pageIndex++].data, PAGE_SIZE, _fileName, false);

        }
    }
    return true;
}
/*void printPage( char *content ) {
    for ( int i = 0 ; i < PAGE_SIZE; i+=8 ) {
        int orderKey, custKey;
        memcpy(&orderKey, content+i, 4);
        memcpy(&custKey, content+4+i, 4);
        printf("orderKey:%d, custKey:%d\n", orderKey, custKey);
    }
}*/

int pagesManager::compare(const void * a, const void * b)
{
    a = (char*)a + 4;
    b = (char*)b + 4;
    int entryA = *(int*)a;
    int entryB = *(int*)b;
    return ( entryA - entryB );
}

void pagesManager::sortAllPages( propertyTypeName type, int fileIndex ) {
    for ( int index = 0 ; index < pageIndex; index++ ) {
        //printf("pages[%d] before sorting is:\n", index);
        //printPage(pages[index].data);
        //printf("sorting pages[%d] content...\n", index);
        //quick_sort(pages[index].data, type, 0, PAGE_SIZE-1);
        qsort(pages[index].data, PAGE_SIZE/8, 8, compare);
        //printf("pages [%d] after sorting is: \n", index);
        //printPage(pages[index].data);
    }
    char _fileName[20] = "";
    sprintf(_fileName, "temp%d.dat",fileIndex);
    FILE *fptr = fopen(_fileName, "wb");
    megrePagesToFile( fptr );
    fclose(fptr);
    
}

void pagesManager::megrePagesToFile( FILE *fptr ) {
    page *lastPage = &pages[pageIndex];
    for ( int i = 0 ; i < pageIndex; i++) {
        pages[i].offSet = 0;
    }
    bool allPagesHaveRead = false;
    bool *readPages = new bool[pageIndex];
    memset(readPages, false, pageIndex);
    //when the pages have not been read
    while ( !allPagesHaveRead ) {
        int min = 0;
        bool first = true;
        for ( int i = 0 ; i < pageIndex; i++ ) {
            if ( !readPages[i] ) {
                if ( first ) {
                    first = false;
                    min = i;
                    //printf("current min custKey:%d\n", *(int*)(pages[min].data + pages[min].offSet+4));

                } else if( compare(pages[i].data+pages[i].offSet, pages[min].data+pages[min].offSet ) < 0) {
                    min = i;
                    //printf("current min custKey:%d\n", *(int*)(pages[min].data + pages[min].offSet+4));
                }
            }
        }
        memcpy(lastPage->data+lastPage->offSet, pages[min].data+pages[min].offSet, 8);
        pages[min].offSet += 8;
        //printf("write pages[%d] to last page, offset: %d\n", min, pages[min].offSet);
        if ( pages[min].offSet >= PAGE_SIZE ) {
            readPages[min] = true;
        }
        lastPage->offSet += 8;
        //if the last page is full, write it to file
        if ( lastPage->offSet >= PAGE_SIZE ) {
            //printf("writing contens to temp file...\n");
            //printPage(lastPage->data);
            fwrite(lastPage->data, sizeof(char), PAGE_SIZE, fptr);
            lastPage->offSet = 0;
        }
        int i;
        for ( i = 0 ; i < pageIndex; i++) {
            if (!readPages[i]) {
                break;
            }
        }
        if ( i == pageIndex ) {
            allPagesHaveRead = true;
        }
    }
    //all pages have been read and write to temp file
    pageIndex = 0;
}

bool pagesManager::readFileToOnePage( FILE *&fptr ) {
    pageIndex++;
    if ( pageIndex >= NUMBER_OF_PAGE ) {
        return false;
    }
    fread(pages[pageIndex].data, sizeof(char), PAGE_SIZE, fptr);
    return true;
}

double pagesManager::compress() {
    FILE *fptr = fopen( fileNameMap(__SORTED_CUST_KEY_FILE__) , "rb");
    FILE *outputFptr = fopen(compressCustKeyFileName, "wb");
    page *outputPage = &pages[NUMBER_OF_PAGE-1];
    outputPage->offSet = 0;
    readFileToOnePage(fptr);
    clearPage();
    while ( !feof(fptr) ) {
        readFileToOnePage(fptr);
        //printPage(pages[pageIndex].data);
        int currentCustKey;
        int currentCount = 1;
        int nextCustKey;
        memcpy( &currentCustKey, pages[pageIndex].data+sizeof(int), sizeof(int));
        for ( int i = 1; i < (int)( PAGE_SIZE / (sizeof(int) * 2) ); i++) {
            memcpy( &nextCustKey, pages[pageIndex].data + i * sizeof(int) * 2 + sizeof(int), sizeof(int));
            if ( nextCustKey == currentCustKey ) {
                currentCount++;
            } else {
                memcpy(outputPage->data+outputPage->offSet, &currentCustKey, sizeof(int));
                outputPage->offSet += sizeof(int);
                memcpy(outputPage->data+outputPage->offSet, &currentCount, sizeof(int));
                outputPage->offSet += sizeof(int);
                currentCount = 1;
                currentCustKey = nextCustKey;
                if ( outputPage->offSet >= PAGE_SIZE ) {
                    //printPage(outputPage->data);
                    fwrite(outputPage->data, sizeof(char), PAGE_SIZE, outputFptr);
                    outputPage->offSet = 0;
                }
            }
        }
        clearPage();
    }
    fseek(fptr, 0, SEEK_END);
    long beforeSize = ftell(fptr);
    beforeSize /= 2;
    fclose(fptr);
    
    fseek(outputFptr, 0, SEEK_END);
    long compressedSize = ftell(outputFptr);
    fclose(outputFptr);
    
    return 1.0 * compressedSize / beforeSize;
    
}

