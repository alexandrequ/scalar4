#include "delayed_average.h"

void initDelayMovingAverage(DelayedMovingAverage* toInit, int size, int delay){
    int i;
    int j;
    
    // Les malloc ne marchent pas si on alloue pas manuellement un stack de memoir à cet effet
    // pendant la compilation
    //toInit = malloc(sizeof(DelayedMovingAverage)+ sizeof(float)*(size+delay)*3);
    toInit->size=size;
    toInit->delay=delay;
    toInit->invSize=1.0/size;
    toInit->delayStart=delay;
    toInit->delayEnd=0;
    
    for(i=0; i<size+delay; i++){
        for(j=0; j<3; j++)
            toInit->sampleArray[3*i+j]=0;
    }
    for(j=0; j<3; j++)
        toInit->delayedAverage[j]=0;
}

void updateAverage(DelayedMovingAverage* toUpdate, float measure[3]){
    
    // Updating the delayed moving average
    int i = 0;
    
    for(i=0; i<3; i++){
        // Substracts the older measure from the average
        toUpdate->delayedAverage[i] -= toUpdate->sampleArray[3*toUpdate->delayStart+i] * toUpdate->invSize;
        // Replace the older measur by the last measure, at the beggining of the delaying FIFO
        toUpdate->sampleArray[3*toUpdate->delayStart+i]=measure[i];
        // Add the older measure of the delaying FIFO to the average 
        toUpdate->delayedAverage[i] += toUpdate->sampleArray[3*toUpdate->delayEnd+i] * toUpdate->invSize;
    }
    
    // On incrémente les indices
    toUpdate->delayStart++;
    toUpdate->delayEnd++;
    
    if(toUpdate->delayStart == toUpdate->size+toUpdate->delay)
        toUpdate->delayStart=0;
    if(toUpdate->delayEnd == toUpdate->size+toUpdate->delay)
        toUpdate->delayEnd=0;
}
