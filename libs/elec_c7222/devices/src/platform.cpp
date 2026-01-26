// Grader platform implementation (no hardware init).
#include "platform.hpp"
#include <cassert>

namespace c7222 {

	Platform* Platform::instance_{nullptr};

Platform* Platform::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new Platform();
		assert(instance_ != nullptr && "Failed to allocate Platform singleton instance");
	}
	return instance_;
}

Platform::Platform(): initialized_(false), arch_initialized_(false) {

}


} // namespace c7222
