/**
 * @file app_gap.hpp
 * @brief Minimal GAP event handler used by BLE examples.
 *
 * This header declares a small helper class that implements
 * `c7222::Gap::EventHandler` and prints GAP events to the console. It is used
 * by BLE examples to track the state of advertising, scanning, and connections
 * without adding application-specific behavior. The intent is educational:
 * demonstrate how to register a handler and what information is available in
 * each callback.
 *
 * The class is deliberately minimal. It keeps a `Gap` pointer so it can
 * restart advertising after a disconnect, which is common in examples that
 * should remain discoverable. It also keeps an optional `AttributeServer`
 * pointer so that, on connection, it can propagate the active connection
 * handle to the GATT layer and enable attribute access.
 */
#pragma once

#include <cstdint>
#include <cstdio>

#include "attribute_server.hpp"
#include "gap.hpp"

/**
 * @brief Minimal GAP event handler for example applications.
 *
 * Tracks and prints GAP events. Used in BLE examples as a lightweight way to
 * observe advertising, scanning, and connection activity.
 *
 * The stored `Gap` instance is used to restart advertising on disconnect.
 * The optional `AttributeServer` instance is used to set the active connection
 * handle when a connection completes, which is required for GATT operations.
 */
class GapEventHandler : public c7222::Gap::EventHandler {
   public:
	/**
	 * @brief Construct a handler with optional GAP and AttributeServer pointers.
	 *
	 * @param gap GAP singleton used for actions like restarting advertising.
	 * @param attribute_server Optional attribute server updated on connect.
	 */
	explicit GapEventHandler(c7222::Gap* gap = c7222::Gap::GetInstance(),
							 c7222::AttributeServer* attribute_server = nullptr);

	/**
	 * @brief Attach an attribute server after construction.
	 *
	 * When a connection is established, the handler will call
	 * `AttributeServer::SetConnectionHandle()` with the active handle.
	 */
	void SetAttributeServer(c7222::AttributeServer* attribute_server);

	/// Logs scan requests received by the advertiser.
	void OnScanRequestReceived(uint8_t advertising_handle,
							   const c7222::BleAddress& scanner_address) const override;
	/// Logs the start of advertising.
	void OnAdvertisingStart(uint8_t status) const override;
	/// Logs the end of advertising.
	void OnAdvertisingEnd(uint8_t status,
						  c7222::ConnectionHandle connection_handle) const override;
	/// Logs legacy advertising reports.
	void OnAdvertisingReport(const c7222::Gap::AdvertisingReport& report) const override;
	/// Logs extended advertising reports.
	void OnExtendedAdvertisingReport(
		const c7222::Gap::ExtendedAdvertisingReport& report) const override;
	/// Logs a scan timeout event.
	void OnScanTimeout(uint8_t status) const override;
	/// Logs periodic advertising sync establishment.
	void OnPeriodicAdvertisingSyncEstablished(uint8_t status,
											  c7222::ConnectionHandle sync_handle) const override;
	/// Logs a periodic advertising report.
	void OnPeriodicAdvertisingReport(c7222::ConnectionHandle sync_handle,
									 int8_t tx_power,
									 int8_t rssi,
									 uint8_t data_status,
									 const uint8_t* data,
									 uint8_t data_length) const override;
	/// Logs loss of periodic advertising sync.
	void OnPeriodicAdvertisingSyncLoss(c7222::ConnectionHandle sync_handle) const override;
	/// Logs new connections and updates the attribute server handle.
	void OnConnectionComplete(uint8_t status,
							  c7222::ConnectionHandle con_handle,
							  const c7222::BleAddress& address,
							  uint16_t conn_interval,
							  uint16_t conn_latency,
							  uint16_t supervision_timeout) const override;
	/// Logs a request to update connection parameters.
	void OnUpdateConnectionParametersRequest(c7222::ConnectionHandle con_handle,
											 uint16_t min_interval,
											 uint16_t max_interval,
											 uint16_t latency,
											 uint16_t supervision_timeout) const override;
	/// Logs completion of connection parameter updates.
	void OnConnectionParametersUpdateComplete(uint8_t status,
											  c7222::ConnectionHandle con_handle,
											  uint16_t conn_interval,
											  uint16_t conn_latency,
											  uint16_t supervision_timeout) const override;
	/// Logs disconnections and restarts advertising.
	void OnDisconnectionComplete(uint8_t status,
								 c7222::ConnectionHandle con_handle,
								 uint8_t reason) const override;
	/// Logs PHY read results.
	void OnReadPhy(uint8_t status,
				   c7222::ConnectionHandle con_handle,
				   c7222::Gap::Phy tx_phy,
				   c7222::Gap::Phy rx_phy) const override;
	/// Logs PHY update completion.
	void OnPhyUpdateComplete(uint8_t status,
							 c7222::ConnectionHandle con_handle,
							 c7222::Gap::Phy tx_phy,
							 c7222::Gap::Phy rx_phy) const override;
	/// Logs data length changes.
	void OnDataLengthChange(c7222::ConnectionHandle con_handle,
							uint16_t tx_size,
							uint16_t rx_size) const override;
	/// Logs that privacy mode has been enabled.
	void OnPrivacyEnabled() const override;
	/// Logs the negotiated security level.
	void OnSecurityLevel(c7222::ConnectionHandle con_handle,
						 uint8_t security_level) const override;
	/// Logs completion of dedicated bonding.
	void OnDedicatedBondingCompleted(uint8_t status,
									 const c7222::BleAddress& address) const override;
	/// Logs inquiry results.
	void OnInquiryResult(const c7222::Gap::InquiryResult& result) const override;
	/// Logs completion of inquiry.
	void OnInquiryComplete(uint8_t status) const override;
	/// Logs RSSI measurements.
	void OnRssiMeasurement(c7222::ConnectionHandle con_handle, int8_t rssi) const override;
	/// Logs presence of local OOB data.
	void OnLocalOobData(bool oob_data_present,
						const uint8_t* c_192,
						const uint8_t* r_192,
						const uint8_t* c_256,
						const uint8_t* r_256) const override;
	/// Logs the start of pairing.
	void OnPairingStarted(c7222::ConnectionHandle con_handle,
						  const c7222::BleAddress& address,
						  bool ssp,
						  bool initiator) const override;
	/// Logs completion of pairing.
	void OnPairingComplete(c7222::ConnectionHandle con_handle,
						   const c7222::BleAddress& address,
						   uint8_t status) const override;

   private:
	c7222::Gap* gap_ = nullptr;
	c7222::AttributeServer* attribute_server_ = nullptr;
};
