/**
 * @file ble_address.hpp
 * @brief BLE address wrapper.
 */
#ifndef ELEC_C7222_BLE_ADDRESS_H_
#define ELEC_C7222_BLE_ADDRESS_H_

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ostream>

namespace c7222 {

/**
 * @brief BLE address container with an associated address type.
 */
class BleAddress {
  public:
	static constexpr size_t kLength = 6;
	using RawAddress = std::array<uint8_t, kLength>;
	/**
	 * @brief Bluetooth address types.
	 */
	enum class AddressType : uint8_t {
		kLePublic = 0x00,
		kLeRandom = 0x01,
		kLePublicIdentity = 0x02,
		kLeRandomIdentity = 0x03,
		kSco = 0x04,
		kAcl = 0x05,
		kUnknown = 0xFF
	};

	/**
	 * @brief Construct an empty address with type Unknown.
	 */
	BleAddress() = default;

	/**
	 * @brief Construct from address type and raw Bluetooth address.
	 *
	 * @param type Address type.
	 * @param address Raw Bluetooth address (6 bytes).
	 */
	BleAddress(AddressType type, const RawAddress& address)
		: type_(type),
		  address_(address) {}

	BleAddress(AddressType type, const uint8_t address[kLength])
		: type_(type) {
		std::copy(address, address + kLength, address_.begin());
	}

	/**
	 * @brief Get the stored address type.
	 */
	AddressType type() const {
		return type_;
	}

	/**
	 * @brief Set the address type.
	 *
	 * @param type New address type.
	 */
	void set_type(AddressType type) {
		type_ = type;
	}

	/**
	 * @brief Get a pointer to the raw address bytes.
	 */
	const uint8_t* bytes() const {
		return address_.data();
	}

	/**
	 * @brief Get a const reference to the raw address array.
	 */
	const RawAddress& raw() const {
		return address_;
	}

	static const RawAddress& null_addr() {
		static const RawAddress addr = {};
		return addr;
	}

	/**
	 * @brief Copy the raw address into a caller-provided buffer.
	 *
	 * @param out Destination address buffer.
	 */
	void copy_to(RawAddress& out) const {
		out = address_;
	}

	/**
	 * @brief Copy the raw address into a caller-provided array.
	 *
	 * @param out Destination address buffer.
	 */
	void copy_to(uint8_t (&out)[kLength]) const {
		std::copy(address_.begin(), address_.end(), out);
	}

	/**
	 * @brief Compare both address type and address bytes.
	 */
	bool operator==(const BleAddress& other) const {
		return type_ == other.type_ && address_ == other.address_;
	}

	/**
	 * @brief Compare both address type and address bytes.
	 */
	bool operator!=(const BleAddress& other) const {
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const BleAddress& addr);
	
  private:
	AddressType type_ = AddressType::kUnknown;
	RawAddress address_{};
};

std::ostream& operator<<(std::ostream& os, const BleAddress& addr);

} // namespace c7222

#endif // ELEC_C7222_BLE_ADDRESS_H_
