#ifndef PAGES_MANAGER_H
#define PAGES_MANAGER_H

#include <cstring> //size_t memcpy
#include <cstdio>
#include "Page.h"

#include "Constant.h"
/**
 *  pagesManager class defination
 */
class pagesManager {
private:
    Page *pages;
    /**
     *  sigletron instance
     *  private constructor and destructor
     */
    static pagesManager *_instance;
    pagesManager();
    ~pagesManager();
    
    //helper function for qsort
    static int compare( const void *a, const void *b );

public:
    static pagesManager *sharedManager();
    
    //operations on one page
    //just simple to call functions in Page
    void writePageAtIndexToFile( size_t index, FILE* fptr );
    
    void clearPageAtIndex( size_t index );
    
    void readFileToPageAtIndex( FILE *fptr, int index );
    
    bool searchKeyInPageAtIndex( const char* key, size_t keySize, char *entryOutput, size_t entrySize, int index );
    
    //operations on pages
    bool insertData( char *data, size_t _size, size_t pageCondition );

    int readFileToPages( FILE* fptr, int begin, int end );
    
    void clearPages( int begin, int end );
    
    //extern sort helper functions
    void sortPages( int begin, int end );
    
    void megrePagesToFile( int begin, int end, FILE* tempFptr );
    
    void compressPagesToFile( FILE *fptr, int begin, int end );

};

#endif /* defined(__jgC_StoreDB__pagesManager__) */
