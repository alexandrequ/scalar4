#ifndef MACROS_H
#define MACROS_H

//mettre a un un bit
#define  set_bit(register_name,bit_position) register_name|(1<<bit_position)
//mettre a 0 un bit
#define clear_bit(register_name,bit_position) register_name &~(1<<bit_position)
//inverser un bit
#define toogle_bit(register_name,bit_position) register_name^(1<<bit_position)
//valeur d'un bit
#define test_bit(register_name,bit_position) (register_name&(1<<bit_position))>>bit_position
//lire un champ de bit
#define read_field(register_name,start_position,size_field) (register_name&(((1<<size_field)-1)<<start_position))>>start_position
//ecrire dans un champ de bit
#define write_field(register_name,start_position,size_field,field_value)((register_name&~(((1<<size_field)-1)<<start_position))|(field_value<<start_position))  


#endif

