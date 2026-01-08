#include "ble_error.hpp"

#include <bluetooth.h>

#include <cstddef>

namespace c7222::btstack_map {
namespace {

struct BleErrorMapEntry {
	BleError error;
	uint8_t btstack;
};

constexpr BleErrorMapEntry kBleErrorMap[] = {
	{BleError::ErrorCodeSuccess, ERROR_CODE_SUCCESS},
	{BleError::ErrorCodeUnknownHciCommand, ERROR_CODE_UNKNOWN_HCI_COMMAND},
	{BleError::ErrorCodeUnknownConnectionIdentifier, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER},
	{BleError::ErrorCodeHardwareFailure, ERROR_CODE_HARDWARE_FAILURE},
	{BleError::ErrorCodePageTimeout, ERROR_CODE_PAGE_TIMEOUT},
	{BleError::ErrorCodeAuthenticationFailure, ERROR_CODE_AUTHENTICATION_FAILURE},
	{BleError::ErrorCodePinOrKeyMissing, ERROR_CODE_PIN_OR_KEY_MISSING},
	{BleError::ErrorCodeMemoryCapacityExceeded, ERROR_CODE_MEMORY_CAPACITY_EXCEEDED},
	{BleError::ErrorCodeConnectionTimeout, ERROR_CODE_CONNECTION_TIMEOUT},
	{BleError::ErrorCodeConnectionLimitExceeded, ERROR_CODE_CONNECTION_LIMIT_EXCEEDED},
	{BleError::ErrorCodeSynchronousConnectionLimitToADeviceExceeded,
	 ERROR_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED},
	{BleError::ErrorCodeAclConnectionAlreadyExists, ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS},
	{BleError::ErrorCodeCommandDisallowed, ERROR_CODE_COMMAND_DISALLOWED},
	{BleError::ErrorCodeConnectionRejectedDueToLimitedResources, ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES},
	{BleError::ErrorCodeConnectionRejectedDueToSecurityReasons, ERROR_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS},
	{BleError::ErrorCodeConnectionRejectedDueToUnacceptableBdAddr,
	 ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR},
	{BleError::ErrorCodeUnsupportedFeatureOrParameterValue, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE},
	{BleError::ErrorCodeInvalidHciCommandParameters, ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS},
	{BleError::ErrorCodeRemoteUserTerminatedConnection, ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION},
	{BleError::ErrorCodeConnectionAcceptTimeoutExceeded, ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED},
	{BleError::ErrorCodeRemoteDeviceTerminatedConnectionDueToLowResources,
	 ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES},
	{BleError::ErrorCodeRemoteDeviceTerminatedConnectionDueToPowerOff,
	 ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF},
	{BleError::ErrorCodeConnectionTerminatedByLocalHost, ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST},
	{BleError::ErrorCodeRepeatedAttempts, ERROR_CODE_REPEATED_ATTEMPTS},
	{BleError::ErrorCodePairingNotAllowed, ERROR_CODE_PAIRING_NOT_ALLOWED},
	{BleError::ErrorCodeUnknownLmpPdu, ERROR_CODE_UNKNOWN_LMP_PDU},
	{BleError::ErrorCodeUnsupportedRemoteFeatureUnsupportedLmpFeature,
	 ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE},
	{BleError::ErrorCodeScoOffsetRejected, ERROR_CODE_SCO_OFFSET_REJECTED},
	{BleError::ErrorCodeScoIntervalRejected, ERROR_CODE_SCO_INTERVAL_REJECTED},
	{BleError::ErrorCodeScoAirModeRejected, ERROR_CODE_SCO_AIR_MODE_REJECTED},
	{BleError::ErrorCodeInvalidLmpParametersInvalidLlParameters, ERROR_CODE_INVALID_LMP_PARAMETERS_INVALID_LL_PARAMETERS},
	{BleError::ErrorCodeUnspecifiedError, ERROR_CODE_UNSPECIFIED_ERROR},
	{BleError::ErrorCodeUnsupportedLmpParameterValueUnsupportedLlParameterValue,
	 ERROR_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE_UNSUPPORTED_LL_PARAMETER_VALUE},
	{BleError::ErrorCodeRoleChangeNotAllowed, ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED},
	{BleError::ErrorCodeLmpResponseTimeoutLlResponseTimeout, ERROR_CODE_LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT},
	{BleError::ErrorCodeLmpErrorTransactionCollision, ERROR_CODE_LMP_ERROR_TRANSACTION_COLLISION},
	{BleError::ErrorCodeLmpPduNotAllowed, ERROR_CODE_LMP_PDU_NOT_ALLOWED},
	{BleError::ErrorCodeEncryptionModeNotAcceptable, ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE},
	{BleError::ErrorCodeLinkKeyCannotBeChanged, ERROR_CODE_LINK_KEY_CANNOT_BE_CHANGED},
	{BleError::ErrorCodeRequestedQosNotSupported, ERROR_CODE_REQUESTED_QOS_NOT_SUPPORTED},
	{BleError::ErrorCodeInstantPassed, ERROR_CODE_INSTANT_PASSED},
	{BleError::ErrorCodePairingWithUnitKeyNotSupported, ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED},
	{BleError::ErrorCodeDifferentTransactionCollision, ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION},
	{BleError::ErrorCodeReserved, ERROR_CODE_RESERVED},
	{BleError::ErrorCodeQosUnacceptableParameter, ERROR_CODE_QOS_UNACCEPTABLE_PARAMETER},
	{BleError::ErrorCodeQosRejected, ERROR_CODE_QOS_REJECTED},
	{BleError::ErrorCodeChannelClassificationNotSupported, ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED},
	{BleError::ErrorCodeInsufficientSecurity, ERROR_CODE_INSUFFICIENT_SECURITY},
	{BleError::ErrorCodeParameterOutOfMandatoryRange, ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE},
	{BleError::ErrorCodeRoleSwitchPending, ERROR_CODE_ROLE_SWITCH_PENDING},
	{BleError::ErrorCodeReservedSlotViolation, ERROR_CODE_RESERVED_SLOT_VIOLATION},
	{BleError::ErrorCodeRoleSwitchFailed, ERROR_CODE_ROLE_SWITCH_FAILED},
	{BleError::ErrorCodeExtendedInquiryResponseTooLarge, ERROR_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE},
	{BleError::ErrorCodeSecureSimplePairingNotSupportedByHost,
	 ERROR_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST},
	{BleError::ErrorCodeHostBusyPairing, ERROR_CODE_HOST_BUSY_PAIRING},
	{BleError::ErrorCodeConnectionRejectedDueToNoSuitableChannelFound,
	 ERROR_CODE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND},
	{BleError::ErrorCodeControllerBusy, ERROR_CODE_CONTROLLER_BUSY},
	{BleError::ErrorCodeUnacceptableConnectionParameters, ERROR_CODE_UNACCEPTABLE_CONNECTION_PARAMETERS},
	{BleError::ErrorCodeDirectedAdvertisingTimeout, ERROR_CODE_DIRECTED_ADVERTISING_TIMEOUT},
	{BleError::ErrorCodeConnectionTerminatedDueToMicFailure, ERROR_CODE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE},
	{BleError::ErrorCodeConnectionFailedToBeEstablished, ERROR_CODE_CONNECTION_FAILED_TO_BE_ESTABLISHED},
	{BleError::ErrorCodeMacConnectionFailed, ERROR_CODE_MAC_CONNECTION_FAILED},
	{BleError::ErrorCodeCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging,
	 ERROR_CODE_COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING},

	{BleError::BtstackConnectionToBtdaemonFailed, BTSTACK_CONNECTION_TO_BTDAEMON_FAILED},
	{BleError::BtstackActivationFailedSystemBluetooth, BTSTACK_ACTIVATION_FAILED_SYSTEM_BLUETOOTH},
	{BleError::BtstackActivationPoweronFailed, BTSTACK_ACTIVATION_POWERON_FAILED},
	{BleError::BtstackActivationFailedUnknown, BTSTACK_ACTIVATION_FAILED_UNKNOWN},
	{BleError::BtstackNotActivated, BTSTACK_NOT_ACTIVATED},
	{BleError::BtstackBusy, BTSTACK_BUSY},
	{BleError::BtstackMemoryAllocFailed, BTSTACK_MEMORY_ALLOC_FAILED},
	{BleError::BtstackAclBuffersFull, BTSTACK_ACL_BUFFERS_FULL},

	{BleError::L2capCommandRejectReasonCommandNotUnderstood, L2CAP_COMMAND_REJECT_REASON_COMMAND_NOT_UNDERSTOOD},
	{BleError::L2capCommandRejectReasonSignalingMtuExceeded, L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED},
	{BleError::L2capCommandRejectReasonInvalidCidInRequest, L2CAP_COMMAND_REJECT_REASON_INVALID_CID_IN_REQUEST},
	{BleError::L2capConnectionResponseResultSuccessful, L2CAP_CONNECTION_RESPONSE_RESULT_SUCCESSFUL},
	{BleError::L2capConnectionResponseResultPending, L2CAP_CONNECTION_RESPONSE_RESULT_PENDING},
	{BleError::L2capConnectionResponseResultRefusedPsm, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_PSM},
	{BleError::L2capConnectionResponseResultRefusedSecurity, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_SECURITY},
	{BleError::L2capConnectionResponseResultRefusedResources, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_RESOURCES},
	{BleError::L2capConnectionResponseResultErtmNotSupported, L2CAP_CONNECTION_RESPONSE_RESULT_ERTM_NOT_SUPPORTED},
	{BleError::L2capConnectionResponseResultRtxTimeout, L2CAP_CONNECTION_RESPONSE_RESULT_RTX_TIMEOUT},
	{BleError::L2capConnectionBasebandDisconnect, L2CAP_CONNECTION_BASEBAND_DISCONNECT},
	{BleError::L2capServiceAlreadyRegistered, L2CAP_SERVICE_ALREADY_REGISTERED},
	{BleError::L2capDataLenExceedsRemoteMtu, L2CAP_DATA_LEN_EXCEEDS_REMOTE_MTU},
	{BleError::L2capServiceDoesNotExist, L2CAP_SERVICE_DOES_NOT_EXIST},
	{BleError::L2capLocalCidDoesNotExist, L2CAP_LOCAL_CID_DOES_NOT_EXIST},
	{BleError::L2capConnectionResponseUnknownError, L2CAP_CONNECTION_RESPONSE_UNKNOWN_ERROR},

	{BleError::RfcommMultiplexerStopped, RFCOMM_MULTIPLEXER_STOPPED},
	{BleError::RfcommChannelAlreadyRegistered, RFCOMM_CHANNEL_ALREADY_REGISTERED},
	{BleError::RfcommNoOutgoingCredits, RFCOMM_NO_OUTGOING_CREDITS},
	{BleError::RfcommAggregateFlowOff, RFCOMM_AGGREGATE_FLOW_OFF},
	{BleError::RfcommDataLenExceedsMtu, RFCOMM_DATA_LEN_EXCEEDS_MTU},

	{BleError::HfpRemoteRejectsAudioConnection, HFP_REMOTE_REJECTS_AUDIO_CONNECTION},

	{BleError::SdpHandleAlreadyRegistered, SDP_HANDLE_ALREADY_REGISTERED},
	{BleError::SdpQueryIncomplete, SDP_QUERY_INCOMPLETE},
	{BleError::SdpServiceNotFound, SDP_SERVICE_NOT_FOUND},
	{BleError::SdpHandleInvalid, SDP_HANDLE_INVALID},
	{BleError::SdpQueryBusy, SDP_QUERY_BUSY},

	{BleError::AttHandleValueIndicationInProgress, ATT_HANDLE_VALUE_INDICATION_IN_PROGRESS},
	{BleError::AttHandleValueIndicationTimeout, ATT_HANDLE_VALUE_INDICATION_TIMEOUT},
	{BleError::AttHandleValueIndicationDisconnect, ATT_HANDLE_VALUE_INDICATION_DISCONNECT},

	{BleError::GattClientNotConnected, GATT_CLIENT_NOT_CONNECTED},
	{BleError::GattClientBusy, GATT_CLIENT_BUSY},
	{BleError::GattClientInWrongState, GATT_CLIENT_IN_WRONG_STATE},
	{BleError::GattClientDifferentContextForAddressAlreadyExists,
	 GATT_CLIENT_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS},
	{BleError::GattClientValueTooLong, GATT_CLIENT_VALUE_TOO_LONG},
	{BleError::GattClientCharacteristicNotificationNotSupported, GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED},
	{BleError::GattClientCharacteristicIndicationNotSupported,
	 GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED},

	{BleError::BnepServiceAlreadyRegistered, BNEP_SERVICE_ALREADY_REGISTERED},
	{BleError::BnepChannelNotConnected, BNEP_CHANNEL_NOT_CONNECTED},
	{BleError::BnepDataLenExceedsMtu, BNEP_DATA_LEN_EXCEEDS_MTU},
	{BleError::BnepSetupConnectionError, BNEP_SETUP_CONNECTION_ERROR},

	{BleError::ObexUnknownError, OBEX_UNKNOWN_ERROR},
	{BleError::ObexConnectFailed, OBEX_CONNECT_FAILED},
	{BleError::ObexDisconnected, OBEX_DISCONNECTED},
	{BleError::ObexNotFound, OBEX_NOT_FOUND},
	{BleError::ObexNotAcceptable, OBEX_NOT_ACCEPTABLE},
	{BleError::ObexAborted, OBEX_ABORTED},

	{BleError::MeshErrorAppkeyIndexInvalid, MESH_ERROR_APPKEY_INDEX_INVALID},
};

} // namespace

bool to_btstack(BleError error, uint8_t& out) {
	for (const auto& entry : kBleErrorMap) {
		if (entry.error == error) {
			out = entry.btstack;
			return true;
		}
	}
	return false;
}

bool from_btstack_error(uint8_t code, BleError& out) {
	for (const auto& entry : kBleErrorMap) {
		if (entry.btstack == code) {
			out = entry.error;
			return true;
		}
	}
	return false;
}

} // namespace c7222::btstack_map
