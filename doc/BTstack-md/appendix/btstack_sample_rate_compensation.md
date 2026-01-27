
# Sample rate compensation API {#sec:btstack_sample_rate_compensation_api}


**src/btstack_sample_rate_compensation.h** : 
Prevents buffer over/under-run at the audio receiver by compensating for varying/different playback/receiving sample rates.

Intended to measure the L2CAP packet sample rate and with the provided playback sample rate calculates a compensation ratio
which compensates for drift between playback and reception.

Requires the audio interface to provide the current playback sample rate.



    
    #define FLOAT_TO_Q15(a)   ((signed)((a)*(UINT16_C(1)<<15)+0.5f))
    #define FLOAT_TO_Q8(a)    ((signed)((a)*(UINT16_C(1)<<8)+0.5f))
    #define FLOAT_TO_Q7(a)    ((signed)((a)*(UINT16_C(1)<<7)+0.5f))
    
    #define Q16_TO_FLOAT(a)   ((float)(a)/(UINT32_C(1)<<16))
    #define Q15_TO_FLOAT(a)   ((float)(a)/(UINT32_C(1)<<15))
    #define Q8_TO_FLOAT(a)    ((float)(a)/(UINT32_C(1)<<8))
    #define Q7_TO_FLOAT(a)    ((float)(a)/(UINT32_C(1)<<7))
    
    //#define DEBUG_RATIO_CALCULATION
    
    typedef struct {
        uint32_t count;         // 17bit are usable to count samples, recommended for max 96kHz
        uint32_t last;          // time stamp of last measurement
        uint32_t rate_state;    // unsigned Q17.8
        uint32_t ratio_state;   // unsigned Q16.16
        uint32_t constant_playback_sample_rate; // playback sample rate if no real one is available
    #ifdef DEBUG_RATIO_CALCULATION
        double sample_rate;
        double ratio;
    #endif
    } btstack_sample_rate_compensation_t;
    
    /**
     * @brief Initialize sample rate compensation
     * @param self pointer to current instance
     * @param time stamp at which to start sample rate measurement
     */
    void btstack_sample_rate_compensation_init( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms, uint32_t sample_rate, uint32_t ratioQ15 );
    
    /**
     * @brief reset sample rate compensation
     * @param self pointer to current instance
     * @param time stamp at which to start sample rate measurement
     */
    void btstack_sample_rate_compensation_reset( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms );
    
    /**
     * @brief update sample rate compensation with the current playback sample rate decoded samples
     * @param self pointer to current instance
     * @param time stamp for current samples
     * @param samples for current time stamp
     * @param playback sample rate
     */
    uint32_t btstack_sample_rate_compensation_update( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms, uint32_t samples, uint32_t playback_sample_rate );
    
