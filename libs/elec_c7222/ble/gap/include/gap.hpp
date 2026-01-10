/**
 * @file gap.hpp
 * @brief Provides a C++ wrapper for the BTstack Generic Access Profile (GAP).
 */
#ifndef ELEC_C7222_BLE_GAP_HPP
#define ELEC_C7222_BLE_GAP_HPP

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <list>
#include <map>
#include <vector>

#include "advertisement_data.hpp"
#include "ble_address.hpp"
#include "ble_error.hpp"
#include "non_copyable.hpp"

namespace c7222 {

enum class BleError : uint8_t;

using ConnectionHandle = uint16_t;

/**
 * @class Gap
 * @brief Manages Generic Access Profile (GAP) functionality for BLE.
 *
 * @details
 * This class provides a high-level, object-oriented interface for managing the
 * BLE GAP layer, acting as a C++ wrapper around the underlying C-based BTstack
 * API. It simplifies common GAP operations such as advertising, scanning, and
 * connection management by maintaining state and handling HCI event dispatching.
 *
 * The `Gap` class is implemented as a singleton, accessible via `getInstance()`,
 * ensuring a single point of control for the device's GAP layer.
 *
 * ---
 * ### Design Logic
 *
 * The class abstracts away the low-level details of the BTstack. It works by:
 * 1.  **Configuration Caching:** Storing advertising parameters, data, and other
 *     settings within the class.
 * 2.  **State Management:** Tracking the advertising state (`isAdvertisingEnabled()`)
 *     and connection status (`isConnected()`).
 * 3.  **Event-Driven Callbacks:** Using the `EventHandler` interface, which users
 *     can implement to react to BLE events (e.g., connection, disconnection,
 *     advertising reports) in a clean, C++ idiomatic way.
 * 4.  **HCI Event Dispatching:** The `dispatch_ble_hci_packet` method is the entry
 *     point for raw HCI events from the BTstack, which are then parsed and
 *     forwarded to the appropriate `EventHandler` methods.
 *
 * ---
 * ### Advertising Configuration
 *
 * To start advertising, you must configure three main components:
 * 1.  **Advertising Parameters:** Define the "how" of advertising.
 *     - **Type:** Connectable, scannable, non-connectable, etc.
 *     - **Interval:** The frequency of advertising packets.
 *     - **Channels:** Which BLE channels to advertise on.
 *     This is configured using `setAdvertisingParameters()`.
 *
 * 2.  **Advertising Data:** The main payload (up to 31 bytes) that is broadcast
 *     to all listening devices. This typically includes flags and the device name.
 *     This is configured using `setAdvertisingData()`.
 *
 * 3.  **Scan Response Data (Optional):** An additional 31-byte payload that a
 *     central device can request after seeing the initial advertisement. This is
 *     often used for supplementary information that doesn't fit in the main payload.
 *     This is configured using `setScanResponseData()`.
 *
 * ### Applying Configurations and Utilities
 *
 * - **`setAdvertisingParameters(const AdvertisementParameters& params)`:** Takes a struct
 *   that defines the advertising behavior. A default-constructed `AdvertisementParameters`
 *   provides standard connectable, undirected advertising settings.
 *
 * - **`setAdvertisingData(...)` / `setScanResponseData(...)`:** These methods accept
 *   a raw pointer and length. For convenience, `setAdvertisingData` is overloaded to
 *   accept a `std::vector<uint8_t>` or an `AdvertisementDataBuilder` instance, which
 *   is the recommended utility for safely constructing valid advertising payloads.
 *
 * - **`startAdvertising()` / `stopAdvertising()`:** Simple methods to enable or
 *   disable advertising.
 *
 * ### Dynamic Data Updates
 *
 * You can update advertising or scan response data at any time, even while
 * advertising is active. The `setAdvertisingData` and `setScanResponseData` methods
 * will automatically handle the underlying requirements of the RPi Pico BLE stack:
 * they temporarily stop advertising, apply the new data, and restart advertising
 * if it was previously enabled. This ensures a seamless update without manual
 * intervention.
 *
 * ---
 * ### Complete Code Example
 *
 * Below is a complete example of setting up and starting a connectable BLE
 * advertisement with a device name.
 *
 * ```cpp
 * #include "gap.hpp"
 * #include "advertisement_data.hpp"
 * #include <iostream>
 *
 * // --- 1. Implement an event handler ---
 * class MyGapEventHandler : public c7222::Gap::EventHandler {
 * public:
 *     void onAdvertisingStart(uint8_t status) const override {
 *         if (status == 0) {
 *             std::cout << "Advertising started successfully." << std::endl;
 *         } else {
 *             std::cout << "Failed to start advertising, status: " << (int)status << std::endl;
 *         }
 *     }
 *
 *     void onConnectionComplete(uint8_t status, c7222::ConnectionHandle handle,
 *                               const c7222::BleAddress& address,
 *                               uint16_t, uint16_t, uint16_t) const override {
 *         if (status == 0) {
 *             std::cout << "Device connected: " << address << std::endl;
 *         }
 *     }
 *
 *     void onDisconnectionComplete(uint8_t status, c7222::ConnectionHandle handle,
 *                                  uint8_t reason) const override {
 *         std::cout << "Device disconnected, reason: " << (int)reason << std::endl;
 *         // After disconnection, we can restart advertising.
 *         c7222::Gap::getInstance().startAdvertising();
 *         std::cout << "Advertising restarted." << std::endl;
 *     }
 * };
 *
 * // --- Main application logic ---
 * void setup_ble_advertising() {
 *     // Get the singleton instance of the Gap class.
 *     auto& gap = c7222::Gap::getInstance();
 *
 *     // --- 2. Register the event handler ---
 *     // Note: The handler instance must exist for the lifetime of the application.
 *     static MyGapEventHandler my_handler;
 *     gap.addEventHandler(my_handler);
 *
 *     // --- 3. Configure advertising parameters ---
 *     // We can use the default parameters for standard connectable advertising.
 *     c7222::Gap::AdvertisementParameters params;
 *     // Set a custom interval: 200ms to 250ms
 *     // Interval is in units of 0.625 ms, so 320 * 0.625 = 200ms, 400 * 0.625 = 250ms
 *     params.min_interval = 320;
 *     params.max_interval = 400;
 *     gap.setAdvertisingParameters(params);
 *
 *     // --- 4. Build and set advertising data ---
 *     c7222::AdvertisementDataBuilder ad_builder;
 *     ad_builder.add(c7222::AdvertisementData::Type::Flags,
 *                    c7222::AdvertisementData::Flags::LeGeneralDiscoverable |
 *                    c7222::AdvertisementData::Flags::BrEdrNotSupported);
 *     ad_builder.add(c7222::AdvertisementData::Type::CompleteLocalName, "PicoW-BLE");
 *     gap.setAdvertisingData(ad_builder);
 *
 *     // --- 5. Start advertising ---
 *     gap.startAdvertising();
 * }
 *
 * // In your main function, you would call setup_ble_advertising()
 * // and then process BLE events in your main loop.
 * // For example, in a loop:
 * // ble_stack_process();
 *
 * ```
 */
class Gap : public NonCopyableNonMovable {
  public:
	/**
	 * @brief Event IDs used by Gap::EventHandler.
	 */
	enum class EventId : uint8_t {
		/**
		 * Security level update for an active connection.
		 * BTstack event: GAP_EVENT_SECURITY_LEVEL.
		 */
		SecurityLevel,
		/**
		 * Dedicated bonding procedure finished with status and peer address.
		 * BTstack event: GAP_EVENT_DEDICATED_BONDING_COMPLETED.
		 */
		DedicatedBondingCompleted,
		/**
		 * Legacy advertising report while scanning.
		 * BTstack event: GAP_EVENT_ADVERTISING_REPORT.
		 */
		AdvertisingReport,
		/**
		 * Extended advertising report while scanning.
		 * BTstack event: GAP_EVENT_EXTENDED_ADVERTISING_REPORT.
		 */
		ExtendedAdvertisingReport,
		/**
		 * Classic inquiry result payload received.
		 * BTstack event: GAP_EVENT_INQUIRY_RESULT.
		 */
		InquiryResult,
		/**
		 * Inquiry procedure completed.
		 * BTstack event: GAP_EVENT_INQUIRY_COMPLETE.
		 */
		InquiryComplete,
		/**
		 * RSSI measurement result for a connection.
		 * BTstack event: GAP_EVENT_RSSI_MEASUREMENT.
		 */
		RssiMeasurement,
		/**
		 * Local out-of-band data generated or available.
		 * BTstack event: GAP_EVENT_LOCAL_OOB_DATA.
		 */
		LocalOobData,
		/**
		 * Pairing procedure has started.
		 * BTstack event: GAP_EVENT_PAIRING_STARTED.
		 */
		PairingStarted,
		/**
		 * Pairing procedure has completed with status.
		 * BTstack event: GAP_EVENT_PAIRING_COMPLETE.
		 */
		PairingComplete,
		/**
		 * A connection was terminated with a reason code.
		 * BTstack event: HCI_EVENT_DISCONNECTION_COMPLETE.
		 */
		DisconnectionComplete,
		/**
		 * Generic HCI command completion event (check opcode for details).
		 * BTstack event: HCI_EVENT_COMMAND_COMPLETE.
		 */
		CommandComplete,
		/**
		 * Scan request received by an advertiser.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED.
		 */
		LeScanRequestReceived,
		/**
		 * LE scan procedure timed out.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_TIMEOUT.
		 */
		LeScanTimeout,
		/**
		 * Periodic advertising sync established.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHMENT.
		 */
		LePeriodicAdvertisingSyncEstablished,
		/**
		 * Periodic advertising report received.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT.
		 */
		LePeriodicAdvertisingReport,
		/**
		 * Periodic advertising sync lost.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST.
		 */
		LePeriodicAdvertisingSyncLost,
		/**
		 * LE connection complete (legacy subevent).
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_COMPLETE.
		 */
		LeConnectionComplete,
		/**
		 * LE enhanced connection complete (addresses included).
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE.
		 */
		LeEnhancedConnectionComplete,
		/**
		 * Remote device requests connection parameter updates.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST.
		 */
		LeRemoteConnectionParameterRequest,
		/**
		 * Connection parameters update completed.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE.
		 */
		LeConnectionUpdateComplete,
		/**
		 * LE PHY update procedure completed.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE.
		 */
		LePhyUpdateComplete,
		/**
		 * LE data length update reported for a connection.
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE.
		 */
		LeDataLengthChange,
		/**
		 * Extended advertising set terminated (timeout or connection).
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED.
		 */
		LeAdvertisingSetTerminated,
		/**
		 * L2CAP connection parameter update request event.
		 * BTstack event: L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST.
		 */
		L2capConnectionParameterUpdateRequest,
		/**
		 * Privacy enabled and ready; raised after successful configuration.
		 * BTstack: no dedicated event.
		 */
		PrivacyEnabled
	};
	/**
	 * @brief Extended advertising event properties (bitfield).
	 */
	enum class AdvertisingEventType : uint16_t {
		/**
		 * Advertising is connectable.
		 */
		Connectable = 0x0001,
		/**
		 * Advertising is scannable.
		 */
		Scannable = 0x0002,
		/**
		 * Advertising is directed to a specific peer.
		 */
		Directed = 0x0004,
		/**
		 * High duty cycle directed advertising.
		 */
		HighDutyCycle = 0x0008,
		/**
		 * Legacy advertising PDUs.
		 */
		Legacy = 0x0010,
		/**
		 * Advertiser address is omitted from the report.
		 */
		Anonymous = 0x0020,
		/**
		 * Include the TX power in the report.
		 */
		IncludeTxPower = 0x0040
	};

	/**
	 * @brief LE PHY values reported in extended advertising reports.
	 */
	enum class Phy : uint8_t {
		/**
		 * No PHY specified / not available.
		 */
		None = 0x00,
		/**
		 * LE 1M PHY.
		 */
		Le1M = 0x01,
		/**
		 * LE 2M PHY.
		 */
		Le2M = 0x02,
		/**
		 * LE Coded PHY.
		 */
		LeCoded = 0x03
	};

	/**
	 * @brief Legacy advertising types for LE advertising parameters.
	 */
	enum class AdvertisingType : uint8_t {
		/**
		 * Connectable undirected advertising (ADV_IND).
		 */
		AdvInd = 0x00,
		/**
		 * Connectable directed advertising (ADV_DIRECT_IND).
		 */
		AdvDirectInd = 0x01,
		/**
		 * Scannable undirected advertising (ADV_SCAN_IND).
		 */
		AdvScanInd = 0x02,
		/**
		 * Non-connectable undirected advertising (ADV_NONCONN_IND).
		 */
		AdvNonConnInd = 0x03
	};

	/**
	 * @brief Direct address type for directed advertising.
	 */
	enum class DirectAddressType : uint8_t {
		/**
		 * Public device address.
		 */
		Public = 0x00,
		/**
		 * Random device address.
		 */
		Random = 0x01
	};

	/**
	 * @brief Advertising channel map bitfield.
	 */
	enum class AdvertisingChannelMap : uint8_t {
		/**
		 * Channel 37 enabled.
		 */
		Channel37 = 0x01,
		/**
		 * Channel 38 enabled.
		 */
		Channel38 = 0x02,
		/**
		 * Channel 39 enabled.
		 */
		Channel39 = 0x04,
		/**
		 * All advertising channels enabled (37, 38, 39).
		 */
		All = 0x07
	};

	/**
	 * @brief Advertising filter policy.
	 */
	enum class AdvertisingFilterPolicy : uint8_t {
		/**
		 * Allow any scan and any connect request.
		 */
		ScanAnyConnectAny = 0x00,
		/**
		 * Allow scan from whitelist, connect from any.
		 */
		ScanWhitelistConnectAny = 0x01,
		/**
		 * Allow scan from any, connect from whitelist.
		 */
		ScanAnyConnectWhitelist = 0x02,
		/**
		 * Allow scan and connect from whitelist only.
		 */
		ScanWhitelistConnectWhitelist = 0x03
	};

	struct AdvertisingReport {
		/**
		 * Advertising event properties.
		 */
		AdvertisingEventType advertising_event_type;
		/**
		 * Advertiser address.
		 */
		BleAddress address;
		/**
		 * RSSI in dBm (signed).
		 */
		int8_t rssi;
		/**
		 * Advertising data payload (valid during callback only).
		 */
		const uint8_t* data;
		/**
		 * Number of bytes in the advertising data payload.
		 */
		uint8_t data_length;
		friend std::ostream& operator<<(std::ostream& os, const AdvertisingReport& ar);
	};

	struct ExtendedAdvertisingReport {
		/**
		 * Advertising event properties.
		 */
		AdvertisingEventType advertising_event_type;
		/**
		 * Advertiser address.
		 */
		BleAddress address;
		/**
		 * Primary advertising PHY.
		 */
		Phy primary_phy;
		/**
		 * Secondary advertising PHY (or None if not present).
		 */
		Phy secondary_phy;
		/**
		 * Advertising set identifier (SID).
		 */
		uint8_t advertising_sid;
		/**
		 * Advertiser TX power in dBm (signed).
		 */
		int8_t tx_power;
		/**
		 * RSSI in dBm (signed).
		 */
		int8_t rssi;
		/**
		 * Periodic advertising interval (unit: 1.25 ms).
		 */
		uint16_t periodic_advertising_interval;
		/**
		 * Direct address for directed advertising (if present).
		 */
		BleAddress direct_address;
		/**
		 * Advertising data payload (valid during callback only).
		 */
		const uint8_t* data;
		/**
		 * Number of bytes in the advertising data payload.
		 */
		uint8_t data_length;
		friend std::ostream& operator<<(std::ostream& os, const ExtendedAdvertisingReport& ear);
	};

	struct InquiryResult {
		/**
		 * Peer device address.
		 */
		BleAddress address;
		/**
		 * Page scan repetition mode.
		 */
		uint8_t page_scan_repetition_mode;
		/**
		 * Class of device (CoD) value.
		 */
		uint32_t class_of_device;
		/**
		 * Clock offset (little-endian in HCI event).
		 */
		uint16_t clock_offset;
		/**
		 * True if RSSI value is available.
		 */
		bool rssi_available;
		/**
		 * RSSI in dBm (valid when rssi_available is true).
		 */
		int8_t rssi;
		/**
		 * True if device ID fields are available.
		 */
		bool device_id_available;
		/**
		 * Device ID vendor ID source (Bluetooth SIG or USB).
		 */
		uint16_t device_id_vendor_id_source;
		/**
		 * Device ID vendor ID.
		 */
		uint16_t device_id_vendor_id;
		/**
		 * Device ID product ID.
		 */
		uint16_t device_id_product_id;
		/**
		 * Device ID version.
		 */
		uint16_t device_id_version;
		/**
		 * True if device name is available.
		 */
		bool name_available;
		/**
		 * Device name payload (valid when name_available is true).
		 */
		const uint8_t* name;
		/**
		 * Length of the name payload.
		 */
		uint8_t name_len;

		friend std::ostream& operator<<(std::ostream& os, const InquiryResult& ir);
	};

	struct EventHandler {
		/**
		 * @brief GAP event callback interface.
		 *
		 * Handlers are invoked from the BTstack event dispatch context. Event data
		 * references (pointers/arrays) are only valid during the callback; copy any
		 * data you need to retain beyond the call.
		 */
		/**
		 * Called when the controller reports a scan request to this advertiser.
		 *
		 * @param advertising_handle Advertising set handle.
		 * @param scanner_address Address of the scanning device.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED.
		 */
		virtual void onScanRequestReceived(uint8_t advertising_handle, const BleAddress& scanner_address) const {}

		/**
		 * Called when advertising enable completes.
		 *
		 * @param status HCI status (0x00 for success).
		 *
		 * BTstack event: HCI_EVENT_COMMAND_COMPLETE for HCI_LE_SET_ADVERTISING_ENABLE.
		 */
		virtual void onAdvertisingStart(uint8_t status) const {}

		/**
		 * Called when advertising is disabled or terminated by a connection.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param connection_handle Connection handle if ended due to connection, otherwise 0.
		 *
		 * BTstack events:
		 * - HCI_EVENT_COMMAND_COMPLETE for HCI_LE_SET_ADVERTISING_ENABLE (disable).
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_COMPLETE /
		 *   HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE when advertising ends due to connection.
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED for extended advertising.
		 */
		virtual void onAdvertisingEnd(uint8_t status, ConnectionHandle connection_handle) const {}

		/**
		 * Called when GAP_EVENT_ADVERTISING_REPORT is received.
		 *
		 * @param report Advertising report payload.
		 *
		 * BTstack event: GAP_EVENT_ADVERTISING_REPORT.
		 */
		virtual void onAdvertisingReport(const AdvertisingReport& report) const {}

		/**
		 * Called when GAP_EVENT_EXTENDED_ADVERTISING_REPORT is received.
		 *
		 * @param report Extended advertising report payload.
		 *
		 * BTstack event: GAP_EVENT_EXTENDED_ADVERTISING_REPORT.
		 */
		virtual void onExtendedAdvertisingReport(const ExtendedAdvertisingReport& report) const {}

		/**
		 * Called when a scan timeout is reported.
		 *
		 * @param status HCI status (0x00 for success).
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_TIMEOUT.
		 */
		virtual void onScanTimeout(uint8_t status) const {}

		/**
		 * Called when periodic advertising sync is established.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param sync_handle Sync handle assigned by controller.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHMENT.
		 */
		virtual void onPeriodicAdvertisingSyncEstablished(uint8_t status, ConnectionHandle sync_handle) const {}

		/**
		 * Called when a periodic advertising report is received.
		 *
		 * @param sync_handle Sync handle for the periodic advertiser.
		 * @param tx_power Transmit power in dBm (signed).
		 * @param rssi RSSI in dBm (signed).
		 * @param data_status Data status flag from the controller.
		 * @param data Periodic advertising data (valid during callback only).
		 * @param data_length Number of bytes in the data payload.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT.
		 */
		virtual void onPeriodicAdvertisingReport(ConnectionHandle sync_handle,
												 int8_t tx_power,
												 int8_t rssi,
												 uint8_t data_status,
												 const uint8_t* data,
												 uint8_t data_length) const {}

		/**
		 * Called when periodic advertising sync is lost.
		 *
		 * @param sync_handle Sync handle that was lost.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST.
		 */
		virtual void onPeriodicAdvertisingSyncLoss(ConnectionHandle sync_handle) const {}

		/**
		 * Called when a LE connection completes.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param con_handle Connection handle.
		 * @param address Peer device address.
		 * @param conn_interval Connection interval (unit: 1.25 ms).
		 * @param conn_latency Peripheral latency (number of events to skip).
		 * @param supervision_timeout Supervision timeout (unit: 10 ms).
		 *
		 * BTstack events:
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_COMPLETE
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE
		 */
		virtual void onConnectionComplete(uint8_t status,
										  ConnectionHandle con_handle,
										  const BleAddress& address,
										  uint16_t conn_interval,
										  uint16_t conn_latency,
										  uint16_t supervision_timeout) const {}

		/**
		 * Called when the peer requests connection parameter updates.
		 *
		 * @param con_handle Connection handle.
		 * @param min_interval Minimum requested interval (unit: 1.25 ms).
		 * @param max_interval Maximum requested interval (unit: 1.25 ms).
		 * @param latency Requested slave latency.
		 * @param supervision_timeout Requested supervision timeout (unit: 10 ms).
		 *
		 * BTstack events:
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST
		 * - L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST (if using L2CAP events).
		 */
		virtual void onUpdateConnectionParametersRequest(ConnectionHandle con_handle,
														 uint16_t min_interval,
														 uint16_t max_interval,
														 uint16_t latency,
														 uint16_t supervision_timeout) const {}

		/**
		 * Called when connection parameters have been updated.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param con_handle Connection handle.
		 * @param conn_interval Connection interval (unit: 1.25 ms).
		 * @param conn_latency Peripheral latency.
		 * @param supervision_timeout Supervision timeout (unit: 10 ms).
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE.
		 */
		virtual void onConnectionParametersUpdateComplete(uint8_t status,
														  ConnectionHandle con_handle,
														  uint16_t conn_interval,
														  uint16_t conn_latency,
														  uint16_t supervision_timeout) const {}

		/**
		 * Called when a connection is disconnected.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param con_handle Connection handle.
		 * @param reason HCI disconnect reason.
		 *
		 * BTstack event: HCI_EVENT_DISCONNECTION_COMPLETE.
		 */
		virtual void onDisconnectionComplete(uint8_t status, ConnectionHandle con_handle, uint8_t reason) const {}

		/**
		 * Called when LE PHYs have been read.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param con_handle Connection handle.
		 * @param tx_phy Transmit PHY.
		 * @param rx_phy Receive PHY.
		 *
		 * BTstack event: HCI_EVENT_COMMAND_COMPLETE for HCI_LE_READ_PHY.
		 */
		virtual void onReadPhy(uint8_t status, ConnectionHandle con_handle, Phy tx_phy, Phy rx_phy) const {}

		/**
		 * Called when the PHY update process completes.
		 *
		 * @param status HCI status (0x00 for success).
		 * @param con_handle Connection handle.
		 * @param tx_phy Updated transmit PHY.
		 * @param rx_phy Updated receive PHY.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE.
		 */
		virtual void onPhyUpdateComplete(uint8_t status, ConnectionHandle con_handle, Phy tx_phy, Phy rx_phy) const {}

		/**
		 * Called when data length changes for a connection.
		 *
		 * @param con_handle Connection handle.
		 * @param tx_size Max TX payload size (octets).
		 * @param rx_size Max RX payload size (octets).
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE.
		 */
		virtual void onDataLengthChange(ConnectionHandle con_handle, uint16_t tx_size, uint16_t rx_size) const {}

		/**
		 * Called when privacy becomes enabled and ready.
		 *
		 * BTstack: no dedicated event; call after successful privacy configuration.
		 */
		virtual void onPrivacyEnabled() const {}

		/**
		 * Called when GAP_EVENT_SECURITY_LEVEL is received.
		 *
		 * @param con_handle Connection handle for the link.
		 * @param security_level Negotiated security level.
		 *
		 * BTstack event: GAP_EVENT_SECURITY_LEVEL.
		 */
		virtual void onSecurityLevel(ConnectionHandle con_handle, uint8_t security_level) const {}

		/**
		 * Called when GAP_EVENT_DEDICATED_BONDING_COMPLETED is received.
		 *
		 * @param status Status of the bonding procedure.
		 * @param address Peer device address.
		 *
		 * BTstack event: GAP_EVENT_DEDICATED_BONDING_COMPLETED.
		 */
		virtual void onDedicatedBondingCompleted(uint8_t status, const BleAddress& address) const {}

		/**
		 * Called when GAP_EVENT_INQUIRY_RESULT is received.
		 *
		 * @param result Inquiry result payload.
		 *
		 * BTstack event: GAP_EVENT_INQUIRY_RESULT.
		 */
		virtual void onInquiryResult(const InquiryResult& result) const {}

		/**
		 * Called when GAP_EVENT_INQUIRY_COMPLETE is received.
		 *
		 * @param status Status of the inquiry procedure.
		 *
		 * BTstack event: GAP_EVENT_INQUIRY_COMPLETE.
		 */
		virtual void onInquiryComplete(uint8_t status) const {}

		/**
		 * Called when GAP_EVENT_RSSI_MEASUREMENT is received.
		 *
		 * @param con_handle Connection handle.
		 * @param rssi RSSI in dBm (signed).
		 *
		 * BTstack event: GAP_EVENT_RSSI_MEASUREMENT.
		 */
		virtual void onRssiMeasurement(ConnectionHandle con_handle, int8_t rssi) const {}

		/**
		 * Called when GAP_EVENT_LOCAL_OOB_DATA is received.
		 *
		 * @param oob_data_present True if OOB data is available.
		 * @param c_192 192-bit confirm value (16 bytes).
		 * @param r_192 192-bit random value (16 bytes).
		 * @param c_256 256-bit confirm value (16 bytes).
		 * @param r_256 256-bit random value (16 bytes).
		 *
		 * BTstack event: GAP_EVENT_LOCAL_OOB_DATA.
		 */
		virtual void onLocalOobData(bool oob_data_present,
									const uint8_t* c_192,
									const uint8_t* r_192,
									const uint8_t* c_256,
									const uint8_t* r_256) const {}

		/**
		 * Called when GAP_EVENT_PAIRING_STARTED is received.
		 *
		 * @param con_handle Connection handle.
		 * @param address Peer address.
		 * @param ssp True if SSP is used.
		 * @param initiator True if local device initiated pairing.
		 *
		 * BTstack event: GAP_EVENT_PAIRING_STARTED.
		 */
		virtual void
		onPairingStarted(ConnectionHandle con_handle, const BleAddress& address, bool ssp, bool initiator) const {}

		/**
		 * Called when GAP_EVENT_PAIRING_COMPLETE is received.
		 *
		 * @param con_handle Connection handle.
		 * @param address Peer address.
		 * @param status Status of the pairing procedure.
		 *
		 * BTstack event: GAP_EVENT_PAIRING_COMPLETE.
		 */
		virtual void onPairingComplete(ConnectionHandle con_handle, const BleAddress& address, uint8_t status) const {}

	  protected:
		/**
		 * Prevent polymorphic deletion and avoid unnecessary virtual destructor
		 * as the Gap class will never delete the instance it contains.
		 */
		~EventHandler() = default;
	};

	struct AdvertisementParameters {
		/**
		 * @brief Legacy advertising type (ADV_IND, ADV_SCAN_IND, etc.).
		 *
		 * Maps to the HCI LE Advertising Type field.
		 */
		AdvertisingType advertising_type;
		/**
		 * @brief Direct address type used for directed advertising.
		 *
		 * Ignored unless using ADV_DIRECT_IND.
		 */
		DirectAddressType direct_address_type;
		/**
		 * @brief Direct target address for directed advertising.
		 *
		 * Use all-zero when not directed. Address type is taken from
		 * direct_address_type.
		 */
		BleAddress direct_address;
		/**
		 * @brief Minimum advertising interval (unit: 0.625 ms).
		 */
		uint16_t min_interval;
		/**
		 * @brief Maximum advertising interval (unit: 0.625 ms).
		 */
		uint16_t max_interval;
		/**
		 * @brief Advertising channel map bitfield.
		 *
		 * Combination of Channel37/38/39.
		 */
		uint8_t channel_map;
		/**
		 * @brief Advertising filter policy.
		 *
		 * Controls scan/connect whitelist behavior.
		 */
		AdvertisingFilterPolicy filter_policy;

		/**
		 * @brief Construct typical connectable undirected advertising parameters.
		 *
		 * Uses ADV_IND with a 100-150 ms interval on all channels and no whitelist.
		 */
		AdvertisementParameters()
			: advertising_type(AdvertisingType::AdvInd), direct_address_type(DirectAddressType::Public),
			  direct_address{}, min_interval(0x00A0), max_interval(0x00F0),
			  channel_map(static_cast<uint8_t>(AdvertisingChannelMap::All)),
			  filter_policy(AdvertisingFilterPolicy::ScanAnyConnectAny) {}
	};

	struct PreferredConnectionParameters {
		/**
		 * @brief Minimum connection interval (unit: 1.25 ms).
		 */
		uint16_t min_interval;
		/**
		 * @brief Maximum connection interval (unit: 1.25 ms).
		 */
		uint16_t max_interval;
		/**
		 * @brief Slave latency (number of connection events to skip).
		 */
		uint16_t slave_latency;
		/**
		 * @brief Supervision timeout (unit: 10 ms).
		 *
		 * Must satisfy: supervision_timeout > (1 + slave_latency) * max_interval * 2.
		 */
		uint16_t supervision_timeout;
	};

	struct ConnectionParameters {
		/**
		 * @brief Current connection interval (unit: 1.25 ms).
		 */
		uint16_t interval;
		/**
		 * @brief Current slave latency (number of connection events to skip).
		 */
		uint16_t latency;
		/**
		 * @brief Current supervision timeout (unit: 10 ms).
		 */
		uint16_t supervision_timeout;
	};

	/**
	 * @brief Set a fixed random address for advertising.
	 */
	void setRandomAddress(const BleAddress& address);

	/**
	 * @brief Configure legacy advertising parameters.
	 */
	void setAdvertisingParameters(const AdvertisementParameters& params);

	/**
	 * @brief Set legacy advertising data payload.
	 */
	void setAdvertisingData(uint8_t length, const uint8_t* data);

	/**
	 * @brief Set legacy advertising data payload from a vector.
	 *
	 * @param data Advertising data payload.
	 */
	void setAdvertisingData(const std::vector<uint8_t>& data) {
		setAdvertisingData(static_cast<uint8_t>(data.size()), data.data());
	}

	/**
	 * @brief Set legacy advertising data payload from an AdvertisementDataBuilder.
	 *
	 * @param data_builder AdvertisementDataBuilder instance containing the payload.
	 * @note The data from the builder is copied.
	 */
	void setAdvertisingData(const AdvertisementDataBuilder& data_builder) {
		auto data = data_builder.data();
		setAdvertisingData(static_cast<uint8_t>(data.size()), data.data());
	}

	/**
	 * @brief Set scan response data payload (ADV_SCAN_IND).
	 */
	void setScanResponseData(uint8_t length, const uint8_t* data);

	/**
	 * @brief Enable or disable advertising.
	 */
	void enableAdvertising(bool enabled);

	/**
	 * @brief Convenience helpers for starting/stopping advertising.
	 */
	void startAdvertising();
	void stopAdvertising();

	/**
	 * @brief Request a connection parameter update (peripheral role).
	 */
	BleError requestConnectionParameterUpdate(ConnectionHandle con_handle, const PreferredConnectionParameters& params);

	/**
	 * @brief Update connection parameters (central role).
	 */
	BleError updateConnectionParameters(ConnectionHandle con_handle, const PreferredConnectionParameters& params);

	/**
	 * @brief Read the RSSI for a connection.
	 */
	BleError readRssi(ConnectionHandle con_handle);

	/**
	 * @brief Disconnect a connection by handle.
	 */
	BleError disconnect(ConnectionHandle con_handle);

	/**
	 * @brief Read the local device address.
	 */
	void getLocalAddress(BleAddress& address);

	/**
	 * @brief Register an event handler.
	 *
	 * The handler is stored as a pointer; it must outlive the Gap instance.
	 */
	void addEventHandler(const EventHandler& handler);

	/**
	 * @brief Get cached connection parameters for a handle.
	 *
	 * @param con_handle Connection handle.
	 * @param out Parameters output.
	 * @return true if parameters are known for the handle.
	 */
	bool getConnectionParameters(ConnectionHandle con_handle, ConnectionParameters& out) const {
		const auto it = connection_parameters_.find(con_handle);
		if(it == connection_parameters_.end()) {
			return false;
		}
		out = it->second;
		return true;
	}

	/**
	 * @brief Check if advertising is currently enabled.
	 */
	bool isAdvertisingEnabled() const {
		return advertisement_enabled_;
	}

	/**
	 * @brief Check if advertising parameters have been set.
	 */
	bool isAdvertisingParametersSet() const {
		return advertising_params_set_;
	}

	/**
	 * @brief Check if a connection is active.
	 */
	bool isConnected() const {
		return connected_;
	}

	/**
	 * @brief Get the random address if set.
	 *
	 * @param address Output address.
	 * @return true if a random address has been set.
	 */
	bool getRandomAddress(BleAddress& address) const {
		if(!random_address_set_) {
			return false;
		}
		address = random_address_;
		return true;
	}

	/**
	 * @brief Check if a random address has been set.
	 */
	bool isRandomAddressSet() const {
		return random_address_set_;
	}

	/**
	 * @brief Get the current advertising parameters.
	 *
	 * @param params Output parameters.
	 * @return true if advertising parameters have been set.
	 */
	bool getAdvertisingParameters(AdvertisementParameters& params) const {
		if(!advertising_params_set_) {
			return false;
		}
		params = advertising_params_;
		return true;
	}

	/**
	 * @brief Get the advertising data payload.
	 *
	 * @return Pointer to advertising data, or nullptr if not set.
	 */
	const std::vector<uint8_t>& getAdvertisingData() const {
		return advertising_data_;
	}

	/**
	 * @brief Check if advertising data has been set.
	 */
	bool isAdvertisingDataSet() const {
		return advertising_data_set_;
	}

	/**
	 * @brief Get the scan response data payload.
	 *
	 * @return Pointer to scan response data, or nullptr if not set.
	 */
	const std::vector<uint8_t>& getScanResponseData() const {
		return scan_response_data_;
	}

	/**
	 * @brief Check if scan response data has been set.
	 */
	bool isScanResponseDataSet() const {
		return scan_response_data_set_;
	}

	/**
	 * @brief Access the cached connection parameter map.
	 *
	 * The map is populated from connection-related events.
	 */
	const std::map<ConnectionHandle, ConnectionParameters>& connectionParameters() const {
		return connection_parameters_;
	}

	/**
	 * @brief Get the singleton instance.
	 */
	static Gap& getInstance() {
		if(instance_ == nullptr) {
			instance_ = new Gap();
		}
		return *instance_;
	}

	/**
	 * @brief Dispatch a raw HCI packet into the GAP event pipeline.
	 *
	 * @param packet_type HCI packet type (expected HCI_EVENT_PACKET).
	 * @param packet_data Pointer to packet bytes.
	 * @param packet_data_size Packet length in bytes.
	 * @return BLE error status.
	 */
	virtual BleError dispatch_ble_hci_packet(uint8_t packet_type,
											 const uint8_t* packet_data,
											 uint16_t packet_data_size);

  protected:
	/**
	 * @brief Dispatch a mapped GAP event to registered handlers.
	 *
	 * @param event_id Mapped GAP event ID.
	 * @param event_data Pointer to event data bytes.
	 * @param event_data_size Event length in bytes.
	 * @return BLE error status.
	 */
	virtual BleError dispatch_event(EventId event_id, const uint8_t* event_data, uint16_t event_data_size);

  private:
	Gap() = default;
	Gap(const Gap&) = delete;
	Gap& operator=(const Gap&) = delete;

	~Gap() = default;

	/**
	 * @brief Singleton instance pointer.
	 *
	 * Lazily allocated by getInstance() and never freed.
	 */
	inline static Gap* instance_ = nullptr;

	/**
	 * @brief True when advertising is enabled by the application.
	 */
	bool advertisement_enabled_ = false;
	/**
	 * @brief True once setAdvertisingParameters() has been called.
	 */
	bool advertising_params_set_ = false;
	/**
	 * @brief True when at least one connection is active.
	 */
	bool connected_ = false;

	/**
	 * @brief Cached random address used for advertising.
	 */
	BleAddress random_address_{};
	/**
	 * @brief True once setRandomAddress() has been called.
	 */
	bool random_address_set_ = false;

	/**
	 * @brief Cached legacy advertising parameters.
	 */
	AdvertisementParameters advertising_params_{};
	/**
	 * @brief Cached legacy advertising payload bytes.
	 */
	std::vector<uint8_t> advertising_data_{};
	/**
	 * @brief True once setAdvertisingData() has been called.
	 */
	bool advertising_data_set_ = false;

	/**
	 * @brief Cached scan response payload bytes.
	 */
	std::vector<uint8_t> scan_response_data_{};
	/**
	 * @brief True once setScanResponseData() has been called.
	 */
	bool scan_response_data_set_ = false;

	/**
	 * @brief Cached connection parameters per handle.
	 */
	std::map<ConnectionHandle, ConnectionParameters> connection_parameters_;
	/**
	 * @brief Registered event handlers.
	 */
	std::list<const EventHandler*> event_handlers_;
};

} // namespace c7222

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingEventType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::Phy phy);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::DirectAddressType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingChannelMap map);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingFilterPolicy policy);

constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	return static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs);
}
constexpr uint8_t operator|(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator|=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) | rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

constexpr uint8_t operator&(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator^(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator^=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator^(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

#endif // ELEC_C7222_BLE_GAP_HPP
