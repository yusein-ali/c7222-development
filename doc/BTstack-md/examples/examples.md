#

In this section, we will describe a number of examples from the
*example* folder. Here is a list of existing examples:


- Hello World example:

    - [led_counter](#sec:ledcounterExample): Hello World - Blinking an LED without Bluetooth.

- GAP examples:

    - [gap_inquiry](#sec:gapinquiryExample): GAP Classic Inquiry.

    - [gap_link_keys](#sec:gaplinkkeysExample): GAP Link Key Management (Classic).

- Low Energy examples:

    - [gap_le_advertisements](#sec:gapleadvertisementsExample): GAP LE Advertisements Scanner.

    - [gatt_browser](#sec:gattbrowserExample): GATT Client - Discover Primary Services.

    - [gatt_counter](#sec:gattcounterExample): GATT Server - Heartbeat Counter over GATT.

    - [gatt_streamer_server](#sec:gattstreamerserverExample): Performance - Stream Data over GATT (Server).

    - [gatt_battery_query](#sec:gattbatteryqueryExample): GATT Battery Service Client.

    - [gatt_device_information_query](#sec:gattdeviceinformationqueryExample): GATT Device Information Service Client.

    - [gatt_heart_rate_client](#sec:gattheartrateclientExample): GATT Heart Rate Sensor Client .

    - [nordic_spp_le_counter](#sec:nordicspplecounterExample): LE Nordic SPP-like Heartbeat Server .

    - [nordic_spp_le_streamer](#sec:nordicspplestreamerExample): LE Nordic SPP-like Streamer Server .

    - [ublox_spp_le_counter](#sec:ubloxspplecounterExample): LE u-blox SPP-like Heartbeat Server.

    - [sm_pairing_central](#sec:smpairingcentralExample): LE Central - Test Pairing Methods.

    - [sm_pairing_peripheral](#sec:smpairingperipheralExample): LE Peripheral - Test Pairing Methods.

    - [le_credit_based_flow_control_mode_client](#sec:lecreditbasedflowcontrolmodeclientExample): LE Credit-Based Flow-Control Mode Client - Send Data over L2CAP.

    - [le_credit_based_flow_control_mode_server](#sec:lecreditbasedflowcontrolmodeserverExample): LE Credit-Based Flow-Control Mode Server - Receive data over L2CAP.

    - [att_delayed_response](#sec:attdelayedresponseExample): LE Peripheral - Delayed Response.

    - [ancs_client_demo](#sec:ancsclientdemoExample): LE ANCS Client - Apple Notification Service.

    - [le_mitm](#sec:lemitmExample): LE Man-in-the-Middle Tool.

- Performance examples:

    - [le_streamer_client](#sec:lestreamerclientExample): Performance - Stream Data over GATT (Client).

    - [gatt_streamer_server](#sec:gattstreamerserverExample): Performance - Stream Data over GATT (Server).

    - [le_credit_based_flow_control_mode_client](#sec:lecreditbasedflowcontrolmodeclientExample): LE Credit-Based Flow-Control Mode Client - Send Data over L2CAP.

    - [le_credit_based_flow_control_mode_server](#sec:lecreditbasedflowcontrolmodeserverExample): LE Credit-Based Flow-Control Mode Server - Receive data over L2CAP.

    - [spp_streamer_client](#sec:sppstreamerclientExample): Performance - Stream Data over SPP (Client).

    - [spp_streamer](#sec:sppstreamerExample): Performance - Stream Data over SPP (Server).

- Audio examples:

    - [a2dp_sink_demo](#sec:a2dpsinkdemoExample): A2DP Sink - Receive Audio Stream and Control Playback.

    - [a2dp_source_demo](#sec:a2dpsourcedemoExample): A2DP Source - Stream Audio and Control Volume.

    - [avrcp_browsing_client](#sec:avrcpbrowsingclientExample): AVRCP Browsing - Browse Media Players and Media Information.

    - [hfp_ag_demo](#sec:hfpagdemoExample): HFP AG - Audio Gateway.

    - [hfp_hf_demo](#sec:hfphfdemoExample): HFP HF - Hands-Free.

    - [hsp_ag_demo](#sec:hspagdemoExample): HSP AG - Audio Gateway.

    - [hsp_hs_demo](#sec:hsphsdemoExample): HSP HS - Headset.

    - [sine_player](#sec:sineplayerExample): Audio Driver - Play Sine .

    - [mod_player](#sec:modplayerExample): Audio Driver - Play 80's MOD Song.

    - [audio_duplex](#sec:audioduplexExample): Audio Driver - Forward Audio from Source to Sink.

- SPP Server examples:

    - [spp_counter](#sec:sppcounterExample): SPP Server - Heartbeat Counter over RFCOMM.

    - [spp_flowcontrol](#sec:sppflowcontrolExample): SPP Server - RFCOMM Flow Control.

- Networking examples:

    - [pan_lwip_http_server](#sec:panlwiphttpserverExample): PAN - lwIP HTTP and DHCP Server .

    - [panu_demo](#sec:panudemoExample): BNEP/PANU (Linux only).

- HID examples:

    - [hid_keyboard_demo](#sec:hidkeyboarddemoExample): HID Keyboard Classic.

    - [hid_mouse_demo](#sec:hidmousedemoExample): HID Mouse Classic.

    - [hid_host_demo](#sec:hidhostdemoExample): HID Host Classic.

    - [hog_keyboard_demo](#sec:hogkeyboarddemoExample): HID Keyboard LE.

    - [hog_mouse_demo](#sec:hogmousedemoExample): HID Mouse LE.

    - [hog_boot_host_demo](#sec:hogboothostdemoExample): HID Boot Host LE.

- Dual Mode examples:

    - [spp_and_gatt_counter](#sec:sppandgattcounterExample): Dual Mode - SPP and LE Counter.

    - [gatt_streamer_server](#sec:gattstreamerserverExample): Performance - Stream Data over GATT (Server).

- SDP Queries examples:

    - [sdp_general_query](#sec:sdpgeneralqueryExample): SDP Client - Query Remote SDP Records.

    - [sdp_rfcomm_query](#sec:sdprfcommqueryExample): SDP Client - Query RFCOMM SDP record.

    - [sdp_bnep_query](#sec:sdpbnepqueryExample): SDP Client - Query BNEP SDP record.

- Phone Book Access example:

    - [pbap_client_demo](#sec:pbapclientdemoExample): PBAP Client - Get Contacts from Phonebook Server.

- Testing example:

    - [dut_mode_classic](#sec:dutmodeclassicExample): Testing - Enable Device Under Test (DUT) Mode for Classic.




## Hello World - Blinking an LED without Bluetooth {#sec:ledcounterExample}

Source Code: [led_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/led_counter.c)



  The example demonstrates how to provide a periodic timer to toggle an LED and send debug messages to the console as a minimal BTstack test.

### Periodic Timer Setup 


  As timers in BTstack are single shot, the periodic counter is implemented by re-registering the timer source in the heartbeat handler callback function. Listing [here](#lst:ledcounterLEDToggler) shows heartbeat handler adapted to periodically toggle an LED and print number of toggles.  


~~~~ {#lst:ledcounterLEDToggler .c caption="{Periodic counter}"}

    static void heartbeat_handler(btstack_timer_source_t *ts){
      UNUSED(ts);
    
      // increment counter
      counter++;
    
      // print and log
      log_info("BTstack Counter %u",   counter);
      printf("BTstack counter %04u\n\r", counter);
    
      // toggle LED
      hal_led_toggle();
    
      // re-register timer
      btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(&heartbeat);
    } 

~~~~



### Main Application Setup


  Listing [here](#lst:ledcounterMainConfiguration) shows main application code. It configures the heartbeat tier and adds it to the run loop.


~~~~ {#lst:ledcounterMainConfiguration .c caption="{Setup heartbeat timer}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      // set one-shot timer
      heartbeat.process = &heartbeat_handler;
      btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(&heartbeat);
    
      printf("Running...\n\r");
      return 0;
    }

~~~~



## GAP Classic Inquiry {#sec:gapinquiryExample}

Source Code: [gap_inquiry.c](https://github.com/bluekitchen/btstack/tree/master/example/gap_inquiry.c)



  The Generic Access Profile (GAP) defines how Bluetooth devices discover and establish a connection with each other. In this example, the application discovers  surrounding Bluetooth devices and collects their Class of Device (CoD), page scan mode, clock offset, and RSSI. After that, the remote name of each device is requested. In the following section we outline the Bluetooth logic part, i.e., how the packet handler handles the asynchronous events and data packets.

### Bluetooth Logic 


  The Bluetooth logic is implemented as a state machine within the packet handler. In this example, the following states are passed sequentially: INIT, and ACTIVE.

  In INIT, an inquiry  scan is started, and the application transits to  ACTIVE state.

  In ACTIVE, the following events are processed:
 
- GAP Inquiry result event: BTstack provides a unified inquiry result that contain Class of Device (CoD), page scan mode, clock offset. RSSI and name (from EIR) are optional.
- Inquiry complete event: the remote name is requested for devices without a fetched  name. The state of a remote name can be one of the following:  REMOTE_NAME_REQUEST, REMOTE_NAME_INQUIRED, or REMOTE_NAME_FETCHED.
- Remote name request complete event: the remote name is stored in the table and the  state is updated to REMOTE_NAME_FETCHED. The query of remote names is continued.



  For more details on discovering remote devices, please see Section on [GAP](../profiles/#sec:GAPdiscoverRemoteDevices).

### Main Application Setup


  Listing [here](#lst:gapinquiryMainConfiguration) shows main application code. It registers the HCI packet handler and starts the Bluetooth stack.


~~~~ {#lst:gapinquiryMainConfiguration .c caption="{Setup packet handler for GAP inquiry}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]) {
      (void)argc;
      (void)argv;
    
      // enabled EIR
      hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
        
      return 0;
    }

~~~~



## GAP Link Key Management (Classic) {#sec:gaplinkkeysExample}

Source Code: [gap_link_keys.c](https://github.com/bluekitchen/btstack/tree/master/example/gap_link_keys.c)



  Shows how to iterate over the Classic Link Keys stored in NVS Link Keys are per device-device bonding. If the Bluetooth Controller can be swapped,  e.g. on desktop systems, a Link Key DB for each Controller is needed. We need to wait until the Bluetooth Stack has started up and selected  the correct Link Key DB based on the Controller's BD_ADDR.

### GAP Link Key Logic 


  List stored link keys

### Bluetooth Logic 


  Wait for Bluetooth startup before listing the stored link keys

### Main Application Setup


  Listing [here](#lst:gaplinkkeysMainConfiguration) shows main application code. It registers the HCI packet handler and starts the Bluetooth stack.


~~~~ {#lst:gaplinkkeysMainConfiguration .c caption="{Setup packet handler for GAP inquiry}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]) {
      (void)argc;
      (void)argv;
    
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
        
      return 0;
    }

~~~~



## GAP LE Advertisements Scanner {#sec:gapleadvertisementsExample}

Source Code: [gap_le_advertisements.c](https://github.com/bluekitchen/btstack/tree/master/example/gap_le_advertisements.c)



  This example shows how to scan and parse advertisements.



### GAP LE setup for receiving advertisements


  GAP LE advertisements are received as custom HCI events of the  GAP_EVENT_ADVERTISING_REPORT type. To receive them, you'll need to register the HCI packet handler, as shown in Listing [here](#lst:gapleadvertisementsGAPLEAdvSetup).


~~~~ {#lst:gapleadvertisementsGAPLEAdvSetup .c caption="{Setting up GAP LE client for receiving advertisements}"}

    static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void gap_le_advertisements_setup(void){
      // Active scanning, 100% (scan interval = scan window)
      gap_set_scan_parameters(1,48,48);
      gap_start_scan(); 
    
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    }
    

~~~~



### GAP LE Advertising Data Dumper


  Here, we use the definition of advertising data types and flags as specified in  [Assigned Numbers GAP](https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile) and [Supplement to the Bluetooth Core Specification, v4](https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=282152).


~~~~ {#lst:gapleadvertisementsGAPLEAdvDataTypesAndFlags .c caption="{Advertising data types and flags}"}

    static const char * ad_types[] = {
      "", 
      "Flags",
      "Incomplete List of 16-bit Service Class UUIDs",
      "Complete List of 16-bit Service Class UUIDs",
      "Incomplete List of 32-bit Service Class UUIDs",
      "Complete List of 32-bit Service Class UUIDs",
      "Incomplete List of 128-bit Service Class UUIDs",
      "Complete List of 128-bit Service Class UUIDs",
      "Shortened Local Name",
      "Complete Local Name",
      "Tx Power Level",
      "", 
      "", 
      "Class of Device",
      "Simple Pairing Hash C",
      "Simple Pairing Randomizer R",
      "Device ID",
      "Security Manager TK Value",
      "Slave Connection Interval Range",
      "",
      "List of 16-bit Service Solicitation UUIDs",
      "List of 128-bit Service Solicitation UUIDs",
      "Service Data",
      "Public Target Address",
      "Random Target Address",
      "Appearance",
      "Advertising Interval"
    };
    
    static const char * flags[] = {
      "LE Limited Discoverable Mode",
      "LE General Discoverable Mode",
      "BR/EDR Not Supported",
      "Simultaneous LE and BR/EDR to Same Device Capable (Controller)",
      "Simultaneous LE and BR/EDR to Same Device Capable (Host)",
      "Reserved",
      "Reserved",
      "Reserved"
    };

~~~~



  BTstack offers an iterator for parsing sequence of advertising data (AD) structures,  see [BLE advertisements parser API](../appendix/apis/#ble-advertisements-parser-api). After initializing the iterator, each AD structure is dumped according to its type.


~~~~ {#lst:gapleadvertisementsGAPLEAdvDataParsing .c caption="{Parsing advertising data}"}

    static void dump_advertisement_data(const uint8_t * adv_data, uint8_t adv_size){
      ad_context_t context;
      bd_addr_t address;
      uint8_t uuid_128[16];
      for (ad_iterator_init(&context, adv_size, (uint8_t *)adv_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type  = ad_iterator_get_data_type(&context);
        uint8_t size     = ad_iterator_get_data_len(&context);
        const uint8_t * data = ad_iterator_get_data(&context);
        
        if (data_type > 0 && data_type < 0x1B){
          printf("  %s: ", ad_types[data_type]);
        } 
        int i;
        // Assigned Numbers GAP
      
        switch (data_type){
          case BLUETOOTH_DATA_TYPE_FLAGS:
            // show only first octet, ignore rest
            for (i=0; i<8;i++){
              if (data[0] & (1<<i)){
                printf("%s; ", flags[i]);
              }
            }
            break;
          case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_LIST_OF_16_BIT_SERVICE_SOLICITATION_UUIDS:
            for (i=0; i<size;i+=2){
              printf("%04X ", little_endian_read_16(data, i));
            }
            break;
          case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_LIST_OF_32_BIT_SERVICE_SOLICITATION_UUIDS:
            for (i=0; i<size;i+=4){
              printf("%04"PRIX32, little_endian_read_32(data, i));
            }
            break;
          case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS:
          case BLUETOOTH_DATA_TYPE_LIST_OF_128_BIT_SERVICE_SOLICITATION_UUIDS:
            reverse_128(data, uuid_128);
            printf("%s", uuid128_to_str(uuid_128));
            break;
          case BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME:
          case BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME:
            for (i=0; i<size;i++){
              printf("%c", (char)(data[i]));
            }
            break;
          case BLUETOOTH_DATA_TYPE_TX_POWER_LEVEL:
            printf("%d dBm", *(int8_t*)data);
            break;
          case BLUETOOTH_DATA_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE:
            printf("Connection Interval Min = %u ms, Max = %u ms", little_endian_read_16(data, 0) * 5/4, little_endian_read_16(data, 2) * 5/4);
            break;
          case BLUETOOTH_DATA_TYPE_SERVICE_DATA:
            for (i=0; i<size;i+=2){
              printf("%02X ", data[i]);
            }
            break;
          case BLUETOOTH_DATA_TYPE_PUBLIC_TARGET_ADDRESS:
          case BLUETOOTH_DATA_TYPE_RANDOM_TARGET_ADDRESS:
            reverse_bd_addr(data, address);
            printf("%s", bd_addr_to_str(address));
            break;
          case BLUETOOTH_DATA_TYPE_APPEARANCE: 
            // https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
            printf("%02X", little_endian_read_16(data, 0) );
            break;
          case BLUETOOTH_DATA_TYPE_ADVERTISING_INTERVAL:
            printf("%u ms", little_endian_read_16(data, 0) * 5/8 );
            break;
          case BLUETOOTH_DATA_TYPE_3D_INFORMATION_DATA:
            for (i=0; i<size;i+=2){
              printf("%02X ", data[i]);
            }
            break;
          case BLUETOOTH_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA: // Manufacturer Specific Data 
          case BLUETOOTH_DATA_TYPE_CLASS_OF_DEVICE:
          case BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_HASH_C:
          case BLUETOOTH_DATA_TYPE_SIMPLE_PAIRING_RANDOMIZER_R:
          case BLUETOOTH_DATA_TYPE_DEVICE_ID: 
          case BLUETOOTH_DATA_TYPE_SECURITY_MANAGER_OUT_OF_BAND_FLAGS:
          default:
            printf("  Advertising Data Type 0x%2x not handled yet", data_type);
            break;
        }    
        printf("\n");
      }
      printf("\n");
    }

~~~~



### HCI packet handler


  The HCI packet handler has to start the scanning,  and to handle received advertisements. Advertisements are received  as HCI event packets of the GAP_EVENT_ADVERTISING_REPORT type, see Listing [here](#lst:gapleadvertisementsGAPLEAdvPacketHandler).  


~~~~ {#lst:gapleadvertisementsGAPLEAdvPacketHandler .c caption="{Scanning and receiving advertisements}"}

    
    static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      bd_addr_t address;
      uint8_t address_type;
      uint8_t event_type;
      int8_t rssi;
      uint8_t length;
      const uint8_t * data;
    
      switch (hci_event_packet_get_type(packet)) {
        case GAP_EVENT_ADVERTISING_REPORT:
          gap_event_advertising_report_get_address(packet, address);
          event_type = gap_event_advertising_report_get_advertising_event_type(packet);
          address_type = gap_event_advertising_report_get_address_type(packet);
          rssi = gap_event_advertising_report_get_rssi(packet);
          length = gap_event_advertising_report_get_data_length(packet);
          data = gap_event_advertising_report_get_data(packet);
          printf("Advertisement (legacy) event: evt-type %u, addr-type %u, addr %s, rssi %d, data[%u] ", event_type,
             address_type, bd_addr_to_str(address), rssi, length);
          printf_hexdump(data, length);
          dump_advertisement_data(data, length);
          break;
    #ifdef ENABLE_LE_EXTENDED_ADVERTISING
        case GAP_EVENT_EXTENDED_ADVERTISING_REPORT:
          gap_event_extended_advertising_report_get_address(packet, address);
          event_type = gap_event_extended_advertising_report_get_advertising_event_type(packet);
          address_type = gap_event_extended_advertising_report_get_address_type(packet);
          rssi = gap_event_extended_advertising_report_get_rssi(packet);
          length = gap_event_extended_advertising_report_get_data_length(packet);
          data = gap_event_extended_advertising_report_get_data(packet);
          printf("Advertisement (extended) event: evt-type %u, addr-type %u, addr %s, rssi %d, data[%u] ", event_type,
             address_type, bd_addr_to_str(address), rssi, length);
          printf_hexdump(data, length);
          dump_advertisement_data(data, length);
          break;
    #endif
        default:
          break;
      }
    }

~~~~



## GATT Client - Discover Primary Services {#sec:gattbrowserExample}

Source Code: [gatt_browser.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_browser.c)



  This example shows how to use the GATT Client API to discover primary services and their characteristics of the first found device that is advertising its services.

 The logic is divided between the HCI and GATT client packet handlers. The HCI packet handler is responsible for finding a remote device,  connecting to it, and for starting the first GATT client query. Then, the GATT client packet handler receives all primary services and requests the characteristics of the last one to keep the example short.



### GATT client setup


  In the setup phase, a GATT client must register the HCI and GATT client packet handlers, as shown in Listing [here](#lst:gattbrowserGATTClientSetup). Additionally, the security manager can be setup, if signed writes, or encrypted, or authenticated connection are required, to access the characteristics, as explained in Section on [SMP](../protocols/#sec:smpProtocols).


~~~~ {#lst:gattbrowserGATTClientSetup .c caption="{Setting up GATT client}"}

    
    // Handles connect, disconnect, and advertising report events,  
    // starts the GATT client, and sends the first query.
    static void handle_hci_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    // Handles GATT client query results, sends queries and the 
    // GAP disconnect command when the querying is done.
    static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void gatt_client_setup(void){
    
      // Initialize L2CAP and register HCI event handler
      l2cap_init();
    
      // Initialize GATT client 
      gatt_client_init();
    
      // Optinoally, Setup security manager
      sm_init();
      sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    
      // register for HCI events
      hci_event_callback_registration.callback = &handle_hci_event;
      hci_add_event_handler(&hci_event_callback_registration);
    }

~~~~



### HCI packet handler


  The HCI packet handler has to start the scanning,  to find the first advertising device, to stop scanning, to connect to and later to disconnect from it, to start the GATT client upon the connection is completed, and to send the first query - in this case the gatt_client_discover_primary_services() is called, see  Listing [here](#lst:gattbrowserGATTBrowserHCIPacketHandler).  


~~~~ {#lst:gattbrowserGATTBrowserHCIPacketHandler .c caption="{Connecting and disconnecting from the GATT client}"}

    static void handle_hci_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
      advertising_report_t report;
      
      uint8_t event = hci_event_packet_get_type(packet);
      switch (event) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started
          if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) break;
          if (cmdline_addr_found){
            printf("Trying to connect to %s\n", bd_addr_to_str(cmdline_addr));
            gap_connect(cmdline_addr, 0);
            break;
          }
          printf("BTstack activated, start scanning!\n");
          gap_set_scan_parameters(0,0x0030, 0x0030);
          gap_start_scan();
          break;
        case GAP_EVENT_ADVERTISING_REPORT:
          fill_advertising_report_from_packet(&report, packet);
          dump_advertising_report(&report);
    
          // stop scanning, and connect to the device
          gap_stop_scan();
          gap_connect(report.address,report.address_type);
          break;
        case HCI_EVENT_META_GAP:
          // wait for connection complete
          if (hci_event_gap_meta_get_subevent_code(packet) !=  GAP_SUBEVENT_LE_CONNECTION_COMPLETE) break;
          printf("\nGATT browser - CONNECTED\n");
          connection_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
          // query primary services
          gatt_client_discover_primary_services(handle_gatt_client_event, connection_handle);
          break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
          printf("\nGATT browser - DISCONNECTED\n");
          break;
        default:
          break;
      }
    }

~~~~



### GATT Client event handler


  Query results and further queries are handled by the GATT client packet handler, as shown in Listing [here](#lst:gattbrowserGATTBrowserQueryHandler). Here, upon receiving the primary services, the gatt_client_discover_characteristics_for_service() query for the last received service is sent. After receiving the characteristics for the service, gap_disconnect is called to terminate the connection. Upon disconnect, the HCI packet handler receives the disconnect complete event.  


~~~~ {#lst:gattbrowserGATTBrowserQueryHandler .c caption="{Handling of the GATT client queries}"}

    
    static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(packet_type);
      UNUSED(channel);
      UNUSED(size);
    
      gatt_client_service_t service;
      gatt_client_characteristic_t characteristic;
      switch(hci_event_packet_get_type(packet)){
        case GATT_EVENT_SERVICE_QUERY_RESULT:
          gatt_event_service_query_result_get_service(packet, &service);
          dump_service(&service);
          services[service_count++] = service;
          break;
        case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
          gatt_event_characteristic_query_result_get_characteristic(packet, &characteristic);
          dump_characteristic(&characteristic);
          break;
        case GATT_EVENT_QUERY_COMPLETE:
          // GATT_EVENT_QUERY_COMPLETE of search characteristics
          if (service_index < service_count) {
            service = services[service_index++];
            printf("\nGATT browser - CHARACTERISTIC for SERVICE %s, [0x%04x-0x%04x]\n",
              uuid128_to_str(service.uuid128), service.start_group_handle, service.end_group_handle);
            gatt_client_discover_characteristics_for_service(handle_gatt_client_event, connection_handle, &service);
            break;
          }
          service_index = 0;
          break;
        default:
          break;
      }
    }

~~~~



## GATT Server - Heartbeat Counter over GATT {#sec:gattcounterExample}

Source Code: [gatt_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_counter.c)



  All newer operating systems provide GATT Client functionality. The LE Counter examples demonstrates how to specify a minimal GATT Database with a custom GATT Service and a custom Characteristic that sends periodic notifications. 

### Main Application Setup


  Listing [here](#lst:gattcounterMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled ATT Database generated from $le_counter.gatt$.  Additionally, it enables the Battery Service Server with the current battery level. Finally, it configures the advertisements  and the heartbeat handler and boots the Bluetooth stack.  In this example, the Advertisement contains the Flags attribute and the device name. The flag 0x06 indicates: LE General Discoverable Mode and BR/EDR not supported.


~~~~ {#lst:gattcounterMainConfiguration .c caption="{Init L2CAP SM ATT Server and start heartbeat timer}"}

    static int  le_notification_enabled;
    static btstack_timer_source_t heartbeat;
    static btstack_packet_callback_registration_t hci_event_callback_registration;
    static hci_con_handle_t con_handle;
    
    #ifdef ENABLE_GATT_OVER_CLASSIC
    static uint8_t gatt_service_buffer[70];
    #endif
    
    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
    static void  heartbeat_handler(struct btstack_timer_source *ts);
    static void beat(void);
    
    // Flags general discoverable, BR/EDR supported (== not supported flag not set) when ENABLE_GATT_OVER_CLASSIC is enabled
    #ifdef ENABLE_GATT_OVER_CLASSIC
    #define APP_AD_FLAGS 0x02
    #else
    #define APP_AD_FLAGS 0x06
    #endif
    
    const uint8_t adv_data[] = {
      // Flags general discoverable
      0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
      // Name
      0x0b, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'L', 'E', ' ', 'C', 'o', 'u', 'n', 't', 'e', 'r', 
      // Incomplete List of 16-bit Service Class UUIDs -- FF10 - only valid for testing!
      0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xff,
    };
    const uint8_t adv_data_len = sizeof(adv_data);
    
    static void le_counter_setup(void){
    
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
    
    #ifdef ENABLE_GATT_OVER_CLASSIC
      // init SDP, create record for GATT and register with SDP
      sdp_init();
      memset(gatt_service_buffer, 0, sizeof(gatt_service_buffer));
      gatt_create_sdp_record(gatt_service_buffer, sdp_create_service_record_handle(), ATT_SERVICE_GATT_SERVICE_START_HANDLE, ATT_SERVICE_GATT_SERVICE_END_HANDLE);
      btstack_assert(de_get_len( gatt_service_buffer) <= sizeof(gatt_service_buffer));
      sdp_register_service(gatt_service_buffer);
    
      // configure Classic GAP
      gap_set_local_name("GATT Counter BR/EDR 00:00:00:00:00:00");
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_discoverable_control(1);
    #endif
    
      // setup ATT server
      att_server_init(profile_data, att_read_callback, att_write_callback);  
    
      // setup advertisements
      uint16_t adv_int_min = 0x0030;
      uint16_t adv_int_max = 0x0030;
      uint8_t adv_type = 0;
      bd_addr_t null_addr;
      memset(null_addr, 0, 6);
      gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
      gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
      gap_advertisements_enable(1);
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for ATT event
      att_server_register_packet_handler(packet_handler);
    
      // set one-shot timer
      heartbeat.process = &heartbeat_handler;
      btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(&heartbeat);
    
      // beat once
      beat();
    }

~~~~



### Heartbeat Handler


  The heartbeat handler updates the value of the single Characteristic provided in this example, and request a ATT_EVENT_CAN_SEND_NOW to send a notification if enabled see Listing [here](#lst:gattcounterheartbeat).


~~~~ {#lst:gattcounterheartbeat .c caption="{Hearbeat Handler}"}

    static int  counter = 0;
    static char counter_string[30];
    static int  counter_string_len;
    
    static void beat(void){
      counter++;
      counter_string_len = btstack_snprintf_assert_complete(counter_string, sizeof(counter_string), "BTstack counter %04u", counter);
      puts(counter_string);
    }
    
    static void heartbeat_handler(struct btstack_timer_source *ts){
      if (le_notification_enabled) {
        beat();
        att_server_request_can_send_now_event(con_handle);
      }
    
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 

~~~~



### Packet Handler


  The packet handler is used to:
 
- stop the counter after a disconnect
- send a notification when the requested ATT_EVENT_CAN_SEND_NOW is received




~~~~ {#lst:gattcounterpacketHandler .c caption="{Packet Handler}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
      
      switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
          le_notification_enabled = 0;
          break;
        case ATT_EVENT_CAN_SEND_NOW:
          att_server_notify(con_handle, ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE, (uint8_t*) counter_string, counter_string_len);
          break;
        default:
          break;
      }
    }
    

~~~~



### ATT Read


  The ATT Server handles all reads to constant data. For dynamic data like the custom characteristic, the registered att_read_callback is called. To handle long characteristics and long reads, the att_read_callback is first called with buffer == NULL, to request the total value length. Then it will be called again requesting a chunk of the value. See Listing [here](#lst:gattcounterattRead).


~~~~ {#lst:gattcounterattRead .c caption="{ATT Read}"}

    
    // ATT Client Read Callback for Dynamic Data
    // - if buffer == NULL, don't copy data, just return size of value
    // - if buffer != NULL, copy data and return number bytes copied
    // @param offset defines start of attribute value
    static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
      UNUSED(connection_handle);
    
      if (att_handle == ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE){
        return att_read_callback_handle_blob((const uint8_t *)counter_string, counter_string_len, offset, buffer, buffer_size);
      }
      return 0;
    }

~~~~



### ATT Write


  The only valid ATT writes in this example are to the Client Characteristic Configuration, which configures notification and indication and to the the Characteristic Value. If the ATT handle matches the client configuration handle, the new configuration value is stored and used in the heartbeat handler to decide if a new value should be sent. If the ATT handle matches the characteristic value handle, we print the write as hexdump See Listing [here](#lst:gattcounterattWrite).


~~~~ {#lst:gattcounterattWrite .c caption="{ATT Write}"}

    static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
      switch (att_handle){
        case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE:
          le_notification_enabled = little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
          con_handle = connection_handle;
          break;
        case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE:
          printf("Write: transaction mode %u, offset %u, data (%u bytes): ", transaction_mode, offset, buffer_size);
          printf_hexdump(buffer, buffer_size);
          break;
        default:
          break;
      }
      return 0;
    }

~~~~



## Performance - Stream Data over GATT (Server) {#sec:gattstreamerserverExample}

Source Code: [gatt_streamer_server.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_streamer_server.c)



  All newer operating systems provide GATT Client functionality. This example shows how to get a maximal throughput via BLE:
 
- send whenever possible,
- use the max ATT MTU.



  In theory, we should also update the connection parameters, but we already get a connection interval of 30 ms and there's no public way to use a shorter  interval with iOS (if we're not implementing an HID device).



  Note: To start the streaming, run the example. On remote device use some GATT Explorer, e.g. LightBlue, BLExplr to enable notifications.

### Main Application Setup


  Listing [here](#lst:gattstreamerserverMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager, and configures the ATT Server with the pre-compiled ATT Database generated from $le_streamer.gatt$. Finally, it configures the advertisements  and boots the Bluetooth stack. 


~~~~ {#lst:gattstreamerserverMainConfiguration .c caption="{Init L2CAP, SM, ATT Server, and enable advertisements}"}

    
    static void le_streamer_setup(void){
    
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
    
    #ifdef ENABLE_GATT_OVER_CLASSIC
      // init SDP, create record for GATT and register with SDP
      sdp_init();
      memset(gatt_service_buffer, 0, sizeof(gatt_service_buffer));
      gatt_create_sdp_record(gatt_service_buffer, 0x10001, ATT_SERVICE_GATT_SERVICE_START_HANDLE, ATT_SERVICE_GATT_SERVICE_END_HANDLE);
      sdp_register_service(gatt_service_buffer);
      printf("SDP service record size: %u\n", de_get_len(gatt_service_buffer));
    
      // configure Classic GAP
      gap_set_local_name("GATT Streamer BR/EDR 00:00:00:00:00:00");
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_discoverable_control(1);
    #endif
    
      // setup ATT server
      att_server_init(profile_data, NULL, att_write_callback);  
      
      // register for HCI events
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for ATT events
      att_server_register_packet_handler(att_packet_handler);
    
      // setup advertisements
      uint16_t adv_int_min = 0x0030;
      uint16_t adv_int_max = 0x0030;
      uint8_t adv_type = 0;
      bd_addr_t null_addr;
      memset(null_addr, 0, 6);
      gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
      gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
      gap_advertisements_enable(1);
    
      // init client state
      init_connections();
    }

~~~~



### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent and received. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s.


~~~~ {#lst:gattstreamerservertracking .c caption="{Tracking throughput}"}

    
    static void test_reset(le_streamer_connection_t * context){
      context->test_data_start = btstack_run_loop_get_time_ms();
      context->test_data_sent = 0;
      context->test_data_received = 0;
    }
    
    static void test_track_evaluate(le_streamer_connection_t * context) {
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - context->test_data_start;
    
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second_sent = context->test_data_sent * 1000 / time_passed;
      int bytes_per_second_received = context->test_data_received * 1000 / time_passed;
      int bytes_per_second_total = bytes_per_second_sent + bytes_per_second_received;
      printf("%c: Sent %u.%03u kB/s, Received %u.%03u kB/s => Total %u.%03u kB/s\n", context->name,
        bytes_per_second_sent   / 1000, bytes_per_second_sent   % 1000,
        bytes_per_second_received / 1000, bytes_per_second_received % 1000,
        bytes_per_second_total  / 1000, bytes_per_second_total  % 1000);
      // store into test data buffer for use with GATT Browser
      btstack_snprintf_best_effort(context->test_data, sizeof(context->test_data),
        "Notification Throughput %u.%03u kB/s",
        bytes_per_second_total  / 1000, bytes_per_second_total  % 1000);
    
      // restart
      test_reset(context);
    }
    
    static void test_track_data(le_streamer_connection_t * context, uint32_t bytes_sent, uint32_t bytes_received){
      context->test_data_sent   += bytes_sent;
      context->test_data_received += bytes_received;
      test_track_evaluate(context);
    }
    

~~~~



### HCI Packet Handler


  The packet handler is used track incoming connections and to stop notifications on disconnect It is also a good place to request the connection parameter update as indicated  in the commented code block.


~~~~ {#lst:gattstreamerserverhciPacketHandler .c caption="{Packet Handler}"}

    static void hci_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
      
      if (packet_type != HCI_EVENT_PACKET) return;
    
      uint16_t conn_interval;
      hci_con_handle_t con_handle;
      static const char * const phy_names[] = {
        "Reserved", "1 M", "2 M", "Codec"
      };
    
      switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
            printf("To start the streaming, please run the le_streamer_client example on other device, or use some GATT Explorer, e.g. LightBlue, BLExplr.\n");
          } 
          break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
          con_handle = hci_event_disconnection_complete_get_connection_handle(packet);
          printf("- LE Connection 0x%04x: disconnect, reason %02x\n", con_handle, hci_event_disconnection_complete_get_reason(packet));          
          break;
        case HCI_EVENT_META_GAP:
          switch (hci_event_gap_meta_get_subevent_code(packet)) {
            case GAP_SUBEVENT_LE_CONNECTION_COMPLETE:
              // print connection parameters (without using float operations)
              con_handle  = gap_subevent_le_connection_complete_get_connection_handle(packet);
              conn_interval = gap_subevent_le_connection_complete_get_conn_interval(packet);
              printf("- LE Connection 0x%04x: connected - connection interval %u.%02u ms, latency %u\n", con_handle, conn_interval * 125 / 100,
                25 * (conn_interval & 3), gap_subevent_le_connection_complete_get_conn_latency(packet));
    
              // request min con interval 15 ms for iOS 11+
              printf("- LE Connection 0x%04x: request 15 ms connection interval\n", con_handle);
              gap_request_connection_parameter_update(con_handle, 12, 12, 4, 0x0048);
              break;
            default:
              break;
          }
          break;
        case HCI_EVENT_LE_META:
          switch (hci_event_le_meta_get_subevent_code(packet)) {
            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
              // print connection parameters (without using float operations)
              con_handle  = hci_subevent_le_connection_update_complete_get_connection_handle(packet);
              conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
              printf("- LE Connection 0x%04x: connection update - connection interval %u.%02u ms, latency %u\n", con_handle, conn_interval * 125 / 100,
                25 * (conn_interval & 3), hci_subevent_le_connection_update_complete_get_conn_latency(packet));
              break;
            case HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE:
              con_handle = hci_subevent_le_data_length_change_get_connection_handle(packet);
              printf("- LE Connection 0x%04x: data length change - max %u bytes per packet\n", con_handle,
                   hci_subevent_le_data_length_change_get_max_tx_octets(packet));
              break;
            case HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE:
              con_handle = hci_subevent_le_phy_update_complete_get_connection_handle(packet);
              printf("- LE Connection 0x%04x: PHY update - using LE %s PHY now\n", con_handle,
                   phy_names[hci_subevent_le_phy_update_complete_get_tx_phy(packet)]);
              break;
            default:
              break;
          }
          break;
          
        default:
          break;
      }
    }
    

~~~~



### ATT Packet Handler


  The packet handler is used to track the ATT MTU Exchange and trigger ATT send


~~~~ {#lst:gattstreamerserverattPacketHandler .c caption="{Packet Handler}"}

    static void att_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      int mtu;
      le_streamer_connection_t * context;
      switch (packet_type) {
        case HCI_EVENT_PACKET:
          switch (hci_event_packet_get_type(packet)) {
            case ATT_EVENT_CONNECTED:
              // setup new 
              context = connection_for_conn_handle(HCI_CON_HANDLE_INVALID);
              if (!context) break;
              context->connection_handle = att_event_connected_get_handle(packet);
              context->test_data_len = btstack_min(att_server_get_mtu(context->connection_handle) - 3, sizeof(context->test_data));
              // store into test data buffer for use with GATT Browser
              btstack_snprintf_best_effort(context->test_data, context  ->test_data_len, "GATT Streamer starting up...");
              printf("%c: ATT connected, handle 0x%04x, test data len %u\n", context->name, context->connection_handle, context->test_data_len);
              break;
            case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
              mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
              context = connection_for_conn_handle(att_event_mtu_exchange_complete_get_handle(packet));
              if (!context) break;
              context->test_data_len = btstack_min(mtu - 3, sizeof(context->test_data));
              printf("%c: ATT MTU = %u => use test data of len %u\n", context->name, mtu, context->test_data_len);
              break;
            case ATT_EVENT_CAN_SEND_NOW:
              streamer();
              break;
            case ATT_EVENT_DISCONNECTED:
              context = connection_for_conn_handle(att_event_disconnected_get_handle(packet));
              if (!context) break;
              // free connection
              printf("%c: ATT disconnected, handle 0x%04x\n", context->name, context->connection_handle);          
              context->le_notification_enabled = 0;
              context->connection_handle = HCI_CON_HANDLE_INVALID;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

~~~~



### Streamer


  The streamer function checks if notifications are enabled and if a notification can be sent now. It creates some test data - a single letter that gets increased every time - and tracks the data sent.


~~~~ {#lst:gattstreamerserverstreamer .c caption="{Streaming code}"}

    static void streamer(void){
    
      // find next active streaming connection
      int old_connection_index = connection_index;
      while (1){
        // active found?
        if ((le_streamer_connections[connection_index].connection_handle != HCI_CON_HANDLE_INVALID) &&
          (le_streamer_connections[connection_index].le_notification_enabled)) break;
        
        // check next
        next_connection_index();
    
        // none found
        if (connection_index == old_connection_index) return;
      }
    
      le_streamer_connection_t * context = &le_streamer_connections[connection_index];
      
      // send
      att_server_notify(context->connection_handle, context->value_handle, (uint8_t*) context->test_data, context->test_data_len);
    
      // track
      test_track_data(context, context->test_data_len, 0);
    
      // request next send event
      att_server_request_can_send_now_event(context->connection_handle);
    
      // check next
      next_connection_index();
    } 

~~~~



### ATT Write


  The only valid ATT write in this example is to the Client Characteristic Configuration, which configures notification and indication. If the ATT handle matches the client configuration handle, the new configuration value is stored. If notifications get enabled, an ATT_EVENT_CAN_SEND_NOW is requested. See Listing [here](#lst:gattstreamerserverattWrite).


~~~~ {#lst:gattstreamerserverattWrite .c caption="{ATT Write}"}

    static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
      UNUSED(offset);
    
      if (transaction_mode != ATT_TRANSACTION_MODE_NONE) return 0;
    
      le_streamer_connection_t * context = connection_for_conn_handle(con_handle);
      switch(att_handle){
        case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE:
          context->le_notification_enabled = little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
          printf("%c: Notifications enabled %u\n", context->name, context->le_notification_enabled); 
          if (context->le_notification_enabled){
            switch (att_handle){
              case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE:
                context->value_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE;
                break;
              default:
                break;
            }
            att_server_request_can_send_now_event(context->connection_handle);
          }
          test_reset(context);
          break;
        case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE:
          test_track_data(context, 0, buffer_size);
          break;
        default:
          printf("Write to 0x%04x, len %u\n", att_handle, buffer_size);
          break;
      }
      return 0;
    }

~~~~



## GATT Battery Service Client {#sec:gattbatteryqueryExample}

Source Code: [gatt_battery_query.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_battery_query.c)



  This example demonstrates how to use the GATT Battery Service client to  receive battery level information. The client supports querying of multiple  battery services instances of on the remote device.  The example scans for remote devices and connects to the first found device and starts the battery service client.

### Main Application Setup


  The Listing [here](#lst:gattbatteryqueryMainConfiguration) shows how to setup Battery Service client.  Besides calling init() method for each service, you'll also need to register HCI packet handler  to handle advertisements, as well as connect and disconect events.



  Handling of GATT Battery Service events will be later delegated to a sepparate packet  handler, i.e. gatt_client_event_handler.

 @note There are two additional files associated with this client to allow a remote device to query out GATT database:
 
- gatt_battary_query.gatt - contains the declaration of the provided GATT Services and Characteristics.
- gatt_battary_query.h    - contains the binary representation of gatt_battary_query.gatt. 




~~~~ {#lst:gattbatteryqueryMainConfiguration .c caption="{Setup Device Battery Client service}"}

    static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void gatt_client_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void battery_service_client_setup(void){
      // Init L2CAP
      l2cap_init();
    
      // Setup ATT server - only needed if LE Peripheral does ATT queries on its own, e.g. Android phones
      att_server_init(profile_data, NULL, NULL);  
    
      // GATT Client setup
      gatt_client_init();
      // Device Information Service Client setup
      battery_service_client_init();
    
      sm_init();
      sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    
      hci_event_callback_registration.callback = &hci_event_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    } 

~~~~




~~~~ {#lst:gattbatteryquerypacketHandler .c caption="{Packet Handler}"}

    static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    ...
    
      if (packet_type != HCI_EVENT_PACKET){
        return;  
      } 
    
      switch (hci_event_packet_get_type(packet)) {
    ...
        case HCI_EVENT_META_GAP:
          // Wait for connection complete
          if (hci_event_gap_meta_get_subevent_code(packet) !=  GAP_SUBEVENT_LE_CONNECTION_COMPLETE) break;
          
    ...
          // Get connection handle from event
          connection_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
          
          // Connect to remote Battery Service. 
          // On succesful connection, the client tries to register for notifications. If notifications 
          // are not supported by remote Battery Service, the client will automatically poll the battery level - here every 2 seconds.
          // If poll_interval_ms is 0, polling is disabled, and only notifications will be received (for manual polling, 
          // see battery_service_client.h).
          // All GATT Battery Service events are handled by the gatt_client_event_handler.
          (void) battery_service_client_connect(connection_handle, gatt_client_event_handler, 2000, &battery_service_cid);
    
          app_state = APP_STATE_CONNECTED;
          printf("Battery service connected.\n");
          break;
    
        case HCI_EVENT_DISCONNECTION_COMPLETE:
          connection_handle = HCI_CON_HANDLE_INVALID;
          // Disconnect battery service
          battery_service_client_disconnect(battery_service_cid);
          
    ...
          printf("Disconnected %s\n", bd_addr_to_str(report.address));
          printf("Restart scan.\n");
          app_state = APP_STATE_W4_SCAN_RESULT;
          gap_start_scan();
          break;
        default:
          break;
      }
    }

~~~~




~~~~ {#lst:gattbatteryquerygattclienteventhandler .c caption="{GATT Client Event Handler}"}

    // The gatt_client_event_handler receives following events from remote device:
    //  - GATTSERVICE_SUBEVENT_BATTERY_SERVICE_CONNECTED
    //  - GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL   
    // 
    static void gatt_client_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    ...
      uint8_t status;
      uint8_t att_status;
    
      if (hci_event_packet_get_type(packet) != HCI_EVENT_GATTSERVICE_META){
        return;
      }
      
      switch (hci_event_gattservice_meta_get_subevent_code(packet)){
        case GATTSERVICE_SUBEVENT_BATTERY_SERVICE_CONNECTED:
          status = gattservice_subevent_battery_service_connected_get_status(packet);
          switch (status){
            case ERROR_CODE_SUCCESS:
              printf("Battery service client connected, found %d services, poll bitmap 0x%02x\n", 
                gattservice_subevent_battery_service_connected_get_num_instances(packet),
                gattservice_subevent_battery_service_connected_get_poll_bitmap(packet));
                battery_service_client_read_battery_level(battery_service_cid, 0);
              break;
            default:
              printf("Battery service client connection failed, status 0x%02x.\n", status);
              add_to_blacklist(report.address);
              gap_disconnect(connection_handle);
              break;
          }
          break;
    
        case GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL:
          att_status = gattservice_subevent_battery_service_level_get_att_status(packet);
          if (att_status != ATT_ERROR_SUCCESS){
            printf("Battery level read failed, ATT Error 0x%02x\n", att_status);
          } else {
            printf("Service index: %d, Battery level: %d\n", 
              gattservice_subevent_battery_service_level_get_service_index(packet),
              gattservice_subevent_battery_service_level_get_level(packet));
              
          }
          break;
    
        default:
          break;
      }
    }

~~~~



## GATT Device Information Service Client {#sec:gattdeviceinformationqueryExample}

Source Code: [gatt_device_information_query.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_device_information_query.c)



  This example demonstrates how to use the GATT Device Information Service client to  receive device information such as various IDs and revisions. The example scans  for remote devices and connects to the first found device. If the remote device provides a Device Information Service, the information is collected and printed in console output, otherwise,  the device will be blacklisted and the scan restarted.

### Main Application Setup


  The Listing [here](#lst:gattdeviceinformationqueryMainConfiguration) shows how to setup Device Information Service client.  Besides calling init() method for each service, you'll also need to register HCI packet handler  to handle advertisements, as well as connect and disconect events.



  Handling of GATT Device Information Service events will be later delegated to a sepparate packet  handler, i.e. gatt_client_event_handler.

 @note There are two additional files associated with this client to allow a remote device to query out GATT database:
 
- gatt_device_information_query.gatt - contains the declaration of the provided GATT Services and Characteristics.
- gatt_device_information_query.h    - contains the binary representation of gatt_device_information_query.gatt. 




~~~~ {#lst:gattdeviceinformationqueryMainConfiguration .c caption="{Setup Device Information Service Client service}"}

    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void gatt_client_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void device_information_service_client_setup(void){
      // Init L2CAP
      l2cap_init();
    
      // Setup ATT server - only needed if LE Peripheral does ATT queries on its own, e.g. Android phones
      att_server_init(profile_data, NULL, NULL);  
    
      // GATT Client setup
      gatt_client_init();
      // Device Information Service Client setup
      device_information_service_client_init();
    
      sm_init();
      sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    }

~~~~




~~~~ {#lst:gattdeviceinformationquerypacketHandler .c caption="{Packet Handler}"}

    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    ...
        case HCI_EVENT_META_GAP:
          // wait for connection complete
          if (hci_event_gap_meta_get_subevent_code(packet) !=  GAP_SUBEVENT_LE_CONNECTION_COMPLETE) break;
          
    ...
          // get connection handle from event
          connection_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
          
          // Connect to remote Device Information Service. The client will query the remote service and emit events,
          // that will be passed on to gatt_client_event_handler.
          status = device_information_service_client_query(connection_handle, gatt_client_event_handler);
          btstack_assert(status == ERROR_CODE_SUCCESS);
          
          printf("Device Information connected.\n");
          
          app_state = APP_STATE_CONNECTED;
          break;
    ...
      uint8_t att_status = 0;
    
      if (hci_event_packet_get_type(packet) != HCI_EVENT_GATTSERVICE_META){
        return;
      }
      
      switch (hci_event_gattservice_meta_get_subevent_code(packet)){
        case GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MANUFACTURER_NAME:
          att_status = gattservice_subevent_device_information_manufacturer_name_get_att_status(packet);
          if (att_status != ATT_ERROR_SUCCESS){
            printf("Manufacturer Name read failed, ATT Error 0x%02x\n", att_status);
          } else {
            printf("Manufacturer Name: %s\n", gattservice_subevent_device_information_manufacturer_name_get_value(packet));
          }
          break;
    
        // ...
    ...
        case GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_DONE:
          att_status = gattservice_subevent_device_information_serial_number_get_att_status(packet);
          switch (att_status){
            case ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE:
              printf("Device Information service client not found.\n");
              add_to_blacklist(report.address);
              gap_disconnect(connection_handle);
              break;
            case ATT_ERROR_SUCCESS:
              printf("Query done\n");
              break;
            default:
              printf("Query failed, ATT Error 0x%02x\n", att_status);
              break;
    
          }
          if (att_status != ATT_ERROR_SUCCESS){
            if (att_status == ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE)
            printf("Query failed, ATT Error 0x%02x\n", att_status);
          } else {
            printf("Query done\n");
          }
          break;
        default:
          break;
      }
    }

~~~~



## GATT Heart Rate Sensor Client  {#sec:gattheartrateclientExample}

Source Code: [gatt_heart_rate_client.c](https://github.com/bluekitchen/btstack/tree/master/example/gatt_heart_rate_client.c)



  Connects for Heart Rate Sensor and reports measurements.

## LE Nordic SPP-like Heartbeat Server  {#sec:nordicspplecounterExample}

Source Code: [nordic_spp_le_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/nordic_spp_le_counter.c)



### Main Application Setup


  Listing [here](#lst:nordicspplecounterMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled ATT Database generated from $nordic_ssp_le_counter.gatt$.  Additionally, it enables the Battery Service Server with the current battery level. Finally, it configures the advertisements  and the heartbeat handler and boots the Bluetooth stack.  In this example, the Advertisement contains the Flags attribute and the device name. The flag 0x06 indicates: LE General Discoverable Mode and BR/EDR not supported.


~~~~ {#lst:nordicspplecounterMainConfiguration .c caption="{Init L2CAP SM ATT Server and start heartbeat timer}"}

    static btstack_timer_source_t heartbeat;
    static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;
    static btstack_context_callback_registration_t send_request;
    
    const uint8_t adv_data[] = {
      // Flags general discoverable, BR/EDR not supported
      2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
      // Name
      8, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'n', 'R', 'F',' ', 'S', 'P', 'P',
      // UUID ...
      17, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x9e, 0xca, 0xdc, 0x24, 0xe, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x1, 0x0, 0x40, 0x6e,
    };
    const uint8_t adv_data_len = sizeof(adv_data);
    

~~~~



### Heartbeat Handler


  The heartbeat handler updates the value of the single Characteristic provided in this example, and request a ATT_EVENT_CAN_SEND_NOW to send a notification if enabled see Listing [here](#lst:nordicspplecounterheartbeat).


~~~~ {#lst:nordicspplecounterheartbeat .c caption="{Hearbeat Handler}"}

    static int  counter = 0;
    static char counter_string[30];
    static int  counter_string_len;
    
    static void beat(void){
      counter++;
      counter_string_len = btstack_snprintf_assert_complete(counter_string, sizeof(counter_string), "BTstack counter %03u", counter);
    }
    
    static void nordic_can_send(void * context){
      UNUSED(context);
      printf("SEND: %s\n", counter_string);
      nordic_spp_service_server_send(con_handle, (uint8_t*) counter_string, counter_string_len);
    }
    
    static void heartbeat_handler(struct btstack_timer_source *ts){
      if (con_handle != HCI_CON_HANDLE_INVALID) {
        beat();
        send_request.callback = &nordic_can_send;
        nordic_spp_service_server_request_can_send_now(&send_request, con_handle);
      }
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 

~~~~



## LE Nordic SPP-like Streamer Server  {#sec:nordicspplestreamerExample}

Source Code: [nordic_spp_le_streamer.c](https://github.com/bluekitchen/btstack/tree/master/example/nordic_spp_le_streamer.c)



  All newer operating systems provide GATT Client functionality. This example shows how to get a maximal throughput via BLE:
 
- send whenever possible,
- use the max ATT MTU.



  In theory, we should also update the connection parameters, but we already get a connection interval of 30 ms and there's no public way to use a shorter  interval with iOS (if we're not implementing an HID device).



  Note: To start the streaming, run the example. On remote device use some GATT Explorer, e.g. LightBlue, BLExplr to enable notifications.

### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:nordicspplestreamertracking .c caption="{Tracking throughput}"}

    
    static void test_reset(nordic_spp_le_streamer_connection_t * context){
      context->test_data_start = btstack_run_loop_get_time_ms();
      context->test_data_sent = 0;
    }
    
    static void test_track_sent(nordic_spp_le_streamer_connection_t * context, int bytes_sent){
      context->test_data_sent += bytes_sent;
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - context->test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second = context->test_data_sent * 1000 / time_passed;
      printf("%c: %"PRIu32" bytes sent-> %u.%03u kB/s\n", context->name, context->test_data_sent, bytes_per_second / 1000, bytes_per_second % 1000);
    
      // restart
      context->test_data_start = now;
      context->test_data_sent  = 0;
    }

~~~~



### HCI Packet Handler


  The packet handler prints the welcome message and requests a connection paramter update for LE Connections


~~~~ {#lst:nordicspplestreamerhciPacketHandler .c caption="{Packet Handler}"}

    static void hci_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
      
      uint16_t conn_interval;
      hci_con_handle_t con_handle;
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
            printf("To start the streaming, please run nRF Toolbox -> UART to connect.\n");
          } 
          break;
        case HCI_EVENT_META_GAP:
          switch (hci_event_gap_meta_get_subevent_code(packet)) {
            case GAP_SUBEVENT_LE_CONNECTION_COMPLETE:
              // print connection parameters (without using float operations)
              con_handle  = gap_subevent_le_connection_complete_get_connection_handle(packet);
              conn_interval = gap_subevent_le_connection_complete_get_conn_interval(packet);
              printf("LE Connection - Connection Interval: %u.%02u ms\n", conn_interval * 125 / 100, 25 * (conn_interval & 3));
              printf("LE Connection - Connection Latency: %u\n", gap_subevent_le_connection_complete_get_conn_latency(packet));
    
              // request min con interval 15 ms for iOS 11+
              printf("LE Connection - Request 15 ms connection interval\n");
              gap_request_connection_parameter_update(con_handle, 12, 12, 4, 0x0048);
              break;
            default:
              break;
          }
          break;
    
        case HCI_EVENT_LE_META:
          switch (hci_event_le_meta_get_subevent_code(packet)) {
            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
              // print connection parameters (without using float operations)
              con_handle  = hci_subevent_le_connection_update_complete_get_connection_handle(packet);
              conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
              printf("LE Connection - Connection Param update - connection interval %u.%02u ms, latency %u\n", conn_interval * 125 / 100,
                25 * (conn_interval & 3), hci_subevent_le_connection_update_complete_get_conn_latency(packet));
              break;
            default:
              break;
          }
          break;  
        default:
          break;
      }
    }

~~~~



### ATT Packet Handler


  The packet handler is used to setup and tear down the spp-over-gatt connection and its MTU


~~~~ {#lst:nordicspplestreamerattPacketHandler .c caption="{Packet Handler}"}

    static void att_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
      
      if (packet_type != HCI_EVENT_PACKET) return;
    
      int mtu;
      nordic_spp_le_streamer_connection_t * context;
      
      switch (hci_event_packet_get_type(packet)) {
        case ATT_EVENT_CONNECTED:
          // setup new 
          context = connection_for_conn_handle(HCI_CON_HANDLE_INVALID);
          if (!context) break;
          context->counter = 'A';
          context->test_data_len = ATT_DEFAULT_MTU - 4;   // -1 for nordic 0x01 packet type
          context->connection_handle = att_event_connected_get_handle(packet);
          break;
        case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
          mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
          context = connection_for_conn_handle(att_event_mtu_exchange_complete_get_handle(packet));
          if (!context) break;
          context->test_data_len = btstack_min(mtu - 3, sizeof(context->test_data));
          printf("%c: ATT MTU = %u => use test data of len %u\n", context->name, mtu, context->test_data_len);
          break;
    
        default:
          break;
      }
    }

~~~~



### Streamer


  The streamer function checks if notifications are enabled and if a notification can be sent now. It creates some test data - a single letter that gets increased every time - and tracks the data sent.


~~~~ {#lst:nordicspplestreamerstreamer .c caption="{Streaming code}"}

    static void nordic_can_send(void * some_context){
      UNUSED(some_context);
    
      // find next active streaming connection
      int old_connection_index = connection_index;
      while (1){
        // active found?
        if ((nordic_spp_le_streamer_connections[connection_index].connection_handle != HCI_CON_HANDLE_INVALID) &&
          (nordic_spp_le_streamer_connections[connection_index].le_notification_enabled)) break;
        
        // check next
        next_connection_index();
    
        // none found
        if (connection_index == old_connection_index) return;
      }
    
      nordic_spp_le_streamer_connection_t * context = &nordic_spp_le_streamer_connections[connection_index];
    
      // create test data
      context->counter++;
      if (context->counter > 'Z') context->counter = 'A';
      memset(context->test_data, context->counter, context->test_data_len);
    
      // send
      nordic_spp_service_server_send(context->connection_handle, (uint8_t*) context->test_data, context->test_data_len);
    
      // track
      test_track_sent(context, context->test_data_len);
    
      // request next send event
      nordic_spp_service_server_request_can_send_now(&context->send_request, context->connection_handle);
    
      // check next
      next_connection_index();
    } 

~~~~



## LE u-blox SPP-like Heartbeat Server {#sec:ubloxspplecounterExample}

Source Code: [ublox_spp_le_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/ublox_spp_le_counter.c)



### Main Application Setup


  Listing [here](#lst:ubloxspplecounterMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled ATT Database generated from $ublox_le_counter.gatt$.  Additionally, it enables the Battery Service Server with the current battery level. Finally, it configures the advertisements  and the heartbeat handler and boots the Bluetooth stack.  In this example, the Advertisement contains the Flags attribute and the device name. The flag 0x06 indicates: LE General Discoverable Mode and BR/EDR not supported.


~~~~ {#lst:ubloxspplecounterMainConfiguration .c caption="{Init L2CAP SM ATT Server and start heartbeat timer}"}

    static btstack_timer_source_t heartbeat;
    static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;
    static btstack_context_callback_registration_t send_request;
    static btstack_packet_callback_registration_t  hci_event_callback_registration;
    
    const uint8_t adv_data[] = {
      // Flags general discoverable, BR/EDR not supported
      2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
      // Name
      5, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, '6', '-','5', '6', 
      // UUID ...
      17, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x1, 0xd7, 0xe9, 0x1, 0x4f, 0xf3, 0x44, 0xe7, 0x83, 0x8f, 0xe2, 0x26, 0xb9, 0xe1, 0x56, 0x24,
    };
    
    const uint8_t adv_data_len = sizeof(adv_data);
    

~~~~



### Heartbeat Handler


  The heartbeat handler updates the value of the single Characteristic provided in this example, and request a ATT_EVENT_CAN_SEND_NOW to send a notification if enabled see Listing [here](#lst:ubloxspplecounterheartbeat).


~~~~ {#lst:ubloxspplecounterheartbeat .c caption="{Hearbeat Handler}"}

    static int  counter = 0;
    static char counter_string[30];
    static int  counter_string_len;
    
    static void beat(void){
      counter++;
      counter_string_len = btstack_snprintf_assert_complete(counter_string, sizeof(counter_string), "BTstack counter %03u", counter);
    }
    
    static void ublox_can_send(void * context){
      UNUSED(context);
      beat();
      printf("SEND: %s\n", counter_string);
      ublox_spp_service_server_send(con_handle, (uint8_t*) counter_string, counter_string_len);
    }
    
    static void heartbeat_handler(struct btstack_timer_source *ts){
      if (con_handle != HCI_CON_HANDLE_INVALID) {
        send_request.callback = &ublox_can_send;
        ublox_spp_service_server_request_can_send_now(&send_request, con_handle);
      }
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 

~~~~



### Packet Handler


  The packet handler is used to:
 
- stop the counter after a disconnect




~~~~ {#lst:ubloxspplecounterpacketHandler .c caption="{Packet Handler}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      switch (packet_type) {
        case HCI_EVENT_PACKET:
          switch (hci_event_packet_get_type(packet)) {
            case HCI_EVENT_DISCONNECTION_COMPLETE:
              con_handle = HCI_CON_HANDLE_INVALID;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

~~~~



## LE Central - Test Pairing Methods {#sec:smpairingcentralExample}

Source Code: [sm_pairing_central.c](https://github.com/bluekitchen/btstack/tree/master/example/sm_pairing_central.c)



  Depending on the Authentication requiremens and IO Capabilities, the pairing process uses different short and long term key generation method. This example helps explore the different options incl. LE Secure Connections. It scans for advertisements and connects to the first device that lists a  random service.

### GAP LE setup for receiving advertisements


  GAP LE advertisements are received as custom HCI events of the  GAP_EVENT_ADVERTISING_REPORT type. To receive them, you'll need to register the HCI packet handler, as shown in Listing [here](#lst:smpairingcentralGAPLEAdvSetup).


~~~~ {#lst:smpairingcentralGAPLEAdvSetup .c caption="{Setting up GAP LE client for receiving advertisements}"}

    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void sm_pairing_central_setup(void) {
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
    
      // setup ATT server
      att_server_init(profile_data, NULL, NULL);
    
      // setup GATT Client
      gatt_client_init();
    
      // register handler
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      sm_event_callback_registration.callback = &sm_packet_handler;
      sm_add_event_handler(&sm_event_callback_registration);
    
    
      // Configuration
    
      // Enable mandatory authentication for GATT Client
      // - if un-encrypted connections are not supported, e.g. when connecting to own device, this enforces authentication
      // gatt_client_set_required_security_level(LEVEL_2);
    
      /**
       * Choose ONE of the following configurations
       * Bonding is disabled to allow for repeated testing. It can be enabled by or'ing
       * SM_AUTHREQ_BONDING to the authentication requirements like this:
       * sm_set_authentication_requirements( X | SM_AUTHREQ_BONDING)
    
      // LE Legacy Pairing, Just Works
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
      // sm_set_authentication_requirements(0);
    
      // LE Legacy Pairing, Passkey entry initiator enter, responder (us) displays
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_MITM_PROTECTION);
      // sm_use_fixed_passkey_in_display_role(FIXED_PASSKEY);
    
    #ifdef ENABLE_LE_SECURE_CONNECTIONS
    
      // enable LE Secure Connections Only mode - disables Legacy pairing
      // sm_set_secure_connections_only_mode(true);
    
      // LE Secure Connections, Just Works
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION);
    
      // LE Secure Connections, Numeric Comparison
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    
      // LE Secure Pairing, Passkey entry initiator (us) enters, responder displays
      // sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
      // sm_use_fixed_passkey_in_display_role(FIXED_PASSKEY);
    
      // LE Secure Pairing, Passkey entry initiator (us) displays, responder enters
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    #endif
    }
    

~~~~



### HCI packet handler


  The HCI packet handler has to start the scanning,  and to handle received advertisements. Advertisements are received  as HCI event packets of the GAP_EVENT_ADVERTISING_REPORT type, see Listing [here](#lst:smpairingcentralHCIPacketHandler).  


~~~~ {#lst:smpairingcentralHCIPacketHandler .c caption="{Scanning and receiving advertisements}"}

    
    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
      hci_con_handle_t con_handle;
      uint8_t status;
    
      switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
            printf("Start scaning!\n");
            gap_set_scan_parameters(1,0x0030, 0x0030);
            gap_start_scan(); 
          }
          break;
        case GAP_EVENT_ADVERTISING_REPORT:{
          bd_addr_t address;
          gap_event_advertising_report_get_address(packet, address);
          uint8_t address_type = gap_event_advertising_report_get_address_type(packet);
          uint8_t length = gap_event_advertising_report_get_data_length(packet);
          const uint8_t * data = gap_event_advertising_report_get_data(packet);
          // printf("Advertisement event: addr-type %u, addr %s, data[%u] ",
          //   address_type, bd_addr_to_str(address), length);
          // printf_hexdump(data, length);
          if (!ad_data_contains_uuid16(length, (uint8_t *) data, REMOTE_SERVICE)) break;
          printf("Found remote with UUID %04x, connecting...\n", REMOTE_SERVICE);
          gap_stop_scan();
          gap_connect(address,address_type);
          break;
        }
        case HCI_EVENT_META_GAP:
          // wait for connection complete
          if (hci_event_gap_meta_get_subevent_code(packet) != GAP_SUBEVENT_LE_CONNECTION_COMPLETE) break;
          con_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
          printf("Connection complete\n");
    
          // for testing, choose one of the following actions
    
          // manually start pairing
          sm_request_pairing(con_handle);
    
          // gatt client request to authenticated characteristic in sm_pairing_peripheral (short cut, uses hard-coded value handle)
          // gatt_client_read_value_of_characteristic_using_value_handle(&hci_packet_handler, con_handle, 0x0009);
    
          // general gatt client request to trigger mandatory authentication
          // gatt_client_discover_primary_services(&hci_packet_handler, con_handle);
          break;
        case GATT_EVENT_QUERY_COMPLETE:
          status = gatt_event_query_complete_get_att_status(packet);
          switch (status){
            case ATT_ERROR_INSUFFICIENT_ENCRYPTION:
              printf("GATT Query result: Insufficient Encryption\n");
              break;
            case ATT_ERROR_INSUFFICIENT_AUTHENTICATION:
              printf("GATT Query result: Insufficient Authentication\n");
              break;
            case ATT_ERROR_BONDING_INFORMATION_MISSING:
              printf("GATT Query result: Bonding Information Missing\n");
              break;
            case ATT_ERROR_SUCCESS:
              printf("GATT Query result: OK\n");
              break;
            default:
              printf("GATT Query result: 0x%02x\n", gatt_event_query_complete_get_att_status(packet));
              break;
          }
          break;
        default:
          break;
      }
    }
    

~~~~



### HCI packet handler


  The SM packet handler receives Security Manager Events required for pairing. It also receives events generated during Identity Resolving see Listing [here](#lst:smpairingcentralSMPacketHandler).  


~~~~ {#lst:smpairingcentralSMPacketHandler .c caption="{Scanning and receiving advertisements}"}

    
    static void sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      bd_addr_t addr;
      bd_addr_type_t addr_type;
    
      switch (hci_event_packet_get_type(packet)) {
        case SM_EVENT_JUST_WORKS_REQUEST:
          printf("Just works requested\n");
          sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
          break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
          printf("Confirming numeric comparison: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
          sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
          break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
          printf("Display Passkey: %"PRIu32"\n", sm_event_passkey_display_number_get_passkey(packet));
          break;
        case SM_EVENT_PASSKEY_INPUT_NUMBER:
          printf("Passkey Input requested\n");
          printf("Sending fixed passkey %"PRIu32"\n", (uint32_t) FIXED_PASSKEY);
          sm_passkey_input(sm_event_passkey_input_number_get_handle(packet), FIXED_PASSKEY);
          break;
        case SM_EVENT_PAIRING_STARTED:
          printf("Pairing started\n");
          break;
        case SM_EVENT_PAIRING_COMPLETE:
          switch (sm_event_pairing_complete_get_status(packet)){
            case ERROR_CODE_SUCCESS:
              printf("Pairing complete, success. CTKD active %u\n", sm_event_pairing_complete_get_ctkd_active(packet));
              break;
            case ERROR_CODE_CONNECTION_TIMEOUT:
              printf("Pairing failed, timeout\n");
              break;
            case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
              printf("Pairing failed, disconnected\n");
              break;
            case ERROR_CODE_AUTHENTICATION_FAILURE:
              printf("Pairing failed, authentication failure with reason = %u\n", sm_event_pairing_complete_get_reason(packet));
              break;
            default:
              break;
          }
          break;
        case SM_EVENT_REENCRYPTION_STARTED:
          sm_event_reencryption_complete_get_address(packet, addr);
          printf("Bonding information exists for addr type %u, identity addr %s -> start re-encryption\n",
               sm_event_reencryption_started_get_addr_type(packet), bd_addr_to_str(addr));
          break;
        case SM_EVENT_REENCRYPTION_COMPLETE:
          switch (sm_event_reencryption_complete_get_status(packet)){
            case ERROR_CODE_SUCCESS:
              printf("Re-encryption complete, success\n");
              break;
            case ERROR_CODE_CONNECTION_TIMEOUT:
              printf("Re-encryption failed, timeout\n");
              break;
            case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
              printf("Re-encryption failed, disconnected\n");
              break;
            case ERROR_CODE_PIN_OR_KEY_MISSING:
              printf("Re-encryption failed, bonding information missing\n\n");
              printf("Assuming remote lost bonding information\n");
              printf("Deleting local bonding information and start new pairing...\n");
              sm_event_reencryption_complete_get_address(packet, addr);
              addr_type = sm_event_reencryption_started_get_addr_type(packet);
              gap_delete_bonding(addr_type, addr);
              sm_request_pairing(sm_event_reencryption_complete_get_handle(packet));
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

~~~~



## LE Peripheral - Test Pairing Methods {#sec:smpairingperipheralExample}

Source Code: [sm_pairing_peripheral.c](https://github.com/bluekitchen/btstack/tree/master/example/sm_pairing_peripheral.c)



  Depending on the Authentication requiremens and IO Capabilities, the pairing process uses different short and long term key generation method. This example helps explore the different options incl. LE Secure Connections.

### Main Application Setup


  Listing [here](#lst:smpairingperipheralMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled ATT Database generated from $sm_pairing_peripheral.gatt$. Finally, it configures the advertisements  and boots the Bluetooth stack.  In this example, the Advertisement contains the Flags attribute, the device name, and a 16-bit (test) service 0x1111 The flag 0x06 indicates: LE General Discoverable Mode and BR/EDR not supported. Various examples for IO Capabilites and Authentication Requirements are given below.


~~~~ {#lst:smpairingperipheralMainConfiguration .c caption="{Setup stack to advertise}"}

    static btstack_packet_callback_registration_t sm_event_callback_registration;
    static btstack_packet_callback_registration_t hci_event_callback_registration;
    
    static void sm_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    const uint8_t adv_data[] = {
      // Flags general discoverable, BR/EDR not supported
      0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
      // Name
      0x0b, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'S', 'M', ' ', 'P', 'a', 'i', 'r', 'i', 'n', 'g', 
      // Incomplete List of 16-bit Service Class UUIDs -- 1111 - only valid for testing!
      0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x11, 0x11,
    };
    const uint8_t adv_data_len = sizeof(adv_data);
    
    static void sm_peripheral_setup(void){
    
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
    
      // setup ATT server
      att_server_init(profile_data, NULL, NULL);
    
      // setup GATT Client
      gatt_client_init();
    
      // setup advertisements
      uint16_t adv_int_min = 0x0030;
      uint16_t adv_int_max = 0x0030;
      uint8_t adv_type = 0;
      bd_addr_t null_addr;
      memset(null_addr, 0, 6);
      gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
      gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
      gap_advertisements_enable(1);
    
      // register handler
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      sm_event_callback_registration.callback = &sm_packet_handler;
      sm_add_event_handler(&sm_event_callback_registration);
    
      // Configuration
    
      // Enable mandatory authentication for GATT Client
      // - if un-encrypted connections are not supported, e.g. when connecting to own device, this enforces authentication
      // gatt_client_set_required_security_level(LEVEL_2);
    
      /**
       * Choose ONE of the following configurations
       * Bonding is disabled to allow for repeated testing. It can be enabled by or'ing
       * SM_AUTHREQ_BONDING to the authentication requirements like this:
       * sm_set_authentication_requirements( X | SM_AUTHREQ_BONDING)
    
      // LE Legacy Pairing, Just Works
      // sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
      // sm_set_authentication_requirements(0);
    
      // LE Legacy Pairing, Passkey entry initiator enter, responder (us) displays
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_MITM_PROTECTION);
      // sm_use_fixed_passkey_in_display_role(123456);
    
    #ifdef ENABLE_LE_SECURE_CONNECTIONS
    
      // enable LE Secure Connections Only mode - disables Legacy pairing
      // sm_set_secure_connections_only_mode(true);
    
      // LE Secure Connections, Just Works
      // sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION);
    
      // LE Secure Connections, Numeric Comparison
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    
      // LE Secure Pairing, Passkey entry initiator enter, responder (us) displays
      // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
      // sm_use_fixed_passkey_in_display_role(123456);
    
      // LE Secure Pairing, Passkey entry initiator displays, responder (us) enter
      // sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_ONLY);
      // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    #endif
    }
    

~~~~



### Security Manager Packet Handler


  The packet handler is used to handle Security Manager events


~~~~ {#lst:smpairingperipheralpacketHandler .c caption="{Security Manager Packet Handler}"}



### HCI Packet Handler
    static void sm_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      hci_con_handle_t con_handle;
      bd_addr_t addr;
      bd_addr_type_t addr_type;
      uint8_t status;
    
      switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_META_GAP:
          switch (hci_event_gap_meta_get_subevent_code(packet)) {
            case GAP_SUBEVENT_LE_CONNECTION_COMPLETE:
              printf("Connection complete\n");
              con_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
              UNUSED(con_handle);
    
              // for testing, choose one of the following actions
    
              // manually start pairing
              // sm_request_pairing(con_handle);
    
              // gatt client request to authenticated characteristic in sm_pairing_central (short cut, uses hard-coded value handle)
              // gatt_client_read_value_of_characteristic_using_value_handle(&packet_handler, con_handle, 0x0009);
    
              // general gatt client request to trigger mandatory authentication
              // gatt_client_discover_primary_services(&packet_handler, con_handle);
              break;
            default:
              break;
          }
          break;
        case SM_EVENT_JUST_WORKS_REQUEST:
          printf("Just Works requested\n");
          sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
          break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
          printf("Confirming numeric comparison: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
          sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
          break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
          printf("Display Passkey: %"PRIu32"\n", sm_event_passkey_display_number_get_passkey(packet));
          break;
        case SM_EVENT_IDENTITY_CREATED:
          sm_event_identity_created_get_identity_address(packet, addr);
          printf("Identity created: type %u address %s\n", sm_event_identity_created_get_identity_addr_type(packet), bd_addr_to_str(addr));
          break;
        case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
          sm_event_identity_resolving_succeeded_get_identity_address(packet, addr);
          printf("Identity resolved: type %u address %s\n", sm_event_identity_resolving_succeeded_get_identity_addr_type(packet), bd_addr_to_str(addr));
          break;
        case SM_EVENT_IDENTITY_RESOLVING_FAILED:
          sm_event_identity_created_get_address(packet, addr);
          printf("Identity resolving failed\n");
          break;
        case SM_EVENT_PAIRING_STARTED:
          printf("Pairing started\n");
          break;
        case SM_EVENT_PAIRING_COMPLETE:
          switch (sm_event_pairing_complete_get_status(packet)){
            case ERROR_CODE_SUCCESS:
              printf("Pairing complete, success. CTKD active %u\n", sm_event_pairing_complete_get_ctkd_active(packet));
              break;
            case ERROR_CODE_CONNECTION_TIMEOUT:
              printf("Pairing failed, timeout\n");
              break;
            case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
              printf("Pairing failed, disconnected\n");
              break;
            case ERROR_CODE_AUTHENTICATION_FAILURE:
              printf("Pairing failed, authentication failure with reason = %u\n", sm_event_pairing_complete_get_reason(packet));
              break;
            default:
              break;
          }
          break;
        case SM_EVENT_REENCRYPTION_STARTED:
          sm_event_reencryption_complete_get_address(packet, addr);
          printf("Bonding information exists for addr type %u, identity addr %s -> re-encryption started\n",
               sm_event_reencryption_started_get_addr_type(packet), bd_addr_to_str(addr));
          break;
        case SM_EVENT_REENCRYPTION_COMPLETE:
          switch (sm_event_reencryption_complete_get_status(packet)){
            case ERROR_CODE_SUCCESS:
              printf("Re-encryption complete, success\n");
              break;
            case ERROR_CODE_CONNECTION_TIMEOUT:
              printf("Re-encryption failed, timeout\n");
              break;
            case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
              printf("Re-encryption failed, disconnected\n");
              break;
            case ERROR_CODE_PIN_OR_KEY_MISSING:
              printf("Re-encryption failed, bonding information missing\n\n");
              printf("Assuming remote lost bonding information\n");
              printf("Deleting local bonding information to allow for new pairing...\n");
              sm_event_reencryption_complete_get_address(packet, addr);
              addr_type = sm_event_reencryption_started_get_addr_type(packet);
              gap_delete_bonding(addr_type, addr);
              break;
            default:
              break;
          }
          break;
        case GATT_EVENT_QUERY_COMPLETE:
          status = gatt_event_query_complete_get_att_status(packet);
          switch (status){
            case ATT_ERROR_INSUFFICIENT_ENCRYPTION:
              printf("GATT Query failed, Insufficient Encryption\n");
              break;
            case ATT_ERROR_INSUFFICIENT_AUTHENTICATION:
              printf("GATT Query failed, Insufficient Authentication\n");
              break;
            case ATT_ERROR_BONDING_INFORMATION_MISSING:
              printf("GATT Query failed, Bonding Information Missing\n");
              break;
            case ATT_ERROR_SUCCESS:
              printf("GATT Query successful\n");
              break;
            default:
              printf("GATT Query failed, status 0x%02x\n", gatt_event_query_complete_get_att_status(packet));
              break;
          }
          break;
        default:
          break;
      }
    }
    
    /*
     *
    static void hci_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      hci_con_handle_t con_handle;
    
      switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_META_GAP:
          switch (hci_event_gap_meta_get_subevent_code(packet)) {
            case GAP_SUBEVENT_LE_CONNECTION_COMPLETE:
              printf("Connection complete\n");
              con_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
              UNUSED(con_handle);
    
              // for testing, choose one of the following actions
    
              // manually start pairing
              // sm_request_pairing(con_handle);
    
              // gatt client request to authenticated characteristic in sm_pairing_central (short cut, uses hard-coded value handle)
              // gatt_client_read_value_of_characteristic_using_value_handle(&packet_handler, con_handle, 0x0009);
    
              // general gatt client request to trigger mandatory authentication
              // gatt_client_discover_primary_services(&packet_handler, con_handle);
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }
    

~~~~



  The packet handler is used to handle new connections, can trigger Security Request

## LE Credit-Based Flow-Control Mode Client - Send Data over L2CAP {#sec:lecreditbasedflowcontrolmodeclientExample}

Source Code: [le_credit_based_flow_control_mode_client.c](https://github.com/bluekitchen/btstack/tree/master/example/le_credit_based_flow_control_mode_client.c)



  Connects to 'LE CBM Server' and streams data via L2CAP Channel in LE Credit-Based Flow-Control Mode (CBM)

### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:lecreditbasedflowcontrolmodeclienttracking .c caption="{Tracking throughput}"}

    
    #define REPORT_INTERVAL_MS 3000
    
    // support for multiple clients
    typedef struct {
      char name;
      hci_con_handle_t connection_handle;
      uint16_t cid;
      int  counter;
      char test_data[TEST_PACKET_SIZE];
      int  test_data_len;
      uint32_t test_data_sent;
      uint32_t test_data_start;
    } le_cbm_connection_t;
    
    static le_cbm_connection_t le_cbm_connection;
    
    static void test_reset(le_cbm_connection_t * context){
      context->test_data_start = btstack_run_loop_get_time_ms();
      context->test_data_sent = 0;
    }
    
    static void test_track_data(le_cbm_connection_t * context, int bytes_transferred){
      context->test_data_sent += bytes_transferred;
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - context->test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second = context->test_data_sent * 1000 / time_passed;
      printf("%c: %"PRIu32" bytes -> %u.%03u kB/s\n", context->name, context->test_data_sent, bytes_per_second / 1000, bytes_per_second % 1000);
    
      // restart
      context->test_data_start = now;
      context->test_data_sent  = 0;
    }

~~~~



### Streamer


  The streamer function checks if notifications are enabled and if a notification can be sent now. It creates some test data - a single letter that gets increased every time - and tracks the data sent.


~~~~ {#lst:lecreditbasedflowcontrolmodeclientstreamer .c caption="{Streaming code}"}

    static void streamer(void){
    
      // create test data
      le_cbm_connection.counter++;
      if (le_cbm_connection.counter > 'Z') le_cbm_connection.counter = 'A';
      memset(le_cbm_connection.test_data, le_cbm_connection.counter, le_cbm_connection.test_data_len);
    
      // send
      l2cap_send(le_cbm_connection.cid, (uint8_t *) le_cbm_connection.test_data, le_cbm_connection.test_data_len);
    
      // track
      test_track_data(&le_cbm_connection, le_cbm_connection.test_data_len);
    
      // request another packet
      l2cap_request_can_send_now_event(le_cbm_connection.cid);
    } 

~~~~



### SM Packet Handler


  The packet handler is used to handle pairing requests

## LE Credit-Based Flow-Control Mode Server - Receive data over L2CAP {#sec:lecreditbasedflowcontrolmodeserverExample}

Source Code: [le_credit_based_flow_control_mode_server.c](https://github.com/bluekitchen/btstack/tree/master/example/le_credit_based_flow_control_mode_server.c)



  iOS 11 and newer supports L2CAP channels in LE Credit-Based Flow-Control Mode for fast transfer over LE [https://github.com/bluekitchen/CBL2CAPChannel-Demo](Basic iOS example on GitHub)

### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:lecreditbasedflowcontrolmodeservertracking .c caption="{Tracking throughput}"}

    
    static void test_reset(le_cbm_connection_t * context){
      context->test_data_start = btstack_run_loop_get_time_ms();
      context->test_data_sent = 0;
    }
    
    static void test_track_data(le_cbm_connection_t * context, int bytes_transferred){
      context->test_data_sent += bytes_transferred;
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - context->test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second = context->test_data_sent * 1000 / time_passed;
      printf("%c: %"PRIu32" bytes sent-> %u.%03u kB/s\n", context->name, context->test_data_sent, bytes_per_second / 1000, bytes_per_second % 1000);
    
      // restart
      context->test_data_start = now;
      context->test_data_sent  = 0;
    }

~~~~



### Streamer


  The streamer function checks if notifications are enabled and if a notification can be sent now. It creates some test data - a single letter that gets increased every time - and tracks the data sent.


~~~~ {#lst:lecreditbasedflowcontrolmodeserverstreamer .c caption="{Streaming code}"}

    static void streamer(void){
    
      if (le_cbm_connection.cid == 0) return;
    
      // create test data
      le_cbm_connection.counter++;
      if (le_cbm_connection.counter > 'Z') le_cbm_connection.counter = 'A';
      memset(le_cbm_connection.test_data, le_cbm_connection.counter, le_cbm_connection.test_data_len);
    
      // send
      l2cap_send(le_cbm_connection.cid, (uint8_t *) le_cbm_connection.test_data, le_cbm_connection.test_data_len);
    
      // track
      test_track_data(&le_cbm_connection, le_cbm_connection.test_data_len);
    
      // request another packet
      l2cap_request_can_send_now_event(le_cbm_connection.cid);
    } 

~~~~



### HCI + L2CAP Packet Handler


  The packet handler is used to stop the notifications and reset the MTU on connect It would also be a good place to request the connection parameter update as indicated  in the commented code block.


~~~~ {#lst:lecreditbasedflowcontrolmodeserverpacketHandler .c caption="{Packet Handler}"}



### SM Packet Handler


### Main Application Setup


## LE Peripheral - Delayed Response {#sec:attdelayedresponseExample}

Source Code: [att_delayed_response.c](https://github.com/bluekitchen/btstack/tree/master/example/att_delayed_response.c)



  If the value for a GATT Chararacteristic isn't availabl for read, the value ATT_READ_RESPONSE_PENDING can be returned. When the value is available,  att_server_response_ready is then called to complete the ATT request.

 Similarly, the error code ATT_ERROR_WRITE_RESPONSE_PENING can be returned when it is unclear if a write can be performed or not. When the decision was made, att_server_response_ready is is then called to complete the ATT request.

### Main Application Setup


  Listing [here](#lst:attdelayedresponseMainConfiguration) shows main application code. It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled ATT Database generated from $att_delayed_response.gatt$.  Additionally, it enables the Battery Service Server with the current battery level. Finally, it configures the advertisements and boots the Bluetooth stack.  In this example, the Advertisement contains the Flags attribute and the device name. The flag 0x06 indicates: LE General Discoverable Mode and BR/EDR not supported.


~~~~ {#lst:attdelayedresponseMainConfiguration .c caption="{Init L2CAP SM ATT Server}"}

    #ifdef ENABLE_ATT_DELAYED_RESPONSE
    static btstack_timer_source_t att_timer;
    static hci_con_handle_t con_handle;
    static int value_ready;
    #endif
    
    static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
    
    const uint8_t adv_data[] = {
      // Flags general discoverable, BR/EDR not supported
      0x02, 0x01, 0x06, 
      // Name
      0x08, 0x09, 'D', 'e', 'l', 'a', 'y', 'e', 'd', 
    };
    const uint8_t adv_data_len = sizeof(adv_data);
    
    const char * test_string = "Delayed response";
    
    static void example_setup(void){
    
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
    
      // setup ATT server
      att_server_init(profile_data, att_read_callback, att_write_callback);
    
      // setup advertisements
      uint16_t adv_int_min = 0x0030;
      uint16_t adv_int_max = 0x0030;
      uint8_t adv_type = 0;
      bd_addr_t null_addr;
      memset(null_addr, 0, 6);
      gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
      gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
      gap_advertisements_enable(1);
    }

~~~~



### att_invalidate_value Handler


  The att_invalidate_value handler 'invalidates' the value of the single Characteristic provided in this example

### att_update_value Handler


  The att_update_value handler 'updates' the value of the single Characteristic provided in this example


~~~~ {#lst:attdelayedresponseattreaddelay .c caption="{ATT Read Delay Handler}"}

    #ifdef ENABLE_ATT_DELAYED_RESPONSE
    static void att_update_value(struct btstack_timer_source *ts){
      UNUSED(ts);
      value_ready = 1;
    
      // trigger ATT Server to try request again
      int status = att_server_response_ready(con_handle);
    
      printf("Value updated -> complete ATT request - status 0x%02x\n", status);
    
      // simulated value becoming stale again
      att_timer.process = &att_invalidate_value;
      btstack_run_loop_set_timer(&att_timer, ATT_VALUE_DELAY_MS);
      btstack_run_loop_add_timer(&att_timer);
    } 
    #endif
    

~~~~



### ATT Read


  The ATT Server handles all reads to constant data. For dynamic data like the custom characteristic, the registered att_read_callback is called. To handle long characteristics and long reads, the att_read_callback is first called with buffer == NULL, to request the total value length. Then it will be called again requesting a chunk of the value. See Listing [here](#lst:attdelayedresponseattRead).


~~~~ {#lst:attdelayedresponseattRead .c caption="{ATT Read}"}



### ATT Write
    
    // ATT Client Read Callback for Dynamic Data
    // - if buffer == NULL, don't copy data, just return size of value
    // - if buffer != NULL, copy data and return number bytes copied
    // @param offset defines start of attribute value
    static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    
    #ifdef ENABLE_ATT_DELAYED_RESPONSE
      switch (att_handle){
        case ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE:
          if (value_ready){
            return att_read_callback_handle_blob((const uint8_t *)test_string, strlen(test_string), offset, buffer, buffer_size);
          } else {
            printf("Read callback for handle 0x%02x, but value not ready -> report response pending\n", att_handle);
            con_handle = connection_handle;
            return ATT_READ_RESPONSE_PENDING;
          }
          break;
        case ATT_READ_RESPONSE_PENDING:
          // virtual handle indicating all attributes have been queried
          printf("Read callback for virtual handle 0x%02x - all attributes have been queried (important for read multiple or read by type) -> start updating values\n", att_handle);
          // simulated delayed response for example
          att_timer.process = &att_update_value;
          btstack_run_loop_set_timer(&att_timer, ATT_VALUE_DELAY_MS);
          btstack_run_loop_add_timer(&att_timer);
          return 0;
        default:
          break;
      }
    #else
      UNUSED(connection_handle);
      // useless code when ENABLE_ATT_DELAYED_RESPONSE is not defined - but avoids built errors
      if (att_handle == ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE){
        printf("ENABLE_ATT_DELAYED_RESPONSE not defined in btstack_config.h, responding right away");
        return att_read_callback_handle_blob((const uint8_t *)test_string, (uint16_t) strlen(test_string), offset, buffer, buffer_size);
      }
    #endif
    
      return 0;
    }
    
    /*
    
    static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
      UNUSED(transaction_mode);
      UNUSED(offset);
      UNUSED(buffer_size);
      UNUSED(connection_handle);
      
      if (att_handle == ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE) {
        printf("Write request, value: ");
        printf_hexdump(buffer, buffer_size);
    #ifdef ENABLE_ATT_DELAYED_RESPONSE
        if (value_ready){
          printf("Write callback, value ready\n");
          return 0;          
        } else {
          printf("Write callback for handle 0x%02x, but not ready -> return response pending\n", att_handle);
        }
        // simulated delayed response for example
        att_timer.process = &att_update_value;
        btstack_run_loop_set_timer(&att_timer, ATT_VALUE_DELAY_MS);
        btstack_run_loop_add_timer(&att_timer);
        return ATT_ERROR_WRITE_RESPONSE_PENDING;
    #else
        printf("ENABLE_ATT_DELAYED_RESPONSE not defined in btstack_config.h, responding right away");
        return 0;
    #endif
      }
      return 0;
    }
    

~~~~



## LE ANCS Client - Apple Notification Service {#sec:ancsclientdemoExample}

Source Code: [ancs_client_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/ancs_client_demo.c)



## LE Man-in-the-Middle Tool {#sec:lemitmExample}

Source Code: [le_mitm.c](https://github.com/bluekitchen/btstack/tree/master/example/le_mitm.c)



  The example first does an LE scan and allows the user to select a Peripheral device. Then, it connects to the Peripheral and starts advertising with the same data as the Peripheral device. ATT Requests and responses are forwarded between the peripheral and the central Security requests are handled locally.

 @note A Bluetooth Controller that supports Central and Peripheral Role at the same time is required for this example. See chipset/README.md



## Performance - Stream Data over GATT (Client) {#sec:lestreamerclientExample}

Source Code: [le_streamer_client.c](https://github.com/bluekitchen/btstack/tree/master/example/le_streamer_client.c)



  Connects to 'LE Streamer' and subscribes to test characteristic



### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:lestreamerclienttracking .c caption="{Tracking throughput}"}

    
    #define TEST_MODE_WRITE_WITHOUT_RESPONSE 1
    #define TEST_MODE_DUPLEX         3
    
    // configure test mode: send only, receive only, full duplex
    #define TEST_MODE TEST_MODE_DUPLEX
    
    #define REPORT_INTERVAL_MS 1000
    
    static void test_reset(le_streamer_client_connection_t * context){
      context->test_data_start = btstack_run_loop_get_time_ms();
      context->test_data_sent = 0;
      context->test_data_received = 0;
    }
    
    static void test_track_data(le_streamer_client_connection_t * context, uint32_t bytes_sent, uint32_t bytes_received){
      context->test_data_sent   += bytes_sent;
      context->test_data_received += bytes_received;
    
      // report
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - context->test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
    
      // print throughput
      int bytes_per_second_sent   = context->test_data_sent   * 1000 / time_passed;
      int bytes_per_second_received = context->test_data_received * 1000 / time_passed;
      int bytes_per_second_total = bytes_per_second_sent + bytes_per_second_received;
      printf("%c: Sent %u.%03u kB/s, Received %u.%03u kB/s => Total %u.%03u kB/s\n", context->name,
        bytes_per_second_sent   / 1000, bytes_per_second_sent   % 1000,
        bytes_per_second_received / 1000, bytes_per_second_received % 1000,
        bytes_per_second_total  / 1000, bytes_per_second_total  % 1000);
    
      // restart
      test_reset(context);
    }

~~~~



## Performance - Stream Data over SPP (Client) {#sec:sppstreamerclientExample}

Source Code: [spp_streamer_client.c](https://github.com/bluekitchen/btstack/tree/master/example/spp_streamer_client.c)



  Note: The SPP Streamer Client scans for and connects to SPP Streamer, and measures the throughput.

### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:sppstreamerclienttracking .c caption="{Tracking throughput}"}

    #define REPORT_INTERVAL_MS 3000
    static uint32_t test_data_transferred;
    static uint32_t test_data_start;
    
    static void test_reset(void){
      test_data_start = btstack_run_loop_get_time_ms();
      test_data_transferred = 0;
    }
    
    static void test_track_transferred(int bytes_sent){
      test_data_transferred += bytes_sent;
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second = test_data_transferred * 1000 / time_passed;
      printf("%u bytes -> %u.%03u kB/s\n", (int) test_data_transferred, (int) bytes_per_second / 1000, bytes_per_second % 1000);
    
      // restart
      test_data_start = now;
      test_data_transferred  = 0;
    }

~~~~



### SDP Query Packet Handler


  Store RFCOMM Channel for SPP service and initiates RFCOMM connection

### Gerenal Packet Handler


  Handles startup (BTSTACK_EVENT_STATE), inquiry, pairing, starts SDP query for SPP service, and RFCOMM connection

### Main Application Setup


  As with the packet and the heartbeat handlers, the combined app setup contains the code from the individual example setups.


~~~~ {#lst:sppstreamerclientMainConfiguration .c caption="{Init L2CAP RFCOMM SDO SM ATT Server and start heartbeat timer}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      UNUSED(argc);
      (void)argv;
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      rfcomm_init();
    
    #ifdef ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE_FOR_RFCOMM
      // setup ERTM management
      rfcomm_enable_l2cap_ertm(&rfcomm_ertm_request_handler, &rfcomm_ertm_released_handler);
    #endif
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // init SDP
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
    
      // turn on!
    	hci_power_control(HCI_POWER_ON);
    	  
      return 0;
    }

~~~~



## Performance - Stream Data over SPP (Server) {#sec:sppstreamerExample}

Source Code: [spp_streamer.c](https://github.com/bluekitchen/btstack/tree/master/example/spp_streamer.c)



  After RFCOMM connections gets open, request a RFCOMM_EVENT_CAN_SEND_NOW via rfcomm_request_can_send_now_event().

  When we get the RFCOMM_EVENT_CAN_SEND_NOW, send data and request another one.



  Note: To test, run the example, pair from a remote  device, and open the Virtual Serial Port.

### Track throughput


  We calculate the throughput by setting a start time and measuring the amount of  data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s and reset the counter and start time.


~~~~ {#lst:sppstreamertracking .c caption="{Tracking throughput}"}

    #define REPORT_INTERVAL_MS 3000
    static uint32_t test_data_transferred;
    static uint32_t test_data_start;
    
    static void test_reset(void){
      test_data_start = btstack_run_loop_get_time_ms();
      test_data_transferred = 0;
    }
    
    static void test_track_transferred(int bytes_sent){
      test_data_transferred += bytes_sent;
      // evaluate
      uint32_t now = btstack_run_loop_get_time_ms();
      uint32_t time_passed = now - test_data_start;
      if (time_passed < REPORT_INTERVAL_MS) return;
      // print speed
      int bytes_per_second = test_data_transferred * 1000 / time_passed;
      printf("%u bytes -> %u.%03u kB/s\n", (int) test_data_transferred, (int) bytes_per_second / 1000, bytes_per_second % 1000);
    
      // restart
      test_data_start = now;
      test_data_transferred  = 0;
    }

~~~~



### Packet Handler


  The packet handler of the combined example is just the combination of the individual packet handlers.

### Main Application Setup


  As with the packet and the heartbeat handlers, the combined app setup contains the code from the individual example setups.


~~~~ {#lst:sppstreamerMainConfiguration .c caption="{Init L2CAP RFCOMM SDP SPP}"}

    int btstack_main(int argc, const char * argv[])
    {
      (void)argc;
      (void)argv;
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      rfcomm_init();
      rfcomm_register_service(packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff);
    
    #ifdef ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE_FOR_RFCOMM
      // setup ERTM management
      rfcomm_enable_l2cap_ertm(&rfcomm_ertm_request_handler, &rfcomm_ertm_released_handler);
    #endif
    
      // init SDP, create record for SPP and register with SDP
      sdp_init();
      memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
      spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), RFCOMM_SERVER_CHANNEL, "SPP Streamer");
      btstack_assert(de_get_len( spp_service_buffer) <= sizeof(spp_service_buffer));
      sdp_register_service(spp_service_buffer);
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // short-cut to find other SPP Streamer
      gap_set_class_of_device(TEST_COD);
    
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_set_local_name("SPP Streamer 00:00:00:00:00:00");
      gap_discoverable_control(1);
    
      spp_create_test_data();
    
      // turn on!
    	hci_power_control(HCI_POWER_ON);
    	  
      return 0;
    }

~~~~



## A2DP Sink - Receive Audio Stream and Control Playback {#sec:a2dpsinkdemoExample}

Source Code: [a2dp_sink_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/a2dp_sink_demo.c)



  This A2DP Sink example demonstrates how to use the A2DP Sink service to  receive an audio data stream from a remote A2DP Source device. In addition, the AVRCP Controller is used to get information on currently played media,  such are title, artist and album, as well as to control the playback,  i.e. to play, stop, repeat, etc. If HAVE_BTSTACK_STDIN is set, press SPACE on  the console to show the available AVDTP and AVRCP commands.



  To test with a remote device, e.g. a mobile phone, pair from the remote device with the demo, then start playing music on the remote device. Alternatively, set the device_addr_string to the Bluetooth address of your  remote device in the code, and call connect from the UI.



  For more info on BTstack audio, see our blog post  [A2DP Sink and Source on STM32 F4 Discovery Board](http://bluekitchen-gmbh.com/a2dp-sink-and-source-on-stm32-f4-discovery-board/).



### Main Application Setup


  The Listing [here](#lst:a2dpsinkdemoMainConfiguration) shows how to set up AD2P Sink and AVRCP services. Besides calling init() method for each service, you'll also need to register several packet handlers:
 
- hci_packet_handler - handles legacy pairing, here by using fixed '0000' pin code.
- a2dp_sink_packet_handler - handles events on stream connection status (established, released), the media codec configuration, and, the status of the stream itself (opened, paused, stopped).
- handle_l2cap_media_data_packet - used to receive streaming data. If STORE_TO_WAV_FILE directive (check btstack_config.h) is used, the SBC decoder will be used to decode the SBC data into PCM frames. The resulting PCM frames are then processed in the SBC Decoder callback.
- avrcp_packet_handler - receives AVRCP connect/disconnect event.
- avrcp_controller_packet_handler - receives answers for sent AVRCP commands.
- avrcp_target_packet_handler - receives AVRCP commands, and registered notifications.
- stdin_process - used to trigger AVRCP commands to the A2DP Source device, such are get now playing info, start, stop, volume control. Requires HAVE_BTSTACK_STDIN.



  To announce A2DP Sink and AVRCP services, you need to create corresponding SDP records and register them with the SDP service. 



  Note, currently only the SBC codec is supported.  If you want to store the audio data in a file, you'll need to define STORE_TO_WAV_FILE.  If STORE_TO_WAV_FILE directive is defined, the SBC decoder needs to get initialized when a2dp_sink_packet_handler receives event A2DP_SUBEVENT_STREAM_STARTED.  The initialization of the SBC decoder requires a callback that handles PCM data:
 
- handle_pcm_data - handles PCM audio frames. Here, they are stored in a wav file if STORE_TO_WAV_FILE is defined, and/or played using the audio library.




~~~~ {#lst:a2dpsinkdemoMainConfiguration .c caption="{Setup Audio Sink and AVRCP services}"}

    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void a2dp_sink_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t * packet, uint16_t event_size);
    static void handle_l2cap_media_data_packet(uint8_t seid, uint8_t *packet, uint16_t size);
    static void avrcp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void avrcp_controller_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void avrcp_target_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    #ifdef HAVE_BTSTACK_STDIN
    static void stdin_process(char cmd);
    #endif
    
    static int setup_demo(void){
    
      // init protocols
      l2cap_init();
      sdp_init();
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    #ifdef ENABLE_AVRCP_COVER_ART
      goep_client_init();
      avrcp_cover_art_client_init();
    #endif
    
      // Init profiles
      a2dp_sink_init();
      avrcp_init();
      avrcp_controller_init();
      avrcp_target_init();
    
    
      // Configure A2DP Sink
      a2dp_sink_register_packet_handler(&a2dp_sink_packet_handler);
      a2dp_sink_register_media_handler(&handle_l2cap_media_data_packet);
      a2dp_sink_demo_stream_endpoint_t * stream_endpoint = &a2dp_sink_demo_stream_endpoint;
      avdtp_stream_endpoint_t * local_stream_endpoint = a2dp_sink_create_stream_endpoint(AVDTP_AUDIO,
                                                 AVDTP_CODEC_SBC, media_sbc_codec_capabilities, sizeof(media_sbc_codec_capabilities),
                                                 stream_endpoint->media_sbc_codec_configuration, sizeof(stream_endpoint->media_sbc_codec_configuration));
      btstack_assert(local_stream_endpoint != NULL);
      // - Store stream enpoint's SEP ID, as it is used by A2DP API to identify the stream endpoint
      stream_endpoint->a2dp_local_seid = avdtp_local_seid(local_stream_endpoint);
    
    
      // Configure AVRCP Controller + Target
      avrcp_register_packet_handler(&avrcp_packet_handler);
      avrcp_controller_register_packet_handler(&avrcp_controller_packet_handler);
      avrcp_target_register_packet_handler(&avrcp_target_packet_handler);
      
    
      // Configure SDP
    
      // - Create and register A2DP Sink service record
      memset(sdp_avdtp_sink_service_buffer, 0, sizeof(sdp_avdtp_sink_service_buffer));
      a2dp_sink_create_sdp_record(sdp_avdtp_sink_service_buffer, sdp_create_service_record_handle(),
                    AVDTP_SINK_FEATURE_MASK_HEADPHONE, NULL, NULL);
      btstack_assert(de_get_len( sdp_avdtp_sink_service_buffer) <= sizeof(sdp_avdtp_sink_service_buffer));
      sdp_register_service(sdp_avdtp_sink_service_buffer);
    
      // - Create AVRCP Controller service record and register it with SDP. We send Category 1 commands to the media player, e.g. play/pause
      memset(sdp_avrcp_controller_service_buffer, 0, sizeof(sdp_avrcp_controller_service_buffer));
      uint16_t controller_supported_features = 1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER;
    #ifdef AVRCP_BROWSING_ENABLED
      controller_supported_features |= 1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_BROWSING;
    #endif
    #ifdef ENABLE_AVRCP_COVER_ART
      controller_supported_features |= 1 << AVRCP_CONTROLLER_SUPPORTED_FEATURE_COVER_ART_GET_LINKED_THUMBNAIL;
    #endif
      avrcp_controller_create_sdp_record(sdp_avrcp_controller_service_buffer, sdp_create_service_record_handle(),
                         controller_supported_features, NULL, NULL);
      btstack_assert(de_get_len( sdp_avrcp_controller_service_buffer) <= sizeof(sdp_avrcp_controller_service_buffer));
      sdp_register_service(sdp_avrcp_controller_service_buffer);
    
      // - Create and register A2DP Sink service record
      //   -  We receive Category 2 commands from the media player, e.g. volume up/down
      memset(sdp_avrcp_target_service_buffer, 0, sizeof(sdp_avrcp_target_service_buffer));
      uint16_t target_supported_features = 1 << AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_MONITOR_OR_AMPLIFIER;
      avrcp_target_create_sdp_record(sdp_avrcp_target_service_buffer,
                       sdp_create_service_record_handle(), target_supported_features, NULL, NULL);
      btstack_assert(de_get_len( sdp_avrcp_target_service_buffer) <= sizeof(sdp_avrcp_target_service_buffer));
      sdp_register_service(sdp_avrcp_target_service_buffer);
    
      // - Create and register Device ID (PnP) service record
      memset(device_id_sdp_service_buffer, 0, sizeof(device_id_sdp_service_buffer));
      device_id_create_sdp_record(device_id_sdp_service_buffer,
                    sdp_create_service_record_handle(), DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
      btstack_assert(de_get_len( device_id_sdp_service_buffer) <= sizeof(device_id_sdp_service_buffer));
      sdp_register_service(device_id_sdp_service_buffer);
    
    
      // Configure GAP - discovery / connection
    
      // - Set local name with a template Bluetooth address, that will be automatically
      //   replaced with an actual address once it is available, i.e. when BTstack boots
      //   up and starts talking to a Bluetooth module.
      gap_set_local_name("A2DP Sink Demo 00:00:00:00:00:00");
    
      // - Allow to show up in Bluetooth inquiry
      gap_discoverable_control(1);
    
      // - Set Class of Device - Service Class: Audio, Major Device Class: Audio, Minor: Headphone
      gap_set_class_of_device(0x200404);
    
      // - Allow for role switch in general and sniff mode
      gap_set_default_link_policy_settings( LM_LINK_POLICY_ENABLE_ROLE_SWITCH | LM_LINK_POLICY_ENABLE_SNIFF_MODE );
    
      // - Allow for role switch on outgoing connections
      //   - This allows A2DP Source, e.g. smartphone, to become master when we re-connect to it.
      gap_set_allow_role_switch(true);
    
    
      // Register for HCI events
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // Inform about audio playback / test options
    #ifdef HAVE_POSIX_FILE_IO
      if (!btstack_audio_sink_get_instance()){
        printf("No audio playback.\n");
      } else {
        printf("Audio playback supported.\n");
      }
    #ifdef STORE_TO_WAV_FILE 
       printf("Audio will be stored to \'%s\' file.\n",  wav_filename);
    #endif
    #endif
      return 0;
    }

~~~~



### Handle Media Data Packet 


  Here the audio data, are received through the handle_l2cap_media_data_packet callback. Currently, only the SBC media codec is supported. Hence, the media data consists of the media packet header and the SBC packet. The SBC frame will be stored in a ring buffer for later processing (instead of decoding it to PCM right away which would require a much larger buffer). If the audio stream wasn't started already and there are enough SBC frames in the ring buffer, start playback.

## A2DP Source - Stream Audio and Control Volume {#sec:a2dpsourcedemoExample}

Source Code: [a2dp_source_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/a2dp_source_demo.c)



  This A2DP Source example demonstrates how to send an audio data stream  to a remote A2DP Sink device and how to switch between two audio data sources.   In addition, the AVRCP Target is used to answer queries on currently played media, as well as to handle remote playback control, i.e. play, stop, repeat, etc. If HAVE_BTSTACK_STDIN  is set, press SPACE on the console to show the available AVDTP and AVRCP commands.



  To test with a remote device, e.g. a Bluetooth speaker, set the device_addr_string to the Bluetooth address of your  remote device in the code, and use the UI to connect and start playback. 



  For more info on BTstack audio, see our blog post  [A2DP Sink and Source on STM32 F4 Discovery Board](http://bluekitchen-gmbh.com/a2dp-sink-and-source-on-stm32-f4-discovery-board/).



### Main Application Setup


  The Listing [here](#lst:a2dpsourcedemoMainConfiguration) shows how to setup AD2P Source and AVRCP services.  Besides calling init() method for each service, you'll also need to register several packet handlers:
 
- hci_packet_handler - handles legacy pairing, here by using fixed '0000' pin code.
- a2dp_source_packet_handler - handles events on stream connection status (established, released), the media codec configuration, and, the commands on stream itself (open, pause, stopp).
- avrcp_packet_handler - receives connect/disconnect event.
- avrcp_controller_packet_handler - receives answers for sent AVRCP commands.
- avrcp_target_packet_handler - receives AVRCP commands, and registered notifications.
- stdin_process - used to trigger AVRCP commands to the A2DP Source device, such are get now playing info, start, stop, volume control. Requires HAVE_BTSTACK_STDIN.



  To announce A2DP Source and AVRCP services, you need to create corresponding SDP records and register them with the SDP service. 


~~~~ {#lst:a2dpsourcedemoMainConfiguration .c caption="{Setup Audio Source and AVRCP Target services}"}

    static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void a2dp_source_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t * event, uint16_t event_size);
    static void avrcp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void avrcp_target_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void avrcp_controller_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    #ifdef HAVE_BTSTACK_STDIN
    static void stdin_process(char cmd);
    #endif
    
    static int a2dp_source_and_avrcp_services_init(void){
    
      // Request role change on reconnecting headset to always use them in slave mode
      hci_set_master_slave_policy(0);
      // enabled EIR
      hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // Initialize  A2DP Source
      a2dp_source_init();
      a2dp_source_register_packet_handler(&a2dp_source_packet_handler);
    
      // Create stream endpoint
      avdtp_stream_endpoint_t * local_stream_endpoint = a2dp_source_create_stream_endpoint(AVDTP_AUDIO, AVDTP_CODEC_SBC, media_sbc_codec_capabilities, sizeof(media_sbc_codec_capabilities), media_sbc_codec_configuration, sizeof(media_sbc_codec_configuration));
      if (!local_stream_endpoint){
        printf("A2DP Source: not enough memory to create local stream endpoint\n");
        return 1;
      }
    
      avdtp_set_preferred_sampling_frequency(local_stream_endpoint, A2DP_SOURCE_DEMO_PREFERRED_SAMPLING_RATE);
    
      // Store stream enpoint's SEP ID, as it is used by A2DP API to indentify the stream endpoint
      media_tracker.local_seid = avdtp_local_seid(local_stream_endpoint);
      avdtp_source_register_delay_reporting_category(media_tracker.local_seid);
    
      // Initialize AVRCP Service
      avrcp_init();
      avrcp_register_packet_handler(&avrcp_packet_handler);
      // Initialize AVRCP Target
      avrcp_target_init();
      avrcp_target_register_packet_handler(&avrcp_target_packet_handler);
    
      // Initialize AVRCP Controller
      avrcp_controller_init();
      avrcp_controller_register_packet_handler(&avrcp_controller_packet_handler);
    
      // Initialize SDP, 
      sdp_init();
      
      // Create A2DP Source service record and register it with SDP
      memset(sdp_a2dp_source_service_buffer, 0, sizeof(sdp_a2dp_source_service_buffer));
      a2dp_source_create_sdp_record(sdp_a2dp_source_service_buffer, sdp_create_service_record_handle(), AVDTP_SOURCE_FEATURE_MASK_PLAYER, NULL, NULL);
      btstack_assert(de_get_len( sdp_a2dp_source_service_buffer) <= sizeof(sdp_a2dp_source_service_buffer));
      sdp_register_service(sdp_a2dp_source_service_buffer);
      
      // Create AVRCP Target service record and register it with SDP. We receive Category 1 commands from the headphone, e.g. play/pause
      memset(sdp_avrcp_target_service_buffer, 0, sizeof(sdp_avrcp_target_service_buffer));
      uint16_t supported_features = AVRCP_FEATURE_MASK_CATEGORY_PLAYER_OR_RECORDER;
    #ifdef AVRCP_BROWSING_ENABLED
      supported_features |= AVRCP_FEATURE_MASK_BROWSING;
    #endif
      avrcp_target_create_sdp_record(sdp_avrcp_target_service_buffer, sdp_create_service_record_handle(), supported_features, NULL, NULL);
      btstack_assert(de_get_len( sdp_avrcp_target_service_buffer) <= sizeof(sdp_avrcp_target_service_buffer));
      sdp_register_service(sdp_avrcp_target_service_buffer);
    
      // Create AVRCP Controller service record and register it with SDP. We send Category 2 commands to the headphone, e.g. volume up/down
      memset(sdp_avrcp_controller_service_buffer, 0, sizeof(sdp_avrcp_controller_service_buffer));
      uint16_t controller_supported_features = AVRCP_FEATURE_MASK_CATEGORY_MONITOR_OR_AMPLIFIER;
      avrcp_controller_create_sdp_record(sdp_avrcp_controller_service_buffer, sdp_create_service_record_handle(), controller_supported_features, NULL, NULL);
      btstack_assert(de_get_len( sdp_avrcp_controller_service_buffer) <= sizeof(sdp_avrcp_controller_service_buffer));
      sdp_register_service(sdp_avrcp_controller_service_buffer);
    
      // Register Device ID (PnP) service SDP record
      memset(device_id_sdp_service_buffer, 0, sizeof(device_id_sdp_service_buffer));
      device_id_create_sdp_record(device_id_sdp_service_buffer, sdp_create_service_record_handle(), DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
      btstack_assert(de_get_len( device_id_sdp_service_buffer) <= sizeof(device_id_sdp_service_buffer));
      sdp_register_service(device_id_sdp_service_buffer);
    
      // Set local name with a template Bluetooth address, that will be automatically
      // replaced with a actual address once it is available, i.e. when BTstack boots
      // up and starts talking to a Bluetooth module.
      gap_set_local_name("A2DP Source 00:00:00:00:00:00");
      gap_discoverable_control(1);
      gap_set_class_of_device(0x200408);
      
      // Register for HCI events.
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
    #ifdef ENABLE_MODPLAYER
      data_source = STREAM_MOD;
    #else
      data_source = STREAM_SINE;
    #endif
    
      // Parse human readable Bluetooth address.
      sscanf_bd_addr(device_addr_string, device_addr);
    
    #ifdef HAVE_BTSTACK_STDIN
      btstack_stdin_setup(stdin_process);
    #endif
      return 0;
    }

~~~~



## AVRCP Browsing - Browse Media Players and Media Information {#sec:avrcpbrowsingclientExample}

Source Code: [avrcp_browsing_client.c](https://github.com/bluekitchen/btstack/tree/master/example/avrcp_browsing_client.c)



  This example demonstrates how to use the AVRCP Controller Browsing service to  browse madia players and media information on a remote AVRCP Source device. 



  To test with a remote device, e.g. a mobile phone, pair from the remote device with the demo, then use the UI for browsing. If HAVE_BTSTACK_STDIN is set,  press SPACE on the console to show the available AVDTP and AVRCP commands.



### Main Application Setup


  The Listing [here](#lst:avrcpbrowsingclientMainConfiguration) shows how to setup AVRCP Controller Browsing service.  To announce AVRCP Controller Browsing service, you need to create corresponding SDP record and register it with the SDP service.  You'll also need to register several packet handlers:
 
- stdin_process callback - used to trigger AVRCP commands, such are get media players, playlists, albums, etc. Requires HAVE_BTSTACK_STDIN.
- avrcp_browsing_controller_packet_handler - used to receive answers for AVRCP commands.




~~~~ {#lst:avrcpbrowsingclientMainConfiguration .c caption="{Setup Audio Sink and AVRCP Controller services}"}

    static void avrcp_browsing_controller_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void avrcp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void a2dp_sink_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    #ifdef HAVE_BTSTACK_STDIN
    static void stdin_process(char cmd);
    #endif
    
    
    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      // Initialize L2CAP.
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      a2dp_sink_init();
      a2dp_sink_register_packet_handler(&a2dp_sink_packet_handler);
    
      avdtp_stream_endpoint_t * local_stream_endpoint = a2dp_sink_create_stream_endpoint(AVDTP_AUDIO, 
        AVDTP_CODEC_SBC, media_sbc_codec_capabilities, sizeof(media_sbc_codec_capabilities), 
        media_sbc_codec_configuration, sizeof(media_sbc_codec_configuration));
      if (!local_stream_endpoint){
        printf("A2DP Sink: not enough memory to create local stream endpoint\n");
        return 1;
      }
      a2dp_local_seid = avdtp_local_seid(local_stream_endpoint);
    
      // Initialize AVRCP service.
      avrcp_init();
      // Initialize AVRCP Controller & Target Service.
      avrcp_controller_init();
      avrcp_target_init();
    
      avrcp_register_packet_handler(&avrcp_packet_handler);
      avrcp_controller_register_packet_handler(&avrcp_packet_handler);
      avrcp_target_register_packet_handler(&avrcp_packet_handler);
    
      // Initialize AVRCP Browsing Service. 
      avrcp_browsing_init();
      avrcp_browsing_controller_init();
      avrcp_browsing_target_init();
      
      // Register for HCI events.
      avrcp_browsing_controller_register_packet_handler(&avrcp_browsing_controller_packet_handler);
      avrcp_browsing_target_register_packet_handler(&avrcp_browsing_controller_packet_handler);
      avrcp_browsing_register_packet_handler(&avrcp_browsing_controller_packet_handler);
      
      // Initialize SDP. 
      sdp_init();
      // setup AVDTP sink
      memset(sdp_avdtp_sink_service_buffer, 0, sizeof(sdp_avdtp_sink_service_buffer));
      a2dp_sink_create_sdp_record(sdp_avdtp_sink_service_buffer, sdp_create_service_record_handle(), AVDTP_SINK_FEATURE_MASK_HEADPHONE, NULL, NULL);
      btstack_assert(de_get_len( sdp_avdtp_sink_service_buffer) <= sizeof(sdp_avdtp_sink_service_buffer));
      sdp_register_service(sdp_avdtp_sink_service_buffer);
      
      // Create AVRCP service record and register it with SDP.
      memset(sdp_avrcp_browsing_controller_service_buffer, 0, sizeof(sdp_avrcp_browsing_controller_service_buffer));
    
      uint16_t supported_features = AVRCP_FEATURE_MASK_CATEGORY_PLAYER_OR_RECORDER;
    #ifdef AVRCP_BROWSING_ENABLED
      supported_features |= AVRCP_FEATURE_MASK_BROWSING;
    #endif
      avrcp_controller_create_sdp_record(sdp_avrcp_browsing_controller_service_buffer, sdp_create_service_record_handle(), supported_features, NULL, NULL);
      btstack_assert(de_get_len( sdp_avrcp_browsing_controller_service_buffer) <= sizeof(sdp_avrcp_browsing_controller_service_buffer));
      sdp_register_service(sdp_avrcp_browsing_controller_service_buffer);
      
      // Set local name with a template Bluetooth address, that will be automatically
      // replaced with a actual address once it is available, i.e. when BTstack boots
      // up and starts talking to a Bluetooth module.
      gap_set_local_name("AVRCP Browsing Client 00:00:00:00:00:00");
      gap_discoverable_control(1);
      gap_set_class_of_device(0x200408);
      
      // Register for HCI events.
      hci_event_callback_registration.callback = &avrcp_browsing_controller_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
      
    
    #ifdef HAVE_BTSTACK_STDIN
      // Parse human readable Bluetooth address.
      sscanf_bd_addr(device_addr_string, device_addr);
      btstack_stdin_setup(stdin_process);
    #endif
      printf("Starting BTstack ...\n");
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HFP AG - Audio Gateway {#sec:hfpagdemoExample}

Source Code: [hfp_ag_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hfp_ag_demo.c)



  This HFP Audio Gateway example demonstrates how to receive  an output from a remote HFP Hands-Free (HF) unit, and,  if HAVE_BTSTACK_STDIN is defined, how to control the HFP HF. 

### Main Application Setup


  Listing [here](#lst:hfpagdemoMainConfiguration) shows main application code.  To run a HFP AG service you need to initialize the SDP, and to create and register HFP AG record with it.  The packet_handler is used for sending commands to the HFP HF. It also receives the HFP HF's answers. The stdin_process callback allows for sending commands to the HFP HF.  At the end the Bluetooth stack is started.


~~~~ {#lst:hfpagdemoMainConfiguration .c caption="{Setup HFP Audio Gateway}"}

    
    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      sco_demo_init();
    
      // Request role change on reconnecting headset to always use them in slave mode
      hci_set_master_slave_policy(0);
    
      gap_set_local_name("HFP AG Demo 00:00:00:00:00:00");
      gap_discoverable_control(1);
    
      // L2CAP
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      uint16_t supported_features           =
        (1<<HFP_AGSF_ESCO_S4)           |
        (1<<HFP_AGSF_HF_INDICATORS)         |
        (1<<HFP_AGSF_CODEC_NEGOTIATION)       |
        (1<<HFP_AGSF_EXTENDED_ERROR_RESULT_CODES) |
        (1<<HFP_AGSF_ENHANCED_CALL_CONTROL)     |
        (1<<HFP_AGSF_ENHANCED_CALL_STATUS)    |
        (1<<HFP_AGSF_ABILITY_TO_REJECT_A_CALL)  |
        (1<<HFP_AGSF_IN_BAND_RING_TONE)       |
        (1<<HFP_AGSF_VOICE_RECOGNITION_FUNCTION)  |
        (1<<HFP_AGSF_ENHANCED_VOICE_RECOGNITION_STATUS) |
        (1<<HFP_AGSF_VOICE_RECOGNITION_TEXT) |
        (1<<HFP_AGSF_EC_NR_FUNCTION) |
        (1<<HFP_AGSF_THREE_WAY_CALLING);
    
      // HFP
      rfcomm_init();
      hfp_ag_init(rfcomm_channel_nr);
      hfp_ag_init_supported_features(supported_features);
      hfp_ag_init_codecs(sizeof(codecs), codecs);
      hfp_ag_init_ag_indicators(ag_indicators_nr, ag_indicators);
      hfp_ag_init_hf_indicators(hf_indicators_nr, hf_indicators); 
      hfp_ag_init_call_hold_services(call_hold_services_nr, call_hold_services);
      hfp_ag_init_apple_identification("BTstack", 0);
      hfp_ag_set_subscriber_number_information(&subscriber_number, 1);
    
      // SDP Server
      sdp_init();
      memset(hfp_service_buffer, 0, sizeof(hfp_service_buffer));
      hfp_ag_create_sdp_record_with_codecs( hfp_service_buffer, sdp_create_service_record_handle(),
                          rfcomm_channel_nr, hfp_ag_service_name, 0, supported_features, sizeof(codecs), codecs);
      btstack_assert(de_get_len( hfp_service_buffer) <= sizeof(hfp_service_buffer));
      sdp_register_service(hfp_service_buffer);
      
      // register for HCI events and SCO packets
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
      hci_register_sco_packet_handler(&packet_handler);
    
      // register for HFP events
      hfp_ag_register_packet_handler(&packet_handler);
    
      // parse human readable Bluetooth address
      sscanf_bd_addr(device_addr_string, device_addr);
    
    #ifdef HAVE_BTSTACK_STDIN
      btstack_stdin_setup(stdin_process);
    #endif  
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HFP HF - Hands-Free {#sec:hfphfdemoExample}

Source Code: [hfp_hf_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hfp_hf_demo.c)



  This  HFP Hands-Free example demonstrates how to receive  an output from a remote HFP audio gateway (AG), and,  if HAVE_BTSTACK_STDIN is defined, how to control the HFP AG. 

### Main Application Setup


  Listing [here](#lst:hfphfdemoMainConfiguration) shows main application code.  To run a HFP HF service you need to initialize the SDP, and to create and register HFP HF record with it.  The packet_handler is used for sending commands to the HFP AG. It also receives the HFP AG's answers. The stdin_process callback allows for sending commands to the HFP AG.  At the end the Bluetooth stack is started.


~~~~ {#lst:hfphfdemoMainConfiguration .c caption="{Setup HFP Hands-Free unit}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      // Init protocols
      // init L2CAP
      l2cap_init();
      rfcomm_init();
      sdp_init();
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // Init profiles
      uint16_t hf_supported_features      =
        (1<<HFP_HFSF_ESCO_S4)         |
        (1<<HFP_HFSF_CLI_PRESENTATION_CAPABILITY) |
        (1<<HFP_HFSF_HF_INDICATORS)     |
        (1<<HFP_HFSF_CODEC_NEGOTIATION)   |
        (1<<HFP_HFSF_ENHANCED_CALL_STATUS)  |
        (1<<HFP_HFSF_VOICE_RECOGNITION_FUNCTION)  |
        (1<<HFP_HFSF_ENHANCED_VOICE_RECOGNITION_STATUS) |
        (1<<HFP_HFSF_VOICE_RECOGNITION_TEXT) |
        (1<<HFP_HFSF_EC_NR_FUNCTION) |
        (1<<HFP_HFSF_REMOTE_VOLUME_CONTROL);
    
      hfp_hf_init(rfcomm_channel_nr);
      hfp_hf_init_supported_features(hf_supported_features);
      hfp_hf_init_hf_indicators(sizeof(indicators)/sizeof(uint16_t), indicators);
      hfp_hf_init_codecs(sizeof(codecs), codecs);
      hfp_hf_register_packet_handler(hfp_hf_packet_handler);
      // Set Apple Accessory Information: features = battery reporting & docked/powered, battery=9, docked=1
      hfp_hf_apple_set_identification(BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 0x0001, "0001", 3);
      hfp_hf_apple_set_battery_level(9);
      hfp_hf_apple_set_docked_state(1);
    
      // Configure SDP
    
      // - Create and register HFP HF service record
      memset(hfp_service_buffer, 0, sizeof(hfp_service_buffer));
      hfp_hf_create_sdp_record_with_codecs(hfp_service_buffer, sdp_create_service_record_handle(),
                   rfcomm_channel_nr, hfp_hf_service_name, hf_supported_features, sizeof(codecs), codecs);
      btstack_assert(de_get_len( hfp_service_buffer) <= sizeof(hfp_service_buffer));
      sdp_register_service(hfp_service_buffer);
    
      // Configure GAP - discovery / connection
    
      // - Set local name with a template Bluetooth address, that will be automatically
      //   replaced with an actual address once it is available, i.e. when BTstack boots
      //   up and starts talking to a Bluetooth module.
      gap_set_local_name("HFP HF Demo 00:00:00:00:00:00");
    
      // - Allow to show up in Bluetooth inquiry
      gap_discoverable_control(1);
    
      // - Set Class of Device - Service Class: Audio, Major Device Class: Audio, Minor: Hands-Free device
      gap_set_class_of_device(0x200408);
    
      // - Allow for role switch in general and sniff mode
      gap_set_default_link_policy_settings( LM_LINK_POLICY_ENABLE_ROLE_SWITCH | LM_LINK_POLICY_ENABLE_SNIFF_MODE );
    
      // - Allow for role switch on outgoing connections - this allows HFP AG, e.g. smartphone, to become master when we re-connect to it
      gap_set_allow_role_switch(true);
    
      // Register for HCI events and SCO packets
      hci_event_callback_registration.callback = &hci_packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
      hci_register_sco_packet_handler(&hci_packet_handler);
    
    
      // Init SCO / HFP audio processing
      sco_demo_init();
    
    #ifdef HAVE_BTSTACK_STDIN
      // parse human readable Bluetooth address
      sscanf_bd_addr(device_addr_string, device_addr);
      btstack_stdin_setup(stdin_process);
    #endif
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HSP AG - Audio Gateway {#sec:hspagdemoExample}

Source Code: [hsp_ag_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hsp_ag_demo.c)



  This example implements a HSP Audio Gateway device that sends and receives  audio signal over HCI SCO. It demonstrates how to receive  an output from a remote headset (HS), and,  if HAVE_BTSTACK_STDIN is defined, how to control the HS. 

### Audio Transfer Setup 


  A pre-computed sine wave (160Hz) is used as the input audio signal. 160 Hz.  To send and receive an audio signal, ENABLE_SCO_OVER_HCI has to be defined. 

 Tested working setups: 
 
- Ubuntu 14 64-bit, CC2564B connected via FTDI USB-2-UART adapter, 921600 baud
- Ubuntu 14 64-bit, CSR USB dongle
- OS X 10.11, CSR USB dongle



### Main Application Setup


  Listing [here](#lst:hspagdemoMainConfiguration) shows main application code.  To run a HSP Audio Gateway service you need to initialize the SDP, and to create and register HSP AG record with it.  In this example, the SCO over HCI is used to receive and send an audio signal.

 Two packet handlers are registered:
 
- The HCI SCO packet handler receives audio data.
- The HSP AG packet handler is used to trigger sending of audio data and commands to the HS. It also receives the AG's answers.




~~~~ {#lst:hspagdemoMainConfiguration .c caption="{Setup HSP Audio Gateway}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      sco_demo_init();
      sco_demo_set_codec(HFP_CODEC_CVSD);
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      sdp_init();
    
      memset((uint8_t *)hsp_service_buffer, 0, sizeof(hsp_service_buffer));
      hsp_ag_create_sdp_record(hsp_service_buffer, sdp_create_service_record_handle(), rfcomm_channel_nr, hsp_ag_service_name);
      btstack_assert(de_get_len( hsp_service_buffer) <= sizeof(hsp_service_buffer));
      sdp_register_service(hsp_service_buffer);
      
      rfcomm_init();
    
      hsp_ag_init(rfcomm_channel_nr);
      hsp_ag_register_packet_handler(&packet_handler);
    
      // register for SCO packets
      hci_register_sco_packet_handler(&packet_handler);
    
      // parse human readable Bluetooth address
      sscanf_bd_addr(device_addr_string, device_addr);
    
    #ifdef HAVE_BTSTACK_STDIN
      btstack_stdin_setup(stdin_process);
    #endif
    
      gap_set_local_name(device_name);
      gap_discoverable_control(1);
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_set_class_of_device(0x400204);
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HSP HS - Headset {#sec:hsphsdemoExample}

Source Code: [hsp_hs_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hsp_hs_demo.c)



  This example implements a HSP Headset device that sends and receives  audio signal over HCI SCO. It demonstrates how to receive  an output from a remote audio gateway (AG), and,  if HAVE_BTSTACK_STDIN is defined, how to control the AG. 

### Audio Transfer Setup 


  A pre-computed sine wave (160Hz) is used as the input audio signal. 160 Hz.  To send and receive an audio signal, ENABLE_SCO_OVER_HCI has to be defined. 

 Tested working setups: 
 
- Ubuntu 14 64-bit, CC2564B connected via FTDI USB-2-UART adapter, 921600 baud
- Ubuntu 14 64-bit, CSR USB dongle
- OS X 10.11, CSR USB dongle



### Main Application Setup


  Listing [here](#lst:hsphsdemoMainConfiguration) shows main application code.  To run a HSP Headset service you need to initialize the SDP, and to create and register HSP HS record with it.  In this example, the SCO over HCI is used to receive and send an audio signal.

 Two packet handlers are registered:
 
- The HCI SCO packet handler receives audio data.
- The HSP HS packet handler is used to trigger sending of audio data and commands to the AG. It also receives the AG's answers.




~~~~ {#lst:hsphsdemoMainConfiguration .c caption="{Setup HSP Headset}"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      sco_demo_init();
      sco_demo_set_codec(HFP_CODEC_CVSD);
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      sdp_init();
      memset(hsp_service_buffer, 0, sizeof(hsp_service_buffer));
      hsp_hs_create_sdp_record(hsp_service_buffer, sdp_create_service_record_handle(), rfcomm_channel_nr, hsp_hs_service_name, 0);
      btstack_assert(de_get_len( hsp_service_buffer) <= sizeof(hsp_service_buffer));
      sdp_register_service(hsp_service_buffer);
    
      rfcomm_init();
    
      hsp_hs_init(rfcomm_channel_nr);
    
      // register for HCI events and SCO packets
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
      hci_register_sco_packet_handler(&packet_handler);
    
      // register for HSP events
      hsp_hs_register_packet_handler(packet_handler);
    
    #ifdef HAVE_BTSTACK_STDIN
      btstack_stdin_setup(stdin_process);
    #endif
    
      gap_set_local_name("HSP HS Demo 00:00:00:00:00:00");
      gap_discoverable_control(1);
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_set_class_of_device(0x240404);
    
      // Parse human readable Bluetooth address.
      sscanf_bd_addr(device_addr_string, device_addr);
      
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## Audio Driver - Play Sine  {#sec:sineplayerExample}

Source Code: [sine_player.c](https://github.com/bluekitchen/btstack/tree/master/example/sine_player.c)



   Play sine to test and validate audio output with simple wave form.

## Audio Driver - Play 80's MOD Song {#sec:modplayerExample}

Source Code: [mod_player.c](https://github.com/bluekitchen/btstack/tree/master/example/mod_player.c)



## Audio Driver - Forward Audio from Source to Sink {#sec:audioduplexExample}

Source Code: [audio_duplex.c](https://github.com/bluekitchen/btstack/tree/master/example/audio_duplex.c)



## SPP Server - Heartbeat Counter over RFCOMM {#sec:sppcounterExample}

Source Code: [spp_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/spp_counter.c)



  The Serial port profile (SPP) is widely used as it provides a serial port over Bluetooth. The SPP counter example demonstrates how to setup an SPP service, and provide a periodic timer over RFCOMM.   



  Note: To test, please run the spp_counter example, and then pair from  a remote device, and open the Virtual Serial Port.

### SPP Service Setup 


  To provide an SPP service, the L2CAP, RFCOMM, and SDP protocol layers  are required. After setting up an RFCOMM service with channel nubmer RFCOMM_SERVER_CHANNEL, an SDP record is created and registered with the SDP server. Example code for SPP service setup is provided in Listing [here](#lst:sppcounterSPPSetup). The SDP record created by function spp_create_sdp_record consists of a basic SPP definition that uses the provided RFCOMM channel ID and service name. For more details, please have a look at it in \path{src/sdp_util.c}.  The SDP record is created on the fly in RAM and is deterministic. To preserve valuable RAM, the result could be stored as constant data inside the ROM.   


~~~~ {#lst:sppcounterSPPSetup .c caption="{SPP service setup}"}

    static void spp_service_setup(void){
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      rfcomm_init();
      rfcomm_register_service(packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff);  // reserved channel, mtu limited by l2cap
    
      // init SDP, create record for SPP and register with SDP
      sdp_init();
      memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
      spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), RFCOMM_SERVER_CHANNEL, "SPP Counter");
      btstack_assert(de_get_len( spp_service_buffer) <= sizeof(spp_service_buffer));
      sdp_register_service(spp_service_buffer);
    }

~~~~



### Periodic Timer Setup


  The heartbeat handler increases the real counter every second,  and sends a text string with the counter value, as shown in Listing [here](#lst:sppcounterPeriodicCounter). 


~~~~ {#lst:sppcounterPeriodicCounter .c caption="{Periodic Counter}"}

    static btstack_timer_source_t heartbeat;
    static char lineBuffer[30];
    static void  heartbeat_handler(struct btstack_timer_source *ts){
      static int counter = 0;
    
      if (rfcomm_channel_id){
        btstack_snprintf_assert_complete(lineBuffer, sizeof(lineBuffer), "BTstack counter %04u\n", ++counter);
        printf("%s", lineBuffer);
    
        rfcomm_request_can_send_now_event(rfcomm_channel_id);
      }
    
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 
    
    static void one_shot_timer_setup(void){
      // set one-shot timer
      heartbeat.process = &heartbeat_handler;
      btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(&heartbeat);
    }

~~~~



### Bluetooth Logic 


  The Bluetooth logic is implemented within the  packet handler, see Listing [here](#lst:sppcounterSppServerPacketHandler). In this example,  the following events are passed sequentially: 
 
- BTSTACK_EVENT_STATE,
- HCI_EVENT_PIN_CODE_REQUEST (Standard pairing) or 
- HCI_EVENT_USER_CONFIRMATION_REQUEST (Secure Simple Pairing),
- RFCOMM_EVENT_INCOMING_CONNECTION,
- RFCOMM_EVENT_CHANNEL_OPENED, 
- RFCOMM_EVENT_CHANNEL_CLOSED



  Upon receiving HCI_EVENT_PIN_CODE_REQUEST event, we need to handle authentication. Here, we use a fixed PIN code "0000".

 When HCI_EVENT_USER_CONFIRMATION_REQUEST is received, the user will be  asked to accept the pairing request. If the IO capability is set to  SSP_IO_CAPABILITY_DISPLAY_YES_NO, the request will be automatically accepted.

 The RFCOMM_EVENT_INCOMING_CONNECTION event indicates an incoming connection. Here, the connection is accepted. More logic is need, if you want to handle connections from multiple clients. The incoming RFCOMM connection event contains the RFCOMM channel number used during the SPP setup phase and the newly assigned RFCOMM channel ID that is used by all BTstack commands and events.

 If RFCOMM_EVENT_CHANNEL_OPENED event returns status greater then 0, then the channel establishment has failed (rare case, e.g., client crashes). On successful connection, the RFCOMM channel ID and MTU for this channel are made available to the heartbeat counter. After opening the RFCOMM channel,  the communication between client and the application takes place. In this example, the timer handler increases the real counter every second. 

 RFCOMM_EVENT_CAN_SEND_NOW indicates that it's possible to send an RFCOMM packet on the rfcomm_cid that is include


~~~~ {#lst:sppcounterSppServerPacketHandler .c caption="{SPP Server - Heartbeat Counter over RFCOMM}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
    
    ...
            case HCI_EVENT_PIN_CODE_REQUEST:
              // inform about pin code request
              printf("Pin code request - using '0000'\n");
              hci_event_pin_code_request_get_bd_addr(packet, event_addr);
              gap_pin_code_response(event_addr, "0000");
              break;
    
            case HCI_EVENT_USER_CONFIRMATION_REQUEST:
              // ssp: inform about user confirmation request
              printf("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n", little_endian_read_32(packet, 8));
              printf("SSP User Confirmation Auto accept\n");
              break;
    
            case RFCOMM_EVENT_INCOMING_CONNECTION:
              rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr);
              rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
              rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
              printf("RFCOMM channel %u requested for %s\n", rfcomm_channel_nr, bd_addr_to_str(event_addr));
              rfcomm_accept_connection(rfcomm_channel_id);
              break;
             
            case RFCOMM_EVENT_CHANNEL_OPENED:
              if (rfcomm_event_channel_opened_get_status(packet)) {
                printf("RFCOMM channel open failed, status 0x%02x\n", rfcomm_event_channel_opened_get_status(packet));
              } else {
                rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
                printf("RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n", rfcomm_channel_id, mtu);
              }
              break;
            case RFCOMM_EVENT_CAN_SEND_NOW:
              rfcomm_send(rfcomm_channel_id, (uint8_t*) lineBuffer, (uint16_t) strlen(lineBuffer));  
              break;
    
    ...
    }

~~~~



## SPP Server - RFCOMM Flow Control {#sec:sppflowcontrolExample}

Source Code: [spp_flowcontrol.c](https://github.com/bluekitchen/btstack/tree/master/example/spp_flowcontrol.c)



  This example adds explicit flow control for incoming RFCOMM data to the  SPP heartbeat counter example. We will highlight the changes compared to the  SPP counter example. 

### SPP Service Setup   


  Listing [here](#lst:sppflowcontrolexplicitFlowControl) shows how to provide one initial credit during RFCOMM service initialization. Please note that providing a single credit effectively reduces the credit-based (sliding window) flow control to a stop-and-wait flow control that limits the data throughput substantially.  


~~~~ {#lst:sppflowcontrolexplicitFlowControl .c caption="{Providing one initial credit during RFCOMM service initialization}"}

    static void spp_service_setup(void){   
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // init L2CAP
      l2cap_init();
      
      // init RFCOMM
      rfcomm_init();
      // reserved channel, mtu limited by l2cap, 1 credit
      rfcomm_register_service_with_initial_credits(&packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff, 1);  
    
      // init SDP, create record for SPP and register with SDP
      sdp_init();
      memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
      spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), 1, "SPP Counter");
      btstack_assert(de_get_len( spp_service_buffer) <= sizeof(spp_service_buffer));
      sdp_register_service(spp_service_buffer);
    }

~~~~



### Periodic Timer Setup  


  Explicit credit management is recommended when received RFCOMM data cannot be processed immediately. In this example, delayed processing of received data is simulated with the help of a periodic timer as follows. When the packet handler receives a data packet, it does not provide a new credit, it sets a flag instead, see Listing [here](#lst:sppflowcontrolphManual).  If the flag is set, a new credit will be granted by the heartbeat handler, introducing a delay of up to 1 second. The heartbeat handler code is shown in Listing [here](#lst:sppflowcontrolhbhManual). 


~~~~ {#lst:sppflowcontrolhbhManual .c caption="{Heartbeat handler with manual credit management}"}

    static void  heartbeat_handler(struct btstack_timer_source *ts){
      if (rfcomm_send_credit){
        rfcomm_grant_credits(rfcomm_channel_id, 1);
        rfcomm_send_credit = 0;
      }
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 

~~~~




~~~~ {#lst:sppflowcontrolphManual .c caption="{Packet handler with manual credit management}"}

    // Bluetooth logic
    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    ...
        case RFCOMM_DATA_PACKET:
          for (i=0;i<size;i++){
            putchar(packet[i]);
          };
          putchar('\n');
          rfcomm_send_credit = 1;
          break;
    ...
    }

~~~~



## PAN - lwIP HTTP and DHCP Server  {#sec:panlwiphttpserverExample}

Source Code: [pan_lwip_http_server.c](https://github.com/bluekitchen/btstack/tree/master/example/pan_lwip_http_server.c)



  Bluetooth PAN is mainly used for Internet Tethering, where e.g. a mobile phone provides internet connection to a laptop or a tablet.

 Instead of regular internet access, it's also possible to provide a Web app on a  Bluetooth device, e.g. for configuration or maintenance. For some device, this can be a more effective way to provide an interface compared to dedicated smartphone applications (for Android and iOS).

 Before iOS 11, accessing an HTTP server via Bluetooth PAN was not supported on the iPhone, but on iPod and iPad. With iOS 11, this works as expected.

 After pairing your device, please open the URL http://192.168.7.1 in your web browser.

### Packet Handler


  All BNEP events are handled in the platform/bnep_lwip.c BNEP-LWIP Adapter. Here, we only print status information and handle pairing requests.


~~~~ {#lst:panlwiphttpserverpacketHandler .c caption="{Packet Handler}"}



### PAN BNEP Setup


### DHCP Server Configuration


### Large File Download


### DHCP Server Setup


### Main


## BNEP/PANU (Linux only) {#sec:panudemoExample}

Source Code: [panu_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/panu_demo.c)



  This example implements both a PANU client and a server. In server mode, it  sets up a BNEP server and registers a PANU SDP record and waits for incoming connections. In client mode, it connects to a remote device, does an SDP Query to identify the PANU service and initiates a BNEP connection.

 Note: currently supported only on Linux and provides a TAP network interface which you can configure yourself.

 To enable client mode, uncomment ENABLE_PANU_CLIENT below.

### Main application configuration


  In the application configuration, L2CAP and BNEP are initialized and a BNEP service, for server mode, is registered, before the Bluetooth stack gets started, as shown in Listing [here](#lst:panudemoPanuSetup).


~~~~ {#lst:panudemoPanuSetup .c caption="{Panu setup}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    #ifdef ENABLE_PANU_CLIENT
    static void handle_sdp_client_query_result(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    #endif
    static void network_send_packet_callback(const uint8_t * packet, uint16_t size);
    
    static void panu_setup(void){
    
      // Initialize L2CAP
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // init SDP, create record for PANU and register with SDP
      sdp_init();
      memset(panu_sdp_record, 0, sizeof(panu_sdp_record));
      uint16_t network_packet_types[] = { NETWORK_TYPE_IPv4, NETWORK_TYPE_ARP, 0};  // 0 as end of list
    
      // Initialise BNEP
      bnep_init();
      // Minimum L2CAP MTU for bnep is 1691 bytes
    #ifdef ENABLE_PANU_CLIENT
      bnep_register_service(packet_handler, BLUETOOTH_SERVICE_CLASS_PANU, 1691);
      // PANU
      pan_create_panu_sdp_record(panu_sdp_record, sdp_create_service_record_handle(), network_packet_types, NULL, NULL, BNEP_SECURITY_NONE);
    #else
      bnep_register_service(packet_handler, BLUETOOTH_SERVICE_CLASS_NAP, 1691);
      // NAP Network Access Type: Other, 1 MB/s
      pan_create_nap_sdp_record(panu_sdp_record, sdp_create_service_record_handle(), network_packet_types, NULL, NULL, BNEP_SECURITY_NONE, PAN_NET_ACCESS_TYPE_OTHER, 1000000, NULL, NULL);
    #endif
      btstack_assert(de_get_len( panu_sdp_record) <= sizeof(panu_sdp_record));
      sdp_register_service(panu_sdp_record);
    
      // Initialize network interface
      btstack_network_init(&network_send_packet_callback);
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    }

~~~~



### SDP parser callback 


  The SDP parsers retrieves the BNEP PAN UUID as explained in   Section [on SDP BNEP Query example](#sec:sdpbnepqueryExample}.

### Packet Handler


  The packet handler responds to various HCI Events.


~~~~ {#lst:panudemopacketHandler .c caption="{Packet Handler}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
    {
    ...
      switch (packet_type) {
    		case HCI_EVENT_PACKET:
          event = hci_event_packet_get_type(packet);
          switch (event) {      
    #ifdef ENABLE_PANU_CLIENT
            case BTSTACK_EVENT_STATE:
              if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
                printf("Start SDP BNEP query for remote PAN Network Access Point (NAP).\n");
                sdp_client_query_uuid16(&handle_sdp_client_query_result, remote_addr, BLUETOOTH_SERVICE_CLASS_NAP);
              }
              break;
    #endif
    ...
    
    				case BNEP_EVENT_CHANNEL_OPENED:
              if (bnep_event_channel_opened_get_status(packet)) {
                printf("BNEP channel open failed, status 0x%02x\n", bnep_event_channel_opened_get_status(packet));
              } else {
                bnep_cid  = bnep_event_channel_opened_get_bnep_cid(packet);
                uuid_source = bnep_event_channel_opened_get_source_uuid(packet);
                uuid_dest   = bnep_event_channel_opened_get_destination_uuid(packet);
                mtu     = bnep_event_channel_opened_get_mtu(packet);
                bnep_event_channel_opened_get_remote_address(packet, event_addr);
                printf("BNEP connection open succeeded to %s source UUID 0x%04x dest UUID: 0x%04x, max frame size %u\n", bd_addr_to_str(event_addr), uuid_source, uuid_dest, mtu);
    
                gap_local_bd_addr(local_addr);
                btstack_network_up(local_addr);
                printf("Network Interface %s activated\n", btstack_network_get_name());
              }
    					break;
            
            case BNEP_EVENT_CHANNEL_TIMEOUT:
              printf("BNEP channel timeout! Channel will be closed\n");
              break;
    
            case BNEP_EVENT_CHANNEL_CLOSED:
              printf("BNEP channel closed\n");
              btstack_network_down();
              break;
    
            case BNEP_EVENT_CAN_SEND_NOW:
              if (network_buffer_len > 0) {
                bnep_send(bnep_cid, (uint8_t*) network_buffer, network_buffer_len);
                network_buffer_len = 0;
                btstack_network_packet_sent();
              }
              break;
              
            default:
              break;
          }
          break;
    
        case BNEP_DATA_PACKET:
          // Write out the ethernet frame to the network interface
          btstack_network_process_packet(packet, size);
          break;      
          
        default:
          break;
      }
    }

~~~~



  When BTSTACK_EVENT_STATE with state HCI_STATE_WORKING is received and the example is started in client mode, the remote SDP BNEP query is started.

  BNEP_EVENT_CHANNEL_OPENED is received after a BNEP connection was established or  or when the connection fails. The status field returns the error code.

 The TAP network interface is then configured. A data source is set up and registered with the  run loop to receive Ethernet packets from the TAP interface.

 The event contains both the source and destination UUIDs, as well as the MTU for this connection and the BNEP Channel ID, which is used for sending Ethernet packets over BNEP.

  If there is a timeout during the connection setup, BNEP_EVENT_CHANNEL_TIMEOUT will be received and the BNEP connection  will be closed

  BNEP_EVENT_CHANNEL_CLOSED is received when the connection gets closed.

  BNEP_EVENT_CAN_SEND_NOW indicates that a new packet can be send. This triggers the send of a  stored network packet. The tap datas source can be enabled again

  Ethernet packets from the remote device are received in the packet handler with type BNEP_DATA_PACKET. It is forwarded to the TAP interface.

### Network packet handler


  A pointer to the network packet is stored and a BNEP_EVENT_CAN_SEND_NOW requested


~~~~ {#lst:panudemonetworkPacketHandler .c caption="{Network Packet Handler}"}

    static void network_send_packet_callback(const uint8_t * packet, uint16_t size){
      network_buffer = packet;
      network_buffer_len = size;
      bnep_request_can_send_now_event(bnep_cid);
    }

~~~~



## HID Keyboard Classic {#sec:hidkeyboarddemoExample}

Source Code: [hid_keyboard_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hid_keyboard_demo.c)



  This HID Device example demonstrates how to implement an HID keyboard. Without a HAVE_BTSTACK_STDIN, a fixed demo text is sent If HAVE_BTSTACK_STDIN is defined, you can type from the terminal

### Main Application Setup


  Listing [here](#lst:hidkeyboarddemoMainConfiguration) shows main application code.  To run a HID Device service you need to initialize the SDP, and to create and register HID Device record with it.  At the end the Bluetooth stack is started.


~~~~ {#lst:hidkeyboarddemoMainConfiguration .c caption="{Setup HID Device}"}

    
    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      // allow to get found by inquiry
      gap_discoverable_control(1);
      // use Limited Discoverable Mode; Peripheral; Keyboard as CoD
      gap_set_class_of_device(0x2540);
      // set local name to be identified - zeroes will be replaced by actual BD ADDR
      gap_set_local_name("HID Keyboard Demo 00:00:00:00:00:00");
      // allow for role switch in general and sniff mode
      gap_set_default_link_policy_settings( LM_LINK_POLICY_ENABLE_ROLE_SWITCH | LM_LINK_POLICY_ENABLE_SNIFF_MODE );
      // allow for role switch on outgoing connections - this allow HID Host to become master when we re-connect to it
      gap_set_allow_role_switch(true);
    
      // L2CAP
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // SDP Server
      sdp_init();
      memset(hid_service_buffer, 0, sizeof(hid_service_buffer));
    
      uint8_t hid_virtual_cable = 0;
      uint8_t hid_remote_wake = 1;
      uint8_t hid_reconnect_initiate = 1;
      uint8_t hid_normally_connectable = 1;
    
      hid_sdp_record_t hid_params = {
        // hid sevice subclass 2540 Keyboard, hid counntry code 33 US
        0x2540, 33, 
        hid_virtual_cable, hid_remote_wake, 
        hid_reconnect_initiate, hid_normally_connectable,
        hid_boot_device,
        host_max_latency, host_min_timeout,
        3200,
        hid_descriptor_keyboard,
        sizeof(hid_descriptor_keyboard),
        hid_device_name
      };
      
      hid_create_sdp_record(hid_service_buffer, sdp_create_service_record_handle(), &hid_params);
      btstack_assert(de_get_len( hid_service_buffer) <= sizeof(hid_service_buffer));
      sdp_register_service(hid_service_buffer);
    
      // See https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers if you don't have a USB Vendor ID and need a Bluetooth Vendor ID
      // device info: BlueKitchen GmbH, product 1, version 1
      device_id_create_sdp_record(device_id_sdp_service_buffer, sdp_create_service_record_handle(), DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
      btstack_assert(de_get_len( device_id_sdp_service_buffer) <= sizeof(device_id_sdp_service_buffer));
      sdp_register_service(device_id_sdp_service_buffer);
    
      // HID Device
      hid_device_init(hid_boot_device, sizeof(hid_descriptor_keyboard), hid_descriptor_keyboard);
         
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for HID events
      hid_device_register_packet_handler(&packet_handler);
    
    #ifdef HAVE_BTSTACK_STDIN
      sscanf_bd_addr(device_addr_string, device_addr);
      btstack_stdin_setup(stdin_process);
    #endif
    
      btstack_ring_buffer_init(&send_buffer, send_buffer_storage, sizeof(send_buffer_storage));
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HID Mouse Classic {#sec:hidmousedemoExample}

Source Code: [hid_mouse_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hid_mouse_demo.c)



  This HID Device example demonstrates how to implement an HID keyboard. Without a HAVE_BTSTACK_STDIN, a fixed demo text is sent If HAVE_BTSTACK_STDIN is defined, you can type from the terminal

### Main Application Setup


  Listing [here](#lst:hidmousedemoMainConfiguration) shows main application code. To run a HID Device service you need to initialize the SDP, and to create and register HID Device record with it. At the end the Bluetooth stack is started.


~~~~ {#lst:hidmousedemoMainConfiguration .c caption="{Setup HID Device}"}

    
    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]){
      (void)argc;
      (void)argv;
    
      // allow to get found by inquiry
      gap_discoverable_control(1);
      // use Limited Discoverable Mode; Peripheral; Pointing Device as CoD
      gap_set_class_of_device(0x2580);
      // set local name to be identified - zeroes will be replaced by actual BD ADDR
      gap_set_local_name("HID Mouse Demo 00:00:00:00:00:00");
      // allow for role switch in general and sniff mode
      gap_set_default_link_policy_settings( LM_LINK_POLICY_ENABLE_ROLE_SWITCH | LM_LINK_POLICY_ENABLE_SNIFF_MODE );
      // allow for role switch on outgoing connections - this allow HID Host to become master when we re-connect to it
      gap_set_allow_role_switch(true);
    
      // L2CAP
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // SDP Server
      sdp_init();
    
      uint8_t hid_virtual_cable = 0;
      uint8_t hid_remote_wake = 1;
      uint8_t hid_reconnect_initiate = 1;
      uint8_t hid_normally_connectable = 1;
    
      hid_sdp_record_t hid_params = {
        // hid sevice subclass 2580 Mouse, hid counntry code 33 US
        0x2580, 33, 
        hid_virtual_cable, hid_remote_wake, 
        hid_reconnect_initiate, hid_normally_connectable,
        hid_boot_device, 
        0xFFFF, 0xFFFF, 3200,
        hid_descriptor_mouse_boot_mode,
        sizeof(hid_descriptor_mouse_boot_mode), 
        hid_device_name
      };
    
      memset(hid_service_buffer, 0, sizeof(hid_service_buffer));
      hid_create_sdp_record(hid_service_buffer, sdp_create_service_record_handle(), &hid_params);
      btstack_assert(de_get_len( hid_service_buffer) <= sizeof(hid_service_buffer));
      sdp_register_service(hid_service_buffer);
    
      // See https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers if you don't have a USB Vendor ID and need a Bluetooth Vendor ID
      // device info: BlueKitchen GmbH, product 2, version 1
      device_id_create_sdp_record(device_id_sdp_service_buffer, sdp_create_service_record_handle(), DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 2, 1);
      btstack_assert(de_get_len( device_id_sdp_service_buffer) <= sizeof(device_id_sdp_service_buffer));
      sdp_register_service(device_id_sdp_service_buffer);
    
      // HID Device
      hid_device_init(hid_boot_device, sizeof(hid_descriptor_mouse_boot_mode), hid_descriptor_mouse_boot_mode);
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for HID
      hid_device_register_packet_handler(&packet_handler);
    
    #ifdef HAVE_BTSTACK_STDIN
      btstack_stdin_setup(stdin_process);
    #endif
      // turn on!
      hci_power_control(HCI_POWER_ON);
      return 0;
    }

~~~~



## HID Host Classic {#sec:hidhostdemoExample}

Source Code: [hid_host_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hid_host_demo.c)



  This example implements a HID Host. For now, it connects to a fixed device. It will connect in Report protocol mode if this mode is supported by the HID Device, otherwise it will fall back to BOOT protocol mode. 

### Main application configuration


  In the application configuration, L2CAP and HID host are initialized, and the link policies  are set to allow sniff mode and role change. 


~~~~ {#lst:hidhostdemoPanuSetup .c caption="{Panu setup}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void hid_host_setup(void){
    
      // Initialize L2CAP
      l2cap_init();
    
    #ifdef ENABLE_BLE
      // Initialize LE Security Manager. Needed for cross-transport key derivation
      sm_init();
    #endif
    
      // Initialize HID Host
      hid_host_init(hid_descriptor_storage, sizeof(hid_descriptor_storage));
      hid_host_register_packet_handler(packet_handler);
    
      // Allow sniff mode requests by HID device and support role switch
      gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_SNIFF_MODE | LM_LINK_POLICY_ENABLE_ROLE_SWITCH);
    
      // try to become master on incoming connections
      hci_set_master_slave_policy(HCI_ROLE_MASTER);
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // make discoverable to allow HID device to initiate connection
      gap_discoverable_control(1);
    
      // Disable stdout buffering
      setvbuf(stdin, NULL, _IONBF, 0);
    }

~~~~



### HID Report Handler


  Use BTstack's compact HID Parser to process incoming HID Report in Report protocol mode.  Iterate over all fields and process fields with usage page = 0x07 / Keyboard Check if SHIFT is down and process first character (don't handle multiple key presses)



### Packet Handler


  The packet handler responds to various HID events.


~~~~ {#lst:hidhostdemopacketHandler .c caption="{Packet Handler}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
    {
    ...
      switch (packet_type) {
    		case HCI_EVENT_PACKET:
          event = hci_event_packet_get_type(packet);
          
          switch (event) {      
    #ifndef HAVE_BTSTACK_STDIN
            case BTSTACK_EVENT_STATE:
              if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
                status = hid_host_connect(remote_addr, hid_host_report_mode, &hid_host_cid);
                if (status != ERROR_CODE_SUCCESS){
                  printf("HID host connect failed, status 0x%02x.\n", status);
                }
              }
              break;
    #endif
    ...
            case HCI_EVENT_HID_META:
              switch (hci_event_hid_meta_get_subevent_code(packet)){
    
                case HID_SUBEVENT_INCOMING_CONNECTION:
                  // There is an incoming connection: we can accept it or decline it
    
                  // If there are no resources to handle the connection,
                  // the status will be ERROR_CODE_MEMORY_CAPACITY_EXCEEDED
    
                  // The hid_host_report_mode in the hid_host_accept_connection function
                  // allows the application to request a protocol mode.
    
                  // For available protocol modes, see hid_protocol_mode_t in btstack_hid.h file.
                  if (hid_subevent_incoming_connection_get_status(packet) == ERROR_CODE_SUCCESS) {
                    hid_host_accept_connection(hid_subevent_incoming_connection_get_hid_cid(packet), hid_host_report_mode);
                  }
                  break;
                
                case HID_SUBEVENT_CONNECTION_OPENED:
                  // The status field of this event indicates if the control and interrupt
                  // connections were opened successfully.
                  status = hid_subevent_connection_opened_get_status(packet);
                  if (status != ERROR_CODE_SUCCESS) {
                    printf("Connection failed, status 0x%02x\n", status);
                    app_state = APP_IDLE;
                    hid_host_cid = 0;
                    return;
                  }
                  app_state = APP_CONNECTED;
                  hid_host_descriptor_available = false;
                  hid_host_cid = hid_subevent_connection_opened_get_hid_cid(packet);
                  hid_host_caps_lock = false;
                  hid_host_led_report_len = 0;
                  printf("HID Host connected.\n");
                  break;
    
                case HID_SUBEVENT_DESCRIPTOR_AVAILABLE:
                  // This event will follows HID_SUBEVENT_CONNECTION_OPENED event. 
                  // For incoming connections, i.e. HID Device initiating the connection,
                  // the HID_SUBEVENT_DESCRIPTOR_AVAILABLE is delayed, and some HID  
                  // reports may be received via HID_SUBEVENT_REPORT event. It is up to 
                  // the application if these reports should be buffered or ignored until 
                  // the HID descriptor is available.
                  status = hid_subevent_descriptor_available_get_status(packet);
                  if (status == ERROR_CODE_SUCCESS){
                    hid_host_descriptor_available = true;
                    printf("HID Descriptor available, please start typing.\n");
                    hid_host_demo_lookup_caps_lock_led();
                  } else {
                    printf("Cannot handle input report, HID Descriptor is not available, status 0x%02x\n", status);
                  }
                  break;
    
                case HID_SUBEVENT_REPORT:
                  // Handle input report.
                  if (hid_host_descriptor_available){
                    hid_host_handle_interrupt_report(hid_subevent_report_get_report(packet), hid_subevent_report_get_report_len(packet));
                  } else {
                    printf_hexdump(hid_subevent_report_get_report(packet), hid_subevent_report_get_report_len(packet));
                  }
                  break;
    
                case HID_SUBEVENT_SET_PROTOCOL_RESPONSE:
                  // For incoming connections, the library will set the protocol mode of the
                  // HID Device as requested in the call to hid_host_accept_connection. The event 
                  // reports the result. For connections initiated by calling hid_host_connect, 
                  // this event will occur only if the established report mode is boot mode.
                  status = hid_subevent_set_protocol_response_get_handshake_status(packet);
                  if (status != HID_HANDSHAKE_PARAM_TYPE_SUCCESSFUL){
                    printf("Error set protocol, status 0x%02x\n", status);
                    break;
                  }
                  switch ((hid_protocol_mode_t)hid_subevent_set_protocol_response_get_protocol_mode(packet)){
                    case HID_PROTOCOL_MODE_BOOT:
                      printf("Protocol mode set: BOOT.\n");
                      break;  
                    case HID_PROTOCOL_MODE_REPORT:
                      printf("Protocol mode set: REPORT.\n");
                      break;
                    default:
                      printf("Unknown protocol mode.\n");
                      break; 
                  }
                  break;
    
                case HID_SUBEVENT_CONNECTION_CLOSED:
                  // The connection was closed.
                  hid_host_cid = 0;
                  hid_host_descriptor_available = false;
                  printf("HID Host disconnected.\n");
                  break;
                
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

~~~~



  When BTSTACK_EVENT_STATE with state HCI_STATE_WORKING is received and the example is started in client mode, the remote SDP HID query is started.

## HID Keyboard LE {#sec:hogkeyboarddemoExample}

Source Code: [hog_keyboard_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hog_keyboard_demo.c)



## HID Mouse LE {#sec:hogmousedemoExample}

Source Code: [hog_mouse_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hog_mouse_demo.c)



## HID Boot Host LE {#sec:hogboothostdemoExample}

Source Code: [hog_boot_host_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/hog_boot_host_demo.c)



  This example implements a minimal HID-over-GATT Boot Host. It scans for LE HID devices, connects to it, discovers the Characteristics relevant for the HID Service and enables Notifications on them. It then dumps all Boot Keyboard and Mouse Input Reports

### HOG Boot Keyboard Handler


  Boot Keyboard Input Report contains a report of format [ modifier, reserved, 6 x usage for key 1..6 from keyboard usage] Track new usages, map key usage to actual character and simulate terminal

### HOG Boot Mouse Handler


  Boot Mouse Input Report contains a report of format [ buttons, dx, dy, dz = scroll wheel] Decode packet and print on stdout

 @param packet_type @param channel @param packet @param size

### Test if advertisement contains HID UUID



~~~~ {#lst:hogboothostdemopacketHandler .c caption="{Packet Handler}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    ...
      switch (packet_type) {
        case HCI_EVENT_PACKET:
          event = hci_event_packet_get_type(packet);
          switch (event) {
            case BTSTACK_EVENT_STATE:
              if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) break;
              btstack_assert(app_state == W4_WORKING);
              hog_start_connect();
              break;
            case GAP_EVENT_ADVERTISING_REPORT:
              if (app_state != W4_HID_DEVICE_FOUND) break;
              if (adv_event_contains_hid_service(packet) == false) break;
              // stop scan
              gap_stop_scan();
              // store remote device address and type
              gap_event_advertising_report_get_address(packet, remote_device.addr);
              remote_device.addr_type = gap_event_advertising_report_get_address_type(packet);
              // connect
              printf("Found, connect to device with %s address %s ...\n", remote_device.addr_type == 0 ? "public" : "random" , bd_addr_to_str(remote_device.addr));
              hog_connect();
              break;
            case HCI_EVENT_DISCONNECTION_COMPLETE:
              if (app_state != READY) break;
              
              connection_handle = HCI_CON_HANDLE_INVALID;
              switch (app_state){
                case READY:
                  printf("\nDisconnected, try to reconnect...\n");
                  app_state = W4_TIMEOUT_THEN_RECONNECT;
                  break;
                default:
                  printf("\nDisconnected, start over...\n");
                  app_state = W4_TIMEOUT_THEN_SCAN;
                  break;
              }
              // set timer
              btstack_run_loop_set_timer(&connection_timer, 100);
              btstack_run_loop_set_timer_handler(&connection_timer, &hog_reconnect_timeout);
              btstack_run_loop_add_timer(&connection_timer);
              break;
            case HCI_EVENT_META_GAP:
              // wait for connection complete
              if (hci_event_gap_meta_get_subevent_code(packet) != GAP_SUBEVENT_LE_CONNECTION_COMPLETE) break;
              if (app_state != W4_CONNECTED) return;
              btstack_run_loop_remove_timer(&connection_timer);
              connection_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
              // request security
              app_state = W4_ENCRYPTED;
              sm_request_pairing(connection_handle);
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

~~~~



### HCI packet handler


  The SM packet handler receives Security Manager Events required for pairing. It also receives events generated during Identity Resolving see Listing [here](#lst:hogboothostdemoSMPacketHandler).


~~~~ {#lst:hogboothostdemoSMPacketHandler .c caption="{Scanning and receiving advertisements}"}

    
    static void sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
    
      bool connect_to_service = false;
    
      switch (hci_event_packet_get_type(packet)) {
        case SM_EVENT_JUST_WORKS_REQUEST:
          printf("Just works requested\n");
          sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
          break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
          printf("Confirming numeric comparison: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
          sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
          break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
          printf("Display Passkey: %"PRIu32"\n", sm_event_passkey_display_number_get_passkey(packet));
          break;
        case SM_EVENT_PAIRING_COMPLETE:
          switch (sm_event_pairing_complete_get_status(packet)){
            case ERROR_CODE_SUCCESS:
              printf("Pairing complete, success\n");
              connect_to_service = true;
              break;
            case ERROR_CODE_CONNECTION_TIMEOUT:
              printf("Pairing failed, timeout\n");
              break;
            case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
              printf("Pairing failed, disconnected\n");
              break;
            case ERROR_CODE_AUTHENTICATION_FAILURE:
              printf("Pairing failed, reason = %u\n", sm_event_pairing_complete_get_reason(packet));
              break;
            default:
              break;
          }
          break;
        case SM_EVENT_REENCRYPTION_COMPLETE:
          printf("Re-encryption complete, success\n");
          connect_to_service = true;
          break;
        default:
          break;
      }
    
      if (connect_to_service){
        // continue - query primary services
        app_state = W2_QUERY_HID_SERVICE;
        hog_host_request_to_send();
      }
    }

~~~~




~~~~ {#lst:hogboothostdemoHogBootHostSetup .c caption="{HID-over-GATT Boot Host Setup}"}

    
      l2cap_init();
    
      // setup SM: Display only
      sm_init();
      sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
      sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING);
    
      //
      gatt_client_init();
    
      // register for events from HCI
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for events from Security Manager
      sm_event_callback_registration.callback = &sm_packet_handler;
      sm_add_event_handler(&sm_event_callback_registration);
    

~~~~



## Dual Mode - SPP and LE Counter {#sec:sppandgattcounterExample}

Source Code: [spp_and_gatt_counter.c](https://github.com/bluekitchen/btstack/tree/master/example/spp_and_gatt_counter.c)



  The SPP and LE Counter example combines the Bluetooth Classic SPP Counter and the Bluetooth LE Counter into a single application.



  In this Section, we only point out the differences to the individual examples and how the stack is configured.



  Note: To test, please run the example, and then: 
 
- for SPP pair from a remote device, and open the Virtual Serial Port,
- for LE use some GATT Explorer, e.g. LightBlue, BLExplr, to enable notifications.



### Advertisements 


  The Flags attribute in the Advertisement Data indicates if a device is dual-mode or le-only.


~~~~ {#lst:sppandgattcounteradvertisements .c caption="{Advertisement data: Flag 0x02 indicates dual-mode device}"}

    const uint8_t adv_data[] = {
      // Flags general discoverable
      0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x02,
      // Name
      0x0b, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'L', 'E', ' ', 'C', 'o', 'u', 'n', 't', 'e', 'r', 
      // Incomplete List of 16-bit Service Class UUIDs -- FF10 - only valid for testing!
      0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xff,
    };

~~~~



### Packet Handler


  The packet handler of the combined example is just the combination of the individual packet handlers.

### Heartbeat Handler


  Similar to the packet handler, the heartbeat handler is the combination of the individual ones. After updating the counter, it requests an ATT_EVENT_CAN_SEND_NOW and/or RFCOMM_EVENT_CAN_SEND_NOW


~~~~ {#lst:sppandgattcounterheartbeat .c caption="{Combined Heartbeat handler}"}

    static void heartbeat_handler(struct btstack_timer_source *ts){
    
      if (rfcomm_channel_id || le_notification_enabled) {
        beat();
      }
    
      if (rfcomm_channel_id){
        rfcomm_request_can_send_now_event(rfcomm_channel_id);
      }
    
      if (le_notification_enabled) {
        att_server_request_can_send_now_event(att_con_handle);
      }
    
      btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(ts);
    } 

~~~~



### Main Application Setup


  As with the packet and the heartbeat handlers, the combined app setup contains the code from the individual example setups.


~~~~ {#lst:sppandgattcounterMainConfiguration .c caption="{Init L2CAP RFCOMM SDO SM ATT Server and start heartbeat timer}"}

    int btstack_main(void);
    int btstack_main(void)
    {
      l2cap_init();
    
      rfcomm_init();
      rfcomm_register_service(packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff);
    
      // init SDP, create record for SPP and register with SDP
      sdp_init();
      memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
      spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), RFCOMM_SERVER_CHANNEL, "SPP Counter");
      btstack_assert(de_get_len( spp_service_buffer) <= sizeof(spp_service_buffer));
      sdp_register_service(spp_service_buffer);
    
    #ifdef ENABLE_GATT_OVER_CLASSIC
      // init SDP, create record for GATT and register with SDP
      memset(gatt_service_buffer, 0, sizeof(gatt_service_buffer));
      gatt_create_sdp_record(gatt_service_buffer, 0x10001, ATT_SERVICE_GATT_SERVICE_START_HANDLE, ATT_SERVICE_GATT_SERVICE_END_HANDLE);
      sdp_register_service(gatt_service_buffer);
      printf("SDP service record size: %u\n", de_get_len(gatt_service_buffer));
    #endif
    
      gap_set_local_name("SPP and LE Counter 00:00:00:00:00:00");
      gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
      gap_discoverable_control(1);
    
      // setup SM: Display only
      sm_init();
    
      // setup ATT server
      att_server_init(profile_data, att_read_callback, att_write_callback);  
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // register for ATT events
      att_server_register_packet_handler(packet_handler);
    
      // setup advertisements
      uint16_t adv_int_min = 0x0030;
      uint16_t adv_int_max = 0x0030;
      uint8_t adv_type = 0;
      bd_addr_t null_addr;
      memset(null_addr, 0, 6);
      gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
      gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
      gap_advertisements_enable(1);
    
      // set one-shot timer
      heartbeat.process = &heartbeat_handler;
      btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
      btstack_run_loop_add_timer(&heartbeat);
    
      // beat once
      beat();
    
      // turn on!
    	hci_power_control(HCI_POWER_ON);
    	  
      return 0;
    }

~~~~



## SDP Client - Query Remote SDP Records {#sec:sdpgeneralqueryExample}

Source Code: [sdp_general_query.c](https://github.com/bluekitchen/btstack/tree/master/example/sdp_general_query.c)



  The example shows how the SDP Client is used to get a list of  service records on a remote device. 

### SDP Client Setup


  SDP is based on L2CAP. To receive SDP query events you must register a callback, i.e. query handler, with the SPD parser, as shown in  Listing [here](#lst:sdpgeneralquerySDPClientInit). Via this handler, the SDP client will receive the following events:
 
- SDP_EVENT_QUERY_ATTRIBUTE_VALUE containing the results of the query in chunks,
- SDP_EVENT_QUERY_COMPLETE indicating the end of the query and the status




~~~~ {#lst:sdpgeneralquerySDPClientInit .c caption="{SDP client setup}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void handle_sdp_client_query_result(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void sdp_general_query_init(void){
      // init L2CAP
      l2cap_init();
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    }

~~~~



### SDP Client Query 


  To trigger an SDP query to get the a list of service records on a remote device, you need to call sdp_client_query_uuid16() with the remote address and the UUID of the public browse group, as shown in Listing [here](#lst:sdpgeneralquerySDPQueryUUID).  In this example we used a fixed address for the remote device; please update it with the address of a device in your vicinity, e.g., one reported by the GAP Inquiry example in the previous section.


~~~~ {#lst:sdpgeneralquerySDPQueryUUID .c caption="{Querying a list of service records on a remote device.}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
      uint8_t event = hci_event_packet_get_type(packet);
    
      switch (event) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started 
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
            printf("Connecting to %s\n", bd_addr_to_str(remote_addr));
            sdp_client_query_uuid16(&handle_sdp_client_query_result, remote_addr, BLUETOOTH_PROTOCOL_L2CAP);
          }
          break;
        default:
          break;
      }
    }

~~~~



### Handling SDP Client Query Results 


  The SDP Client returns the results of the query in chunks. Each result packet contains the record ID, the Attribute ID, and a chunk of the Attribute value. In this example, we append new chunks for the same Attribute ID in a large buffer, see Listing [here](#lst:sdpgeneralqueryHandleSDPQUeryResult).

 To save memory, it's also possible to process these chunks directly by a custom stream parser, similar to the way XML files are parsed by a SAX parser. Have a look at *src/sdp_client_rfcomm.c* which retrieves the RFCOMM channel number and the service name.


~~~~ {#lst:sdpgeneralqueryHandleSDPQUeryResult .c caption="{Handling query result chunks.}"}

    static void handle_sdp_client_query_result(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(packet_type);
      UNUSED(channel);
      UNUSED(size);
    
      switch (hci_event_packet_get_type(packet)){
        case SDP_EVENT_QUERY_ATTRIBUTE_VALUE:
          // handle new record
          if (sdp_event_query_attribute_byte_get_record_id(packet) != record_id){
            record_id = sdp_event_query_attribute_byte_get_record_id(packet);
            printf("\n---\nRecord nr. %u\n", record_id);
          }
    
          assertBuffer(sdp_event_query_attribute_byte_get_attribute_length(packet));
    
          attribute_value[sdp_event_query_attribute_byte_get_data_offset(packet)] = sdp_event_query_attribute_byte_get_data(packet);
          if ((uint16_t)(sdp_event_query_attribute_byte_get_data_offset(packet)+1) == sdp_event_query_attribute_byte_get_attribute_length(packet)){
             printf("Attribute 0x%04x: ", sdp_event_query_attribute_byte_get_attribute_id(packet));
             de_dump_data_element(attribute_value);
          }
          break;
        case SDP_EVENT_QUERY_COMPLETE:
          if (sdp_event_query_complete_get_status(packet)){
            printf("SDP query failed 0x%02x\n", sdp_event_query_complete_get_status(packet));
            break;
          } 
          printf("SDP query done.\n");
          break;
        default:
          break;
      }
    }

~~~~



## SDP Client - Query RFCOMM SDP record {#sec:sdprfcommqueryExample}

Source Code: [sdp_rfcomm_query.c](https://github.com/bluekitchen/btstack/tree/master/example/sdp_rfcomm_query.c)



  The example shows how the SDP Client is used to get all RFCOMM service records from a remote device. It extracts the remote RFCOMM Server Channel,  which are needed to connect to a remote RFCOMM service.

## SDP Client - Query BNEP SDP record {#sec:sdpbnepqueryExample}

Source Code: [sdp_bnep_query.c](https://github.com/bluekitchen/btstack/tree/master/example/sdp_bnep_query.c)



  The example shows how the SDP Client is used to get all BNEP service records from a remote device. It extracts the remote BNEP PAN protocol  UUID and the L2CAP PSM, which are needed to connect to a remote BNEP service.

### SDP Client Setup


  As with the previous example, you must register a callback, i.e. query handler, with the SPD parser, as shown in  Listing [here](#lst:sdpbnepquerySDPClientInit). Via this handler, the SDP client will receive events:
 
- SDP_EVENT_QUERY_ATTRIBUTE_VALUE containing the results of the query in chunks,
- SDP_EVENT_QUERY_COMPLETE reporting the status and the end of the query. 




~~~~ {#lst:sdpbnepquerySDPClientInit .c caption="{SDP client setup}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    static void handle_sdp_client_query_result(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    
    static void sdp_bnep_qeury_init(void){
      // init L2CAP
      l2cap_init();
    
      // register for HCI events
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    }

~~~~



### SDP Client Query 



~~~~ {#lst:sdpbnepquerySDPQueryUUID .c caption="{Querying the a list of service records on a remote device.}"}

    static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(channel);
      UNUSED(size);
    
      if (packet_type != HCI_EVENT_PACKET) return;
      uint8_t event = hci_event_packet_get_type(packet);
    
      switch (event) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started 
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
            printf("Start SDP BNEP query.\n");
            sdp_client_query_uuid16(&handle_sdp_client_query_result, remote, BLUETOOTH_PROTOCOL_BNEP);
          }
          break;
        default:
          break;
      }
    }

~~~~



### Handling SDP Client Query Result 


  The SDP Client returns the result of the query in chunks. Each result packet contains the record ID, the Attribute ID, and a chunk of the Attribute value, see Listing [here](#lst:sdpbnepqueryHandleSDPQUeryResult). Here, we show how to parse the Service Class ID List and Protocol Descriptor List, as they contain  the BNEP Protocol UUID and L2CAP PSM respectively.


~~~~ {#lst:sdpbnepqueryHandleSDPQUeryResult .c caption="{Extracting BNEP Protcol UUID and L2CAP PSM}"}

    static void handle_sdp_client_query_result(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
      UNUSED(packet_type);
      UNUSED(channel);
      UNUSED(size);
    
    ...
    
            switch(sdp_event_query_attribute_byte_get_attribute_id(packet)){
              // 0x0001 "Service Class ID List"
              case BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST:
                if (de_get_element_type(attribute_value) != DE_DES) break;
                for (des_iterator_init(&des_list_it, attribute_value); des_iterator_has_more(&des_list_it); des_iterator_next(&des_list_it)){
                  uint8_t * element = des_iterator_get_element(&des_list_it);
                  if (de_get_element_type(element) != DE_UUID) continue;
                  uint32_t uuid = de_get_uuid32(element);
                  switch (uuid){
                    case BLUETOOTH_SERVICE_CLASS_PANU:
                    case BLUETOOTH_SERVICE_CLASS_NAP:
                    case BLUETOOTH_SERVICE_CLASS_GN:
                      printf(" ** Attribute 0x%04x: BNEP PAN protocol UUID: %04x\n", sdp_event_query_attribute_byte_get_attribute_id(packet), (int) uuid);
                      break;
                    default:
                      break;
                  }
                }
                break;
    ...
              case BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST:{
                  printf(" ** Attribute 0x%04x: ", sdp_event_query_attribute_byte_get_attribute_id(packet));
                  
                  uint16_t l2cap_psm = 0;
                  uint16_t bnep_version = 0;
                  for (des_iterator_init(&des_list_it, attribute_value); des_iterator_has_more(&des_list_it); des_iterator_next(&des_list_it)){
                    if (des_iterator_get_type(&des_list_it) != DE_DES) continue;
                    uint8_t * des_element = des_iterator_get_element(&des_list_it);
                    des_iterator_init(&prot_it, des_element);
                    uint8_t * element = des_iterator_get_element(&prot_it);
                    
                    if (de_get_element_type(element) != DE_UUID) continue;
                    uint32_t uuid = de_get_uuid32(element);
                    des_iterator_next(&prot_it);
                    switch (uuid){
                      case BLUETOOTH_PROTOCOL_L2CAP:
                        if (!des_iterator_has_more(&prot_it)) continue;
                        de_element_get_uint16(des_iterator_get_element(&prot_it), &l2cap_psm);
                        break;
                      case BLUETOOTH_PROTOCOL_BNEP:
                        if (!des_iterator_has_more(&prot_it)) continue;
                        de_element_get_uint16(des_iterator_get_element(&prot_it), &bnep_version);
                        break;
                      default:
                        break;
                    }
                  }
                  printf("l2cap_psm 0x%04x, bnep_version 0x%04x\n", l2cap_psm, bnep_version);
                }
                break;
    ...
    }

~~~~



  The Service Class ID List is a Data Element Sequence (DES) of UUIDs.  The BNEP PAN protocol UUID is within this list.

  The Protocol Descriptor List is DES  which contains one DES for each protocol. For PAN serivces, it contains a DES with the L2CAP Protocol UUID and a PSM, and another DES with the BNEP UUID and the the BNEP version.

## PBAP Client - Get Contacts from Phonebook Server {#sec:pbapclientdemoExample}

Source Code: [pbap_client_demo.c](https://github.com/bluekitchen/btstack/tree/master/example/pbap_client_demo.c)



  Note: The Bluetooth address of the remote Phonbook server is hardcoded.  Change it before running example, then use the UI to connect to it, to set and  query contacts.

## Testing - Enable Device Under Test (DUT) Mode for Classic {#sec:dutmodeclassicExample}

Source Code: [dut_mode_classic.c](https://github.com/bluekitchen/btstack/tree/master/example/dut_mode_classic.c)



  DUT mode can be used for production testing. This example just configures  the Bluetooth Controller for DUT mode.

### Bluetooth Logic 


  When BTstack is up and running, send Enable Device Under Test Mode Command and print its result.

  For more details on discovering remote devices, please see Section on [GAP](../profiles/#sec:GAPdiscoverRemoteDevices).

### Main Application Setup


  Listing [here](#lst:dutmodeclassicMainConfiguration) shows main application code. It registers the HCI packet handler and starts the Bluetooth stack.


~~~~ {#lst:dutmodeclassicMainConfiguration .c caption="{Setup packet handler }"}

    int btstack_main(int argc, const char * argv[]);
    int btstack_main(int argc, const char * argv[]) {
      (void)argc;
      (void)argv;
    
      // disable Secure Simple Pairinng
      gap_ssp_set_enable(0);
    
      // make device connectable
      // @note: gap_connectable_control will be enabled when an L2CAP service 
      // (e.g. RFCOMM) is initialized). Therefore, it's not needed in regular applications
      gap_connectable_control(1);
    
      // make device discoverable
      gap_discoverable_control(1);
    
      hci_event_callback_registration.callback = &packet_handler;
      hci_add_event_handler(&hci_event_callback_registration);
    
      // turn on!
      hci_power_control(HCI_POWER_ON);
        
      return 0;
    }

~~~~
