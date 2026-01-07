/**
 * @file advertisement_data.hpp
 * @brief BLE GAP advertisement data builder.
 */
#ifndef TEMPLATE_BLE_GAP_ADVERTISEMENT_DATA_HPP
#define TEMPLATE_BLE_GAP_ADVERTISEMENT_DATA_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>


namespace c7222 {

/**
 * @brief Maximum length for legacy advertising data (length + type + value).
 */
constexpr size_t kLegacyAdvMaxSize = 31;

/**
 * @brief BLE GAP advertisement data types.
 */
enum class AdvertisementDataType : uint8_t {
	Flags = 0x01,
	IncompleteList16BitUUID = 0x02,
	CompleteList16BitUUID = 0x03,
	ShortenedLocalName = 0x08,
	CompleteLocalName = 0x09,
	TxPowerLevel = 0x0A,
	SlaveConnectionIntervalRange = 0x12,
	ServiceData16BitUUID = 0x16,
	ManufacturerSpecific = 0xFF
};

/**
 * @brief Generic advertisement data structure builder.
 *
 * Stores a single AD structure (length + type + value) in a vector.
 */
class AdvertisementData {
  public:
	AdvertisementData(AdvertisementDataType type, const uint8_t* data, size_t size) {
		build(type, data, size);
	}

	AdvertisementData(AdvertisementDataType type, const std::vector<uint8_t>& data) {
		build(type, data.data(), data.size());
	}

	template <typename T>
	AdvertisementData(AdvertisementDataType type, const T* object_ptr) {
		static_assert(!std::is_pointer<T>::value, "object_ptr must point to a concrete type");
		build(type, reinterpret_cast<const std::uint8_t*>(object_ptr), sizeof(T));
	}

	const std::vector<uint8_t>& bytes() const {
		return data_;
	}

	const uint8_t* data() const {
		return data_.data();
	}

	size_t size() const {
		return data_.size();
	}

  private:
	static constexpr size_t kOverhead = 2;

	static void validate_size(AdvertisementDataType type, size_t size) {
		switch (type) {
			case AdvertisementDataType::Flags:
			case AdvertisementDataType::TxPowerLevel:
				if (size != 1) {
					__builtin_trap();
				}
				break;
			case AdvertisementDataType::SlaveConnectionIntervalRange:
				if (size != 4) {
					__builtin_trap();
				}
				break;
			case AdvertisementDataType::IncompleteList16BitUUID:
			case AdvertisementDataType::CompleteList16BitUUID:
				if (size == 0 || (size % 2) != 0) {
					__builtin_trap();
				}
				break;
			case AdvertisementDataType::ServiceData16BitUUID:
			case AdvertisementDataType::ManufacturerSpecific:
				if (size < 2) {
					__builtin_trap();
				}
				break;
			case AdvertisementDataType::ShortenedLocalName:
			case AdvertisementDataType::CompleteLocalName:
				if (size == 0) {
					__builtin_trap();
				}
				break;
			default:
				break;
		}
	}

	void build(AdvertisementDataType type, const uint8_t* data, size_t size) {
		if (size > 0 && data == nullptr) {
			__builtin_trap();
		}

		validate_size(type, size);

		const size_t total = size + kOverhead;
		if (total > kLegacyAdvMaxSize) {
			__builtin_trap();
		}

		data_.reserve(total);
		data_.push_back(static_cast<uint8_t>(size + 1));
		data_.push_back(static_cast<uint8_t>(type));
		if (size > 0) {
			data_.insert(data_.end(), data, data + size);
		}
	}

	std::vector<uint8_t> data_;
};

} // namespace c7222::ble::gap

#endif // TEMPLATE_BLE_GAP_ADVERTISEMENT_DATA_HPP
