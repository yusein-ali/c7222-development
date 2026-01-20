#include "attribute_server.hpp"

namespace c7222 {

BleError AttributeServer::Init(const void* context) {
	services_.clear();
	connection_handle_ = 0;
	initialized_ = false;

	if(context_ == nullptr) {
		context_ = context;
	}
	(void) context_;
	return BleError::kUnsupportedFeatureOrParameterValue;
}

}  // namespace c7222
