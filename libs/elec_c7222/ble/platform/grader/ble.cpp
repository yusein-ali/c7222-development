#include "ble.hpp"

namespace c7222 {

void Ble::EnableHCILoggingToStdout() {
	hci_logging_enabled_ = true;
}

void Ble::DisableHCILoggingToStdout() {
	hci_logging_enabled_ = false;
}

void DumpAttributeServerContext() {
}

}  // namespace c7222
