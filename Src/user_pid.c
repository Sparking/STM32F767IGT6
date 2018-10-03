#include <math.h>
#include "user_pid.h"

void PID_Init(PID_ControllerTypeDef *pid, float Kp, float Ki, float Kd,
        float target, float limit_down, float limit_up)
{
        pid->Kp = Kp;
        pid->Ki = Ki;
        pid->Kd = Kd;
        pid->e[0] = 0.0F;
        pid->e[1] = 0.0F;
        pid->e[2] = 0.0F;
        pid->esum = 0.0F;
        pid->limit[0] = limit_down;
        pid->limit[1] = limit_up;
        pid->target = target;
}

float PID_Out(PID_ControllerTypeDef *pid, float current_value,
        float integral_cancel_value, float last_pid_out)
{
        unsigned char i = 0;
        float current_e = pid->target - current_value;
        float _pid_out;
        pid->e[2] = pid->e[1];
        pid->e[1] = pid->e[0];
        pid->e[0] = current_e;

        /* 积分分离 */
        if (fabs(pid->e[0]) < integral_cancel_value) {
                i = 1;
        }
        if ((last_pid_out >= pid->limit[1] && pid->e[0] < 0.0F) ||
                (last_pid_out <= pid->limit[0] && pid->e[1] > 0.0f))
                i = 0;
        _pid_out = pid->Kp * (pid->e[0] - pid->e[1]) + i * pid->Ki * pid->e[0]
                + pid->Kd * (pid->e[0] - 2 * pid->e[1] + pid->e[2]) + last_pid_out;
        if (_pid_out > pid->limit[1])
                _pid_out = pid->limit[1];
        else if (_pid_out < pid->limit[0])
                _pid_out = pid->limit[0];
        return _pid_out;        
}
