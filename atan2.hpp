// Adapted from the work of Francesco Mazzoli, under the ISC License
#include <immintrin.h>

inline float atan_scalar_approximation(float x) {
    float a1  =  0.99997726f;
    float a3  = -0.33262347f;
    float a5  =  0.19354346f;
    float a7  = -0.11643287f;
    float a9  =  0.05265332f;
    float a11 = -0.01172120f;
  
    float x_sq = x*x;
    return
      x * (a1 + x_sq * (a3 + x_sq * (a5 + x_sq * (a7 + x_sq * (a9 + x_sq * a11)))));
}

template<class F>
F fast_atan2( F y , F x){
    bool swap = fabs(x) < fabs(y);
    float atan_input = (swap ? x : y) / (swap ? y : x);

    // Approximate atan
    float res = atan_scalar_approximation(atan_input);

    // If swapped, adjust atan output
    res = swap ? (atan_input >= 0.0f ? M_PI_2 : -M_PI_2) - res : res;
    // Adjust quadrants
    if      (x >= 0.0f && y >= 0.0f) {}                     // 1st quadrant
    else if (x <  0.0f && y >= 0.0f) { res =  M_PI + res; } // 2nd quadrant
    else if (x <  0.0f && y <  0.0f) { res = -M_PI + res; } // 3rd quadrant
    else if (x >= 0.0f && y <  0.0f) {}
    
    return res;

}

   

