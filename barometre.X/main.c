/* 
 * File:   main.c
 * Author: alexis
 *
 * Created on 3 avril 2019, 23:21
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "timing.h"
#include "barometre.h"
#include "communication_I2C.h"
/*
 * 
 */
int main(int argc, char** argv) {
    init_I2C1();
    return (EXIT_SUCCESS);
}

