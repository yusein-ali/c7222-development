#pragma once

#include <cstdint>
#include <cstdio>

#include "attribute_server.hpp"
#include "ble.hpp"
#include "gap.hpp"

class GapEventHandler : public c7222::Gap::EventHandler {
   public:
	explicit GapEventHandler(c7222::Gap* gap = c7222::Gap::GetInstance(),
							 c7222::AttributeServer* attribute_server = nullptr)
		: gap_(gap), attribute_server_(attribute_server) {}

	void SetAttributeServer(c7222::AttributeServer* attribute_server) {
		attribute_server_ = attribute_server;
	}

	void OnScanRequestReceived(uint8_t advertising_handle,
							   const c7222::BleAddress& scanner_address) const override {
		(void)scanner_address;
		std::printf("GAP event: ScanRequestReceived (handle=%u)\n", advertising_handle);
	}
	void OnAdvertisingStart(uint8_t status) const override {
		(void)status;
	}
	void OnAdvertisingEnd(uint8_t status,
						  c7222::ConnectionHandle connection_handle) const override {
		(void)status;
		(void)connection_handle;
	}
	void OnAdvertisingReport(const c7222::Gap::AdvertisingReport& report) const override {
		std::printf("GAP event: AdvertisingReport (len=%u, rssi=%d)\n",
					report.data_length,
					static_cast<int>(report.rssi));
	}
	void OnExtendedAdvertisingReport(
		const c7222::Gap::ExtendedAdvertisingReport& report) const override {
		std::printf("GAP event: ExtendedAdvertisingReport (len=%u, rssi=%d)\n",
					report.data_length,
					static_cast<int>(report.rssi));
	}
	void OnScanTimeout(uint8_t status) const override {
		std::printf("GAP event: ScanTimeout (status=0x%02X)\n", status);
	}
	void OnPeriodicAdvertisingSyncEstablished(uint8_t status,
											  c7222::ConnectionHandle sync_handle) const override {
		std::printf("GAP event: PeriodicSyncEstablished (status=0x%02X, handle=%u)\n",
					status,
					sync_handle);
	}
	void OnPeriodicAdvertisingReport(c7222::ConnectionHandle sync_handle,
									 int8_t tx_power,
									 int8_t rssi,
									 uint8_t data_status,
									 const uint8_t* data,
									 uint8_t data_length) const override {
		(void)data;
		std::printf(
			"GAP event: PeriodicReport (handle=%u, tx=%d, rssi=%d, status=0x%02X, len=%u)\n",
			sync_handle,
			static_cast<int>(tx_power),
			static_cast<int>(rssi),
			data_status,
			data_length);
	}
	void OnPeriodicAdvertisingSyncLoss(c7222::ConnectionHandle sync_handle) const override {
		std::printf("GAP event: PeriodicSyncLoss (handle=%u)\n", sync_handle);
	}
	void OnConnectionComplete(uint8_t status,
							  c7222::ConnectionHandle con_handle,
							  const c7222::BleAddress& address,
							  uint16_t conn_interval,
							  uint16_t conn_latency,
							  uint16_t supervision_timeout) const override {
		(void)address;
		std::printf(
			"GAP event: ConnectionComplete (status=0x%02X, handle=%u, interval=%u, latency=%u, "
			"timeout=%u)\n",
			status,
			con_handle,
			conn_interval,
			conn_latency,
			supervision_timeout);

		if(attribute_server_ != nullptr) {
			attribute_server_->SetConnectionHandle(con_handle);
		}
	}
	void OnUpdateConnectionParametersRequest(c7222::ConnectionHandle con_handle,
											 uint16_t min_interval,
											 uint16_t max_interval,
											 uint16_t latency,
											 uint16_t supervision_timeout) const override {
		std::printf(
			"GAP event: ConnParamsRequest (handle=%u, min=%u, max=%u, latency=%u, timeout=%u)\n",
			con_handle,
			min_interval,
			max_interval,
			latency,
			supervision_timeout);
	}
	void OnConnectionParametersUpdateComplete(uint8_t status,
											  c7222::ConnectionHandle con_handle,
											  uint16_t conn_interval,
											  uint16_t conn_latency,
											  uint16_t supervision_timeout) const override {
		std::printf(
			"GAP event: ConnParamsUpdateComplete (status=0x%02X, handle=%u, interval=%u, "
			"latency=%u, timeout=%u)\n",
			status,
			con_handle,
			conn_interval,
			conn_latency,
			supervision_timeout);
	}
	void OnDisconnectionComplete(uint8_t status,
								 c7222::ConnectionHandle con_handle,
								 uint8_t reason) const override {
		std::printf("GAP event: DisconnectionComplete (status=0x%02X, handle=%u, reason=0x%02X)\n",
					status,
					con_handle,
					reason);
		if(gap_ != nullptr) {
			gap_->StartAdvertising();
		}
	}
	void OnReadPhy(uint8_t status,
				   c7222::ConnectionHandle con_handle,
				   c7222::Gap::Phy tx_phy,
				   c7222::Gap::Phy rx_phy) const override {
		std::printf("GAP event: ReadPhy (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
					status,
					con_handle,
					static_cast<unsigned>(tx_phy),
					static_cast<unsigned>(rx_phy));
	}
	void OnPhyUpdateComplete(uint8_t status,
							 c7222::ConnectionHandle con_handle,
							 c7222::Gap::Phy tx_phy,
							 c7222::Gap::Phy rx_phy) const override {
		std::printf("GAP event: PhyUpdateComplete (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
					status,
					con_handle,
					static_cast<unsigned>(tx_phy),
					static_cast<unsigned>(rx_phy));
	}
	void OnDataLengthChange(c7222::ConnectionHandle con_handle,
							uint16_t tx_size,
							uint16_t rx_size) const override {
		std::printf("GAP event: DataLengthChange (handle=%u, tx=%u, rx=%u)\n",
					con_handle,
					tx_size,
					rx_size);
	}
	void OnPrivacyEnabled() const override {
		std::printf("GAP event: PrivacyEnabled\n");
	}
	void OnSecurityLevel(c7222::ConnectionHandle con_handle,
						 uint8_t security_level) const override {
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
		if(security_level < 2) {
			std::printf(
				"GAP warning: security level < 2 (authenticated) - authorization will fail\n");
		}
	}
	void OnDedicatedBondingCompleted(uint8_t status,
									 const c7222::BleAddress& address) const override {
		(void)address;
		std::printf("GAP event: DedicatedBondingCompleted (status=0x%02X)\n", status);
	}
	void OnInquiryResult(const c7222::Gap::InquiryResult& result) const override {
		std::printf("GAP event: InquiryResult (rssi_available=%u, name_len=%u)\n",
					result.rssi_available ? 1u : 0u,
					result.name_len);
	}
	void OnInquiryComplete(uint8_t status) const override {
		std::printf("GAP event: InquiryComplete (status=0x%02X)\n", status);
	}
	void OnRssiMeasurement(c7222::ConnectionHandle con_handle, int8_t rssi) const override {
		std::printf("GAP event: RssiMeasurement (handle=%u, rssi=%d)\n",
					con_handle,
					static_cast<int>(rssi));
	}
	void OnLocalOobData(bool oob_data_present,
						const uint8_t* c_192,
						const uint8_t* r_192,
						const uint8_t* c_256,
						const uint8_t* r_256) const override {
		(void)c_192;
		(void)r_192;
		(void)c_256;
		(void)r_256;
		std::printf("GAP event: LocalOobData (present=%u)\n", oob_data_present ? 1u : 0u);
	}
	void OnPairingStarted(c7222::ConnectionHandle con_handle,
						  const c7222::BleAddress& address,
						  bool ssp,
						  bool initiator) const override {
		(void)address;
		std::printf("GAP event: PairingStarted (handle=%u, ssp=%u, initiator=%u)\n",
					con_handle,
					ssp ? 1u : 0u,
					initiator ? 1u : 0u);
	}
	void OnPairingComplete(c7222::ConnectionHandle con_handle,
						   const c7222::BleAddress& address,
						   uint8_t status) const override {
		(void)address;
		std::printf("GAP event: PairingComplete (handle=%u, status=0x%02X)\n", con_handle, status);
	}

   private:
	c7222::Gap* gap_ = nullptr;
	c7222::AttributeServer* attribute_server_ = nullptr;
};
