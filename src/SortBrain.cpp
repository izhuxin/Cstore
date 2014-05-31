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
        //read file to pages, and sort these pages and save to temp files
        int tempFileCount = pagesMemorySort( inputFileName );
        //megre these temp files to one file
        mergeSortTempFiles( tempFileCount, outputFileName );
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
        sprintf( fileName, "temp%d.dat", count - 1 );
        FILE *tempFptr = fopen( fileName, "wb" );
        _pageManager->megrePagesToFile( 0, readPagesCount - 1, tempFptr );
        fclose( tempFptr );
        
        //read next pages of data from file
        readPagesCount = _pageManager->readFileToPages( inputFptr, 0, NUMBER_OF_PAGE - 2);
    }
    
    //end while loop, sort last part of pages
    count++;
    _pageManager->sortPages( 0, readPagesCount - 1 );
    
    //megre pages to temp file
    char fileName[15] = {};
    sprintf(fileName, "temp%d.dat", count - 1 );
    FILE *tempFptr = fopen( fileName, "wb" );
    _pageManager->megrePagesToFile( 0, readPagesCount - 1, tempFptr );
   
    //clear pages
    _pageManager->clearPages( 0, NUMBER_OF_PAGE - 1 );
    
    //close the file
    fclose( tempFptr );
    fclose( inputFptr );

    return count;
}

void SortBrain::mergeSortTempFiles( int count, const char* outputFile ) {
    if ( count <= 0 )
        return;
    FILE *outputFptr = fopen( outputFile, "wb" );
    
    //open temp files
    FILE* *fileArray = new FILE *[count];
    for ( int i = 0 ; i < count; i++ ) {
        char fileName[15];
        sprintf( fileName, "temp%d.dat", i );
        fileArray[i] = fopen( fileName, "rb" );
    }

    _pageManager->megreFilesToFile( fileArray, count, outputFptr );
    
    //close file
    fclose( outputFptr );
    
    //remove temp files
    for ( int i = 0 ; i < count; i++ ) {
        fclose( fileArray[i] );

        char fileName[15];
        sprintf( fileName, "temp%d.dat", i );
        remove( fileName );
    }
    
    delete [] fileArray;
}