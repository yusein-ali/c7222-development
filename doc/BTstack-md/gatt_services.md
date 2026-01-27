#

BTstack allows to implement and use GATT Services in a modular way.

To use an already implemented GATT Service Server, you only have to add it to your application's GATT file with:
  - `#import <service_name.gatt>` for .gatt files located in *src/ble/gatt-service* 
  - `#import "service_name.gatt"` for .gatt files located in the same folder as your application's .gatt file.

Each service will have an API at *src/ble/gatt-service/service_name_server.h*. To activate it, you need to call *service_name_init(..)*.

Please see the .h file for details.

## Battery Service Server {#sec:battery-service-server}

The Battery Service allows to query your device's battery level in a standardized way. 

To use with your application, add `#import <battery_service.gatt>` to your .gatt file.  After adding it to your .gatt file, you call *battery_service_server_init(value)* with the current value of your battery. The valid range for the battery level is 0-100. 

If the battery level changes, you can call *battery_service_server_set_battery_value(value)*.  The service supports sending Notifications if the client enables them. 

See [Battery Service Server API](../appendix/apis/#battery-service-server).


## Battery Service V1 Server {#sec:battery-service-v1-server}

The Battery Service allows to query your device's battery level in a standardized way. 

To use with your application, add `#import <battery_service.gatt>` to your .gatt file.  After adding it to your .gatt file, you call *battery_service_server_init(value)* with the current value of your battery. The valid range for the battery level is 0-100. 

If the battery level changes, you can call *battery_service_server_set_battery_value(value)*.  The service supports sending Notifications if the client enables them. 

See [Battery Service V1 Server API](../appendix/apis/#battery-service-v1-server).


## Bond Management Service Server {#sec:bond-management-service-server}

The Bond Management service server defines how a peer Bluetooth device can manage the storage of bond information,  especially the deletion of it, on the Bluetooth device supporting this service. 

To use with your application, add `#import <bond_management_service.gatt>` to your .gatt file. After adding it to your .gatt file, you call *bond_management_service_server_init(supported_features)*. The input parameter "supported_features"  is a bitmap, which defines how the bond information will be deleted, see BMF_DELETE_* flags below. 

See [Bond Management Service Server API](../appendix/apis/#bond-management-service-server).


## Cycling Power Service Server {#sec:cycling-power-service-server}

The Cycling Power Service allows to query device's power- and  force-related data and optionally speed- and cadence-related data for  use in sports and fitness applications. 

To use with your application, add `#import <cycling_power_service.gatt>`  to your .gatt file. 

See [Cycling Power Service Server API](../appendix/apis/#cycling-power-service-server).


## Cycling Speed and Cadence Service Server {#sec:cycling-speed-and-cadence-service-server}

The Cycling Speed and Cadence Service allows to query  device's speed- and cadence-related data for use in sports and  fitness applications. 

To use with your application, add `#import <cycling_speed_and_cadence_service.gatt>`  to your .gatt file.  

See [Cycling Speed and Cadence Service Server API](../appendix/apis/#cycling-speed-and-cadence-service-server).


## Device Information Service Server {#sec:device-information-service-server}

Th eDevice Information Service allows to query manufacturer and/or  vendor information about a device. 

To use with your application, add `#import <device_information_service.gatt>` to your .gatt file.  

*Note*: instead of calling all setters, you can create a local copy of the .gatt file and remove all Characteristics that are not relevant for your application and define all fixed values in the .gatt file. 

See [Device Information Service Server API](../appendix/apis/#device-information-service-server).


## Heart Rate Service Server {#sec:heart-rate-service-server}

The heart rate service server provides heart rate measurements via notifications. 

Each notification reports the heart rate measurement in beats per minute, and if enabled,  the total energy expended in kilo Joules, as well as RR-intervals in 1/1024 seconds resolution. 

The Energy Expended field represents the accumulated energy expended in kilo Joules since the last time it was reset. If the maximum value of 65535 kilo Joules is reached, it will remain at this value, until a reset command from the client is received. 

The RR-Interval represents the time between two consecutive R waves in  an Electrocardiogram (ECG) waveform. If needed, the RR-Intervals are sent in multiple notifications. 

To use with your application, add `#import <heart_rate_service.gatt>` to your .gatt file. After adding it to your .gatt file, you call *heart_rate_server_init(body_sensor_location, energy_expended_supported)* with the intended sensor location, and a flag indicating if energy expanded is supported. 

If heart rate measurement changes, you can call  *heart_rate_service_server_update_heart_rate_values(heart_rate_bpm, service_sensor_contact_status, rr_interval_count, rr_intervals)*.  This function will trigger sending Notifications if the client enables them. 

If energy expanded is supported, you can call *heart_rate_service_add_energy_expended(energy_expended_kJ)*  with the newly expanded energy. The accumulated energy expended value will be emitted with the next heart rate measurement. 

See [Heart Rate Service Server API](../appendix/apis/#heart-rate-service-server).


## HIDS Device {#sec:hids-device}

Implementation of the GATT HIDS Device To use with your application, add '#import <hids.gatt>' to your .gatt file 

See [HIDS Device API](../appendix/apis/#hids-device).


## Immediate Alert Service Server {#sec:immediate-alert-service-server}

To use with your application, add `#import <immediate_alert_service.gatt>` to your .gatt file.  



See [Immediate Alert Service Server API](../appendix/apis/#immediate-alert-service-server).


## Link Loss Service Server {#sec:link-loss-service-server}



See [Link Loss Service Server API](../appendix/apis/#link-loss-service-server).


## Nordic SPP Service Server {#sec:nordic-spp-service-server}

The Nordic SPP Service is implementation of the Nordic SPP-like profile. 

To use with your application, add `#import <nordic_spp_service.gatt>` to your .gatt file and call all functions below. All strings and blobs need to stay valid after calling the functions. 

See [Nordic SPP Service Server API](../appendix/apis/#nordic-spp-service-server).


## Scan Parameters Service Server {#sec:scan-parameters-service-server}

The Scan Parameters Service enables a remote GATT Client to store the LE scan parameters it is using locally. These parameters can be utilized by the application to optimize power consumption and/or reconnection latency. 

To use with your application, add `#import <scan_parameters_service.gatt>` to your .gatt file and call all functions below. All strings and blobs need to stay valid after calling the functions. 

See [Scan Parameters Service Server API](../appendix/apis/#scan-parameters-service-server).


## Tx Power Service Server {#sec:tx-power-service-server}

The TX Power service exposes a device's current transmit power level when in a connection.  There shall only be one instance of the Tx Power service on a device. 

To use with your application, add `#import <tx_power_service.gatt>` to your .gatt file.  After adding it to your .gatt file, you call *tx_power_service_server_init(value)* with the device's current transmit power level value.  

If the power level value changes, you can call *tx_power_service_server_set_level(tx_power_level_dBm)*.  The service does not support sending Notifications. 

See [Tx Power Service Server API](../appendix/apis/#tx-power-service-server).


## u-blox SPP Service Server {#sec:ublox-spp-service-server}

The u-blox SPP Service is implementation of the u-Blox SPP-like profile. 

To use with your application, add `#import <ublox_spp_service.gatt>` to your .gatt file and call all functions below. All strings and blobs need to stay valid after calling the functions. 

See [u-blox SPP Service Server API](../appendix/apis/#ublox-spp-service-server).

