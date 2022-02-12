#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "Motor.h"

/**
 * @brief This class manages both intances of motor. It executes the PI control for both motors from a task, which is trigger by a timer interrupt.
 * 
 */
class MotorController
{
    public:
        /**
         * @brief Initialize the MotorController instance
         * 
         * @note It is safe to call this function multiple times. It will only create one MotorController instance.
         * 
         * @return Reference to MotorController instance
         */ 
        static MotorController& init();

        /**
         * @brief Set the velocity of both motors in RAD/s
         * 
         * @param setpoint_velocity_a velocity of motor_a
         * @param setpoint_velocity_b velocity of motor_b
         */
        void setVelocity(float setpoint_velocity_a, float setpoint_velocity_b);

        /**
         * @brief Enable the PI controller
         * 
         * @note If enabled _motor_control_loop_task() will execute the updatePIControl() function
         */
        void enablePIcontrol();
        
        /**
         * @brief Disable the PI controller
         * 
         * @note If disabled _motor_control_loop_task() will set the ouput duty cycle proportional to the setpoint_velocity
         */ 
        void disablePIcontrol();

    private:
        MotorController();
        MotorController(MotorController const&) = delete;
        ~MotorController() {};

        /**
         * @brief FreeRTOS Task sets ouput duty_cycle
         */
        static void _motor_control_loop_task(void* pvParameters);
        
        /**
         * @brief Timer interrupt Callback function
         * 
         * @note Notifies _motor_control_loop_task() -> deferred interrupt handling
         */ 
        static bool IRAM_ATTR _motor_control_interrupt(void* args);

        TaskHandle_t _control_loop_task;

        static MotorController* _motor_controller;

        Motor& _motor_a;
        Motor& _motor_b;

        bool _enable_PI_control = true;

        static gpio_config_t _enable_config;
        gpio_num_t _enable_pin;

        static timer_config_t _timer_config;
};