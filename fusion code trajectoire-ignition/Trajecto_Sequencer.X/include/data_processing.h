/*
 * File:   data_processing.h
 * Author: Damien Courteville
 *
 */

#ifndef MEASURE_H
#define	MEASURE_H

#include <stdint.h>

/**
 * @brief D�code les donn�es envoy�es par la Xsens, les stock et les envoi sur le CAN.
 *
 * Lis le "*Xbus_receive_buf" et remplis la structure "measure" avec les mesures
 * au format float.
 * \n - Si xbus_receive_buff->MID est diff�rent de 0x36, la fonction ne fait rien
 * \n - Si une mesure n'est pas pr�sente dans le message de la Xsens,
 * la variable correspondante n'est pas modifi�e.
 *
 * @param id ID du message envoiy� au CAN
 *
 * @post la structure 'acquisition_data' contient les donn�es contenues dans le
 * message de la Xsens. This structure is defined as an external variable in
 * the file trajecto.h.
 */
void read_xbus_data();


#endif	/* MEASURE_H */
