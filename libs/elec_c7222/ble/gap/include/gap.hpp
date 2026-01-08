/**
 * @file gap.hpp
 * @brief GAP event definitions and handlers.
 */
#ifndef ELEC_C7222_BLE_GAP_HPP
#define ELEC_C7222_BLE_GAP_HPP

#include <cstdint>
#include <iosfwd>
#include <list>

#include "ble_address.hpp"
#include "non_copyable.hpp"

namespace c7222 {


using ConnectionHandle = uint16_t;

class Gap : public NonCopyableNonMovable {
	public:
	  /**
	   * @brief Event IDs used by Gap::EventHandler.
	   */
	  enum class EventId : uint8_t {
		  SecurityLevel,
		  DedicatedBondingCompleted,
		  AdvertisingReport,
		  ExtendedAdvertisingReport,
		  InquiryResult,
		  InquiryComplete,
		  RssiMeasurement,
		  LocalOobData,
		  PairingStarted,
		  PairingComplete,
		  DisconnectionComplete,
		  CommandComplete,
		  LeScanRequestReceived,
		  LeScanTimeout,
		  LePeriodicAdvertisingSyncEstablished,
		  LePeriodicAdvertisingReport,
		  LePeriodicAdvertisingSyncLost,
		  LeConnectionComplete,
		  LeEnhancedConnectionComplete,
		  LeRemoteConnectionParameterRequest,
		  LeConnectionUpdateComplete,
		  LePhyUpdateComplete,
		  LeDataLengthChange,
		  LeAdvertisingSetTerminated,
		  L2capConnectionParameterUpdateRequest,
		  PrivacyEnabled
	  };
	  /**
	   * @brief Extended advertising event properties (bitfield).
	   */
	  enum class AdvertisingEventType : uint16_t {
		  Connectable = 0x0001,
		  Scannable = 0x0002,
		  Directed = 0x0004,
		  HighDutyCycle = 0x0008,
		  Legacy = 0x0010,
		  Anonymous = 0x0020,
		  IncludeTxPower = 0x0040
	  };

	  /**
	   * @brief LE PHY values reported in extended advertising reports.
	   */
	  enum class Phy : uint8_t { None = 0x00, Le1M = 0x01, Le2M = 0x02, LeCoded = 0x03 };

	  /**
	   * @brief Legacy advertising types for LE advertising parameters.
	   */
	  enum class AdvertisingType : uint8_t {
		  AdvInd = 0x00,
		  AdvDirectInd = 0x01,
		  AdvScanInd = 0x02,
		  AdvNonConnInd = 0x03
	  };

	  /**
	   * @brief Direct address type for directed advertising.
	   */
	  enum class DirectAddressType : uint8_t { Public = 0x00, Random = 0x01 };

	  /**
	   * @brief Advertising channel map bitfield.
	   */
	  enum class AdvertisingChannelMap : uint8_t { Channel37 = 0x01, Channel38 = 0x02, Channel39 = 0x04, All = 0x07 };

	  /**
	   * @brief Advertising filter policy.
	   */
	  enum class AdvertisingFilterPolicy : uint8_t {
		  ScanAnyConnectAny = 0x00,
		  ScanWhitelistConnectAny = 0x01,
		  ScanAnyConnectWhitelist = 0x02,
		  ScanWhitelistConnectWhitelist = 0x03
	  };

	  struct AdvertisingReport {
		  AdvertisingEventType advertising_event_type;
		  BleAddress address;
		  int8_t rssi;
		  const uint8_t* data;
		  uint8_t data_length;
		  friend std::ostream& operator<<(std::ostream& os, const AdvertisingReport& ar);
	  };

	struct ExtendedAdvertisingReport{
		AdvertisingEventType advertising_event_type;
		BleAddress address;
		Phy primary_phy;
		Phy secondary_phy;
		uint8_t advertising_sid;
		int8_t tx_power;
		int8_t rssi;
		uint16_t periodic_advertising_interval;
		BleAddress direct_address;
		const uint8_t* data;
		uint8_t data_length;
		friend std::ostream& operator<<(std::ostream& os, const ExtendedAdvertisingReport& ear);
	};

	struct InquiryResult{
		BleAddress address;
		uint8_t page_scan_repetition_mode;
		uint32_t class_of_device;
		uint16_t clock_offset;
		bool rssi_available;
		int8_t rssi;
		bool device_id_available;
		uint16_t device_id_vendor_id_source;
		uint16_t device_id_vendor_id;
		uint16_t device_id_product_id;
		uint16_t device_id_version;
		bool name_available;
		const uint8_t* name;
		uint8_t name_len;

		friend std::ostream& operator<<(std::ostream& os, const InquiryResult& ir);
	};

	struct EventHandler{
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
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED.
		 */
		virtual void onScanRequestReceived(uint8_t advertising_handle,
										   const BleAddress& scanner_address) {}

		/**
		 * Called when advertising enable completes.
		 *
		 * BTstack event: HCI_EVENT_COMMAND_COMPLETE for HCI_LE_SET_ADVERTISING_ENABLE.
		 */
		virtual void onAdvertisingStart(uint8_t status) {}

		/**
		 * Called when advertising is disabled or terminated by a connection.
		 *
		 * BTstack events:
		 * - HCI_EVENT_COMMAND_COMPLETE for HCI_LE_SET_ADVERTISING_ENABLE (disable).
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_COMPLETE /
		 *   HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE when advertising ends due to connection.
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED for extended advertising.
		 */
		virtual void onAdvertisingEnd(uint8_t status, ConnectionHandle connection_handle) {}

		/**
		 * Called when GAP_EVENT_ADVERTISING_REPORT is received.
		 *
		 * BTstack event: GAP_EVENT_ADVERTISING_REPORT.
		 */
		virtual void onAdvertisingReport(const AdvertisingReport& report) {}

		/**
		 * Called when GAP_EVENT_EXTENDED_ADVERTISING_REPORT is received.
		 *
		 * BTstack event: GAP_EVENT_EXTENDED_ADVERTISING_REPORT.
		 */
		virtual void onExtendedAdvertisingReport(const ExtendedAdvertisingReport& report) {}

		/**
		 * Called when a scan timeout is reported.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_SCAN_TIMEOUT.
		 */
		virtual void onScanTimeout(uint8_t status) {}

		/**
		 * Called when periodic advertising sync is established.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHMENT.
		 */
		virtual void onPeriodicAdvertisingSyncEstablished(uint8_t status, ConnectionHandle sync_handle) {}

		/**
		 * Called when a periodic advertising report is received.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT.
		 */
		virtual void onPeriodicAdvertisingReport(ConnectionHandle sync_handle,
												 int8_t tx_power,
												 int8_t rssi,
												 uint8_t data_status,
												 const uint8_t* data,
												 uint8_t data_length) {}

		/**
		 * Called when periodic advertising sync is lost.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST.
		 */
		virtual void onPeriodicAdvertisingSyncLoss(ConnectionHandle sync_handle) {}

		/**
		 * Called when a LE connection completes.
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
										  uint16_t supervision_timeout) {}

		/**
		 * Called when the peer requests connection parameter updates.
		 *
		 * BTstack events:
		 * - HCI_EVENT_LE_META + HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST
		 * - L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST (if using L2CAP events).
		 */
		virtual void onUpdateConnectionParametersRequest(ConnectionHandle con_handle,
														 uint16_t min_interval,
														 uint16_t max_interval,
														 uint16_t latency,
														 uint16_t supervision_timeout) {}

		/**
		 * Called when connection parameters have been updated.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE.
		 */
		virtual void onConnectionParametersUpdateComplete(uint8_t status,
														  ConnectionHandle con_handle,
														  uint16_t conn_interval,
														  uint16_t conn_latency,
														  uint16_t supervision_timeout) {}

		/**
		 * Called when a connection is disconnected.
		 *
		 * BTstack event: HCI_EVENT_DISCONNECTION_COMPLETE.
		 */
		virtual void onDisconnectionComplete(uint8_t status,
											 ConnectionHandle con_handle,
											 uint8_t reason) {}

		/**
		 * Called when LE PHYs have been read.
		 *
		 * BTstack event: HCI_EVENT_COMMAND_COMPLETE for HCI_LE_READ_PHY.
		 */
		virtual void onReadPhy(uint8_t status,
							   ConnectionHandle con_handle,
							   Phy tx_phy,
							   Phy rx_phy) {}

		/**
		 * Called when the PHY update process completes.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE.
		 */
		virtual void onPhyUpdateComplete(uint8_t status,
										 ConnectionHandle con_handle,
										 Phy tx_phy,
										 Phy rx_phy) {}

		/**
		 * Called when data length changes for a connection.
		 *
		 * BTstack event: HCI_EVENT_LE_META + HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE.
		 */
		virtual void onDataLengthChange(ConnectionHandle con_handle,
										uint16_t tx_size,
										uint16_t rx_size) {}

		/**
		 * Called when privacy becomes enabled and ready.
		 *
		 * BTstack: no dedicated event; call after successful privacy configuration.
		 */
		virtual void onPrivacyEnabled() {}

		/**
		 * Called when GAP_EVENT_SECURITY_LEVEL is received.
		 *
		 * @param con_handle Connection handle for the link.
		 * @param security_level Negotiated security level.
		 *
		 * BTstack event: GAP_EVENT_SECURITY_LEVEL.
		 */
		virtual void onSecurityLevel(ConnectionHandle con_handle, uint8_t security_level) {}

		/**
		 * Called when GAP_EVENT_DEDICATED_BONDING_COMPLETED is received.
		 *
		 * @param status Status of the bonding procedure.
		 * @param address Peer device address.
		 *
		 * BTstack event: GAP_EVENT_DEDICATED_BONDING_COMPLETED.
		 */
		virtual void onDedicatedBondingCompleted(uint8_t status, const BleAddress& address) {}

		/**
		 * Called when GAP_EVENT_INQUIRY_RESULT is received.
		 *
		 * @param result Inquiry result payload.
		 *
		 * BTstack event: GAP_EVENT_INQUIRY_RESULT.
		 */
		virtual void onInquiryResult(const InquiryResult& result) {}

		/**
		 * Called when GAP_EVENT_INQUIRY_COMPLETE is received.
		 *
		 * @param status Status of the inquiry procedure.
		 *
		 * BTstack event: GAP_EVENT_INQUIRY_COMPLETE.
		 */
		virtual void onInquiryComplete(uint8_t status) {}

		/**
		 * Called when GAP_EVENT_RSSI_MEASUREMENT is received.
		 *
		 * @param con_handle Connection handle.
		 * @param rssi RSSI in dBm (signed).
		 *
		 * BTstack event: GAP_EVENT_RSSI_MEASUREMENT.
		 */
		virtual void onRssiMeasurement(ConnectionHandle con_handle, int8_t rssi) {}

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
									const uint8_t* r_256) {}

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
		virtual void onPairingStarted(ConnectionHandle con_handle,
									  const BleAddress& address,
									  bool ssp,
									  bool initiator) {}

		/**
		 * Called when GAP_EVENT_PAIRING_COMPLETE is received.
		 *
		 * @param con_handle Connection handle.
		 * @param address Peer address.
		 * @param status Status of the pairing procedure.
		 *
		 * BTstack event: GAP_EVENT_PAIRING_COMPLETE.
		 */
		virtual void onPairingComplete(ConnectionHandle con_handle,
									   const BleAddress& address,
									   uint8_t status) {}

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
			: advertising_type(AdvertisingType::AdvInd),
			  direct_address_type(DirectAddressType::Public),
			  direct_address{},
			  min_interval(0x00A0),
			  max_interval(0x00F0),
			  channel_map(static_cast<uint8_t>(AdvertisingChannelMap::All)),
			  filter_policy(AdvertisingFilterPolicy::ScanAnyConnectAny) {}
	};

	struct PreferredConnectionParameters{
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
	int requestConnectionParameterUpdate(ConnectionHandle con_handle,
												const PreferredConnectionParameters& params);

	/**
	 * @brief Update connection parameters (central role).
	 */
	int updateConnectionParameters(ConnectionHandle con_handle,
										  const PreferredConnectionParameters& params);

	/**
	 * @brief Read the RSSI for a connection.
	 */
	int readRssi(ConnectionHandle con_handle);

	/**
	 * @brief Disconnect a connection by handle.
	 */
	uint8_t disconnect(ConnectionHandle con_handle);

	/**
	 * @brief Read the local device address.
	 */
	void getLocalAddress(BleAddress& address);

	void addEventHandler(const EventHandler& handler);

	static Gap& getInstance(){
		if(instance_ == nullptr){
			instance_ = new Gap();
		}
		return *instance_;
	}
	
	private:
	Gap() = default;
	Gap(const Gap&) = delete;
	Gap& operator=(const Gap&) = delete;

	~Gap() = default;

	inline static Gap* instance_ = nullptr;

	bool advertisement_enabled_ = false;
	bool advertising_params_set_ = false;
	bool connected_ = false;

	std::list<ConnectionHandle> connections_;
	std::list<EventHandler> event_handlers_;
};

} // namespace c7222

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingEventType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::Phy phy);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::DirectAddressType type);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingChannelMap map);
std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingFilterPolicy policy);

constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs,
							c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator|(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator|=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs);
constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs,
							c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator&(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator&=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs);
constexpr uint8_t operator^(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator^=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs);
constexpr uint8_t operator^(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs);

#endif // ELEC_C7222_BLE_GAP_HPP
