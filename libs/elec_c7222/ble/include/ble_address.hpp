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
 * @class BleAddress
 * @brief BLE address container with an associated address type.
 *
 * A BLE address is a 6-byte value plus an address type (public/random/etc).
 *
 * ---
 * ### Construction
 *
 * Create an address from a raw 6-byte buffer and a type:
 * @code
 * c7222::BleAddress::RawAddress raw = {0xC0, 0xFF, 0xEE, 0xBA, 0xBE, 0x01};
 * c7222::BleAddress addr(c7222::BleAddress::AddressType::kLeRandom, raw);
 * @endcode
 *
 * ---
 * ### Accessors and Utilities
 *
 * - `GetType()` / `SetType()` access the address type.
 * - `GetBytes()` / `GetRaw()` access the raw address bytes.
 * - `CopyTo()` copies the address into a caller buffer.
 *
 * ---
 * ### Comparison
 *
 * Equality compares both the address type and address bytes.
 */
class BleAddress {
 public:
  /**
   * @brief Number of bytes in a BLE device address.
   */
  static constexpr size_t kLength = 6;

  /**
   * @brief Raw 6-byte address container.
   */
  using RawAddress = std::array<uint8_t, kLength>;

  /**
   * @brief Bluetooth address types (BTstack-compatible values).
   */
  enum class AddressType : uint8_t {
    /**
     * @brief Public Device Address (Vol 3, Part C, 10.8.1).
     */
    kLePublic = 0x00,
    /**
     * @brief Random Device Address (Vol 3, Part C, 10.8.2).
     */
    kLeRandom = 0x01,
    /**
     * @brief Public Identity Address (resolvable via IRK).
     */
    kLePublicIdentity = 0x02,
    /**
     * @brief Random (static or resolvable) Identity Address.
     */
    kLeRandomIdentity = 0x03,
    /**
     * @brief BR/EDR (SCO) address type.
     */
    kSco = 0x04,
    /**
     * @brief BR/EDR (ACL) address type.
     */
    kAcl = 0x05,
    /**
     * @brief Unknown or not provided.
     */
    kUnknown = 0xFF,
  };

  /**
   * @brief Constructs an empty address with type Unknown and zero bytes.
   */
  BleAddress() = default;

  /**
   * @brief Constructs from address type and raw Bluetooth address.
   *
   * Example:
   * @code
   * c7222::BleAddress addr(
   *     c7222::BleAddress::AddressType::kLePublic, raw);
   * @endcode
   */
  BleAddress(AddressType type, const RawAddress& address)
      : type_(type), address_(address) {}

  /**
   * @brief Constructs from address type and a raw 6-byte buffer.
   */
  BleAddress(AddressType type, const uint8_t address[kLength])
      : type_(type) {
    std::copy(address, address + kLength, address_.begin());
  }

  /**
   * @brief Returns the stored address type.
   */
  AddressType GetType() const {
    return type_;
  }

  /**
   * @brief Sets the address type.
   */
  void SetType(AddressType type) {
    type_ = type;
  }

  /**
   * @brief Returns a pointer to the raw address bytes.
   */
  const uint8_t* GetBytes() const {
    return address_.data();
  }

  /**
   * @brief Returns a const reference to the raw address array.
   */
  const RawAddress& GetRaw() const {
    return address_;
  }

  /**
   * @brief Returns a zero-filled address constant.
   */
  static const RawAddress& NullAddress() {
    static const RawAddress addr = {};
    return addr;
  }

  /**
   * @brief Copies the raw address into a caller-provided buffer.
   */
  void CopyTo(RawAddress& out) const {
    out = address_;
  }

  /**
   * @brief Copies the raw address into a caller-provided array.
   */
  void CopyTo(uint8_t (&out)[kLength]) const {
    std::copy(address_.begin(), address_.end(), out);
  }

  /**
   * @brief Compares both address type and address bytes.
   */
  bool operator==(const BleAddress& other) const {
    return type_ == other.type_ && address_ == other.address_;
  }

  /**
   * @brief Compares both address type and address bytes.
   */
  bool operator!=(const BleAddress& other) const {
    return !(*this == other);
  }

  /**
   * @brief Stream output helper.
   */
  friend std::ostream& operator<<(std::ostream& os, const BleAddress& addr);

 private:
  AddressType type_ = AddressType::kUnknown;
  RawAddress address_{};
};

std::ostream& operator<<(std::ostream& os, const BleAddress& addr);

}  // namespace c7222

#endif  // ELEC_C7222_BLE_ADDRESS_H_
