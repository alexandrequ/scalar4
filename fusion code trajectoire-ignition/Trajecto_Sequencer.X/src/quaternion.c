#include <math.h>
#include "quaternion.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

void vect_prod(float result[3], float v1[3], float v2[3]){
    float res[3];
    
    res[0]=v1[1]*v2[2]-v1[2]*v2[1];
    res[1]=v1[2]*v2[0]-v1[0]*v2[2];
    res[2]=v1[0]*v2[1]-v1[1]*v2[0];
    
    result[0]=res[0];
    result[1]=res[1];
    result[2]=res[2];
}

void normalize(float q[4]){
    float n = sqrtf(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
    q[0] /= n;
    q[1] /= n;
    q[2] /= n;
    q[3] /= n;
}

void initial_quaternion(float q[4], float u[3]){
    float norm_u = sqrtf(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);
    float phi = asinf(u[2]/norm_u);
    float c = cosf(phi);
    float x = u[0]/(norm_u*c);
    float y =-u[1]/(norm_u*c);
    float psi = atan2f(y,x);
    
    float c2 = cosf(phi*0.5), c3 = cosf(psi*0.5);
    float s2 = sinf(phi*0.5), s3 = sinf(psi*0.5);
    
    q[0] = c2*c3;
    q[1] = s2*s3;
    q[2] = s2*c3;
    q[3] = c2*s3;
}

void angle_to_quaternion(float quat[4],float tx,float ty,float tz){
    float theta = sqrtf(tx*tx+ty*ty+tz*tz);
    float inv_theta = 1.0/theta;
    
    quat[0]=cosf(theta*0.5);
    quat[1]=tx*inv_theta*sinf(theta*0.5);
    quat[2]=ty*inv_theta*sinf(theta*0.5);
    quat[3]=tz*inv_theta*sinf(theta*0.5);
}

void quaternion_prod(float result[4], float q1[4], float q2[4]){
    float res[4];
    
    res[0]=q1[0]*q2[0]-q1[1]*q2[1]-q1[2]*q2[2]-q1[3]*q2[3];
    res[1]=q1[1]*q2[0]+q1[0]*q2[1]+q1[2]*q2[3]-q1[3]*q2[2];
    res[2]=q1[0]*q2[2]-q1[1]*q2[3]+q1[2]*q2[0]+q1[3]*q2[1];
    res[3]=q1[0]*q2[3]+q1[1]*q2[2]-q1[2]*q2[1]+q1[3]*q2[0];
    
    result[0]=res[0];
    result[1]=res[1];
    result[2]=res[2];
    result[3]=res[3];
}

void quaternion_conjugate(float result[4], float q[4]){
    result[0]=q[0];
    result[1]=-q[1];
    result[2]=-q[2];
    result[3]=-q[3];
}

void rotate_vect_with_quat(float result[3], float v[3], float q[4]){
    float qv[4]={0,v[0],v[1],v[2]}; // quaternion issu de v
    float cq[4]={q[0],-q[1],-q[2],-q[3]}; // conjugé de q
    
    // q*qv*cq
    quaternion_prod(qv,qv,cq);
    quaternion_prod(qv,q,qv);
    
    result[0]=qv[1];
    result[1]=qv[2];
    result[2]=qv[3];
}

void quat_to_euler(float euler[2], float q[4]){
    
    euler[0] = atan2f(-2*(q[2]*q[3]-q[0]*q[1]) , q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3] );
    
    float sina = 2*(q[3]*q[1]+q[2]*q[0]);
    if(sina >= 1.0)
        euler[1] = M_PI/2;
    else if(sina <= -1.0)
        euler[1] = - M_PI/2;
    else
        euler[1] = asinf(sina);
    
    /* Not used : bank
    euler[2] = atan2f(-2*(q[1]*q[2]-q[0]*q[3]) , q[0]*q[0]+q[1]*q[1]-q[2]*q[2]-q[3]*q[3]);
    printf("euler: %f\t%f\t%f\n",euler[0]/PI*180,euler[1]/PI*180,euler[2]/PI*180);
    */
}