//
//  pagesManager.cpp
#include "pagesManager.h"
#include <cstdlib>

#pragma mark -
#pragma mark -sigletron functions
pagesManager* pagesManager::_instance = NULL;

pagesManager* pagesManager::sharedManager() {
    if ( _instance == NULL ) {
        _instance = new pagesManager();
    }
    return _instance;
}

pagesManager::pagesManager() {
    pages = new Page[NUMBER_OF_PAGE]();
}

pagesManager::~pagesManager() {
    delete []pages;
}

#pragma mark -
#pragma mark -operations on one page
void pagesManager::writePageAtIndexToFile( size_t index, FILE* fptr ) {
    pages[index].writePageToFile(fptr);
}

void pagesManager::clearPageAtIndex( size_t index ) {
    pages[index].clearPage();
}

bool pagesManager::searchKeyInPageAtIndex( const char* key, size_t keySize, char *entryOutput, size_t outputSize, int index ) {
    char *temp = new char[outputSize + keySize];
    //return pages[index].searchInPage( key, keySize, keySize + outputSize );
    bool found = pages[index].searchInPage(key, keySize, temp, keySize+outputSize);
    if ( found ) {
        memcpy(entryOutput, temp+keySize, outputSize);
    }
    
    return found;
}

void pagesManager::readFileToPageAtIndex( FILE *fptr, int index ) {
    pages[index].readFileToPage(fptr);
}

#pragma mark -
#pragma mark -operations on pages
bool pagesManager::insertData( char *data, size_t _size, size_t pageCondition ) {
    return pages[pageCondition].insertDataToPage(data, _size);
}

int pagesManager::readFileToPages( FILE* fptr, int begin, int end ) {
    int count = 0;
    for ( int i = begin; i <= end; i++ ) {
        pages[i].readFileToPage(fptr);
        count++;
        if ( feof( fptr ) ) {
            break;
        }
    }
    return count;
}

void pagesManager::clearPages( int begin, int end ) {
    for ( int i = begin; i <= end; i++ ) {
        pages[i].clearPage();
    }
}

#pragma mark -
#pragma mark -extern sort helper functions
void pagesManager::sortPages( int begin, int end ) {
    for ( int i = begin; i <= end; i++ ) {
        qsort( pages[i].getData(), PAGE_SIZE/8, 8, compare );
    }
}

void pagesManager::megrePagesToFile( int begin, int end, FILE* tempFptr ) {
    
    //get last Page for output
    Page *lastPage = &pages[end+1];
    lastPage->clearPage();
    
    //keep track of pages
    int *offsets = new int[end - begin + 1];

    while ( true ) {       //when the pages have not been read
        int min = 0;    //find the minimize of the first entry of pages
        bool first = true;
        
        for ( int i = begin ; i <= end; i++ ) {
            //printPage( pages[i].getData() );
            if ( offsets[ i - begin ] < PAGE_SIZE ) {
                //if this page is not been read at end
                if ( first ) {                  //init the min
                    first = false;
                    min = i;
                    
                } else if( compare(pages[i].getData()+offsets[i-begin],
                                   pages[min].getData()+offsets[min-begin] ) < 0 ) {
                    min = i;
                }
            }
        }   //for loop end, min save the value of the pageIndex of min
        
        while ( !lastPage->insertDataToPage(pages[min].getData()+offsets[min-begin], 8) ) {  //insert the min value to the output page
            
            //if insert fail
            //write page to file
            
            lastPage->writePageToFile( tempFptr );
            lastPage->clearPage();
        }
        offsets[ min - begin ] += 8;    //next entry
        
        //exam if all pages has been read
        int i;
        for ( i = begin ; i <= end; i++) {
            if (offsets[ i - begin ] < PAGE_SIZE) {   //if a page is not at the end
                break;
            }
        }
        
        if ( i == end + 1 ) {
            break;
        }
    }
    //all pages have been read and write to temp file
}

void pagesManager::compressPagesToFile( FILE *fptr, int begin, int end ) {
    Page *lastPage = &pages[ end + 1 ];     //output page
    for ( int i = begin; i <= end; i++ ) {
        //compress pages one by one
        pages[i].compressToPage( lastPage, sizeof(int), sizeof(int), fptr );
    }
    //write remain output page
    if ( !lastPage->isEmpty() ) {
        lastPage->writePageToFile( fptr );
        lastPage->clearPage();
    }
}

int pagesManager::compare(const void * a, const void * b)
{
    a = (char*)a + 4;
    b = (char*)b + 4;
    int entryA = *(int*)a;
    int entryB = *(int*)b;
    return ( entryA - entryB );
}

