/* 
 * File:   quaternion.h
 * Author: Damien Courteville
 *
 * Created on 17 juin 2018, 17:14
 */

#ifndef QUATERNION_H
#define	QUATERNION_H

void vect_prod(float result[3], float v1[3], float v2[3]);

void normalize(float q[4]);

/**
 * Compute a quaternion which represent the rotation that tranform the terrestrial reference frame
 * to the rocket reference frame. Does not take into account the azimutal rotation.
 * - x point upward
 * - z is the projection of of Xsens z axis on the horizontal plane
 * - y complete the frame to have a direct sytem
 * @param q The quaternion which will store the result.
 * @param g A vector in the rocket reference frame that point to the UP of the terrestrial reference frame (for example the gravity vector).
 * 
 * @tip The vector g does not need to be unitary.
 * @warning The azimutal angle is ignored.
 * 
 * @see http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
 */
void initial_quaternion(float q[4], float g[3]);

/**
 * @brief Compute the quaternion that correspond to the rotation vector (tx,ty,tz)
 * 
 * This vector correspond to an axis rotation with an angle theta=|(tx,ty,tz)| and
 * a rotation axis of euclidian vector (tx,ty,tz)/theta.
 * This function is used to convert the 3 angular velocity measured by the
 * Inertial measurement unit to a quaternion. However, this function is a good 
 * approximation only for small step, as the unit reference fram rotate with it.
 * 
 * @param quat The quaternion that will receive the result.
 * @param tx The x component of the rotation vector.
 * @param ty The y component of the rotation vector.
 * @param tz The z component of the rotation vector.
 * 
 * @see http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
 */
void angle_to_quaternion(float quat[4],float tx,float ty,float tz);

/**
 * @brief Compute the product of 2 quaternion: result = q1 q2 .
 * 
 * @tip If we apply 2 rotation to an object, if q1 is associated to the first rotation
 * and q2 to the second one, the quaternion associated to the resulting rotation is
 * q = q2 q1
 * 
 * @param result The quaternion that will receive the result.
 * @param q1 The left quaternion.
 * @param q2 The right quaternion.
 * 
 * @see http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
 */
void quaternion_prod(float result[4], float q1[4], float q2[4]);

/**
 * @brief Computes the conjugate of a quaternion
 * 
 * @param result The quaternion that will receive the result.
 * @param q The quaternion to conjugate.
 * 
 * @see http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
 */
void quaternion_conjugate(float result[4], float q[4]);

/**
 * @brief Rotate a vector with a quaternion.
 * 
 * @param result The vector that will receive the rotated vector.
 * @param v The vector to rotate.
 * @param q The quaternion associated with the rotation.
 * 
 * @see http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
 */
void rotate_vect_with_quat(float result[3], float v[3], float q[4]);

/**
 * @brief Compute the heading an the attitude associated to a quaternion.
 * 
 * @param euler The vector that will receive the heading and the attitude.
 * @param q The quaternion
 * 
 * @post The heading is stored in euler[0] and the attitude in euler[1].
 * 
 * @see http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
 */
void quat_to_euler(float euler[2], float q[4]);

#endif	/* QUATERNION_H */

