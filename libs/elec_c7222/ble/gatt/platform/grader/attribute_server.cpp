#include "attribute_server.hpp"

namespace c7222 {

BleError AttributeServer::InitPlatform(const uint8_t* att_db) {
	(void) att_db;
	return BleError::kUnsupportedFeatureOrParameterValue;
}

}  // namespace c7222
