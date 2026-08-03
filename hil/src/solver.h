#ifndef SOLVER_H
#define SOLVER_H

#define LOOP_HZ 10000
#define TIMESTEP ((1.0 / LOOP_HZ)) 

// Parameters for simulation 
#define m (0.5)
#define g (9.81)
#define Ix (2.32 * 10e-3)
#define Iy (2.32 * 10e-3)
#define Iz (4.00 * 10e-3)
#define L (0.175)
#define gamma (0.02454)
#define kF (double)(6.11 * 1e-8)
#define kM (double)(1.5 * 1e-9)
#define km (double)(20)

#define RAD_TO_RPM (60 / (2.0 * 3.14 ))

// Define state size
#define STATE_SIZE (13 + 4 * 1)
typedef struct 
{
    float x[STATE_SIZE];
}State_raw_t;

void ct_fode1_step(void (*ode_func) (float,  float*, float*, float* , uint8_t), float tn, float* dx, float* x, uint8_t xdim, float* u, float Ts);
void ct_fode4_step(void (*ode_func) (float,  float*, float*, float*, uint8_t), float tn, float* dx, float* x, uint8_t xdim, float* u, float Ts);

void solve_step(State_t* state, State_raw_t* state_raw, MotorCommand_t* m_cmd);

// Comment out to enable the usage of a complex model for both sensor 
// Complex models include noise and bias
#define SENSOR_ACCEL_USE_BASIC_MODEL 
#define SENSOR_GYRO_USE_BASIC_MODEL 

#define MOTOR_MAX_RPM (6000U)


void sensor_accel_model(State_t* state, const float* dx);
void sensor_gyro_model(State_t* state, const float* dx);

#endif // SOLVER_H