//
//  SortBrain.cpp
//  CStore
//
//  Created by Jeason on 14-5-26.
//  Copyright (c) 2014年 Jeason. All rights reserved.
//

#include "SortBrain.h"

SortBrain::SortBrain() {
    _pageManager = pagesManager::sharedManager();
}

SortBrain::~SortBrain() {
    
}

void SortBrain::externSort( const char *inputFileName, const char* outputFileName ) {
        //read file to pages, and sort it to temp files
        int tempFileCount = pagesMemorySort( inputFileName );
        //megre these temp files to one file
        mergeSortTempFiles( tempFileCount, outputFileName );
        _pageManager->clearPages( 0, NUMBER_OF_PAGE - 1 );
}

int SortBrain::pagesMemorySort( const char* inputFile ) {
    //temp file counter
    int count = 0;
    //read pages of data from file
    FILE *inputFptr = fopen( inputFile, "rb" );
    int readPagesCount = _pageManager->readFileToPages( inputFptr, 0, NUMBER_OF_PAGE - 2 );
    while ( readPagesCount == NUMBER_OF_PAGE - 1 ) {      //means end of file
        count++;
        //sort pages
        _pageManager->sortPages( 0, readPagesCount - 1 );
        
        //megre pages to temp file
        char fileName[15] = {};
        sprintf(fileName, "temp%d.dat", count - 1 );
        FILE *tempFptr = fopen( fileName, "wb" );
        _pageManager->megrePagesToFile( 0, readPagesCount - 1, tempFptr );
        fclose( tempFptr );
        
        //read next pages of data from file
        readPagesCount = _pageManager->readFileToPages( inputFptr, 0, NUMBER_OF_PAGE - 2);
    }
    
    //end while loop, sort last part of pages
    count++;
    _pageManager->sortPages( 0, readPagesCount - 1 );
    
    char fileName[15] = {};
    sprintf(fileName, "temp%d.dat", count - 1 );
    FILE *tempFptr = fopen( fileName, "wb" );
    _pageManager->megrePagesToFile( 0, readPagesCount - 1, tempFptr );
    _pageManager->clearPages( 0, NUMBER_OF_PAGE - 1 );
    fclose( tempFptr );
    
    fclose( inputFptr );
    return count;
}

void SortBrain::mergeSortTempFiles(int count, const char* outputFile) {
    if ( count <= 0 )
        return;
    
    //file pointers
    FILE *outputFptr = fopen( outputFile, "wb" );
    
    //create temp file
    FILE* *fileArray = new FILE *[count];
    for ( int i = 0 ; i < count; i++ ) {
        char fileName[15];
        sprintf( fileName, "temp%d.dat", i );
        fileArray[i] = fopen( fileName, "rb" );
    }
    
    //helper variables
    bool *fileHaveRead = new bool[count];
    memset( fileHaveRead, false, count );
    int readCount = 0;
    
    while ( readCount < count ) {   //while not all files are at the end of file
        int pageIndex = 0;
        for ( int i = 0 ; i < count; i++ ) {    //read a page of data from file
            if ( fileHaveRead[i] )
                continue;
            
            _pageManager->readFileToPageAtIndex( fileArray[i], pageIndex );
            pageIndex++;
            if ( feof(fileArray[i]) ) { //if file[i] have been read
                fileHaveRead[i] = true; //mark it
                fclose( fileArray[i] );   //close temp file
                readCount++;
            }
            
        }
        
        //megre pages to file
        _pageManager->megrePagesToFile( 0, pageIndex, outputFptr );
    }
    
    //close file
    fclose( outputFptr );
    
    //remove temp files
    for ( int i = 0 ; i < count; i++ ) {
        char fileName[15];
        sprintf( fileName, "temp%d.dat", i );
        remove( fileName );
    }

    delete [] fileArray;
    //_pageManager->test();
}
