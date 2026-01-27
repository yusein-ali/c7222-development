#

BTstack allows to implement and use GATT Clients in a modular way.


## ANCS Client {#sec:ancs-client}

The ANCS Client implements Notification Consumer (NC) of the [Apple Notification Center Service (ANCS)](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Introduction/Introduction.html). 

See [ANCS Client API](../appendix/apis/#ancs-client).


## Battery Service Client {#sec:battery-service-client}

The Battery Service Client connects to the Battery Services of a remote device  and queries its battery level values. Level updates are either received via notifications  (if supported by the remote Battery Service), or by manual polling. 

See [Battery Service Client API](../appendix/apis/#battery-service-client).


## Device Information Service Client {#sec:device-information-service-client}

The Device Information Service Client retrieves the following information from a remote device: - manufacturer name- model number     - serial number    - hardware revision- firmware revision- software revision- system ID        - IEEE regulatory certification- PNP ID  - UDI for medical devices

See [Device Information Service Client API](../appendix/apis/#device-information-service-client).


## Immediate Alert Service Client {#sec:immediate-alert-service-client}

The Immediate Alert Service Client  

See [Immediate Alert Service Client API](../appendix/apis/#immediate-alert-service-client).


## Link Loss Service Client {#sec:link-loss-service-client}

The Link Loss Service Client  

See [Link Loss Service Client API](../appendix/apis/#link-loss-service-client).


## Scan Parameters Service Client {#sec:scan-parameters-service-client}

The Scan Parameters Service Client allows to store its  LE scan parameters on a remote device such that the remote device  can utilize this information to optimize power consumption and/or  reconnection latency. 

See [Scan Parameters Service Client API](../appendix/apis/#scan-parameters-service-client).


## Tx Power Service Client {#sec:tx-power-service-client}

The Immediate Alert Service Client  

See [Tx Power Service Client API](../appendix/apis/#tx-power-service-client).

