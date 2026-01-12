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
	static Ble* getInstance() {
		if(instance_ == nullptr) {
			instance_ = new Ble();
		}
		return instance_;
	}

	/**
	 * @brief Access the underlying GAP instance.
	 */
	Gap* getGap() {
		return gap_;
	}

	/**
	 * @brief Access the underlying GAP instance (const).
	 */
	const Gap* getGap() const {
		return gap_;
	}

	/**
	 * @brief Register BLE stack-on callback (HCI_STATE_WORKING).
	 * 
	 * @note The callback is invoked when the BLE stack transitions to
	 *       the working state, indicating that BLE operations can proceed.
	 */
	void setOnBleStackOnCallback(std::function<void()> handler) {
		callback_on_ble_stack_on_ = std::move(handler);
	}

	/**
	 * @brief Register BLE stack-on callback from a function pointer.
	 */
	void setOnBleStackOnCallback(void (*handler)()) {
		callback_on_ble_stack_on_ = handler;
	}

	/**
	 * @brief Register BLE stack-on callback from an object/member function.
	 */
	template <typename T>
	void setOnBleStackOnCallback(T* object, void (T::*method)()) {
		callback_on_ble_stack_on_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-on callback from a const object/member function.
	 */
	template <typename T>
	void setOnBleStackOnCallback(const T* object, void (T::*method)() const) {
		callback_on_ble_stack_on_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-off callback (non-working state).
	 */
	void setOnBleStackOffCallback(std::function<void()> handler) {
		callback_on_ble_stack_off_ = std::move(handler);
	}

	/**
	 * @brief Register BLE stack-off callback from a function pointer.
	 */
	void setOnBleStackOffCallback(void (*handler)()) {
		callback_on_ble_stack_off_ = handler;
	}

	/**
	 * @brief Register BLE stack-off callback from an object/member function.
	 */
	template <typename T>
	void setOnBleStackOffCallback(T* object, void (T::*method)()) {
		callback_on_ble_stack_off_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Register BLE stack-off callback from a const object/member function.
	 */
	template <typename T>
	void setOnBleStackOffCallback(const T* object, void (T::*method)() const) {
		callback_on_ble_stack_off_ = std::function<void()>(std::bind(method, object));
	}

	/**
	 * @brief Update BLE stack state and notify callbacks on change.
	 */
	BleError turnOn();

	void turnOff();

	/**
	 * @brief Check whether BLE stack is marked as working.
	 */
	bool isTurnedOn() const {
		return turned_on_;
	}

	void setDeviceName(const std::string& name) {
		if(gap_ == nullptr) {
			return;
		}
		auto& builder = gap_->getAdvertisementDataBuilder();
		auto ad_list = builder.to_advertisement_data_list();
		for(auto it = ad_list.begin(); it != ad_list.end(); ) {
			if(it->getType() == AdvertisementDataType::kCompleteLocalName) {
				it = ad_list.erase(it);
				break;
			} else {
				++it;
			}
		}
		ad_list.emplace_back(AdvertisementDataType::kCompleteLocalName, name.data(), name.size());
		ad_list.sort([](const AdvertisementData& a, const AdvertisementData& b) {
			return static_cast<uint8_t>(a.getType()) < static_cast<uint8_t>(b.getType());
		});

		builder.set(ad_list);
		assert(builder.validate() && "AdvertisementDataBuilder contains invalid data after setting device name.");	
		device_name_ = name;
	}

	void setAdvertisementFlags(uint8_t flags) {
		if(gap_ == nullptr) {
			return;
		}
		auto& builder = gap_->getAdvertisementDataBuilder();
		auto ad_list = builder.to_advertisement_data_list();
		for(auto it = ad_list.begin(); it != ad_list.end();) {
			if(it->getType() == AdvertisementDataType::kFlags) {
				it = ad_list.erase(it);
				break;
			} else {
				++it;
			}
		}
		ad_list.emplace_back(AdvertisementDataType::kFlags, &flags, sizeof(flags));
		ad_list.sort([](const AdvertisementData& a, const AdvertisementData& b) {
			return static_cast<uint8_t>(a.getType()) < static_cast<uint8_t>(b.getType());
		});

		builder.set(ad_list);
		assert(builder.validate() &&
			   "AdvertisementDataBuilder contains invalid data after setting flags.");
		advertisement_flags_ = flags;
	}

	// ---------------------------------------------------------------------
	// GAP convenience wrappers
	// ---------------------------------------------------------------------

	void addGapEventHandler(const Gap::EventHandler& handler) {
		gap_->addEventHandler(handler);
	}
	void removeGapEventHandler(const Gap::EventHandler& handler) {
		gap_->removeEventHandler(handler);
	}
	void clearGapEventHandlers() {
		gap_->clearEventHandlers();
	}

	void setRandomAddress(const BleAddress& address) {
		gap_->setRandomAddress(address);
	}
	void setAdvertisingParameters(const Gap::AdvertisementParameters& params) {
		gap_->setAdvertisingParameters(params);
	}
	void setAdvertisingData(uint8_t length, const uint8_t* data) {
		gap_->setAdvertisingData(length, data);
	}
	void setAdvertisingData(const std::vector<uint8_t>& data) {
		gap_->setAdvertisingData(data);
	}
	void setAdvertisingData(const AdvertisementDataBuilder& data_builder) {
		gap_->setAdvertisingData(data_builder);
	}
	void setAdvertisingData() {
		gap_->setAdvertisingData();
	}
	void setScanResponseData(uint8_t length, const uint8_t* data) {
		gap_->setScanResponseData(length, data);
	}
	void enableAdvertising(bool enabled) {
		gap_->enableAdvertising(enabled);
	}
	void startAdvertising() {
		gap_->startAdvertising();
	}
	void stopAdvertising() {
		gap_->stopAdvertising();
	}
	bool isAdvertisingEnabled() const {
		return gap_->isAdvertisingEnabled();
	}
	bool isConnected() const {
		return gap_->isConnected();
	}
	BleError requestConnectionParameterUpdate(ConnectionHandle con_handle,
											  const Gap::PreferredConnectionParameters& params) {
		return gap_->requestConnectionParameterUpdate(con_handle, params);
	}
	BleError updateConnectionParameters(ConnectionHandle con_handle,
										const Gap::PreferredConnectionParameters& params) {
		return gap_->updateConnectionParameters(con_handle, params);
	}
	BleError readRssi(ConnectionHandle con_handle) {
		return gap_->readRssi(con_handle);
	}
	BleError disconnect(ConnectionHandle con_handle) {
		return gap_->disconnect(con_handle);
	}
	AdvertisementDataBuilder& getAdvertisementDataBuilder() {
		return gap_->getAdvertisementDataBuilder();
	}
	const AdvertisementDataBuilder& getAdvertisementDataBuilder() const {
		return gap_->getAdvertisementDataBuilder();
	}

	virtual BleError dispatchBleHciPacket(uint8_t packet_type,
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
	bool turned_on_ = false;

	void* context_ = nullptr;
};

} // namespace c7222

#endif // ELEC_C7222_BLE_H_
