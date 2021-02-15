#include <math.h>
#include "matrix.h"
void x_rotation_matrix(float matrix[3][3],float angle){
    matrix[0][0]=1;
    matrix[0][1]=0;
    matrix[0][2]=0;
    matrix[1][0]=0;
    matrix[1][1]=cos(angle);
    matrix[1][2]=-sin(angle);
    matrix[2][0]=0;
    matrix[2][1]=sin(angle);
    matrix[2][2]=cos(angle);
}

void y_rotation_matrix(float matrix[3][3],float angle){
    matrix[0][0]=cos(angle);
    matrix[0][1]=0;
    matrix[0][2]=sin(angle);
    matrix[1][0]=0;
    matrix[1][1]=1;
    matrix[1][2]=0;
    matrix[2][0]=-sin(angle);
    matrix[2][1]=0;
    matrix[2][2]=cos(angle);
}

void z_rotation_matrix(float matrix[3][3],float angle){
    matrix[0][0]=cos(angle);
    matrix[0][1]=-sin(angle);
    matrix[0][2]=0;
    matrix[1][0]=sin(angle);
    matrix[1][1]=cos(angle);
    matrix[1][2]=0;
    matrix[2][0]=0;
    matrix[2][1]=0;
    matrix[2][2]=1;
}

void matrix_prod(float result[3][3], float m1[3][3], float m2[3][3]){
    
    float r[3][3]={};
    int i,j,k;
    
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            for(k=0; k<3; k++){
                r[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            result[i][j]=r[i][j];
        }
    }
}

void transpose(float result[3][3], float matrix[3][3]){
    
    int i,j;
    
    for(i=0 ; i<3; i++){
        for(j=0 ; j<3; j++){
            result[i][j]=matrix[j][i];
        }
    }
}

void compute_quaternion(float quat[4], float mat[3][3]){
    
    float r = 0.5*sqrt(1.0 + mat[0][0] + mat[1][1] + mat[2][2]);
    
    quat[0]=r;
    quat[1]= (mat[3][2] - mat[2][3])/(4.0*r);
    quat[2]= (mat[1][3] - mat[3][1])/(4.0*r);
    quat[3]= (mat[2][1] - mat[1][2])/(4.0*r);
}

void matrix_vector_prod(float result[3], float mat[3][3], float vect[3]){
    float res[3] = {};
    int i,j;
    
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            res[i] += mat[i][j]*vect[j];
        }
    }
    
    for(i=0; i<3; i++){
        result[i]=res[i];
    }
}