/*******************************************************************************
  File Name:
    sw_timer.h

  Summary:
    SW Timer component for the IoT(Internet of things) service.
    
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#ifndef IOT_SW_TIMER_H_INCLUDED
#define IOT_SW_TIMER_H_INCLUDED

//#include <asf.h>
#include <stdint.h>
#include "demo_support/iot/conf_sw_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sw_timer_module;

/**
 * Callback Function type of time out event in the timer.
 *
 * \param[in]  timer_id        ID of timer.
 * \param[in]  context         Private data of timer.
 * \param[in]  context         Period of timer handle.
 */
typedef void (*sw_timer_callback_t)(struct sw_timer_module *const module, int timer_id, void *context, int period);

/**
 * \brief SW timer configuration structure
 *
 * Configuration struct for a SW timer instance. This structure should be
 * initialized by the \ref sw_timer_get_config_defaults function before being
 * modified by the user application.
 */
struct sw_timer_config {
    /** HW interface of TCC. */
    uint8_t tcc_dev;
    /** Callback channel of TCC. */
    uint8_t tcc_callback_channel;
    /** Accuracy of timer. If this value is increased, Timer can checks a long time. Unit is milliseconds*/
    uint16_t accuracy;
};

/**
 * \brief SW timer handler structure
 */
struct sw_timer_handle {
    /** A flag that timer is used. */
    uint8_t used;
    /** A flag that timer callback is enabled. */
    uint8_t callback_enable;
    /** A flag that timer is calling the handler function or not. */
    uint8_t busy;
    /** Callback of timer. */
    sw_timer_callback_t callback;
    /** Private data of timer. */
    void *context;
    /** Period of timer. If this value is set to zero, it means this timer operated once. */
    uint32_t period;
    /** Expired time of timer. */
    uint32_t expire_time;
};

/**
 * \brief SW timer module structure
 */
struct sw_timer_module {
    /** Timer handler instances. */
    struct sw_timer_handle handler[CONF_SW_TIMER_COUNT];
    /** Accuracy of timer. */
    uint32_t accuracy;
};

/**
 * \brief Get default configuration of SW timer.
 *
 * \param[in]  config          Pointer of configuration structure which will be used in the timer.
 */
void sw_timer_get_config_defaults(struct sw_timer_config *const config);

/**
 * \brief Initialize the SW timer module.
 *
 * \param[in]  module_inst     Pointer of timer.
 * \param[in]  config          Pointer of configuration structure which will be used in the timer.
 */
void sw_timer_init(struct sw_timer_module *const module_inst, struct sw_timer_config *const config);

/**
 * \brief Start the timer.
 *
 * \param[in]  module_inst     Pointer of timer.
 */
void sw_timer_enable(struct sw_timer_module *const module_inst);

/**
 * \brief Stop the timer.
 *
 * \param[in]  module_inst     Pointer of timer.
 */
void sw_timer_disable(struct sw_timer_module *const module_inst);

/**
 * \brief Register callback.
 *
 * \param[in]  module_inst     Pointer of timer.
 * \param[in]  callback        Callback entry of time out.
 * \param[in]  context         Private data of timer.
 * \param[in]  period          Period of timer.
 *
 * \return If negative value, registering timer was failed. Otherwise this value is ID of timer.
 */
int sw_timer_register_callback(struct sw_timer_module *const module_inst,
        sw_timer_callback_t callback, void *context, uint32_t period);

/**
 * \brief Unregister callback.
 *
 * \param[in]  module_inst     Pointer of timer.
 * \param[in]  timer_id        Timer ID .
 */
void sw_timer_unregister_callback(struct sw_timer_module *const module_inst, int timer_id);

/**
 * \brief Enables callback
 *
 * Enables the callback function registered by the \ref sw_timer_register_callback.
 *
 * \param[in]  module_inst     Pointer to USART software instance struct
 * \param[in]  timer_id        Timer ID .
 * \param[in]  delay           Initialization value of timer.
 */
void sw_timer_enable_callback(struct sw_timer_module *const module_inst, int timer_id, uint32_t delay);

/**
 * \brief Disable callback
 *
 * Disable the callback function registered by the \ref sw_timer_register_callback.
 *
 * \param[in]  module_inst     Pointer to USART software instance struct
 * \param[in]  timer_id        Timer ID .
 */
void sw_timer_disable_callback(struct sw_timer_module *const module_inst, int timer_id);

/**
 * \brief Checks the time out of each timer handlers.
 *
 * This function must be called continuously for the checking the expiration of the timer handle.
 *
 * \param[in]  module_inst     Pointer to USART software instance struct
 */
void sw_timer_task(struct sw_timer_module *const module_inst);

#ifdef __cplusplus
}
#endif

#endif /* IOT_SW_TIMER_H_INCLUDED */