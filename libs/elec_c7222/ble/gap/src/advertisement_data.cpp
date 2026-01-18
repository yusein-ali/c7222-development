#include "advertisement_data.hpp"

namespace c7222 {
bool AdvertisementData::ValidateLength(AdvertisementDataType type, size_t length) {
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

bool AdvertisementData::ValidateBuffer(const uint8_t* adv_data, size_t adv_data_size) {
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
		bool check = ValidateLength(type, length);
		if(!check) {
			return false;
		}
		index += length + 1;
	}
	// check whether we have some extra data at the end
	return (index == adv_data_size) ? true : false;
}

void AdvertisementData::Build(AdvertisementDataType type, const uint8_t* data, size_t size) {
	if(size > 0 && data == nullptr) {
		__builtin_trap();
	}

	assert(ValidateLength(type, size + 1) && "Invalid AdvertisementData size for given type");

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
}