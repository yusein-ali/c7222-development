
# HFP GSM Model API {#sec:hfp_gsm_model_api}


**src/classic/hfp_gsm_model.h** 


    typedef struct {
        bool used_slot;
        hfp_enhanced_call_status_t enhanced_status;
        hfp_enhanced_call_dir_t direction;
        hfp_enhanced_call_mode_t mode;
        hfp_enhanced_call_mpty_t mpty;
        // TODO: sort on drop call, so that index corresponds to table index
        int index;
        uint8_t clip_type;
        char    clip_number[25];
    } hfp_gsm_call_t;
    
    hfp_callheld_status_t hfp_gsm_callheld_status(void);
    hfp_call_status_t hfp_gsm_call_status(void);
    hfp_callsetup_status_t hfp_gsm_callsetup_status(void);
    
    int hfp_gsm_get_number_of_calls(void);
    char * hfp_gsm_last_dialed_number(void);
    void hfp_gsm_clear_last_dialed_number(void);
    void hfp_gsm_set_last_dialed_number(const char* number);
    
    hfp_gsm_call_t * hfp_gsm_call(int index);
    
    int hfp_gsm_call_possible(void);
    
    uint8_t hfp_gsm_clip_type(void);
    char *  hfp_gsm_clip_number(void);
    
    void hfp_gsm_init(void);
    void hfp_gsm_deinit(void);
    
    void hfp_gsm_handler(hfp_ag_call_event_t event, uint8_t index, uint8_t type, const char * number);
    
    
