/* 
 * File:   delayed_average.h
 * Author: d.courteville
 *
 * Created on 6 décembre 2018, 09:43
 */

#ifndef DELAYED_AVERAGE_H
#define	DELAYED_AVERAGE_H

/// The number of measure slot to allocate in the DelayedMovingAverage structure
#define MAX_SAMPLE_NB 500

typedef struct DelayedMovingAverage{
    /// The number of measures to average
    int size;
    /// The number of measures the average is delayed of
    int delay;
    /// The inverse of the size (for optimisation)
    float invSize;
    /// The index of the delay FIFO start
    int delayStart;
    /// The index of the delay FIFO end
    int delayEnd;
    /// The result of the delaed moving average
    float delayedAverage[3];
    /// The array where measures are stored
    float sampleArray[3*MAX_SAMPLE_NB];
} DelayedMovingAverage;

/**
 * @brief Initialise de delayed average structure
 * 
 * @param toInit A pointer to the DelayedMovingAverage to initialize
 * @param size The number of sample in the average
 * @param delay The number of sample the average is delayed for.
 */
void initDelayMovingAverage(DelayedMovingAverage* toInit, int size, int delay);

/**
 * @brief Update the delayed average structure
 * 
 * The measure array is put in the delay FIFO, the older measure of the delay FIFO
 * is put in the average, and the older measure of the average is deleted
 * 
 * @param toUpdate A pointer to the DelayedMovingAverage to update.
 * @param measure The measure to put in the delayed average
 */
void updateAverage(DelayedMovingAverage* toUpdate, float measure[3]);

#endif	/* DELAYED_AVERAGE_H */

