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
	{BleError::kSuccess, ERROR_CODE_SUCCESS},
	{BleError::kUnknownHciCommand, ERROR_CODE_UNKNOWN_HCI_COMMAND},
	{BleError::kUnknownConnectionIdentifier, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER},
	{BleError::kHardwareFailure, ERROR_CODE_HARDWARE_FAILURE},
	{BleError::kPageTimeout, ERROR_CODE_PAGE_TIMEOUT},
	{BleError::kAuthenticationFailure, ERROR_CODE_AUTHENTICATION_FAILURE},
	{BleError::kPinOrKeyMissing, ERROR_CODE_PIN_OR_KEY_MISSING},
	{BleError::kMemoryCapacityExceeded, ERROR_CODE_MEMORY_CAPACITY_EXCEEDED},
	{BleError::kConnectionTimeout, ERROR_CODE_CONNECTION_TIMEOUT},
	{BleError::kConnectionLimitExceeded, ERROR_CODE_CONNECTION_LIMIT_EXCEEDED},
	{BleError::kSynchronousConnectionLimitToADeviceExceeded,
	 ERROR_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED},
	{BleError::kAclConnectionAlreadyExists, ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS},
	{BleError::kCommandDisallowed, ERROR_CODE_COMMAND_DISALLOWED},
	{BleError::kConnectionRejectedDueToLimitedResources, ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES},
	{BleError::kConnectionRejectedDueToSecurityReasons, ERROR_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS},
	{BleError::kConnectionRejectedDueToUnacceptableBdAddr,
	 ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR},
	{BleError::kUnsupportedFeatureOrParameterValue, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE},
	{BleError::kInvalidHciCommandParameters, ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS},
	{BleError::kRemoteUserTerminatedConnection, ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION},
	{BleError::kConnectionAcceptTimeoutExceeded, ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED},
	{BleError::kRemoteDeviceTerminatedConnectionDueToLowResources,
	 ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES},
	{BleError::kRemoteDeviceTerminatedConnectionDueToPowerOff,
	 ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF},
	{BleError::kConnectionTerminatedByLocalHost, ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST},
	{BleError::kRepeatedAttempts, ERROR_CODE_REPEATED_ATTEMPTS},
	{BleError::kPairingNotAllowed, ERROR_CODE_PAIRING_NOT_ALLOWED},
	{BleError::kUnknownLmpPdu, ERROR_CODE_UNKNOWN_LMP_PDU},
	{BleError::kUnsupportedRemoteFeatureUnsupportedLmpFeature,
	 ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE},
	{BleError::kScoOffsetRejected, ERROR_CODE_SCO_OFFSET_REJECTED},
	{BleError::kScoIntervalRejected, ERROR_CODE_SCO_INTERVAL_REJECTED},
	{BleError::kScoAirModeRejected, ERROR_CODE_SCO_AIR_MODE_REJECTED},
	{BleError::kInvalidLmpParametersInvalidLlParameters, ERROR_CODE_INVALID_LMP_PARAMETERS_INVALID_LL_PARAMETERS},
	{BleError::kUnspecifiedError, ERROR_CODE_UNSPECIFIED_ERROR},
	{BleError::kUnsupportedLmpParameterValueUnsupportedLlParameterValue,
	 ERROR_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE_UNSUPPORTED_LL_PARAMETER_VALUE},
	{BleError::kRoleChangeNotAllowed, ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED},
	{BleError::kLmpResponseTimeoutLlResponseTimeout, ERROR_CODE_LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT},
	{BleError::kLmpErrorTransactionCollision, ERROR_CODE_LMP_ERROR_TRANSACTION_COLLISION},
	{BleError::kLmpPduNotAllowed, ERROR_CODE_LMP_PDU_NOT_ALLOWED},
	{BleError::kEncryptionModeNotAcceptable, ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE},
	{BleError::kLinkKeyCannotBeChanged, ERROR_CODE_LINK_KEY_CANNOT_BE_CHANGED},
	{BleError::kRequestedQosNotSupported, ERROR_CODE_REQUESTED_QOS_NOT_SUPPORTED},
	{BleError::kInstantPassed, ERROR_CODE_INSTANT_PASSED},
	{BleError::kPairingWithUnitKeyNotSupported, ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED},
	{BleError::kDifferentTransactionCollision, ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION},
	{BleError::kReserved, ERROR_CODE_RESERVED},
	{BleError::kQosUnacceptableParameter, ERROR_CODE_QOS_UNACCEPTABLE_PARAMETER},
	{BleError::kQosRejected, ERROR_CODE_QOS_REJECTED},
	{BleError::kChannelClassificationNotSupported, ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED},
	{BleError::kInsufficientSecurity, ERROR_CODE_INSUFFICIENT_SECURITY},
	{BleError::kParameterOutOfMandatoryRange, ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE},
	{BleError::kRoleSwitchPending, ERROR_CODE_ROLE_SWITCH_PENDING},
	{BleError::kReservedSlotViolation, ERROR_CODE_RESERVED_SLOT_VIOLATION},
	{BleError::kRoleSwitchFailed, ERROR_CODE_ROLE_SWITCH_FAILED},
	{BleError::kExtendedInquiryResponseTooLarge, ERROR_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE},
	{BleError::kSecureSimplePairingNotSupportedByHost,
	 ERROR_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST},
	{BleError::kHostBusyPairing, ERROR_CODE_HOST_BUSY_PAIRING},
	{BleError::kConnectionRejectedDueToNoSuitableChannelFound,
	 ERROR_CODE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND},
	{BleError::kControllerBusy, ERROR_CODE_CONTROLLER_BUSY},
	{BleError::kUnacceptableConnectionParameters, ERROR_CODE_UNACCEPTABLE_CONNECTION_PARAMETERS},
	{BleError::kDirectedAdvertisingTimeout, ERROR_CODE_DIRECTED_ADVERTISING_TIMEOUT},
	{BleError::kConnectionTerminatedDueToMicFailure, ERROR_CODE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE},
	{BleError::kConnectionFailedToBeEstablished, ERROR_CODE_CONNECTION_FAILED_TO_BE_ESTABLISHED},
	{BleError::kMacConnectionFailed, ERROR_CODE_MAC_CONNECTION_FAILED},
	{BleError::kCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging,
	 ERROR_CODE_COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING},

	{BleError::kBtstackConnectionToBtdaemonFailed, BTSTACK_CONNECTION_TO_BTDAEMON_FAILED},
	{BleError::kBtstackActivationFailedSystemBluetooth, BTSTACK_ACTIVATION_FAILED_SYSTEM_BLUETOOTH},
	{BleError::kBtstackActivationPoweronFailed, BTSTACK_ACTIVATION_POWERON_FAILED},
	{BleError::kBtstackActivationFailedUnknown, BTSTACK_ACTIVATION_FAILED_UNKNOWN},
	{BleError::kBtstackNotActivated, BTSTACK_NOT_ACTIVATED},
	{BleError::kBtstackBusy, BTSTACK_BUSY},
	{BleError::kBtstackMemoryAllocFailed, BTSTACK_MEMORY_ALLOC_FAILED},
	{BleError::kBtstackAclBuffersFull, BTSTACK_ACL_BUFFERS_FULL},

	{BleError::kL2capCommandRejectReasonCommandNotUnderstood, L2CAP_COMMAND_REJECT_REASON_COMMAND_NOT_UNDERSTOOD},
	{BleError::kL2capCommandRejectReasonSignalingMtuExceeded, L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED},
	{BleError::kL2capCommandRejectReasonInvalidCidInRequest, L2CAP_COMMAND_REJECT_REASON_INVALID_CID_IN_REQUEST},
	{BleError::kL2capConnectionResponseResultSuccessful, L2CAP_CONNECTION_RESPONSE_RESULT_SUCCESSFUL},
	{BleError::kL2capConnectionResponseResultPending, L2CAP_CONNECTION_RESPONSE_RESULT_PENDING},
	{BleError::kL2capConnectionResponseResultRefusedPsm, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_PSM},
	{BleError::kL2capConnectionResponseResultRefusedSecurity, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_SECURITY},
	{BleError::kL2capConnectionResponseResultRefusedResources, L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_RESOURCES},
	{BleError::kL2capConnectionResponseResultErtmNotSupported, L2CAP_CONNECTION_RESPONSE_RESULT_ERTM_NOT_SUPPORTED},
	{BleError::kL2capConnectionResponseResultRtxTimeout, L2CAP_CONNECTION_RESPONSE_RESULT_RTX_TIMEOUT},
	{BleError::kL2capConnectionBasebandDisconnect, L2CAP_CONNECTION_BASEBAND_DISCONNECT},
	{BleError::kL2capServiceAlreadyRegistered, L2CAP_SERVICE_ALREADY_REGISTERED},
	{BleError::kL2capDataLenExceedsRemoteMtu, L2CAP_DATA_LEN_EXCEEDS_REMOTE_MTU},
	{BleError::kL2capServiceDoesNotExist, L2CAP_SERVICE_DOES_NOT_EXIST},
	{BleError::kL2capLocalCidDoesNotExist, L2CAP_LOCAL_CID_DOES_NOT_EXIST},
	{BleError::kL2capConnectionResponseUnknownError, L2CAP_CONNECTION_RESPONSE_UNKNOWN_ERROR},

	{BleError::kRfcommMultiplexerStopped, RFCOMM_MULTIPLEXER_STOPPED},
	{BleError::kRfcommChannelAlreadyRegistered, RFCOMM_CHANNEL_ALREADY_REGISTERED},
	{BleError::kRfcommNoOutgoingCredits, RFCOMM_NO_OUTGOING_CREDITS},
	{BleError::kRfcommAggregateFlowOff, RFCOMM_AGGREGATE_FLOW_OFF},
	{BleError::kRfcommDataLenExceedsMtu, RFCOMM_DATA_LEN_EXCEEDS_MTU},

	{BleError::kHfpRemoteRejectsAudioConnection, HFP_REMOTE_REJECTS_AUDIO_CONNECTION},

	{BleError::kSdpHandleAlreadyRegistered, SDP_HANDLE_ALREADY_REGISTERED},
	{BleError::kSdpQueryIncomplete, SDP_QUERY_INCOMPLETE},
	{BleError::kSdpServiceNotFound, SDP_SERVICE_NOT_FOUND},
	{BleError::kSdpHandleInvalid, SDP_HANDLE_INVALID},
	{BleError::kSdpQueryBusy, SDP_QUERY_BUSY},

	{BleError::kAttHandleValueIndicationInProgress, ATT_HANDLE_VALUE_INDICATION_IN_PROGRESS},
	{BleError::kAttHandleValueIndicationTimeout, ATT_HANDLE_VALUE_INDICATION_TIMEOUT},
	{BleError::kAttHandleValueIndicationDisconnect, ATT_HANDLE_VALUE_INDICATION_DISCONNECT},

	{BleError::kAttErrorReadNotPermitted, ATT_ERROR_READ_NOT_PERMITTED},
	{BleError::kAttErrorWriteNotPermitted, ATT_ERROR_WRITE_NOT_PERMITTED},
	{BleError::kAttErrorInvalidAttrValueLength, ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH},

	{BleError::kGattClientNotConnected, GATT_CLIENT_NOT_CONNECTED},
	{BleError::kGattClientBusy, GATT_CLIENT_BUSY},
	{BleError::kGattClientInWrongState, GATT_CLIENT_IN_WRONG_STATE},
	{BleError::kGattClientDifferentContextForAddressAlreadyExists,
	 GATT_CLIENT_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS},
	{BleError::kGattClientValueTooLong, GATT_CLIENT_VALUE_TOO_LONG},
	{BleError::kGattClientCharacteristicNotificationNotSupported, GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED},
	{BleError::kGattClientCharacteristicIndicationNotSupported,
	 GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED},

	{BleError::kBnepServiceAlreadyRegistered, BNEP_SERVICE_ALREADY_REGISTERED},
	{BleError::kBnepChannelNotConnected, BNEP_CHANNEL_NOT_CONNECTED},
	{BleError::kBnepDataLenExceedsMtu, BNEP_DATA_LEN_EXCEEDS_MTU},
	{BleError::kBnepSetupConnectionError, BNEP_SETUP_CONNECTION_ERROR},

	{BleError::kObexUnknownError, OBEX_UNKNOWN_ERROR},
	{BleError::kObexConnectFailed, OBEX_CONNECT_FAILED},
	{BleError::kObexDisconnected, OBEX_DISCONNECTED},
	{BleError::kObexNotFound, OBEX_NOT_FOUND},
	{BleError::kObexNotAcceptable, OBEX_NOT_ACCEPTABLE},
	{BleError::kObexAborted, OBEX_ABORTED},

	{BleError::kMeshErrorAppkeyIndexInvalid, MESH_ERROR_APPKEY_INDEX_INVALID},
};

} // namespace

bool ToBtStack(BleError error, uint8_t& out) {
	for (const auto& entry : kBleErrorMap) {
		if (entry.error == error) {
			out = entry.btstack;
			return true;
		}
	}
	return false;
}

bool FromBtStackError(uint8_t code, BleError& out) {
	for (const auto& entry : kBleErrorMap) {
		if (entry.btstack == code) {
			out = entry.error;
			return true;
		}
	}
	return false;
}

} // namespace c7222::btstack_map
