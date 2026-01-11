#include "gap.hpp"

#include <algorithm>

#include <btstack.h>
#include "gap_maps.hpp"

#if !defined(HCI_OPCODE_HCI_LE_SET_ADVERTISING_ENABLE) && defined(HCI_OPCODE_HCI_LE_SET_ADVERTISE_ENABLE)
#define HCI_OPCODE_HCI_LE_SET_ADVERTISING_ENABLE HCI_OPCODE_HCI_LE_SET_ADVERTISE_ENABLE
#endif

namespace c7222 {
namespace {

constexpr size_t kLegacyAdvertisingDataMaxSize = 31;

} // namespace

using namespace btstack_map;

void Gap::setRandomAddress(const BleAddress& address) {
	random_address_ = address;
	random_address_set_ = true;

	bd_addr_t addr{};
	address.copy_to(addr);
	gap_random_address_set(addr);
}

void Gap::setAdvertisingParameters(const AdvertisementParameters& params) {
	advertising_params_ = params;
	advertising_params_set_ = true;

	const uint8_t adv_type = btstack_map::to_btstack(params.advertising_type);
	const uint8_t direct_type = btstack_map::to_btstack(params.direct_address_type);
	bd_addr_t direct_address{};
	params.direct_address.copy_to(direct_address);
	const uint8_t channel_map = btstack_map::to_btstack_advertising_channel_map(params.channel_map);
	const uint8_t filter_policy = btstack_map::to_btstack(params.filter_policy);

	gap_advertisements_set_params(params.min_interval,
								  params.max_interval,
								  adv_type,
								  direct_type,
								  direct_address,
								  channel_map,
								  filter_policy);
}

void Gap::setAdvertisingData(uint8_t length, const uint8_t* data) {
	const bool was_advertising = isAdvertisingEnabled();
	if(was_advertising) {
		stopAdvertising();
	}

	advertising_data_set_ = true;
	advertising_data_.clear();

	if(data != nullptr && length > 0) {
		const size_t copy_len = std::min<size_t>(length, kLegacyAdvertisingDataMaxSize);
		advertising_data_.assign(data, data + copy_len);
	}

	auto* payload = advertising_data_.empty() ? nullptr : advertising_data_.data();
	gap_advertisements_set_data(static_cast<uint8_t>(advertising_data_.size()), const_cast<uint8_t*>(payload));

	if(was_advertising) {
		startAdvertising();
	}
}

void Gap::setScanResponseData(uint8_t length, const uint8_t* data) {
	const bool was_advertising = isAdvertisingEnabled();
	if (was_advertising) {
		stopAdvertising();
	}

	scan_response_data_set_ = true;
	scan_response_data_.clear();

	if (data != nullptr && length > 0) {
		const size_t copy_len = std::min<size_t>(length, kLegacyAdvertisingDataMaxSize);
		scan_response_data_.assign(data, data + copy_len);
	}

	auto* payload = scan_response_data_.empty() ? nullptr : scan_response_data_.data();
	gap_scan_response_set_data(static_cast<uint8_t>(scan_response_data_.size()),
							   const_cast<uint8_t*>(payload));

	if (was_advertising) {
		startAdvertising();
	}
}

void Gap::enableAdvertising(bool enabled) {
	advertisement_enabled_ = enabled;
	gap_advertisements_enable(enabled ? 1 : 0);
}

void Gap::startAdvertising() {
	enableAdvertising(true);
}

void Gap::stopAdvertising() {
	enableAdvertising(false);
}

BleError Gap::requestConnectionParameterUpdate(ConnectionHandle con_handle,
											   const PreferredConnectionParameters& params) {
	return map_btstack_status(gap_request_connection_parameter_update(con_handle,
																	  params.min_interval,
																	  params.max_interval,
																	  params.slave_latency,
																	  params.supervision_timeout));
}

BleError Gap::updateConnectionParameters(ConnectionHandle con_handle, const PreferredConnectionParameters& params) {
	return map_btstack_status(gap_update_connection_parameters(con_handle,
															   params.min_interval,
															   params.max_interval,
															   params.slave_latency,
															   params.supervision_timeout));
}

BleError Gap::readRssi(ConnectionHandle con_handle) {
	return map_btstack_status(gap_read_rssi(con_handle));
}

BleError Gap::disconnect(ConnectionHandle con_handle) {
	return map_btstack_status(gap_disconnect(con_handle));
}

void Gap::getLocalAddress(BleAddress& address) {
	uint8_t addr_type = BD_ADDR_TYPE_UNKNOWN;
	bd_addr_t addr{};
	gap_le_get_own_address(&addr_type, addr);
	address = BleAddress(map_address_type(addr_type), addr);
}

void Gap::addEventHandler(const EventHandler& handler) {
	event_handlers_.push_back(&handler);
}

BleError Gap::dispatch_ble_hci_packet(uint8_t packet_type, const uint8_t* packet_data, uint16_t packet_data_size) {
	if(packet_type != HCI_EVENT_PACKET || packet_data == nullptr || packet_data_size == 0) {
		return BleError::Success;
	}

	const uint8_t event_code = hci_event_packet_get_type(packet_data);
	uint8_t subevent_code = 0x00;
	if(event_code == HCI_EVENT_LE_META) {
		subevent_code = hci_event_le_meta_get_subevent_code(packet_data);
	}

	EventId event_id;
	if(!btstack_map::from_btstack_event(event_code, subevent_code, event_id)) {
		return BleError::Success;
	}

	return dispatch_event(event_id, packet_data, packet_data_size);
}

BleError Gap::dispatch_event(EventId event_id, const uint8_t* event_data, uint16_t event_data_size) {
	switch(event_id) {
	case EventId::SecurityLevel: {
		const auto con_handle = static_cast<ConnectionHandle>(gap_event_security_level_get_handle(event_data));
		const uint8_t security_level = gap_event_security_level_get_security_level(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onSecurityLevel(con_handle, security_level);
		}
		break;
	}
	case EventId::DedicatedBondingCompleted: {
		const uint8_t status = gap_event_dedicated_bonding_completed_get_status(event_data);
		bd_addr_t addr{};
		gap_event_dedicated_bonding_completed_get_address(event_data, addr);
		const BleAddress address = make_unknown_address(addr);
		for(const auto* handler: event_handlers_) {
			handler->onDedicatedBondingCompleted(status, address);
		}
		break;
	}
	case EventId::AdvertisingReport: {
		AdvertisingReport report{};
		const uint8_t event_type = gap_event_advertising_report_get_advertising_event_type(event_data);
		report.advertising_event_type = map_legacy_advertising_event_type(event_type);
		const uint8_t addr_type = gap_event_advertising_report_get_address_type(event_data);
		bd_addr_t addr{};
		gap_event_advertising_report_get_address(event_data, addr);
		report.address = make_address(addr_type, addr);
		report.rssi = static_cast<int8_t>(gap_event_advertising_report_get_rssi(event_data));
		report.data_length = gap_event_advertising_report_get_data_length(event_data);
		report.data = gap_event_advertising_report_get_data(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onAdvertisingReport(report);
		}
		break;
	}
	case EventId::ExtendedAdvertisingReport: {
		ExtendedAdvertisingReport report{};
		const uint16_t event_type = gap_event_extended_advertising_report_get_advertising_event_type(event_data);
		report.advertising_event_type = btstack_map::from_btstack_advertising_event_type(event_type);
		const uint8_t addr_type = gap_event_extended_advertising_report_get_address_type(event_data);
		bd_addr_t addr{};
		gap_event_extended_advertising_report_get_address(event_data, addr);
		report.address = make_address(addr_type, addr);
		report.primary_phy = map_phy(gap_event_extended_advertising_report_get_primary_phy(event_data));
		report.secondary_phy = map_phy(gap_event_extended_advertising_report_get_secondary_phy(event_data));
		report.advertising_sid = gap_event_extended_advertising_report_get_advertising_sid(event_data);
		report.tx_power = static_cast<int8_t>(gap_event_extended_advertising_report_get_tx_power(event_data));
		report.rssi = static_cast<int8_t>(gap_event_extended_advertising_report_get_rssi(event_data));
		report.periodic_advertising_interval =
			gap_event_extended_advertising_report_get_periodic_advertising_interval(event_data);
		const uint8_t direct_addr_type = gap_event_extended_advertising_report_get_direct_address_type(event_data);
		bd_addr_t direct_addr{};
		gap_event_extended_advertising_report_get_direct_address(event_data, direct_addr);
		report.direct_address = make_address(direct_addr_type, direct_addr);
		report.data_length = gap_event_extended_advertising_report_get_data_length(event_data);
		report.data = gap_event_extended_advertising_report_get_data(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onExtendedAdvertisingReport(report);
		}
		break;
	}
	case EventId::InquiryResult: {
		InquiryResult result{};
		bd_addr_t addr{};
		gap_event_inquiry_result_get_bd_addr(event_data, addr);
		result.address = make_unknown_address(addr);
		result.page_scan_repetition_mode = gap_event_inquiry_result_get_page_scan_repetition_mode(event_data);
		result.class_of_device = gap_event_inquiry_result_get_class_of_device(event_data);
		result.clock_offset = gap_event_inquiry_result_get_clock_offset(event_data);
		result.rssi_available = gap_event_inquiry_result_get_rssi_available(event_data) != 0;
		result.rssi = static_cast<int8_t>(gap_event_inquiry_result_get_rssi(event_data));
		result.device_id_available = gap_event_inquiry_result_get_device_id_available(event_data) != 0;
		result.device_id_vendor_id_source = gap_event_inquiry_result_get_device_id_vendor_id_source(event_data);
		result.device_id_vendor_id = gap_event_inquiry_result_get_device_id_vendor_id(event_data);
		result.device_id_product_id = gap_event_inquiry_result_get_device_id_product_id(event_data);
		result.device_id_version = gap_event_inquiry_result_get_device_id_version(event_data);
		result.name_available = gap_event_inquiry_result_get_name_available(event_data) != 0;
		result.name_len = gap_event_inquiry_result_get_name_len(event_data);
		result.name = gap_event_inquiry_result_get_name(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onInquiryResult(result);
		}
		break;
	}
	case EventId::InquiryComplete: {
		const uint8_t status = gap_event_inquiry_complete_get_status(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onInquiryComplete(status);
		}
		break;
	}
	case EventId::RssiMeasurement: {
		const auto con_handle = static_cast<ConnectionHandle>(gap_event_rssi_measurement_get_con_handle(event_data));
		const int8_t rssi = static_cast<int8_t>(gap_event_rssi_measurement_get_rssi(event_data));
		for(const auto* handler: event_handlers_) {
			handler->onRssiMeasurement(con_handle, rssi);
		}
		break;
	}
	case EventId::LocalOobData: {
		const bool present = gap_event_local_oob_data_get_oob_data_present(event_data) != 0;
		uint8_t c_192[16]{};
		uint8_t r_192[16]{};
		uint8_t c_256[16]{};
		uint8_t r_256[16]{};
		gap_event_local_oob_data_get_c_192(event_data, c_192);
		gap_event_local_oob_data_get_r_192(event_data, r_192);
		gap_event_local_oob_data_get_c_256(event_data, c_256);
		gap_event_local_oob_data_get_r_256(event_data, r_256);
		for(const auto* handler: event_handlers_) {
			handler->onLocalOobData(present, c_192, r_192, c_256, r_256);
		}
		break;
	}
	case EventId::PairingStarted: {
		const auto con_handle = static_cast<ConnectionHandle>(gap_event_pairing_started_get_con_handle(event_data));
		bd_addr_t addr{};
		gap_event_pairing_started_get_bd_addr(event_data, addr);
		const BleAddress address = make_unknown_address(addr);
		const bool ssp = gap_event_pairing_started_get_ssp(event_data) != 0;
		const bool initiator = gap_event_pairing_started_get_initiator(event_data) != 0;
		for(const auto* handler: event_handlers_) {
			handler->onPairingStarted(con_handle, address, ssp, initiator);
		}
		break;
	}
	case EventId::PairingComplete: {
		const auto con_handle = static_cast<ConnectionHandle>(gap_event_pairing_complete_get_con_handle(event_data));
		bd_addr_t addr{};
		gap_event_pairing_complete_get_bd_addr(event_data, addr);
		const BleAddress address = make_unknown_address(addr);
		const uint8_t status = gap_event_pairing_complete_get_status(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onPairingComplete(con_handle, address, status);
		}
		break;
	}
	case EventId::DisconnectionComplete: {
		const uint8_t status = hci_event_disconnection_complete_get_status(event_data);
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_event_disconnection_complete_get_connection_handle(event_data));
		const uint8_t reason = hci_event_disconnection_complete_get_reason(event_data);
		connection_parameters_.erase(con_handle);
		connected_ = !connection_parameters_.empty();
		for(const auto* handler: event_handlers_) {
			handler->onDisconnectionComplete(status, con_handle, reason);
		}
		break;
	}
	case EventId::CommandComplete: {
		const uint16_t opcode = hci_event_command_complete_get_command_opcode(event_data);
		const uint8_t* return_params = hci_event_command_complete_get_return_parameters(event_data);
		const uint8_t status = return_params != nullptr ? return_params[0] : ERROR_CODE_UNSPECIFIED_ERROR;

		if(opcode == HCI_OPCODE_HCI_LE_SET_ADVERTISE_ENABLE ||
		   opcode == HCI_OPCODE_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE) {
			if(advertisement_enabled_) {
				if(status != ERROR_CODE_SUCCESS) {
					advertisement_enabled_ = false;
				}
				for(const auto* handler: event_handlers_) {
					handler->onAdvertisingStart(status);
				}
			} else {
				for(const auto* handler: event_handlers_) {
					handler->onAdvertisingEnd(status, 0);
				}
			}
			break;
		}

		if(opcode == HCI_OPCODE_HCI_LE_READ_PHY && return_params != nullptr) {
			const uint8_t param_len = event_data_size > 1 ? event_data[1] : 0;
			if(param_len >= 8) {
				const auto con_handle = static_cast<ConnectionHandle>(little_endian_read_16(return_params, 1));
				const Gap::Phy tx_phy = map_phy(return_params[3]);
				const Gap::Phy rx_phy = map_phy(return_params[4]);
				for(const auto* handler: event_handlers_) {
					handler->onReadPhy(status, con_handle, tx_phy, rx_phy);
				}
			}
		}
		break;
	}
	case EventId::LeScanRequestReceived: {
		const uint8_t adv_handle = hci_subevent_le_scan_request_received_get_advertising_handle(event_data);
		const uint8_t addr_type = hci_subevent_le_scan_request_received_get_scanner_address_type(event_data);
		bd_addr_t addr{};
		hci_subevent_le_scan_request_received_get_scanner_address(event_data, addr);
		const BleAddress address = make_address(addr_type, addr);
		for(const auto* handler: event_handlers_) {
			handler->onScanRequestReceived(adv_handle, address);
		}
		break;
	}
	case EventId::LeScanTimeout: {
		const uint8_t status = event_data_size > 3 ? event_data[3] : ERROR_CODE_UNSPECIFIED_ERROR;
		for(const auto* handler: event_handlers_) {
			handler->onScanTimeout(status);
		}
		break;
	}
	case EventId::LePeriodicAdvertisingSyncEstablished: {
		const uint8_t status = hci_subevent_le_periodic_advertising_sync_establishment_get_status(event_data);
		const auto sync_handle = static_cast<ConnectionHandle>(
			hci_subevent_le_periodic_advertising_sync_establishment_get_sync_handle(event_data));
		for(const auto* handler: event_handlers_) {
			handler->onPeriodicAdvertisingSyncEstablished(status, sync_handle);
		}
		break;
	}
	case EventId::LePeriodicAdvertisingReport: {
		const auto sync_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_periodic_advertising_report_get_sync_handle(event_data));
		const int8_t tx_power =
			static_cast<int8_t>(hci_subevent_le_periodic_advertising_report_get_tx_power(event_data));
		const int8_t rssi = static_cast<int8_t>(hci_subevent_le_periodic_advertising_report_get_rssi(event_data));
		const uint8_t data_status = hci_subevent_le_periodic_advertising_report_get_data_status(event_data);
		const uint8_t data_length = hci_subevent_le_periodic_advertising_report_get_data_length(event_data);
		const uint8_t* data = hci_subevent_le_periodic_advertising_report_get_data(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onPeriodicAdvertisingReport(sync_handle, tx_power, rssi, data_status, data, data_length);
		}
		break;
	}
	case EventId::LePeriodicAdvertisingSyncLost: {
		const auto sync_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_periodic_advertising_sync_lost_get_sync_handle(event_data));
		for(const auto* handler: event_handlers_) {
			handler->onPeriodicAdvertisingSyncLoss(sync_handle);
		}
		break;
	}
	case EventId::LeConnectionComplete: {
		const uint8_t status = hci_subevent_le_connection_complete_get_status(event_data);
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_connection_complete_get_connection_handle(event_data));
		const uint8_t addr_type = hci_subevent_le_connection_complete_get_peer_address_type(event_data);
		bd_addr_t addr{};
		hci_subevent_le_connection_complete_get_peer_address(event_data, addr);
		const BleAddress address = make_address(addr_type, addr);
		const uint16_t conn_interval = hci_subevent_le_connection_complete_get_conn_interval(event_data);
		const uint16_t conn_latency = hci_subevent_le_connection_complete_get_conn_latency(event_data);
		const uint16_t supervision_timeout = hci_subevent_le_connection_complete_get_supervision_timeout(event_data);

		if(status == ERROR_CODE_SUCCESS) {
			connection_parameters_[con_handle] = {conn_interval, conn_latency, supervision_timeout};
			connected_ = true;
			if(advertisement_enabled_) {
				advertisement_enabled_ = false;
				for(const auto* handler: event_handlers_) {
					handler->onAdvertisingEnd(status, con_handle);
				}
			}
		}

		for(const auto* handler: event_handlers_) {
			handler
				->onConnectionComplete(status, con_handle, address, conn_interval, conn_latency, supervision_timeout);
		}
		break;
	}
	case EventId::LeEnhancedConnectionComplete: {
#if defined(HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V1)
		const uint8_t status = hci_subevent_le_enhanced_connection_complete_v1_get_status(event_data);
		const auto con_handle = static_cast<ConnectionHandle>(
			hci_subevent_le_enhanced_connection_complete_v1_get_connection_handle(event_data));
		const uint8_t addr_type = hci_subevent_le_enhanced_connection_complete_v1_get_peer_address_type(event_data);
		bd_addr_t addr{};
		hci_subevent_le_enhanced_connection_complete_v1_get_peer_addresss(event_data, addr);
		const BleAddress address = make_address(addr_type, addr);
		const uint16_t conn_interval = hci_subevent_le_enhanced_connection_complete_v1_get_conn_interval(event_data);
		const uint16_t conn_latency = hci_subevent_le_enhanced_connection_complete_v1_get_conn_latency(event_data);
		const uint16_t supervision_timeout =
			hci_subevent_le_enhanced_connection_complete_v1_get_supervision_timeout(event_data);

		if(status == ERROR_CODE_SUCCESS) {
			connection_parameters_[con_handle] = {conn_interval, conn_latency, supervision_timeout};
			connected_ = true;
			if(advertisement_enabled_) {
				advertisement_enabled_ = false;
				for(const auto* handler: event_handlers_) {
					handler->onAdvertisingEnd(status, con_handle);
				}
			}
		}

		for(const auto* handler: event_handlers_) {
			handler
				->onConnectionComplete(status, con_handle, address, conn_interval, conn_latency, supervision_timeout);
		}
#endif
		break;
	}
	case EventId::LeRemoteConnectionParameterRequest: {
		const auto con_handle = static_cast<ConnectionHandle>(
			hci_subevent_le_remote_connection_parameter_request_get_connection_handle(event_data));
		const uint16_t min_interval = hci_subevent_le_remote_connection_parameter_request_get_interval_min(event_data);
		const uint16_t max_interval = hci_subevent_le_remote_connection_parameter_request_get_interval_max(event_data);
		const uint16_t latency = hci_subevent_le_remote_connection_parameter_request_get_latency(event_data);
		const uint16_t timeout = hci_subevent_le_remote_connection_parameter_request_get_timeout(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onUpdateConnectionParametersRequest(con_handle, min_interval, max_interval, latency, timeout);
		}
		break;
	}
	case EventId::LeConnectionUpdateComplete: {
		const uint8_t status = hci_subevent_le_connection_update_complete_get_status(event_data);
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_connection_update_complete_get_connection_handle(event_data));
		const uint16_t conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(event_data);
		const uint16_t conn_latency = hci_subevent_le_connection_update_complete_get_conn_latency(event_data);
		const uint16_t supervision_timeout =
			hci_subevent_le_connection_update_complete_get_supervision_timeout(event_data);

		if(status == ERROR_CODE_SUCCESS) {
			connection_parameters_[con_handle] = {conn_interval, conn_latency, supervision_timeout};
		}

		for(const auto* handler: event_handlers_) {
			handler->onConnectionParametersUpdateComplete(status,
														  con_handle,
														  conn_interval,
														  conn_latency,
														  supervision_timeout);
		}
		break;
	}
	case EventId::LePhyUpdateComplete: {
		const uint8_t status = hci_subevent_le_phy_update_complete_get_status(event_data);
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_phy_update_complete_get_connection_handle(event_data));
		const Gap::Phy tx_phy = map_phy(hci_subevent_le_phy_update_complete_get_tx_phy(event_data));
		const uint8_t rx_phy_raw = event_data_size > 7 ? event_data[7] : 0x00;
		const Gap::Phy rx_phy = map_phy(rx_phy_raw);
		for(const auto* handler: event_handlers_) {
			handler->onPhyUpdateComplete(status, con_handle, tx_phy, rx_phy);
		}
		break;
	}
	case EventId::LeDataLengthChange: {
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_data_length_change_get_connection_handle(event_data));
		const uint16_t tx_size = hci_subevent_le_data_length_change_get_max_tx_octets(event_data);
		const uint16_t rx_size = hci_subevent_le_data_length_change_get_max_rx_octets(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onDataLengthChange(con_handle, tx_size, rx_size);
		}
		break;
	}
	case EventId::LeAdvertisingSetTerminated: {
		const uint8_t status = hci_subevent_le_advertising_set_terminated_get_status(event_data);
		const auto con_handle =
			static_cast<ConnectionHandle>(hci_subevent_le_advertising_set_terminated_get_connection_handle(event_data));
		advertisement_enabled_ = false;
		for(const auto* handler: event_handlers_) {
			handler->onAdvertisingEnd(status, con_handle);
		}
		break;
	}
	case EventId::L2capConnectionParameterUpdateRequest: {
		const auto con_handle =
			static_cast<ConnectionHandle>(l2cap_event_connection_parameter_update_request_get_handle(event_data));
		const uint16_t min_interval = l2cap_event_connection_parameter_update_request_get_interval_min(event_data);
		const uint16_t max_interval = l2cap_event_connection_parameter_update_request_get_interval_max(event_data);
		const uint16_t latency = l2cap_event_connection_parameter_update_request_get_latency(event_data);
		const uint16_t timeout = l2cap_event_connection_parameter_update_request_get_timeout_multiplier(event_data);
		for(const auto* handler: event_handlers_) {
			handler->onUpdateConnectionParametersRequest(con_handle, min_interval, max_interval, latency, timeout);
		}
		break;
	}
	case EventId::PrivacyEnabled: {
		for(const auto* handler: event_handlers_) {
			handler->onPrivacyEnabled();
		}
		break;
	}
	}

	return BleError::Success;
}

} // namespace c7222
