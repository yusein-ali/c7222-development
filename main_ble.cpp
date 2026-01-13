#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ble.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include <advertisement_data.hpp>


class GapEventHandler : public c7222::Gap::EventHandler {
  public:
	void OnScanRequestReceived(uint8_t advertising_handle,
							   const c7222::BleAddress& scanner_address) const override {
		(void) scanner_address;
		printf("GAP event: ScanRequestReceived (handle=%u)\n", advertising_handle);
	}
	void OnAdvertisingStart(uint8_t status) const override {
		// printf("GAP event: AdvertisingStart (status=0x%02X)\n", status);
	}
	void OnAdvertisingEnd(uint8_t status, c7222::ConnectionHandle connection_handle) const override {
		// printf("GAP event: AdvertisingEnd (status=0x%02X, handle=%u)\n", status, connection_handle);
	}
	void OnAdvertisingReport(const c7222::Gap::AdvertisingReport& report) const override {
		printf("GAP event: AdvertisingReport (len=%u, rssi=%d)\n",
			   report.data_length,
			   static_cast<int>(report.rssi));
	}
	void OnExtendedAdvertisingReport(const c7222::Gap::ExtendedAdvertisingReport& report) const override {
		printf("GAP event: ExtendedAdvertisingReport (len=%u, rssi=%d)\n",
			   report.data_length,
			   static_cast<int>(report.rssi));
	}
	void OnScanTimeout(uint8_t status) const override {
		printf("GAP event: ScanTimeout (status=0x%02X)\n", status);
	}
	void OnPeriodicAdvertisingSyncEstablished(uint8_t status,
											  c7222::ConnectionHandle sync_handle) const override {
		printf("GAP event: PeriodicSyncEstablished (status=0x%02X, handle=%u)\n", status, sync_handle);
	}
	void OnPeriodicAdvertisingReport(c7222::ConnectionHandle sync_handle,
									 int8_t tx_power,
									 int8_t rssi,
									 uint8_t data_status,
									 const uint8_t* data,
									 uint8_t data_length) const override {
		(void) data;
		printf("GAP event: PeriodicReport (handle=%u, tx=%d, rssi=%d, status=0x%02X, len=%u)\n",
			   sync_handle,
			   static_cast<int>(tx_power),
			   static_cast<int>(rssi),
			   data_status,
			   data_length);
	}
	void OnPeriodicAdvertisingSyncLoss(c7222::ConnectionHandle sync_handle) const override {
		printf("GAP event: PeriodicSyncLoss (handle=%u)\n", sync_handle);
	}
	void OnConnectionComplete(uint8_t status,
							  c7222::ConnectionHandle con_handle,
							  const c7222::BleAddress& address,
							  uint16_t conn_interval,
							  uint16_t conn_latency,
							  uint16_t supervision_timeout) const override {
		(void) address;
		printf("GAP event: ConnectionComplete (status=0x%02X, handle=%u, interval=%u, latency=%u, timeout=%u)\n",
			   status,
			   con_handle,
			   conn_interval,
			   conn_latency,
			   supervision_timeout);
	}
	void OnUpdateConnectionParametersRequest(c7222::ConnectionHandle con_handle,
											 uint16_t min_interval,
											 uint16_t max_interval,
											 uint16_t latency,
											 uint16_t supervision_timeout) const override {
		printf("GAP event: ConnParamsRequest (handle=%u, min=%u, max=%u, latency=%u, timeout=%u)\n",
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
		printf("GAP event: ConnParamsUpdateComplete (status=0x%02X, handle=%u, interval=%u, latency=%u, timeout=%u)\n",
			   status,
			   con_handle,
			   conn_interval,
			   conn_latency,
			   supervision_timeout);
	}
	void OnDisconnectionComplete(uint8_t status,
								 c7222::ConnectionHandle con_handle,
								 uint8_t reason) const override {
		printf("GAP event: DisconnectionComplete (status=0x%02X, handle=%u, reason=0x%02X)\n",
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
		printf("GAP event: ReadPhy (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
			   status,
			   con_handle,
			   static_cast<unsigned>(tx_phy),
			   static_cast<unsigned>(rx_phy));
	}
	void OnPhyUpdateComplete(uint8_t status,
							 c7222::ConnectionHandle con_handle,
							 c7222::Gap::Phy tx_phy,
							 c7222::Gap::Phy rx_phy) const override {
		printf("GAP event: PhyUpdateComplete (status=0x%02X, handle=%u, tx=%u, rx=%u)\n",
			   status,
			   con_handle,
			   static_cast<unsigned>(tx_phy),
			   static_cast<unsigned>(rx_phy));
	}
	void OnDataLengthChange(c7222::ConnectionHandle con_handle,
							uint16_t tx_size,
							uint16_t rx_size) const override {
		printf("GAP event: DataLengthChange (handle=%u, tx=%u, rx=%u)\n", con_handle, tx_size, rx_size);
	}
	void OnPrivacyEnabled() const override {
		printf("GAP event: PrivacyEnabled\n");
	}
	void OnSecurityLevel(c7222::ConnectionHandle con_handle, uint8_t security_level) const override {
		printf("GAP event: SecurityLevel (handle=%u, level=%u)\n", con_handle, security_level);
	}
	void OnDedicatedBondingCompleted(uint8_t status, const c7222::BleAddress& address) const override {
		(void) address;
		printf("GAP event: DedicatedBondingCompleted (status=0x%02X)\n", status);
	}
	void OnInquiryResult(const c7222::Gap::InquiryResult& result) const override {
		printf("GAP event: InquiryResult (rssi_available=%u, name_len=%u)\n",
			   result.rssi_available ? 1u : 0u,
			   result.name_len);
	}
	void OnInquiryComplete(uint8_t status) const override {
		printf("GAP event: InquiryComplete (status=0x%02X)\n", status);
	}
	void OnRssiMeasurement(c7222::ConnectionHandle con_handle, int8_t rssi) const override {
		printf("GAP event: RssiMeasurement (handle=%u, rssi=%d)\n", con_handle, static_cast<int>(rssi));
	}
	void OnLocalOobData(bool oob_data_present,
						const uint8_t* c_192,
						const uint8_t* r_192,
						const uint8_t* c_256,
						const uint8_t* r_256) const override {
		(void) c_192;
		(void) r_192;
		(void) c_256;
		(void) r_256;
		printf("GAP event: LocalOobData (present=%u)\n", oob_data_present ? 1u : 0u);
	}
	void OnPairingStarted(c7222::ConnectionHandle con_handle,
						  const c7222::BleAddress& address,
						  bool ssp,
						  bool initiator) const override {
		(void) address;
		printf("GAP event: PairingStarted (handle=%u, ssp=%u, initiator=%u)\n",
			   con_handle,
			   ssp ? 1u : 0u,
			   initiator ? 1u : 0u);
	}
	void OnPairingComplete(c7222::ConnectionHandle con_handle,
						   const c7222::BleAddress& address,
						   uint8_t status) const override {
		(void) address;
		printf("GAP event: PairingComplete (handle=%u, status=0x%02X)\n", con_handle, status);
	}

	private:
		c7222::Gap* gap_ = c7222::Gap::GetInstance();
};

static GapEventHandler gap_event_handler;


// -------------------------------------------------------------------------
// Packet Handler: Receive events from the BLE Stack
// -------------------------------------------------------------------------
static void on_turn_on(){
	printf("Bluetooth Turned On\n");
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adb = ble->GetAdvertisementDataBuilder();

	gap->AddEventHandler(gap_event_handler);
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	// Generate the packet using the advertisement data class
	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
									c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("Pico2_BLE++");

	uint32_t value = 0x12345678;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											 (uint8_t*) &value,
											 sizeof(value)));

	// ------------------------------------------------
	// generate the advertisement parameters
	// ------------------------------------------------
	// note that the default parameters are fine for most use cases
	{
		c7222::Gap::AdvertisementParameters adv_params;
		// Set a custom interval: 200ms to 250ms
		// Interval is in units of 0.625 ms, so 320 * 0.625 = 200ms, 400 * 0.625 = 250ms
		adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
		adv_params.min_interval = 320;
		adv_params.max_interval = 400;
		// 2. Set Advertisement Parameters (Interval: 100ms approx)
		gap->SetAdvertisingParameters(adv_params);
	}
	// Start Advertising
	gap->StartAdvertising();
	printf("Advertising started as 'Pico2_BLE'...\n");
}



// -------------------------------------------------------------------------
// BLE Application Task
// -------------------------------------------------------------------------
void ble_app_task(void* params) {
	(void) params;
	static uint32_t seconds = 0;
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adb = ble->GetAdvertisementDataBuilder();

	// Initialize CYW43 Architecture (Starts the SDK background worker)
	if(cyw43_arch_init()) {
		printf("CYW43 init failed\n");
		vTaskDelete(NULL);
	}

	printf("CYW43 init complete. Setting up BTstack... here!\n");

	ble->SetOnBleStackOnCallback(on_turn_on);
	ble->TurnOn();

	// Enter infinite loop to keep task alive (or perform other app logic)
	while(true) {
		seconds = xTaskGetTickCount() / 1000;
		// Blink LED to show system is alive
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		vTaskDelay(pdMS_TO_TICKS(500));
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(500));
		// printf("BLE App Task is running...%lu\n", seconds);
		if(gap->IsAdvertisingEnabled()) {
			printf("Updating the manuf specific data to %lu\n", seconds);
			auto ad = c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific, (uint8_t*)&seconds, sizeof(seconds));
			adb.Pop();
			adb.Push(ad);
			ble->SetAdvertisingData();
		} else {
			// printf("Not advertising.\n");
		}
	}
}

// -------------------------------------------------------------------------
// Main
// -------------------------------------------------------------------------
int main() {
	stdio_init_all();
	sleep_ms(2000); // Give serial monitor time to connect
	printf("Starting FreeRTOS BLE Example...\n");

	// Create the BLE application task
	// Stack size 1024 words (4096 bytes) is usually sufficient for basic advertising
	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the Scheduler
	vTaskStartScheduler();

	// Should never reach here
	while(1) {}

	return 0;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	// Breakpoint here!
	// If you hit this, you know 100% that 'pcTaskName' (likely "Startup") ran out of stack.
	while(1) {}
}
