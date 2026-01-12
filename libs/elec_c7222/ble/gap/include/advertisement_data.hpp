/**
 * @file advertisement_data.hpp
 * @brief BLE GAP advertisement data builder.
 */
#ifndef ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_
#define ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <list>
#include <type_traits>
#include <vector>

namespace c7222 {

/**
 * @brief Maximum length for legacy advertising data (length + type + value).
 */
constexpr size_t kAdvertisementDataLegacyMaxSize = 31;
/**
 * \brief Overhead bytes for an AD structure (length + type).
 */
constexpr size_t kAdvertisementDataStructHeaderOverhead = 2;
/**
 * @brief BLE GAP advertisement data types.
 */
enum class AdvertisementDataType : uint8_t {
	kFlags = 0x01,
	kIncompleteList16BitUuid = 0x02,
	kCompleteList16BitUuid = 0x03,
	kShortenedLocalName = 0x08,
	kCompleteLocalName = 0x09,
	kTxPowerLevel = 0x0A,
	kSlaveConnectionIntervalRange = 0x12,
	kServiceData16BitUuid = 0x16,
	kManufacturerSpecific = 0xFF
};

/**
 * @brief Generic advertisement data structure builder.
 *
 * Stores a single AD structure (length + type + value) in a vector.
 */
class AdvertisementData {
  public:
	enum class Flags : uint8_t {
		kLeLimitedDiscoverableMode = 0x01,
		kLeGeneralDiscoverableMode = 0x02,
		kBrEdrNotSupported = 0x04,
		kSimultaneousLeAndBrEdrController = 0x08,
		kSimultaneousLeAndBrEdrHost = 0x10,
		kAll = 0x1f,
	};

	/**
	 * @brief Build an AD structure from a byte vector.
	 *
	 * Use this when your payload is already stored in a std::vector.
	 *
	 * @param type Advertisement data type.
	 * @param data Value bytes.
	 */
	AdvertisementData(AdvertisementDataType type, const std::vector<uint8_t>& data) {
		build(type, data.data(), data.size());
	}

	/**
	 * @brief Build an AD structure from a typed object pointer.
	 *
	 * This overload deduces the payload size as sizeof(T) and copies
	 * the raw bytes of the object into the AD structure.
	 *
	 * Use this for POD/struct payloads that match the AD format.
	 *
	 * @tparam T Concrete (non-pointer) type.
	 * @param type Advertisement data type.
	 * @param object_ptr Pointer to a concrete object.
	 */
	template <typename T> AdvertisementData(AdvertisementDataType type, const T* object_ptr) {
		static_assert(!std::is_pointer<T>::value, "object_ptr must point to a concrete type");
		// static_assert(!std::is_array<T>::value, "object_ptr must not be an array type");
		build(type, reinterpret_cast<const std::uint8_t*>(object_ptr), sizeof(T));
	}

	template <typename T> AdvertisementData(AdvertisementDataType type, const T* object_ptr, size_t elem_count) {
		static_assert(!std::is_pointer<T>::value, "object_ptr must point to a concrete type");
		// static_assert(std::is_array<T>::value, "object_ptr must be an array type");
		build(type, reinterpret_cast<const std::uint8_t*>(object_ptr), sizeof(T) * elem_count);
	}

	/**
	 * @brief Build an AD structure from a typed object reference.
	 *
	 * This overload deduces the payload size as sizeof(T) and copies
	 * the raw bytes of the object into the AD structure.
	 *
	 * Use this for POD/struct payloads that match the AD format.
	 *
	 * @tparam T Concrete type.
	 * @param type Advertisement data type.
	 * @param object_ref Reference to a concrete object.
	 */
	template <typename T> AdvertisementData(AdvertisementDataType type, const T& object_ref) {
		static_assert(!std::is_pointer<T>::value, "object_ref must refer to a concrete type, not a pointer type");
		if(std::is_object<T>::value)
			build(type, reinterpret_cast<const std::uint8_t*>(&object_ref), sizeof(T));
		else {
			T obj = object_ref;
			build(type, reinterpret_cast<const std::uint8_t*>(&obj), sizeof(T));
		}
	}

	/**
	 * @brief Return the full AD structure bytes.
	 *
	 * The returned vector includes the length and type header bytes.
	 */
	const std::vector<uint8_t>& data() const {
		return data_;
	}

	/**
	 * @brief Return a raw pointer to the AD structure bytes.
	 *
	 * The pointer remains valid until the object is modified or destroyed.
	 */
	const uint8_t* bytes() const {
		return data_.data();
	}

	/**
	 * @brief Return the total size of the AD structure (length + type + value).
	 */
	size_t size() const {
		return data_.size();
	}

	AdvertisementDataType getType() const {
		assert(data_.size() >= 2 && "AdvertisementData: data size too small to contain type");
		return static_cast<AdvertisementDataType>(data_[1]);
	}

	uint8_t getLength() const {
		assert(data_.size() >= 1 && "AdvertisementData: data size too small to contain length");
		return data_[0];
	}

  private:
	/**
	 * @brief Iterator to the beginning of the AD structure bytes.
	 */
	std::vector<uint8_t>::iterator begin() {
		return data_.begin();
	}
	/**
	 * @brief Const iterator to the beginning of the AD structure bytes.
	 */
	std::vector<uint8_t>::const_iterator begin() const {
		return data_.begin();
	}
	/**
	 * @brief Const iterator to the beginning of the AD structure bytes.
	 */
	std::vector<uint8_t>::const_iterator cbegin() const {
		return data_.cbegin();
	}
	/**
	 * @brief Iterator to the end of the AD structure bytes.
	 */
	std::vector<uint8_t>::iterator end() {
		return data_.end();
	}
	/**
	 * @brief Const iterator to the end of the AD structure bytes.
	 */
	std::vector<uint8_t>::const_iterator end() const {
		return data_.end();
	}
	/**
	 * @brief Const iterator to the end of the AD structure bytes.
	 */
	std::vector<uint8_t>::const_iterator cend() const {
		return data_.cend();
	}

	/**
	 * @brief Concatenate two AD structures into a raw byte vector.
	 *
	 * Use this when you want to build a single advertising payload from
	 * multiple AD structures.
	 *
	 * @param other AD structure to append.
	 * @return Combined byte vector (no re-validation of 31-byte limit).
	 */
	std::vector<uint8_t> operator+=(const AdvertisementData& other) const {
		std::vector<uint8_t> new_data;
		new_data.reserve(data_.size() + other.size());
		new_data.insert(new_data.end(), data_.cbegin(), data_.cend());
		new_data.insert(new_data.end(), other.data().cbegin(), other.data().cend());
		return new_data;
	}
	friend std::ostream& operator<<(std::ostream& os, const AdvertisementData& ad);
	friend std::ostream& operator<<(std::ostream& os, const AdvertisementData::Flags& flag);

  public:
	/**
	 * @brief Validate the AD length field for a given type.
	 *
	 * Enforces the BLE specification rules for specific data types
	 * (e.g., Flags must be 1 byte, UUID lists must be 16-bit aligned).
	 *
	 * @param type Advertisement data type.
	 * @param length Length field value (type + value bytes).
	 */
	static bool validate_length(AdvertisementDataType type, size_t length) {
		if(length == 0 || (length + 1) > kAdvertisementDataLegacyMaxSize) {
			return false;
		}

		const size_t data_size = length - 1;
		switch(type) {
		case AdvertisementDataType::kFlags:
		case AdvertisementDataType::kTxPowerLevel:
			if(length != 2) {
				return false;
			}
			break;
		case AdvertisementDataType::kSlaveConnectionIntervalRange:
			if(length != 5) {
				return false;
			}
			break;
		case AdvertisementDataType::kIncompleteList16BitUuid:
		case AdvertisementDataType::kCompleteList16BitUuid:
			if(data_size == 0 || (data_size % 2) != 0) {
				return false;
			}
			break;
		case AdvertisementDataType::kServiceData16BitUuid:
		case AdvertisementDataType::kManufacturerSpecific:
			if(length < 3) {
				return false;
			}
			break;
		case AdvertisementDataType::kShortenedLocalName:
		case AdvertisementDataType::kCompleteLocalName:
			if(length < 2) {
				return false;
			}
			break;
		default:
			assert(false && "Unknown AdvertisementDataType");
			return false;
		}
		return true;
	}

	/**
	 * @brief Validate a raw advertising payload buffer.
	 *
	 * Iterates over each AD structure in the buffer, validates the length
	 * and type rules, and ensures the payload fully consumes the buffer
	 * without trailing bytes.
	 *
	 * @param adv_data Pointer to raw advertising bytes.
	 * @param adv_data_size Total buffer size in bytes.
	 * @return true if the buffer is well-formed, false otherwise.
	 */
	static bool validate_buffer(const uint8_t* adv_data, size_t adv_data_size) {
		size_t index = 0;
		size_t data_count = 0;
		// validate size
		if(adv_data_size == 0 || adv_data_size > kAdvertisementDataLegacyMaxSize) {
			return false;
		}
		// parse each AD structure
		while(index < adv_data_size) {
			uint8_t length = adv_data[index];
			AdvertisementDataType type = static_cast<AdvertisementDataType>(adv_data[index + 1]);
			bool check = validate_length(type, length);
			if(!check) {
				return false;
			}
			index += length + 1;
		}
		// check whether we have some extra data at the end
		return (index == adv_data_size) ? true : false;
	}

	/**
	 * @brief Validate a raw advertising payload stored in a vector.
	 *
	 * @param adv_data Raw advertising payload bytes.
	 * @return true if the buffer is well-formed, false otherwise.
	 */
	static bool validate_buffer(const std::vector<uint8_t>& adv_data) {
		return validate_buffer(adv_data.data(), adv_data.size());
	}

  private:
	/**
	 * @brief Build and store the AD structure bytes.
	 *
	 * Adds the length and type header, validates sizes, and copies the payload.
	 * The resulting vector is the exact AD structure to pass to the controller.
	 *
	 * @param type Advertisement data type.
	 * @param data Pointer to payload bytes (may be null if size is 0).
	 * @param size Payload size in bytes.
	 */
	void build(AdvertisementDataType type, const uint8_t* data, size_t size) {
		if(size > 0 && data == nullptr) {
			__builtin_trap();
		}

		assert(validate_length(type, size + 1) && "Invalid AdvertisementData size for given type");

		const size_t total = size + kAdvertisementDataStructHeaderOverhead;
		if(total > kAdvertisementDataLegacyMaxSize) {
			__builtin_trap();
		}

		data_.reserve(total);
		data_.push_back(static_cast<uint8_t>(size + 1));
		data_.push_back(static_cast<uint8_t>(type));
		if(size > 0) {
			data_.insert(data_.end(), data, data + size);
		}
	}

	/**
	 * @brief Stored AD structure bytes (length + type + value).
	 */
	std::vector<uint8_t> data_;
};

/**
 * @brief Concatenate two AD structures into a raw byte vector.
 *
 * Use this when building a single advertising payload from multiple
 * AdvertisementData objects. The caller is responsible for ensuring
 * the combined size stays within the legacy 31-byte limit.
 */
inline std::vector<uint8_t> operator+(const AdvertisementData& lhs, const AdvertisementData& rhs) {
	std::vector<uint8_t> combined;
	combined.reserve(lhs.size() + rhs.size());
	combined.insert(combined.end(), lhs.data().cbegin(), lhs.data().cend());
	combined.insert(combined.end(), rhs.data().cbegin(), rhs.data().cend());
	return combined;
}

/**
 * @brief Builder for assembling a complete advertising payload.
 *
 * Use this class when you want to compose multiple AD structures into a single
 * legacy advertising payload. The builder enforces the 31-byte limit on insert.
 */
class AdvertisementDataBuilder {
  public:
	/**
	 * @brief Create an empty builder.
	 */
	AdvertisementDataBuilder() {
		data_.reserve(kAdvertisementDataLegacyMaxSize);
	}

	/**
	 * @brief Create a builder from a list of AD structures.
	 *
	 * The builder validates the resulting payload.
	 *
	 * @param ads List of AD structures to add.
	 */
	AdvertisementDataBuilder(const std::list<AdvertisementData>& ads) {
		for(const auto& ad: ads) {
			add(ad);
		}
		assert(AdvertisementData::validate_buffer(data_) &&
			   "Built advertisement data is invalid. check the added AdvertisementData items.");
	}

	/**
	 * @brief Replace the payload with a list of AD structures.
	 *
	 * @param ads List of AD structures to set.
	 * @return true if all structures were added, false if a structure would overflow.
	 */
	bool set(const std::list<AdvertisementData>& ads) {
		clear();
		for(const auto& ad: ads) {
			if(!add(ad)) {
				return false;
			}
		}
		assert(AdvertisementData::validate_buffer(data_) &&
			   "Built advertisement data is invalid. check the added AdvertisementData items.");
		return true;
	}

	bool set(uint8_t length, const uint8_t* data) {
		clear();
		if(data != nullptr && length > 0) {
			const size_t copy_len = std::min<size_t>(length, kAdvertisementDataLegacyMaxSize);
			data_.assign(data, data + copy_len);
		}
		assert(AdvertisementData::validate_buffer(data_) &&
			   "Built advertisement data is invalid. check the added AdvertisementData items.");
			return true;
	}

	/**
	 * @brief Remove the last AD structure from the payload.
	 *
	 * @return true if an AD structure was removed, false if the payload is empty or invalid.
	 */
	bool pop() {
		if(data_.empty()) {
			return false;
		}

		size_t index = 0;
		size_t last_start = 0;

		while(index < data_.size()) {
			uint8_t length = data_[index];
			if(length == 0) {
				return false;
			}

			size_t next = index + static_cast<size_t>(length) + 1;
			if(next > data_.size()) {
				return false;
			}

			last_start = index;
			index = next;
		}

		data_.erase(data_.begin() + static_cast<std::ptrdiff_t>(last_start), data_.end());
		return true;
	}

	/**
	 * @brief Add an AD structure to the payload.
	 *
	 * @param ad AD structure to add.
	 * @return true if added, false if the payload would overflow.
	 */
	bool push(const AdvertisementData& ad) {
		return add(ad);
	}

	/**
	 * @brief Add a list of AD structures to the payload.
	 *
	 * @param ads List of AD structures to add.
	 * @return true if all structures were added, false if a structure would overflow.
	 */
	bool add(const std::list<AdvertisementData>& ads) {
		for(const auto& ad: ads) {
			if(!add(ad)) {
				return false;
			}
		}
		assert(AdvertisementData::validate_buffer(data_) &&
			   "Built advertisement data is invalid. check the added AdvertisementData items.");
		return true;
	}
	/**
	 * @brief Add an AD structure to the payload.
	 *
	 * Fails if the addition would exceed 31 bytes.
	 *
	 * @param ad AD structure to add.
	 * @return true if added, false if the payload would overflow.
	 */
	bool add(const AdvertisementData& ad) {
		const auto& ad_data = ad.data();
		if((data_.size() + ad_data.size()) > kAdvertisementDataLegacyMaxSize) {
			return false;
		}
		data_.insert(data_.end(), ad_data.cbegin(), ad_data.cend());
		return true;
	}

	/**
	 * @brief Build and add an AD structure from raw bytes.
	 *
	 * @param type Advertisement data type.
	 * @param data Payload bytes.
	 * @param size Payload length.
	 * @return true if added, false if the payload would overflow.
	 */
	bool add(AdvertisementDataType type, const uint8_t* data, size_t size) {
		AdvertisementData ad(type, data, size);
		return add(ad);
	}

	/**
	 * @brief Build and add an AD structure from a vector.
	 *
	 * @param type Advertisement data type.
	 * @param data Payload bytes.
	 * @return true if added, false if the payload would overflow.
	 */
	bool add(AdvertisementDataType type, const std::vector<uint8_t>& data) {
		AdvertisementData ad(type, data);
		return add(ad);
	}

	/**
	 * @brief Build and add an AD structure from a typed object pointer.
	 *
	 * @tparam T Concrete (non-pointer) type.
	 * @param type Advertisement data type.
	 * @param object_ptr Pointer to a concrete object.
	 * @return true if added, false if the payload would overflow.
	 */
	template <typename T> bool add(AdvertisementDataType type, const T* object_ptr) {
		AdvertisementData ad(type, object_ptr);
		return add(ad);
	}

	/**
	 * @brief Build and add an AD structure from a typed object reference.
	 *
	 * @tparam T Concrete type.
	 * @param type Advertisement data type.
	 * @param object_ref Reference to a concrete object.
	 * @return true if added, false if the payload would overflow.
	 */
	template <typename T> bool add(AdvertisementDataType type, const T& object_ref) {
		AdvertisementData ad(type, object_ref);
		return add(ad);
	}


	/**
	 * @brief Return the raw advertising payload bytes.
	 */
	const std::vector<uint8_t>& data() const {
		return data_;
	}

	/**
	 * @brief Return a raw pointer to the advertising payload bytes.
	 */
	const uint8_t* bytes() const {
		return data_.data();
	}

	/**
	 * @brief Return the total payload size in bytes.
	 */
	size_t size() const {
		return data_.size();
	}

	/**
	 * @brief Clear all stored AD structures.
	 */
	void clear() {
		data_.clear();
	}

	/**
	 * @brief Add an AD structure and enforce uniqueness.
	 *
	 * If an identical AD structure already exists, this asserts.
	 *
	 * @param ad AD structure to add.
	 * @return Reference to this builder.
	 */
	AdvertisementDataBuilder& operator+=(const AdvertisementData& ad) {
		std::list<AdvertisementData> my_ads = to_advertisement_data_list();
		auto it = std::find_if(my_ads.cbegin(), my_ads.cend(), [&ad](const AdvertisementData& existing_ad) {
			return existing_ad.data() == ad.data();
		});
		if(it == my_ads.cend()) {
			add(ad);
		} else {
			// already exists, so we must hard-assert
			assert(false && "AdvertisementData already exists in the builder");
		}
		return *this;
	}

	/**
	 * @brief Return a builder with an additional AD structure.
	 *
	 * @param ad AD structure to add.
	 * @return Builder copy with the AD structure added.
	 */
	AdvertisementDataBuilder operator+(const AdvertisementData& ad) {
		this->operator+=(ad);
		return *this;
	}

	/**
	 * @brief Merge another builder into this one, enforcing uniqueness.
	 *
	 * @param ad Builder to merge in.
	 * @return Reference to this builder.
	 */
	AdvertisementDataBuilder& operator+=(const AdvertisementDataBuilder& ad) {
		std::list<AdvertisementData> ads = ad.to_advertisement_data_list();
		std::list<AdvertisementData> my_ads = to_advertisement_data_list();

		for(const auto& a: ads) {
			auto it = std::find_if(my_ads.cbegin(), my_ads.cend(), [&a](const AdvertisementData& existing_ad) {
				return existing_ad.data() == a.data();
			});
			if(it == my_ads.cend()) {
				add(a);
			} else {
				// already exists, so we must hard-assert
				assert(false && "AdvertisementData already exists in the builder");
			}
		}
	}

	/**
	 * @brief Return a builder that is the merge of two builders.
	 *
	 * @param ad Builder to merge in.
	 * @return Merged builder copy.
	 */
	AdvertisementDataBuilder operator+(const AdvertisementDataBuilder& ad) {
		this->operator+=(ad);
		return *this;
	}

	/**
	 * @brief Validate the assembled payload.
	 */
	bool validate() const {
		return AdvertisementData::validate_buffer(data_);
	}

	/**
	 * @brief Decode the payload into a list of AD structures.
	 */
	std::list<AdvertisementData> to_advertisement_data_list() const {
		std::list<AdvertisementData> ads;
		size_t index = 0;
		while(index < data_.size()) {
			uint8_t length = data_[index];
			AdvertisementDataType type = static_cast<AdvertisementDataType>(data_[index + 1]);
			size_t value_size = length - 1;
			ads.emplace_back(type, &data_[index + 2], value_size);
			index += length + 1;
		}
		return ads;
	}

	/**
	 * @brief Decode a raw advertising payload buffer into a list of AD structures.
	 *
	 * @param adv_data Pointer to raw advertising bytes.
	 * @param adv_data_size Total buffer size in bytes.
	 * @return List of decoded AD structures.
	 */

	static std::list<AdvertisementData> decode_buffer_to_advertisement_data_list(const uint8_t* adv_data,
																				 size_t adv_data_size) {
		std::list<AdvertisementData> ads;
		size_t index = 0;
		while(index < adv_data_size) {
			uint8_t length = adv_data[index];
			AdvertisementDataType type = static_cast<AdvertisementDataType>(adv_data[index + 1]);
			size_t value_size = length - 1;
			ads.emplace_back(type, &adv_data[index + 2], value_size);
			index += length + 1;
		}
		return ads;
	}

	friend std::ostream& operator<<(std::ostream& os, const AdvertisementDataBuilder& adb);

  private:
	/**
	 * @brief Stored raw advertising payload bytes.
	 */
	std::vector<uint8_t> data_;
};

} // namespace c7222

constexpr uint8_t operator|(c7222::AdvertisementData::Flags lhs, c7222::AdvertisementData::Flags rhs) {
	return static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs);
}
constexpr uint8_t operator|(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}
constexpr uint8_t operator|=(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}
constexpr uint8_t operator|(c7222::AdvertisementData::Flags lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) | rhs;
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}

constexpr uint8_t operator&(c7222::AdvertisementData::Flags lhs, c7222::AdvertisementData::Flags rhs) {
	return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

constexpr uint8_t operator&(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}

constexpr uint8_t operator&=(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}

constexpr uint8_t operator&(c7222::AdvertisementData::Flags lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}

constexpr uint8_t operator^(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}
constexpr uint8_t operator^=(uint8_t lhs, c7222::AdvertisementData::Flags rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}
constexpr uint8_t operator^(c7222::AdvertisementData::Flags lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::AdvertisementData::Flags::kAll));
	return ret;
}

#endif // ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_
