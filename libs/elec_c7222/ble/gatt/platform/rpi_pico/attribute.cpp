/**
 * @file attribute.cpp
 * @brief ATT attribute database parser for BTstack GATT stack.
 *
 * Parses a BTstack ATT (Attribute) database into C++ Attribute objects.
 * The database is a binary format with entries describing GATT attributes.
 *
 * Binary Layout (from BTstack att_db.c):
 * - Offset 0-1: Entry size (LE16), 0 = end of database
 * - Offset 2-3: Flags/Properties (LE16) - see Attribute::Properties
 * - Offset 4-5: Handle (LE16)
 * - Offset 6+:  UUID (2 bytes for 16-bit, 16 bytes for 128-bit)
 * - Offset 8/22+: Attribute value (variable length)
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <algorithm>

#include "attribute.hpp"
#include "bluetooth.h"  // For ATT_ERROR_* constants

namespace c7222 {
namespace {

// Binary parsing helpers
constexpr size_t kEntryHeaderSize = 6;  // Size + Flags + Handle
constexpr size_t kUuid16Size = 2;
constexpr size_t kUuid128Size = 16;
constexpr size_t kValue16Offset = kEntryHeaderSize + kUuid16Size;    // 8
constexpr size_t kValue128Offset = kEntryHeaderSize + kUuid128Size;  // 22

/**
 * @brief Read a little-endian 16-bit value from data.
 * @param data Pointer to 2-byte little-endian value
 * @return 16-bit value in host byte order
 */
uint16_t ReadLe16(const uint8_t* data) {
	return *(reinterpret_cast<const uint16_t*>(data));
}

/**
 * @brief Reverse byte order of a 128-bit UUID.
 * BTstack stores UUIDs in little-endian byte order; this reverses to big-endian.
 * @param data Pointer to 16-byte little-endian UUID
 * @return Byte-reversed array
 */
std::array<uint8_t, 16> ReverseUuid128(const uint8_t* data) {
	std::array<uint8_t, 16> out{};
	for(size_t i = 0; i < out.size(); ++i) {
		out[i] = data[out.size() - 1 - i];
	}
	return out;
}

/**
 * @brief Parse a single ATT database entry and construct an Attribute.
 * @param ptr Pointer to entry start
 * @param entry_size Size of entry in bytes
 * @return Parsed Attribute, or invalid if entry is malformed
 */
Attribute ParseEntry(const uint8_t* ptr, uint16_t entry_size) {
	// Entry layout:
	// 0-1: size (already consumed)
	// 2-3: flags (properties)
	// 4-5: handle
	// 6+:  UUID and value

	const uint16_t flags = ReadLe16(ptr + 2);
	const uint8_t* uuid_ptr = ptr + 6;

	// Check for 128-bit UUID
	if((flags & static_cast<uint16_t>(Attribute::Properties::kUuid128)) != 0) {
		// Minimum valid: header(6) + UUID(16) = 22 bytes
		if(entry_size < kValue128Offset) {
			return Attribute();  // Return invalid attribute
		}
		const uint8_t* value_ptr = ptr + kValue128Offset;
		const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue128Offset);
		const std::array<uint8_t, 16> uuid_bytes = ReverseUuid128(uuid_ptr);
		return Attribute(Uuid(uuid_bytes), flags, value_ptr, value_len);
	}

	// 16-bit UUID
	// Minimum valid: header(6) + UUID(2) = 8 bytes
	if(entry_size < kValue16Offset) {
		return Attribute();  // Return invalid attribute
	}
	const uint16_t uuid16 = ReadLe16(uuid_ptr);
	const uint8_t* value_ptr = ptr + kValue16Offset;
	const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue16Offset);
	return Attribute(Uuid(uuid16), flags, value_ptr, value_len);
}

}  // namespace

std::list<Attribute> ParseAttributesFromDb(const uint8_t* db) {
	std::list<Attribute> attributes;
	
	// Validate input
	if(db == nullptr) {
		return attributes;
	}

	// Skip ATT DB version byte (first byte indicates DB format version)
	const uint8_t* ptr = db + 1;

	// Iterate entries until terminator (size = 0)
	while(true) {
		// Read entry size (little-endian)
		const uint16_t entry_size = ReadLe16(ptr);

		// Size = 0 marks end of database
		if(entry_size == 0) {
			break;
		}

		// Validate minimum entry size (must contain header)
		if(entry_size < kEntryHeaderSize) {
			break;  // Malformed entry, stop parsing
		}

		// Parse and add entry (explicit move ensures no copies)
		attributes.emplace_back(std::move(ParseEntry(ptr, entry_size)));

		// Advance to next entry
		ptr += entry_size;
	}

	return attributes;
}

}  // namespace c7222
