
# Bluetooth Power Control API {#sec:btstack_control_api}


**src/btstack_control.h** : 
The Bluetooth Hardware Control API allows HCI to manage Bluetooth chipsets via direct hardware controls.



    
    typedef struct {
    	void (*init) (const void *config);
        int  (*on)   (void);  // <-- turn BT module on and configure
        int  (*off)  (void);  // <-- turn BT module off
        int  (*sleep)(void);  // <-- put BT module to sleep    - only to be called after ON
        int  (*wake) (void);  // <-- wake BT module from sleep - only to be called after SLEEP
        void (*register_for_power_notifications)(void (*cb)(POWER_NOTIFICATION_t event));
    } btstack_control_t;
    
