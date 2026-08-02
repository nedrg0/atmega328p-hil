#include <stdint.h>

void ct_fode1_step(void (*ode_func) (float, float*, float*,  uint8_t), float tn, float* x, uint8_t xdim, float Ts);         // Fixed-step Contionus (State) Solver - Euler
void ct_fode2_step(void (*ode_func) (float, float*, float*,  uint8_t), float tn, float* x, uint8_t xdim, float Ts);         // Fixed-step Contionus (State) Solver - RK2
void ct_fode4_step(void (*ode_func) (float,  float*, float*, float*, uint8_t), float tn, float* dx, float* x, uint8_t xdim, float* u, float Ts);
/*
    All functions that represent a integrator of some type have the same footprint.

    ct_xxxx_step:

    (*ode_func) -Pointer to the state update function f[k, x(k)]
    tn          -Current time. Used for explictly time-dependant systems i.e. xdot(t) = f[t, x(t)].
    x           -Pointer to the state array (vector).
    xdim        -Dimension of the state array (vector).
    Ts          -Sample time (Sometimes, step size).

    How you should implement the (*ode_func) function :
    void ode_func(float tn, float* x,  float* dx, uint8_t xdim)
    {
        dx[0]           = How should x[0] update.
        .               .
        .               .
        .               .
        dx[xdim - 1]    = How should x[xdim - 1] update.
    }

    ct_qxxx_step : Same implementation but uses Q16.16 representation of a decimal instead of floating point.
*/

#ifdef CT_FODE1_STEP_IMPLEMENTATION

void ct_fode1_step(void (*ode_func) (float, float*, float*,  uint8_t), float tn, float* x, uint8_t xdim, float Ts)
{
    float dx[xdim];
    (*ode_func) (tn, x, dx, xdim);

    for (int i = 0; i < xdim; i++) x[i] += Ts * dx[i];
};
#endif //CT_FODE1_STEP_IMPLEMENTATION


#ifdef CT_FODE2_STEP_IMPLEMENTATION

void ct_fode2_step(void (*ode_func) (float, float*, float*, uint8_t), float tn, float* x, uint8_t xdim, float Ts)
{
    // Compute a single Euler "step"
    float dx[xdim];
    (*ode_func) (tn, x, dx, xdim); // Calc dx

    float xk1[xdim];
    
    for (int i =0; i < xdim; i++) xk1[i] = x[i] +  Ts/2 * dx[i];
    (*ode_func) (tn + Ts/2, xk1, dx, xdim); // Compute half-step in dx direction

    for (int i =0; i < xdim; i++) x[i] += Ts * dx[i]; 

}

#endif //CT_FODE2_STEP_IMPLEMENTATION

//TODO: 
// - No need for 4 different arrays, use one add as calculated
// - For SOLVE use a options struct ? 
// - Add support for control vector u(t). It makes the most sense for this to be a function i.e. u(t, x) or something like that. 

#ifdef CT_FODE4_STEP_IMPLEMENTATION

void ct_fode4_step(void (*ode_func) (float,  float*, float*, float* , uint8_t), float tn, float* dx, float* x, uint8_t xdim, float* u, float Ts)
{
    float k1[xdim];
    (*ode_func) (tn, x, k1, u, xdim);  // Compute full step

    float temp[xdim];

    for (int i =0; i < xdim; i++) temp[i] = x[i] + Ts/2 * k1[i]; 

    float k2[xdim];
    (*ode_func) (tn + Ts/2,temp, k2, u, xdim); // COmpute half-step in k1 direction

    for (int i = 0; i < xdim; i++) temp[i] = x[i] + Ts/2 * k2[i];

    float k3[xdim];
    (*ode_func) (tn + Ts/2, temp, k3, u, xdim); // Half step in k2 direction

    for (int i = 0; i < xdim; i++) temp[i] = x[i] + Ts * k3[i];

    float k4[xdim];
    (*ode_func) (tn + Ts, temp, k4, u, xdim); // Full step in k3 direction

    for (int i =0; i < xdim; i++) x[i] += Ts/6 * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]); // Actual step
    for (int i =0; i < xdim; i++) dx[i] = Ts/6 * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
  
}

#endif //CT_FODE4_STEP_IMPLEMENTATION

