//
// Created by shuliang on 19-7-18.
//

#ifndef HARMONICCOORDINATEDEMO_KERNEL_CUH
#define HARMONICCOORDINATEDEMO_KERNEL_CUH

struct uchar4;
typedef struct{
    int x,y;
    float rad;
    int chamfer;
    float t_s,t_a,t_g;
}BC;

void kernelLauncher(uchar4 *d_out,float *d_temp,int w,int h,BC bc);

void resetTemperature(float *d_temp,int w,int h,BC bc);

#endif //HARMONICCOORDINATEDEMO_KERNEL_CUH
