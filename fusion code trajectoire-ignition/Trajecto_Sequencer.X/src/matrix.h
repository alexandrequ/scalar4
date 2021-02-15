/**
 * @file   matrix.h
 *  
 * @author Damien Courteville
 *
 */

#ifndef MATRIX_H
#define	MATRIX_H
/**
 * @brief Initialise 'matrix' en une matrice de rotation d'angle 'angle' 
 * selon l'axe x.
 * 
 * @param matrix La matrice à initialiser.
 * @param angle L'angle de rotation.
 */
void x_rotation_matrix(float matrix[3][3],float angle);

/**
 * @brief Initialise 'matrix' en une matrice de rotation d'angle 'angle' 
 * selon l'axe y.
 * 
 * @param matrix La matrice à initialiser.
 * @param angle L'angle de rotation.
 */
void y_rotation_matrix(float matrix[3][3],float angle);

/**
 * @brief Initialise 'matrix' en une matrice de rotation d'angle 'angle' 
 * selon l'axe y.
 * 
 * @param matrix La matrice à initialiser.
 * @param angle L'angle de rotation.
 */
void z_rotation_matrix(float matrix[3][3],float angle);

/**
 * @brief Do the matrix product result=m1*m2
 * @param result The matrix that receive the result
 * @param m1 The first matrix
 * @param m2 The second matrix
 */
void matrix_prod(float result[3][3], float m1[3][3], float m2[3][3]);

/**
 * @brief Do a matrix transposition
 * @param result The matrix that will receive the transposition
 * @param matrix The matrix to transpose
 */
void transpose(float result[3][3], float matrix[3][3]);

/**
 * @brief Compute the quaternion associated to the rotation matrix
 * @param quat The vector that will receive te quaternion
 * @param mat The rotation matrix
 * 
 * @warning The calculation become unstable if the trace of 'mat' is close to '-1'
 */
void compute_quaternion(float quat[4], float mat[3][3]);

/**
 * @brief Do the matrix product 'result=mat*vect'
 * @param result The vector that will receive the result
 * @param mat
 * @param vect
 */
void matrix_vector_prod(float result[3], float mat[3][3], float vect[3]);

#endif	/* MATRIX_H */

