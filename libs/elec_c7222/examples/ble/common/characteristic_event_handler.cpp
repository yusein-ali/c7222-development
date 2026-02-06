//
// Created by husey on 2/5/2026.
//

#include "characteristic_event_handler.hpp"

#include <cassert>
#include <cstdio>

void CharacteristicEventHandler::OnUpdatesEnabled(bool /*is_indication*/) {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log enablement so students can see CCCD effects.
	std::printf("[%s]: OnUpdatesEnabled\n", text.c_str());
}

void CharacteristicEventHandler::OnUpdatesDisabled() {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log disablement of updates.
	std::printf("[%s]: OnUpdatesDisabled\n", text.c_str());
}

void CharacteristicEventHandler::OnIndicationComplete(uint8_t /*status*/) {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log indication completion for debugging.
	std::printf("[%s]: OnIndicationComplete\n", text.c_str());
}

void CharacteristicEventHandler::OnBroadcastEnabled() {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log broadcast enablement.
	std::printf("[%s]: OnBroadcastEnabled\n", text.c_str());
}

void CharacteristicEventHandler::OnBroadcastDisabled() {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log broadcast disablement.
	std::printf("[%s]: OnBroadcastDisabled\n", text.c_str());
}

void CharacteristicEventHandler::OnRead() {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log read access for visibility in examples.
	std::printf("[%s]: OnRead\n", text.c_str());
}

void CharacteristicEventHandler::OnWrite(const std::vector<uint8_t>& data) {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log write event and dump payload bytes.
	std::printf("[%s]: OnWrite\n", text.c_str());
	for (size_t i = 0; i < data.size(); i += 16) {
		std::printf("  ");
		for (size_t j = i; j < i + 16 && j < data.size(); j++) {
			std::printf("%02x ", data[j]);
		}
		std::printf("\n");
	}
}

void CharacteristicEventHandler::OnConfirmationReceived(bool /*status*/) {
	// Use a friendly label if configured, otherwise fall back to a generic name.
	std::string text = characteristic_name_.empty() ? "Characteristic Event" : characteristic_name_;
	// Log confirmation to show indication acknowledgment.
	std::printf("[%s]: OnConfirmationReceived\n", text.c_str());
}
