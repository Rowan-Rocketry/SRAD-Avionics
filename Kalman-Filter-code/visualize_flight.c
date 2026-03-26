#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    float estimate;
    float gain;
    float est_error;
    float measure_error;
} KalmanFilter;

KalmanFilter kf;

/* Initialize filter */
void kalman_init(float initial_estimate, float est_error, float measure_error)
{
    kf.estimate = initial_estimate;
    kf.est_error = est_error;
    kf.measure_error = measure_error;
    kf.gain = 0.0;
}

/* Calculate Kalman Gain */
void calculate_kalman_gain()
{
    kf.gain = kf.est_error / (kf.est_error + kf.measure_error);
}

/* Update estimate */
void update_estimate(float sensor_value)
{
    kf.estimate = kf.estimate + kf.gain * (sensor_value - kf.estimate);
}

/* Update estimate error */
void calculate_estimate_error()
{
    kf.est_error = (1.0 - kf.gain) * kf.est_error;
}

/* One Kalman iteration */
float kalman_update(float measurement)
{
    calculate_kalman_gain();
    update_estimate(measurement);
    calculate_estimate_error();
    return kf.estimate;
}

int main()
{
    srand(time(NULL));

    kalman_init(
        0.0,   // initial estimate
        1.0,   // estimate error
        10.0   // measurement error
    );

    for(int i = 0; i < 10; i++)
    {
        /* Simulated sensor measurement */
        float sensor_altitude = 60 + (rand() % 21);  // random between 60 and 80

        float filtered = kalman_update(sensor_altitude);

        printf("Sensor: %.2f  Estimate: %.2f\n", sensor_altitude, filtered);
    }

    return 0;
}