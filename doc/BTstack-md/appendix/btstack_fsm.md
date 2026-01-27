
# Finite State Machine (FSM) API {#sec:btstack_fsm_api}


**src/btstack_fsm.h** 


    
    /*
     * @brief Request the transition from the current state to the given new state
     * @param me the current state machine
     * @param target the new state to transit to
     * @result transition status
     */
    btstack_fsm_state_t btstack_fsm_transit(btstack_fsm_t * const me, btstack_fsm_state_handler_t target);
    
    /*
     * @brief Constructs a new state hierarchical machine machine, with storage for maximum hierarchy depth.
     * @param me the current state machine
     * @param initial the initial state
     */
    void btstack_fsm_constructor(btstack_fsm_t * const me, btstack_fsm_state_handler_t initial);
    
    /*
     * @brief Takes the initial transition of the state machine and sending it a BTSTACK_HSM_INIT_SIG
     * @param me the current state machine
     * @param e event
     */
    void btstack_fsm_init(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine, if a transition is requested, leave the old states and enter the new on.
     *        Handling entering/exiting all states on the way.
     * @param me the current state machine
     * @param e event
     */
    btstack_fsm_state_t btstack_fsm_dispatch(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine until it was handled.
     * @param me the current state machine
     * @param e event
     */
    void btstack_fsm_dispatch_until(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    
