#include <stdint.h>

#include "protocol.h"
#include "solver.h"
#include <math.h>

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



void ode_func(float tn, float* x, float* dx, float* u, uint8_t xdim)
{
    // Motor are modeled as first order system 
    // Motor 1
    #ifdef MOTOR_USE_FIRST_ORDER_MODEL
        dx[13] = km * (u[0] - x[13]); 
        // Motor 2
        dx[14] = km * (u[1] - x[14]); 
        // Motor 3
        dx[15] = km * (u[2] - x[15]); 
        // Motor 4
        dx[16] = km * (u[3] - x[16]); 
    #else 
        //TODO : Implement BLDC motor model
    #endif

    double F1, F2, F3, F4;
    F1 = (float) (kF * x[13] * x[13] );
    F2 = (float) (kF * x[14] * x[14] );
    F3 = (float) (kF * x[15] * x[15] );
    F4 = (float) (kF * x[16] * x[16] );

    //Torque vectors
    float T, Tx, Ty, Tz;
    T = F1 + F2 + F3 + F4;
    Tx = L * (F2 - F4);
    Ty = L * (F1 - F3);
    Tz = gamma * (F1 - F2 + F3 - F4);

    //Position x,y,z
    dx[0] = x[3];
    dx[1] = x[4];
    dx[2] = x[5];

    // Velocities vx, vy, vz
    dx[3] = 2 * T / m * (x[7]*x[9] - x[6] * x[8]);
    dx[4] = 2 * T / m * (x[8]*x[9] + x[6] * x[7]);
    dx[5] = T / m * (x[6]*x[6] - x[7]*x[7] -x[8]*x[8] + x[9]*x[9]) - g;
    // Quaternion
    dx[6] = 0.5 * (-x[10] * x[7] - x[11] * x[8] - x[12] * x[9]);
    dx[7] = 0.5 * (x[10] * x[6] + x[12] * x[8] - x[11] * x[9]);
    dx[8] = 0.5 * (x[11] * x[6] - x[12] * x[7] + x[10] * x[9]);
    dx[9] = 0.5 * (x[12] * x[6] + x[11] * x[7] - x[10] * x[8]);

    //Angular velocities
    dx[10] =  1.0/Ix *(Tx - x[11]*x[12] * (Iz - Iy));
    dx[11] =  1.0/Iy *(Ty - x[10]*x[12] * (Iz - Ix));
    dx[12] =  1.0/Iz *(Tz - x[11]*x[10] * (Iy - Ix));


}

void solve_step(State_t* state, State_raw_t* state_raw, MotorCommand_t* m_cmd)
{
    float x_[STATE_SIZE];
    float dx[STATE_SIZE];
    float u[4];

    uint8_t xdim = sizeof(x_)/sizeof(x_[0]);
    uint8_t udim = 4; 
    for(int i = 0 ; i < xdim; i++) x_[i] = state_raw->x[i];


    for (int i = 0 ; i < udim; i++)
    {
        float cmd = (float)m_cmd->motor_cmd[i];
        if (cmd < MOTOR_MIN_COMMAND)
        {
            u[i] = 0.0f;
        }
        else
        {
            u[i] = ((float)(cmd - MOTOR_MIN_COMMAND) / (float)(MOTOR_MAX_COMMAND - MOTOR_MIN_COMMAND))* MOTOR_MAX_RPM;
        }
        
    }


    // Integrate one timestep
    ct_fode4_step(ode_func, 0.0, dx, x_, xdim, u, TIMESTEP);

    //Normalize quaterinon
    float qnorm = sqrtf(x_[6]*x_[6] + x_[7]*x_[7] + x_[8]*x_[8] + x_[9]*x_[9]);
    if (qnorm > 1e-6f) {
        x_[6] /= qnorm;
        x_[7] /= qnorm;
        x_[8] /= qnorm;
        x_[9] /= qnorm;
    } 
    if (x_[2] <= 0.0) x_[2] = 0.0;
    if (x_[2] <= 0.0 && x_[5] <= 0.0) x_[5] = 0.0; 

    for(int i = 0 ; i < xdim; i++) state_raw->x[i] = x_[i];

    // Send accelerometer data and gyro data to controller
    // if SENSOR_X_USE_BASIC_MODEL is defined uses accel and gyro data without any noise  
    // State is already a pointer to state
    #ifdef SENSOR_ACCEL_USE_BASIC_MODEL
        state->accel[0] = LOOP_HZ * dx[3]; 
        state->accel[1] = LOOP_HZ * dx[4]; 
        state->accel[2] = LOOP_HZ * dx[5]; 
    #else 
        sensor_accel_model(state, dx);
    #endif

    #ifdef SENSOR_GYRO_USE_BASIC_MODEL
        state->gyro[0] = LOOP_HZ * dx[10];
        state->gyro[1] = LOOP_HZ * dx[11];
        state->gyro[2] = LOOP_HZ * dx[12];
    #else
        sensor_gyro_model(state, dx);
    #endif
}

// NOT IMPLEMENTED YET!!!
void sensor_accel_model(State_t* state, const float* dx)
{
    state->accel[0] = dx[3]; 
    state->accel[1] = dx[4]; 
    state->accel[2] = dx[5]; 
}
void sensor_gyro_model(State_t* state, const float* dx)
{
    state->gyro[0] = dx[10];
    state->gyro[1] = dx[11];
    state->gyro[2] = dx[12];
}

