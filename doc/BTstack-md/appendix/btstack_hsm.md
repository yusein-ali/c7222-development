
# Hierarchical State Machine (HSM) API {#sec:btstack_hsm_api}


**src/btstack_hsm.h** 


    
    /*
     * @brief Request the transition from the current state to the given new state
     * @param me the current state machine
     * @param target the new state to transit to
     * @result transition status
     */
    btstack_hsm_state_t btstack_hsm_transit(btstack_hsm_t * const me, btstack_hsm_state_handler_t const target);
    
    /*
     * @brief Specifies the upper state in a state hierarchy
     * @param me the current state machine
     * @param target the next parent state in the hierarchy
     * @result transition status
     */
    btstack_hsm_state_t btstack_hsm_super(btstack_hsm_t * const me, btstack_hsm_state_handler_t const target);
    
    /*
     * @brief Placeholder state to mark the top most state in a state hierarchy, the root state. Ignores all events.
     * @param me the current state machine
     * @param e event
     * @result ignored status
     */
    btstack_hsm_state_t btstack_hsm_top(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    
    /*
     * @brief Constructs a new state hierarchical machine machine, with storage for maximum hierarchy depth.
     * @param me the current state machine
     * @param initial the initial state
     * @param array of btstack_hsm_state_handler_t elements with the same number of elements as the maximum number of nested state machines.
     * @param The number of nested state machines.
     */
    void btstack_hsm_constructor(btstack_hsm_t * const me, btstack_hsm_state_handler_t initial, btstack_hsm_state_handler_t path[], int8_t depth);
    
    /*
     * @brief Takes the initial transition of the state machine and sending it a BTSTACK_HSM_INIT_SIG
     * @param me the current state machine
     * @param e event
     */
    void btstack_hsm_init(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine, if a transition is requested, leave the old states and enter the new on.
     *        Honoring the hierarchy and handling entering/exiting all states on the way.
     * @param me the current state machine
     * @param e event
     */
    btstack_hsm_state_t btstack_hsm_dispatch(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    
