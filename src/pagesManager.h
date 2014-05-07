#ifndef PAGES_MANAGER_H
#define PAGES_MANAGER_H

#include <cstring> //size_t memcpy
#include <cstdio>

#include "Constant.h"
/**
 *  pagesManager class defination
 */
class pagesManager {
private:
    page *pages;
    int orderKeyIndex;
    int custKeyIndex;
    int totalPirceIndex;
    int priorityIndex;
    /**
     *  sigletron instance
     *  private constructor and destructor
     */
    static pagesManager *_instance;
    int pageIndex;
    pagesManager();
    ~pagesManager();
    
    /**
     *  copy the entry content into pages[index] memory
     *
     *  @param index pages counter
     *  @param entry the pointer to the content to be copied into page
     *
     *  @return the first non-full page index after insert,
     *          return -1 for all pages have been used
     */
    int insertInPagesAtIndex( int index, const void* entry, size_t size );
    bool writePagesToFile( int low, int high, fileName _fileName );
    int searchInPageAtIndex( int index, int key, size_t size );
    int searchInFile( int key, int pageIndex, fileName _fileName );
    static int compare( const void *a, const void *b );
    void quick_sort( char* data, propertyTypeName type, int begin, int end );

public:
    inline void clearPage() {
        pageIndex = 0;
    }
    static pagesManager *sharedManager();
    void insertOrderKey( int orderKey );
    void insertCustKey( int orderKey, int custKey );
    void insertTotalPrice( int orderKey, double totalPrice );
    void insertShipPriority( int orderKey, SHIPPRIORITY prioriy );
    
    bool queryWithOrderKey( char* arrayOutput, int orderKey );
    
    int writeAllPagesToFile();
    
    bool readFileToOnePage( FILE *&fptr );
    bool readPropertyToPages( fileName _fileName, bool restart );
    void sortAllPages( propertyTypeName type, int fileIndex );
    void megrePagesToFile( FILE *fptr );
    double compress();

};

#endif /* defined(__jgC_StoreDB__pagesManager__) */
