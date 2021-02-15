/* 
 * File:   main.c
 * Author: alexis
 *
 * Created on 27 janvier 2020, 23:32
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/*
 * 
 */
int main(int argc, char** argv) {
    _TRISB0=1;
    _TRISB1=0;
    _LATB0=0;
    if(_RB1==1){
        _LATB0=1;
    }
    
    while(1){
        
    }
    
    return (EXIT_SUCCESS);
}

