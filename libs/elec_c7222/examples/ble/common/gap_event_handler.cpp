#include "gap_event_handler.hpp"

GapEventHandler::GapEventHandler(c7222::Gap* gap,
								 c7222::AttributeServer* attribute_server)
	: gap_(gap), attribute_server_(attribute_server) {}

void GapEventHandler::SetAttributeServer(c7222::AttributeServer* attribute_server) {
	// Late-bind the attribute server so GATT can use the active connection handle.
	attribute_server_ = attribute_server;
}

void GapEventHandler::OnScanRequestReceived(uint8_t advertising_handle,
											const c7222::BleAddress& scanner_address) const {
	(void)scanner_address;
	std::printf("GAP event: ScanRequestReceived (handle=%u)\n", advertising_handle);
}

void GapEventHandler::OnAdvertisingStart(uint8_t status) const {
	(void)status;
}

void GapEventHandler::OnAdvertisingEnd(uint8_t status,
									   c7222::ConnectionHandle connection_handle) const {
	(void)status;
	(void)connection_handle;
}

void GapEventHandler::OnAdvertisingReport(
	const c7222::Gap::AdvertisingReport& report) const {
	std::printf("GAP event: AdvertisingReport (len=%u, rssi=%d)\n",
				report.data_length,
				static_cast<int>(report.rssi));
}

void GapEventHandler::OnExtendedAdvertisingReport(
	const c7222::Gap::ExtendedAdvertisingReport& report) const {
	std::printf("GAP event: ExtendedAdvertisingReport (len=%u, rssi=%d)\n",
				report.data_length,
				static_cast<int>(report.rssi));
}

void GapEventHandler::OnScanTimeout(uint8_t status) const {
	std::printf("GAP event: ScanTimeout (status=0x%02X)\n", status);
}

void GapEventHandler::OnPeriodicAdvertisingSyncEstablished(
	uint8_t status,
	c7222::ConnectionHandle sync_handle) const {
	std::printf("GAP event: PeriodicSyncEstablished (status=0x%02X, handle=%u)\n",
				status,
				sync_handle);
}

void GapEventHandler::OnPeriodicAdvertisingReport(c7222::ConnectionHandle sync_handle,
												  int8_t tx_power,
												  int8_t rssi,
												  uint8_t data_status,
												  const uint8_t* data,
												  uint8_t data_length) const {
	(void)data;
	std::printf(
		"GAP event: PeriodicReport (handle=%u, tx=%d, rssi=%d, status=0x%02X, len=%u)\n",
		sync_handle,
		static_cast<int>(tx_power),
		static_cast<int>(rssi),
		data_status,
		data_length);
}

void GapEventHandler::OnPeriodicAdvertisingSyncLoss(
	c7222::ConnectionHandle sync_handle) const {
	std::printf("GAP event: PeriodicSyncLoss (handle=%u)\n", sync_handle);
}

void GapEventHandler::OnConnectionComplete(uint8_t status,
										   c7222::ConnectionHandle con_handle,
										   const c7222::BleAddress& address,
										   uint16_t conn_interval,
										   uint16_t conn_latency,
										   uint16_t supervision_timeout) const {
	(void)address;
	// Log connection parameters for debugging and teaching.
	std::printf(
		"GAP event: ConnectionComplete (status=0x%02X, handle=%u, interval=%u, latency=%u, "
		"timeout=%u)\n",
		status,
		con_handle,
		conn_interval,
		conn_latency,
		supervision_timeout);

	// Propagate the connection handle to the AttributeServer so GATT operations can proceed.
	if(attribute_server_ != nullptr) {
		attribute_server_->SetConnectionHandle(con_handle);
	}
}

void GapEventHandler::OnUpdateConnectionParametersRequest(
	c7222::ConnectionHandle con_handle,
	uint16_t min_interval,
	uint16_t max_interval,
	uint16_t latency,
	uint16_t supervision_timeout) const {
	std::printf(
		"GAP event: ConnParamsRequest (handle=%u, min=%u, max=%u, latency=%u, timeout=%u)\n",
		con_handle,
		min_interval,
		max_interval,
		latency,
		supervision_timeout);
}

void GapEventHandler::OnConnectionParametersUpdateComplete(
	uint8_t status,
	c7222::ConnectionHandle con_handle,
	uint16_t conn_interval,
	uint16_t conn_latency,
	uint16_t supervision_timeout) const {
	std::printf(
		"GAP event: ConnParamsUpdateComplete (status=0x%02X, handle=%u, interval=%u, "
		"latency=%u, timeout=%u)\n",
		status,
		con_handle,
		conn_interval,
		conn_latency,
		supervision_timeout);
}

void GapEventHandler::OnDisconnectionComplete(uint8_t status,
											  c7222::ConnectionHandle con_handle,
											  uint8_t reason) const {
	std::printf("GAP event: DisconnectionComplete (status=0x%02X, handle=%u, reason=0x%02X)\n",
				status,
				con_handle,
				reason);
	// Keep the example alive by restarting advertising on disconnect.
	if(gap_ != nullptr) {
		gap_->StartAdvertising();
	}
}

void GapEventHandler::OnReadPhy(uint8_t status,
								c7222::ConnectionHandle con_handle,
								c7222::Gap::Phy tx_phy,
								c7222::Gap::Phy rx_phy) const {
	std::printf("GAP event: ReadPhy (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
				status,
				con_handle,
				static_cast<unsigned>(tx_phy),
				static_cast<unsigned>(rx_phy));
}

void GapEventHandler::OnPhyUpdateComplete(uint8_t status,
										  c7222::ConnectionHandle con_handle,
										  c7222::Gap::Phy tx_phy,
										  c7222::Gap::Phy rx_phy) const {
	std::printf("GAP event: PhyUpdateComplete (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
				status,
				con_handle,
				static_cast<unsigned>(tx_phy),
				static_cast<unsigned>(rx_phy));
}

void GapEventHandler::OnDataLengthChange(c7222::ConnectionHandle con_handle,
										 uint16_t tx_size,
										 uint16_t rx_size) const {
	std::printf("GAP event: DataLengthChange (handle=%u, tx=%u, rx=%u)\n",
				con_handle,
				tx_size,
				rx_size);
}

void GapEventHandler::OnPrivacyEnabled() const {
	std::printf("GAP event: PrivacyEnabled\n");
}

void GapEventHandler::OnSecurityLevel(c7222::ConnectionHandle con_handle,
									  uint8_t security_level) const {
	// Decode the numeric security level for readable logs.
	const char* level_name = "Unknown";
	switch(security_level) {
		case 0:
			level_name = "No security";
			break;
		case 1:
			level_name = "Encrypted (unauthenticated)";
			break;
		case 2:
			level_name = "Authenticated";
			break;
		case 3:
			level_name = "Authenticated SC";
			break;
		default:
			break;
	}
	std::printf("GAP event: SecurityLevel (handle=%u, level=%u, %s)\n",
				con_handle,
				security_level,
				level_name);
	// Warn students when they are below authenticated security.
	if(security_level < 2) {
		std::printf(
			"GAP warning: security level < 2 (authenticated) - authorization will fail\n");
	}
}

void GapEventHandler::OnDedicatedBondingCompleted(uint8_t status,
												  const c7222::BleAddress& address) const {
	(void)address;
	std::printf("GAP event: DedicatedBondingCompleted (status=0x%02X)\n", status);
}

void GapEventHandler::OnInquiryResult(const c7222::Gap::InquiryResult& result) const {
	std::printf("GAP event: InquiryResult (rssi_available=%u, name_len=%u)\n",
				result.rssi_available ? 1u : 0u,
				result.name_len);
}

void GapEventHandler::OnInquiryComplete(uint8_t status) const {
	std::printf("GAP event: InquiryComplete (status=0x%02X)\n", status);
}

void GapEventHandler::OnRssiMeasurement(c7222::ConnectionHandle con_handle, int8_t rssi) const {
	std::printf("GAP event: RssiMeasurement (handle=%u, rssi=%d)\n",
				con_handle,
				static_cast<int>(rssi));
}

void GapEventHandler::OnLocalOobData(bool oob_data_present,
									 const uint8_t* c_192,
									 const uint8_t* r_192,
									 const uint8_t* c_256,
									 const uint8_t* r_256) const {
	(void)c_192;
	(void)r_192;
	(void)c_256;
	(void)r_256;
	std::printf("GAP event: LocalOobData (present=%u)\n", oob_data_present ? 1u : 0u);
}

void GapEventHandler::OnPairingStarted(c7222::ConnectionHandle con_handle,
									   const c7222::BleAddress& address,
									   bool ssp,
									   bool initiator) const {
	(void)address;
	std::printf("GAP event: PairingStarted (handle=%u, ssp=%u, initiator=%u)\n",
				con_handle,
				ssp ? 1u : 0u,
				initiator ? 1u : 0u);
}

void GapEventHandler::OnPairingComplete(c7222::ConnectionHandle con_handle,
										const c7222::BleAddress& address,
										uint8_t status) const {
	(void)address;
	std::printf("GAP event: PairingComplete (handle=%u, status=0x%02X)\n",
				con_handle,
				status);
}
