

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile("tst lr, #4                \n" // EXC_RETURN[2]
                   "ite eq                    \n"
                   "mrseq r0, msp             \n"
                   "mrsne r0, psp             \n"
                   "b HardFault_HandlerC      \n");
}



#include "tx_api.h"
#include "core_cm33.h"

typedef struct
{
    uint32_t r0, r1, r2, r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} stacked_regs_t;

/* Volatile so debugger always sees them */
volatile const TX_THREAD *fault_thread;
volatile uint32_t fault_pc;
volatile uint32_t fault_lr;
volatile uint32_t fault_xpsr;
volatile uint32_t fault_cfsr;
volatile uint32_t fault_hfsr;
volatile uint32_t fault_bfar;
volatile uint32_t fault_mmfar;

void HardFault_HandlerC(uint32_t *stack_ptr)
{
    const stacked_regs_t *s = (const stacked_regs_t *)stack_ptr;

    /* 1. Core registers */
    fault_pc = s->pc;
    fault_lr = s->lr;
    fault_xpsr = s->xpsr;

    /* 2. ThreadX: who was running */
    fault_thread = tx_thread_identify();

    /* 3. Cortex‑M33 fault status */
    fault_cfsr = SCB->CFSR;
    fault_hfsr = SCB->HFSR;
    fault_bfar = SCB->BFAR;
    fault_mmfar = SCB->MMFAR;

    __BKPT(0); // break into debugger
    while (1)
        ;
}
