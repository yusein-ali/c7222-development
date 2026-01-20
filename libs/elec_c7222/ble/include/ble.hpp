/**
 * @file ble.hpp
 * @brief BLE top-level singleton wrapper.
 */
#ifndef ELEC_C7222_BLE_H_
#define ELEC_C7222_BLE_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "attribute_server.hpp"
#include "ble_address.hpp"
#include "ble_error.hpp"
#include "gap.hpp"
#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class Ble
 * @brief Singleton entry point for BLE configuration and events.
 */
class Ble : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Get the singleton instance.
	 */
	static Ble* GetInstance() {
		if(instance_ == nullptr) {
			instance_ = new Ble();
		}
		return instance_;
	}

	/**
	 * @brief Access the underlying GAP instance.
	 */
	Gap* GetGap() {
		return gap_;
	}

	/**
	 * @brief Access the underlying GAP instance (const).
	 */
	const Gap* GetGap() const {
		return gap_;
	}

	AttributeServer* EnableAttributeServer(const void* context);
	
	AttributeServer* GetAttributeServer() {
		return attribute_server_;
	}

	const AttributeServer* GetAttributeServer() const {
		return attribute_server_;
	}

	/**
	 * @brief Register BLE stack-on callback (HCI_STATE_WORKING).
	 *
	 * @note The callback is invoked when the BLE stack transitions to
	 *       the working state, indicating that BLE operations can proceed.
	 */
	void SetOnBleStackOnCallback(std::function<void()> handler) {
		callback_on_ble_stack_on_ = std::move(handler);
	}

	/**
	 * @brief Register BLE stack-on callback from a function pointer.
	 */
	void SetOnBleStackOnCallback(void (*handler)()) {
		callback_on_ble_stack_on_ = handler;
	}

	/**
	 * @brief Register BLE stack-on callback from an object/member function.
	 */
	template <typename T>
	void SetOnBleStackOnCallback(T* object, void (T::*method)()) {
		callback_on_ble_stack_on_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-on callback from a const object/member function.
	 */
	template <typename T>
	void SetOnBleStackOnCallback(const T* object, void (T::*method)() const) {
		callback_on_ble_stack_on_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-off callback (non-working state).
	 */
	void SetOnBleStackOffCallback(std::function<void()> handler) {
		callback_on_ble_stack_off_ = std::move(handler);
	}

	/**
	 * @brief Register BLE stack-off callback from a function pointer.
	 */
	void SetOnBleStackOffCallback(void (*handler)()) {
		callback_on_ble_stack_off_ = handler;
	}

	/**
	 * @brief Register BLE stack-off callback from an object/member function.
	 */
	template <typename T>
	void SetOnBleStackOffCallback(T* object, void (T::*method)()) {
		callback_on_ble_stack_off_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-off callback from a const object/member function.
	 */
	template <typename T>
	void SetOnBleStackOffCallback(const T* object, void (T::*method)() const) {
		callback_on_ble_stack_off_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Update BLE stack state and notify callbacks on change.
	 */
	BleError TurnOn();
	void TurnOff();

	/**
	 * @brief Check whether BLE stack is marked as working.
	 */
	bool IsTurnedOn() const {
		return turned_on_;
	}

	void SetDeviceName(const std::string& name);

	void SetAdvertisementFlags(uint8_t flags);

	// ---------------------------------------------------------------------
	// GAP convenience wrappers
	// ---------------------------------------------------------------------

	void AddGapEventHandler(const Gap::EventHandler& handler) {
		gap_->AddEventHandler(handler);
	}
	void RemoveGapEventHandler(const Gap::EventHandler& handler) {
		gap_->RemoveEventHandler(handler);
	}
	void ClearGapEventHandlers() {
		gap_->ClearEventHandlers();
	}

	void SetRandomAddress(const BleAddress& address) {
		gap_->SetRandomAddress(address);
	}
	void SetAdvertisingParameters(const Gap::AdvertisementParameters& params) {
		gap_->SetAdvertisingParameters(params);
	}
	void SetAdvertisingData(const uint8_t* data, size_t size) {
		gap_->SetAdvertisingData(data, size);
	}
	void SetAdvertisingData(const std::vector<uint8_t>& data) {
		gap_->SetAdvertisingData(data);
	}
	void SetAdvertisingData(const AdvertisementDataBuilder& data_builder) {
		gap_->SetAdvertisingData(data_builder);
	}
	void SetAdvertisingData() {
		gap_->SetAdvertisingData();
	}
	void SetScanResponseData(uint8_t length, const uint8_t* data) {
		gap_->SetScanResponseData(length, data);
	}
	void EnableAdvertising(bool enabled) {
		gap_->EnableAdvertising(enabled);
	}
	void StartAdvertising() {
		gap_->StartAdvertising();
	}
	void StopAdvertising() {
		gap_->StopAdvertising();
	}
	bool IsAdvertisingEnabled() const {
		return gap_->IsAdvertisingEnabled();
	}
	bool IsConnected() const {
		return gap_->IsConnected();
	}
	BleError RequestConnectionParameterUpdate(ConnectionHandle con_handle,
											  const Gap::PreferredConnectionParameters& params) {
		return gap_->RequestConnectionParameterUpdate(con_handle, params);
	}
	BleError UpdateConnectionParameters(ConnectionHandle con_handle,
										const Gap::PreferredConnectionParameters& params) {
		return gap_->UpdateConnectionParameters(con_handle, params);
	}
	BleError ReadRssi(ConnectionHandle con_handle) {
		return gap_->ReadRssi(con_handle);
	}
	BleError Disconnect(ConnectionHandle con_handle) {
		return gap_->Disconnect(con_handle);
	}
	AdvertisementDataBuilder& GetAdvertisementDataBuilder() {
		return gap_->GetAdvertisementDataBuilder();
	}
	const AdvertisementDataBuilder& GetAdvertisementDataBuilder() const {
		return gap_->GetAdvertisementDataBuilder();
	}

	virtual BleError DispatchBleHciPacket(uint8_t packet_type,
										  uint8_t channel,
										  const uint8_t* packet_data,
										  uint16_t packet_data_size);

   private:
	Ble();
	virtual ~Ble();

	inline static Ble* instance_ = nullptr;

	std::string device_name_;
	uint8_t advertisement_flags_ = 0;
	/**
	 * @brief Called when BLE stack transitions to working (HCI_STATE_WORKING).
	 */
	std::function<void()> callback_on_ble_stack_on_{};

	/**
	 * @brief Called when BLE stack transitions to non-working state.
	 */
	std::function<void()> callback_on_ble_stack_off_{};

	Gap* gap_ = nullptr;
	AttributeServer* attribute_server_ = nullptr;
	bool turned_on_ = false;

	void* context_ = nullptr;
};

}  // namespace c7222

#endif	// ELEC_C7222_BLE_H_
