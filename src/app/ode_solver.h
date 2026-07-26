#include "fixed_point.h"

void ct_qode2_step(void (*f)(q16_16*, q16_16*), q16_16* x, uint8_t xdim, q16_16 Ts)
{
    q16_16 k1[xdim], k2[xdim], x_temp[xdim];

    f(x, k1);
    for (int i = 0; i < xdim; i++)
        x_temp[i] = q_add(x[i], q_mul(Ts, k1[i]));   // Euler predictor

    f(x_temp, k2);
    for (int i = 0; i < xdim; i++)
        x[i] = q_add(x[i], q_mul(q_div(Ts, itoq(2)), q_add(k1[i], k2[i])));  // average slope
}