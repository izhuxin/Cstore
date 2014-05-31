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
    pages[index].writePageToFile( fptr );
}

void pagesManager::clearPageAtIndex( size_t index ) {
    pages[index].clearPage();
}

bool pagesManager::searchKeyInPageAtIndex( const char* key, size_t keySize, char *entryOutput, size_t outputSize, int index ) {
    char *temp = new char[outputSize + keySize];
    bool found = pages[index].searchInPage( key, keySize, temp, keySize+outputSize );
    if ( found ) {
        memcpy( entryOutput, temp+keySize, outputSize );
    }
    
    return found;
}

void pagesManager::readFileToPageAtIndex( FILE *fptr, int index ) {
    pages[index].readFileToPage( fptr );
}

bool pagesManager::readPropertyFromPageAtIndexWithOffSet( char *property, size_t propertySize, int _offset, int pageIndex ) {
    return pages[pageIndex].readPropertyWithOffSet( property, propertySize, _offset );
}

bool pagesManager::insertData( char *data, size_t _size, size_t pageCondition ) {
    return pages[pageCondition].insertDataToPage( data, _size );
}

#pragma mark -
#pragma mark -operations on pages
int pagesManager::readFileToPages( FILE* fptr, int begin, int end ) {
    int count = 0;      //count for pages that have been read
    for ( int i = begin; i <= end; i++ ) {
        pages[i].readFileToPage(fptr);
        count++;
        if ( feof( fptr ) ) break;  // if end of file, stop read next page
    }
    return count;
}

void pagesManager::clearPages( int begin, int end ) {
    for ( int i = begin; i <= end; i++ ) {
        pages[i].clearPage();
    }
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

#pragma mark -
#pragma mark -extern sort helper functions
void pagesManager::sortPages( int begin, int end ) {
    for ( int i = begin; i <= end; i++ ) {
        // TODO: Implete a algorithm to sort general page with comparable entries
        int entrySize = sizeof(int) + sizeof(int);
        qsort( pages[i].getData(), pages[i].getOffset() / entrySize, entrySize, compare );
    }
}

void pagesManager::megrePagesToFile( int begin, int end, FILE* tempFptr ) {
    //get last Page for output
    Page *lastPage = &pages[ end + 1 ];
    lastPage->clearPage();
    
    //keep track of pages
    int *offsets = new int[end - begin + 1];
    memset( offsets, 0, ( end - begin + 1 ) * sizeof(int) );

    while ( true ) {       //when the pages have not been read
        int min = -1;    //find the minimize of the first entry of pages
        bool first = true;
        
        for ( int i = begin ; i <= end; i++ ) {
            if ( offsets[ i - begin ] < pages[i].getOffset() ) {   //if this page is not been read at end

                if ( first ) {                  //init the min
                    first = false;
                    min = i;
                } else if( compare( pages[i].getData() + offsets[i-begin],
                                   pages[min].getData() + offsets[min-begin] ) < 0 ) {
                    min = i;
                }
            }
        }   //for loop end, min save the value of the pageIndex of min

        while ( !lastPage->insertDataToPage( pages[min].getData() + offsets[min-begin], sizeof(int)+sizeof(int) ) ) {  //insert the min value to the output page
            //if insert fail
            //write page to file
            lastPage->writePageToFile( tempFptr );
            lastPage->clearPage();
        }
        offsets[ min - begin ] += sizeof(int)+sizeof(int);    //next entry
        
        //exam if all pages has been read
        int i;
        for ( i = begin ; i <= end; i++ ) {
            if ( offsets[ i - begin ] < pages[i].getOffset() ) {   //if a page is not at the end
                break;
            }
        }
        
        if ( i == end + 1 ) {     //if all pages has been read, break the loop
            break;
        }
    }
    //write remain page
    lastPage->writePageToFile( tempFptr );
    
    clearPages( begin, end + 1 );
}

void pagesManager::megreFilesToFile( FILE **tempFileArray, int tempFileCount, FILE *outputFptr ) {
    
    //read pages from files
    for ( int i = 0; i < tempFileCount; i++ ) {
        readFileToPageAtIndex( tempFileArray[i], i );
    }
    
    //last page for output
    Page *outputPage = &pages[tempFileCount];
    outputPage->clearPage();
    
    int readCount = 0;  //count the number of files have been read
    
    int *offsets = new int[tempFileCount];  //keep track of pages
    memset( offsets, 0, tempFileCount * sizeof(int) );

    bool *fileHaveRead = new bool[tempFileCount];
    memset( fileHaveRead, false, tempFileCount );
    
    int min = -1;
    int entrySize = sizeof(int) + sizeof(int);
    while ( readCount < tempFileCount ) {
        /* megre pages */
        //find the minimize of the pages
        min = -1;
        bool first = true;
        
        for ( int i = 0; i < tempFileCount; i++ ) {
            if ( offsets[i] >= pages[i].getOffset() && fileHaveRead[i] ) {
                //if this page is end and the file is end
                //it can not be the minimize of the pages
                continue;
                
            }
            if ( first ) {
                first = false;
                min = i;
            } else if ( compare( pages[i].getData() + offsets[i],
                                pages[min].getData() + offsets[min] ) < 0 ) {
                min = i;
            }
        }
        
        //isert the minimize to output page
        while ( !outputPage->insertDataToPage( pages[min].getData() + offsets[min], entrySize ) ) {
            outputPage->writePageToFile( outputFptr );
            outputPage->clearPage();
        }
        offsets[min] += entrySize;
        
        //if pages[min] have been read
        if ( offsets[min] >= pages[min].getOffset() ) {
            //read next page
            readFileToPageAtIndex( tempFileArray[min], min );
            offsets[min] = 0;
            if ( feof( tempFileArray[min] ) ) {     //if file have been read
                readCount++;
                fileHaveRead[min] = true;
            }
        }
    }
    //write remain pages
    for ( int i = 0; i < tempFileCount; i++ ) {
        while ( offsets[i] < pages[i].getOffset() ) {
            while ( !outputPage->insertDataToPage( pages[i].getData() + offsets[i], entrySize ) ) {
                outputPage->writePageToFile( outputFptr );
                outputPage->clearPage();
            }
            offsets[i] += entrySize;

        }
    }
    outputPage->writePageToFile( outputFptr );
    outputPage->clearPage();
    delete [] offsets;
    delete [] fileHaveRead;
    
}



int pagesManager::compare(const void * a, const void * b)
{
    a = (char*)a + sizeof(int);
    b = (char*)b + sizeof(int);
    int entryA = *(int*)a;
    int entryB = *(int*)b;
    return ( entryA - entryB );
}
