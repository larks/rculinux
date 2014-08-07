/*******************************************************************************
 * (c) Copyright 2012 Microsemi SoC Products Group.  All rights reserved.
 *
 * SmartFusion2 COMBLK access functions.
 *
 * SVN $Revision: 5160 $
 * SVN $Date: 2013-03-14 14:50:49 +0000 (Thu, 14 Mar 2013) $
 */
#ifndef __MSS_COMBLK_H_
#define __MSS_COMBLK_H_ 1

#include "m2sxxx.h"

/* Grabbed from arch/arm/common/nvic.c */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/smp.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/mach/irq.h>
#include <asm/hardware/nvic.h>


#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 *
 */
typedef void(*comblk_completion_handler_t)(uint8_t * p_response, uint16_t response_size);

typedef uint32_t (*comblk_page_handler_t)(uint8_t const ** pp_next_page);

typedef void (*comblk_async_event_handler_t)(uint8_t event_opcode);

/* Helper functions */
/*
void NVIC_DisableIRQ(unsigned int irq);
void NVIC_EnableIRQ(unsigned int irq);
void NVIC_ClearPendingIRQ(unsigned int irq);
*/
/*------------------------------------------------------------------------------
 *
 */
void MSS_COMBLK_init(comblk_async_event_handler_t async_event_handler);

/*------------------------------------------------------------------------------
 *
 */
void MSS_COMBLK_send_cmd_with_ptr
(
    uint8_t cmd_opcode,
    uint32_t cmd_params_ptr,
    uint8_t * p_response,
    uint16_t response_size,
    comblk_completion_handler_t completion_handler
);

/*------------------------------------------------------------------------------
 *
 */
void MSS_COMBLK_send_cmd
(
    const uint8_t * p_cmd,
    uint16_t cmd_size,
    const uint8_t * p_data,
    uint32_t data_size,
    uint8_t * p_response,
    uint16_t response_size,
    comblk_completion_handler_t completion_handler
);

/*------------------------------------------------------------------------------
 *
 */
void MSS_COMBLK_read
(
    const uint8_t * p_data,
    uint16_t cmd_size,
    uint8_t * p_response,
    uint16_t response_size,
    comblk_completion_handler_t completion_handler
);

/*------------------------------------------------------------------------------
 *
 */
void MSS_COMBLK_send_paged_cmd
(
    const uint8_t * p_cmd,
    uint16_t cmd_size,
    uint8_t * p_response,
    uint16_t response_size,
    uint32_t (*)(uint8_t const **),
    void (*completion_handler)(uint8_t *, uint16_t)
);

//void ComBlk_IRQHandler(void);
irqreturn_t ComBlk_IRQHandler(int irq, void *dev_id);

#ifdef __cplusplus
}
#endif

#endif /* __MSS_COMBLK_H_ */
