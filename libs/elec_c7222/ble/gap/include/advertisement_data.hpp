/**
 * @file advertisement_data.hpp
 * @brief BLE GAP advertisement data builder.
 */
#ifndef ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_
#define ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_

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
		Build(type, data.data(), data.size());
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
	template <typename T>
	AdvertisementData(AdvertisementDataType type, const T* object_ptr) {
		static_assert(!std::is_pointer<T>::value, "object_ptr must point to a concrete type");
		// static_assert(!std::is_array<T>::value, "object_ptr must not be an array type");
		Build(type, reinterpret_cast<const std::uint8_t*>(object_ptr), sizeof(T));
	}

	/**
	 * @brief Build an AD structure from an array of typed objects.
	 *
	 * Copies elem_count items of type T into the payload.
	 *
	 * @tparam T Concrete (non-pointer) type.
	 * @param type Advertisement data type.
	 * @param object_ptr Pointer to the first element.
	 * @param elem_count Number of elements to copy.
	 */
	template <typename T>
	AdvertisementData(AdvertisementDataType type, const T* object_ptr, size_t elem_count) {
		static_assert(!std::is_pointer<T>::value, "object_ptr must point to a concrete type");
		// static_assert(std::is_array<T>::value, "object_ptr must be an array type");
		Build(type, reinterpret_cast<const std::uint8_t*>(object_ptr), sizeof(T) * elem_count);
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
	template <typename T>
	AdvertisementData(AdvertisementDataType type, const T& object_ref) {
		static_assert(!std::is_pointer<T>::value,
					  "object_ref must refer to a concrete type, not a pointer type");
		if(std::is_object<T>::value)
			Build(type, reinterpret_cast<const std::uint8_t*>(&object_ref), sizeof(T));
		else {
			T obj = object_ref;
			Build(type, reinterpret_cast<const std::uint8_t*>(&obj), sizeof(T));
		}
	}

	/**
	 * @brief Return the full AD structure bytes.
	 *
	 * The returned vector includes the length and type header bytes.
	 */
	const std::vector<uint8_t>& GetData() const {
		return data_;
	}

	/**
	 * @brief Return a raw pointer to the AD structure bytes.
	 *
	 * The pointer remains valid until the object is modified or destroyed.
	 */
	const uint8_t* GetBytes() const {
		return data_.data();
	}

	/**
	 * @brief Return the total size of the AD structure (length + type + value).
	 */
	size_t GetSize() const {
		return data_.size();
	}

	/**
	 * @brief Return the AD structure type.
	 */
	AdvertisementDataType GetType() const {
		assert(data_.size() >= 2 && "AdvertisementData: data size too small to contain type");
		return static_cast<AdvertisementDataType>(data_[1]);
	}

	/**
	 * @brief Return the length field (type + value bytes).
	 */
	uint8_t GetLength() const {
		assert(data_.size() >= 1 && "AdvertisementData: data size too small to contain length");
		return data_[0];
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
	std::vector<uint8_t> operator+=(const AdvertisementData& other) const;

	/**
	 * @brief Compare AD structure bytes for equality.
	 */
	bool operator==(const AdvertisementData& other) const {
		return data_ == other.data_;
	}

	/**
	 * @brief Stream an AD structure for logging/debugging.
	 */
	friend std::ostream& operator<<(std::ostream& os, const AdvertisementData& ad);
	/**
	 * @brief Stream AD Flags for logging/debugging.
	 */
	friend std::ostream& operator<<(std::ostream& os, const AdvertisementData::Flags& flag);

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
	static bool ValidateLength(AdvertisementDataType type, size_t length);

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
	static bool ValidateBuffer(const uint8_t* adv_data, size_t adv_data_size);

	/**
	 * @brief Validate a raw advertising payload stored in a vector.
	 *
	 * @param adv_data Raw advertising payload bytes.
	 * @return true if the buffer is well-formed, false otherwise.
	 */
	static bool ValidateBuffer(const std::vector<uint8_t>& adv_data) {
		return ValidateBuffer(adv_data.data(), adv_data.size());
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
	void Build(AdvertisementDataType type, const uint8_t* data, size_t size);

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
std::vector<uint8_t> operator+(const AdvertisementData& lhs, const AdvertisementData& rhs);

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
	AdvertisementDataBuilder();

	/**
	 * @brief Create a builder from a list of AD structures.
	 *
	 * The builder validates the resulting payload.
	 *
	 * @param ads List of AD structures to add.
	 */
	explicit AdvertisementDataBuilder(const std::list<AdvertisementData>& ads);

	/**
	 * @brief ReplaceOrAdd the payload with a list of AD structures.
	 *
	 * @param ads List of AD structures to set.
	 * @return true if all structures were added, false if a structure would overflow.
	 */
	bool Set(const std::list<AdvertisementData>& ads);

	/**
	 * @brief ReplaceOrAdd the payload from a raw advertising buffer.
	 *
	 * @param data Raw advertising bytes.
	 * @param size Total buffer size in bytes.
	 * @return true if decoded and built successfully, false otherwise.
	 */
	bool Set(const uint8_t* data, size_t size);

	/**
	 * @brief Remove the last AD structure from the payload.
	 *
	 * @return true if an AD structure was removed, false if the payload is empty or invalid.
	 */
	bool Pop();

	/**
	 * @brief Add an AD structure to the payload.
	 *
	 * @param ad AD structure to add.
	 * @return true if added, false if the payload would overflow.
	 */
	bool Push(const AdvertisementData& ad);

	/**
	 * @brief ReplaceOrAdd the AD structure with the same type.
	 *
	 * @param ad AD structure to replace in the list.
	 */
	void ReplaceOrAdd(const AdvertisementData& ad);

	/**
	 * @brief ReplaceOrAdd the AD structure with the same type (move).
	 *
	 * @param ad AD structure to replace in the list (moved).
	 */
	void ReplaceOrAdd(AdvertisementData&& ad);

	/**
	 * @brief Add a list of AD structures to the payload.
	 *
	 * @param ads List of AD structures to add.
	 * @return true if all structures were added, false if a structure would overflow.
	 */
	bool Add(const std::list<AdvertisementData>& ads);
	/**
	 * @brief Add an AD structure to the payload.
	 *
	 * Fails if the addition would exceed 31 bytes.
	 *
	 * @param ad AD structure to add.
	 * @return true if added, false if the payload would overflow.
	 */
	bool Add(const AdvertisementData& ad);
	/**
	 * @brief Return the raw advertising payload bytes.
	 */
	const std::vector<uint8_t>& data() const;

	/**
	 * @brief Return a raw pointer to the advertising payload bytes.
	 */
	const uint8_t* bytes() const;

	/**
	 * @brief Return the total payload size in bytes.
	 */
	size_t size() const;

	/**
	 * @brief Clear all stored AD structures.
	 */
	void Clear();

	/**
	 * @brief Add an AD structure and enforce uniqueness.
	 *
	 * If an identical AD structure already exists, this asserts.
	 *
	 * @param ad AD structure to add.
	 * @return Reference to this builder.
	 */
	AdvertisementDataBuilder& operator+=(const AdvertisementData& ad);

	/**
	 * @brief Return a builder with an additional AD structure.
	 *
	 * @param ad AD structure to add.
	 * @return Builder copy with the AD structure added.
	 */
	AdvertisementDataBuilder operator+(const AdvertisementData& ad);

	/**
	 * @brief Merge another builder into this one, enforcing uniqueness.
	 *
	 * @param ad Builder to merge in.
	 * @return Reference to this builder.
	 */
	AdvertisementDataBuilder& operator+=(const AdvertisementDataBuilder& adb);

	/**
	 * @brief Return a builder that is the merge of two builders.
	 *
	 * @param ad Builder to merge in.
	 * @return Merged builder copy.
	 */
	AdvertisementDataBuilder operator+(const AdvertisementDataBuilder& ad);

	/**
	 * @brief Compare two builders by payload or list contents.
	 */
	bool operator==(const AdvertisementDataBuilder& other) const;
	/**
	 * @brief Validate the assembled payload.
	 */
	bool Validate() const;

	/**
	 * @brief Build the raw payload from stored AD structures.
	 *
	 * Sorts by type, assembles the payload, and validates the result.
	 *
	 * @return true if the payload is valid, false otherwise.
	 */
	bool Build();
	/**
	 * @brief Decode the payload into a list of AD structures.
	 */
	const std::list<AdvertisementData>& advertisement_data_list() const {
		return advertisements_;
	}

	/**
	 * @brief Decode a raw advertising payload buffer into a list of AD structures.
	 *
	 * @param adv_data Pointer to raw advertising bytes.
	 * @param adv_data_size Total buffer size in bytes.
	 * @return List of decoded AD structures.
	 */

	static std::list<AdvertisementData> DecodeBufferToAdvertisementDataList(const uint8_t* adv_data,
																			size_t adv_data_size);

	/**
	 * @brief Stream the builder payload for logging/debugging.
	 */
	friend std::ostream& operator<<(std::ostream& os, const AdvertisementDataBuilder& adb);

   private:
	/**
	 * @brief Stored raw advertising payload bytes.
	 */
	std::list<AdvertisementData> advertisements_;
	/**
	 * @brief Cached raw payload bytes (valid when built_ is true).
	 */
	std::vector<uint8_t> data_;
	/**
	 * @brief True if data_ matches the current advertisements_ list.
	 */
	bool built_ = false;
};

}  // namespace c7222

constexpr uint8_t operator|(c7222::AdvertisementData::Flags lhs,
							c7222::AdvertisementData::Flags rhs) {
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

constexpr uint8_t operator&(c7222::AdvertisementData::Flags lhs,
							c7222::AdvertisementData::Flags rhs) {
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

#endif	// ELEC_C7222_BLE_GAP_ADVERTISEMENT_DATA_H_
