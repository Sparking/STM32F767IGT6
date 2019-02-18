#ifndef _USER_PID_H_
#define _USER_PID_H_

typedef struct pid_controller_type {
        float Ki;
        float Kp;
        float Kd;
        float e[3]; /* e[0]:e(k) e[1]:e(k-1) e[2]:e(k-2) */
        float esum; /* 累计误差 */
        float limit[2];
        float target;
} PID_ControllerTypeDef;

/* PID_Out
 * @current_e:当前值
 * @civ: 积分分离值，当误差大于该值时，切除积分作用
 */
float PID_Out(PID_ControllerTypeDef *pid, float current, float civ,
        float last_pid_out);
void PID_Init(PID_ControllerTypeDef *pid, float Kp, float Ki, float Kd,
        float target, float limit_down, float limit_up);
#endif /* _USER_PID_H_ */
