#include "advertisement_data.hpp"

#include <utility>
#include <algorithm>

namespace c7222 {
bool AdvertisementData::ValidateLength(AdvertisementDataType type, size_t length) {
	if(length == 0 || (length + 1) > kAdvertisementDataLegacyMaxSize) {
		return false;
	}

	const size_t data_size = length - 1;
	switch(type) {
		case AdvertisementDataType::kFlags:
		case AdvertisementDataType::kTxPowerLevel:
			if(data_size != 1) {
				return false;
			}
			break;
		case AdvertisementDataType::kSlaveConnectionIntervalRange:
			if(data_size != 5) {
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
			if(data_size < 3) {
				return false;
			}
		case AdvertisementDataType::kManufacturerSpecific:
		case AdvertisementDataType::kShortenedLocalName:
		case AdvertisementDataType::kCompleteLocalName:
			if(data_size < 1) {
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

std::vector<uint8_t> AdvertisementData::operator+=(const AdvertisementData& other) const {
	std::vector<uint8_t> new_data;
	new_data.reserve(data_.size() + other.GetSize());
	new_data.insert(new_data.end(), data_.cbegin(), data_.cend());
	new_data.insert(new_data.end(), other.GetData().cbegin(), other.GetData().cend());
	return new_data;
}

std::vector<uint8_t> operator+(const AdvertisementData& lhs, const AdvertisementData& rhs) {
	std::vector<uint8_t> combined;
	combined.reserve(lhs.GetSize() + rhs.GetSize());
	combined.insert(combined.end(), lhs.GetData().cbegin(), lhs.GetData().cend());
	combined.insert(combined.end(), rhs.GetData().cbegin(), rhs.GetData().cend());
	return combined;
}

AdvertisementDataBuilder::AdvertisementDataBuilder() {
	data_.reserve(kAdvertisementDataLegacyMaxSize);
	built_ = false;
}

AdvertisementDataBuilder::AdvertisementDataBuilder(const std::list<AdvertisementData>& ads) {
	built_ = false;
	Set(ads);
	Build();
}

bool AdvertisementDataBuilder::Set(const std::list<AdvertisementData>& ads) {
	Clear();
	advertisements_ = ads;
	return Build();
}

bool AdvertisementDataBuilder::Set(const uint8_t* data, size_t size) {
	if(data == data_.data()) {
		// avoid copying from self if already built and the data are equal
		if(built_) {
			// already built, the data is valid
			return true;
		}
	}
	const auto& ads = AdvertisementDataBuilder::DecodeBufferToAdvertisementDataList(data, size);
	Clear();
	return Set(ads);
}

bool AdvertisementDataBuilder::Pop() {
	if(advertisements_.empty()) {
		return false;
	}
	advertisements_.pop_back();
	built_ = false;
	return true;
}

bool AdvertisementDataBuilder::Push(const AdvertisementData& ad) {
	return Add(ad);
}

bool AdvertisementDataBuilder::Add(const std::list<AdvertisementData>& ads) {
	for(const auto& ad: ads) {
		bool ok = Add(ad);
		if(!ok) {
			return false;
		}
	}
	return true;
}
bool AdvertisementDataBuilder::Add(const AdvertisementData& ad) {
	assert(ad.ValidateLength(ad.GetType(), ad.GetSize()) &&
		   "AdvertisementData to add is not valid");
	auto it = std::find_if(advertisements_.cbegin(),
						   advertisements_.cend(),
						   [&ad](const AdvertisementData& existing_ad) {
							   return existing_ad.GetType() == ad.GetType();
						   });
	if(it != advertisements_.cend()) {
		// already exists, so we must not add
		return false;
	}
	// push to the end of the list
	advertisements_.push_back(ad);
	built_ = false;
	return true;
}

void AdvertisementDataBuilder::ReplaceOrAdd(const AdvertisementData& ad) {
	auto it = std::find_if(advertisements_.begin(),
						   advertisements_.end(),
						   [&ad](const AdvertisementData& existing_ad) {
							   return existing_ad.GetType() == ad.GetType();
						   });
	if(it != advertisements_.end()) {
		*it = ad;
	} else {
		advertisements_.push_back(ad);
	}
	built_ = false;
}

void AdvertisementDataBuilder::ReplaceOrAdd(AdvertisementData&& ad) {
	auto it = std::find_if(advertisements_.begin(),
						   advertisements_.end(),
						   [&ad](const AdvertisementData& existing_ad) {
							   return existing_ad.GetType() == ad.GetType();
						   });
	if(it != advertisements_.end()) {
		*it = std::move(ad);
	} else {
		advertisements_.push_back(std::move(ad));
	}
	built_ = false;
}

const std::vector<uint8_t>& AdvertisementDataBuilder::data() const {
	// return data if it is already built
	assert(built_ && "AdvertisementDataBuilder: data not built yet, call build() first!");
	return data_;
}

const uint8_t* AdvertisementDataBuilder::bytes() const {
	// return data if it is already built
	assert(built_ && "AdvertisementDataBuilder: data not built yet, call build() first!");
	return data_.data();
}

size_t AdvertisementDataBuilder::size() const {
	if(built_) {
		return data_.size();
	}
	size_t total = 0;
	for(const auto& ad: advertisements_) {
		total += ad.GetSize();
	}
	return total;
}

void AdvertisementDataBuilder::Clear() {
	advertisements_.clear();
	data_.clear();
	built_ = false;
}

AdvertisementDataBuilder& AdvertisementDataBuilder::operator+=(const AdvertisementData& ad) {
	Add(ad);
	return *this;
}

AdvertisementDataBuilder AdvertisementDataBuilder::operator+(const AdvertisementData& ad) {
	this->operator+=(ad);
	return *this;
}

AdvertisementDataBuilder& AdvertisementDataBuilder::operator+=(const AdvertisementDataBuilder& adb) {
	for(const auto& a: adb.advertisements_) {
		auto it = std::find_if(advertisements_.cbegin(),
							   advertisements_.cend(),
							   [&a](const AdvertisementData& existing_ad) {
								   return existing_ad.GetData() == a.GetData();
							   });
		if(it == advertisements_.cend()) {
			Add(a);
		} else {
			// already exists, so we must hard-assert
			assert(false && "AdvertisementData already exists in the builder");
		}
	}
	return *this;
}

AdvertisementDataBuilder AdvertisementDataBuilder::operator+(const AdvertisementDataBuilder& adb) {
	this->operator+=(adb);
	return *this;
}

bool AdvertisementDataBuilder::operator==(const AdvertisementDataBuilder& other) const {
	if(built_ && other.built_) {
		return data_ == other.data_;
	}
	return advertisements_ == other.advertisements_;
}

bool AdvertisementDataBuilder::Validate() const {
	for(const auto& ad: advertisements_) {
		if(!ad.ValidateLength(ad.GetType(), ad.GetLength())) {
			return false;
		}
	}
	if(size() > kAdvertisementDataLegacyMaxSize) {
		// exceeding the size limit
		return false;
	}
	return true;
}

bool AdvertisementDataBuilder::Build() {
	if(built_) {
		return Validate();
	}
	data_.clear();
	advertisements_.sort([](const AdvertisementData& a, const AdvertisementData& b) {
		return static_cast<uint8_t>(a.GetType()) < static_cast<uint8_t>(b.GetType());
	});
	for(auto& ad: advertisements_) {
		const auto& ad_data = ad.GetData();
		data_.insert(data_.end(), ad_data.cbegin(), ad_data.cend());
	}
	built_ = true;
	return Validate();
}

std::list<AdvertisementData>
AdvertisementDataBuilder::DecodeBufferToAdvertisementDataList(const uint8_t* adv_data,
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
}  // namespace c7222
