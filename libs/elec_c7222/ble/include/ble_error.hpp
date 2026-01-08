/**
 * @file ble_error.hpp
 * @brief BLE error codes.
 */
#ifndef ELEC_C7222_BLE_GAP_BLE_ERROR_HPP
#define ELEC_C7222_BLE_GAP_BLE_ERROR_HPP

#include <cstdint>
#include <ostream>

namespace c7222 {

/**
 * @brief BLE error codes.
 */
enum class BleError : uint8_t {
	ErrorCodeSuccess,
	ErrorCodeUnknownHciCommand,
	ErrorCodeUnknownConnectionIdentifier,
	ErrorCodeHardwareFailure,
	ErrorCodePageTimeout,
	ErrorCodeAuthenticationFailure,
	ErrorCodePinOrKeyMissing,
	ErrorCodeMemoryCapacityExceeded,
	ErrorCodeConnectionTimeout,
	ErrorCodeConnectionLimitExceeded,
	ErrorCodeSynchronousConnectionLimitToADeviceExceeded,
	ErrorCodeAclConnectionAlreadyExists,
	ErrorCodeCommandDisallowed,
	ErrorCodeConnectionRejectedDueToLimitedResources,
	ErrorCodeConnectionRejectedDueToSecurityReasons,
	ErrorCodeConnectionRejectedDueToUnacceptableBdAddr,
	ErrorCodeUnsupportedFeatureOrParameterValue,
	ErrorCodeInvalidHciCommandParameters,
	ErrorCodeRemoteUserTerminatedConnection,
	ErrorCodeConnectionAcceptTimeoutExceeded,
	ErrorCodeRemoteDeviceTerminatedConnectionDueToLowResources,
	ErrorCodeRemoteDeviceTerminatedConnectionDueToPowerOff,
	ErrorCodeConnectionTerminatedByLocalHost,
	ErrorCodeRepeatedAttempts,
	ErrorCodePairingNotAllowed,
	ErrorCodeUnknownLmpPdu,
	ErrorCodeUnsupportedRemoteFeatureUnsupportedLmpFeature,
	ErrorCodeScoOffsetRejected,
	ErrorCodeScoIntervalRejected,
	ErrorCodeScoAirModeRejected,
	ErrorCodeInvalidLmpParametersInvalidLlParameters,
	ErrorCodeUnspecifiedError,
	ErrorCodeUnsupportedLmpParameterValueUnsupportedLlParameterValue,
	ErrorCodeRoleChangeNotAllowed,
	ErrorCodeLmpResponseTimeoutLlResponseTimeout,
	ErrorCodeLmpErrorTransactionCollision,
	ErrorCodeLmpPduNotAllowed,
	ErrorCodeEncryptionModeNotAcceptable,
	ErrorCodeLinkKeyCannotBeChanged,
	ErrorCodeRequestedQosNotSupported,
	ErrorCodeInstantPassed,
	ErrorCodePairingWithUnitKeyNotSupported,
	ErrorCodeDifferentTransactionCollision,
	ErrorCodeReserved,
	ErrorCodeQosUnacceptableParameter,
	ErrorCodeQosRejected,
	ErrorCodeChannelClassificationNotSupported,
	ErrorCodeInsufficientSecurity,
	ErrorCodeParameterOutOfMandatoryRange,
	ErrorCodeRoleSwitchPending,
	ErrorCodeReservedSlotViolation,
	ErrorCodeRoleSwitchFailed,
	ErrorCodeExtendedInquiryResponseTooLarge,
	ErrorCodeSecureSimplePairingNotSupportedByHost,
	ErrorCodeHostBusyPairing,
	ErrorCodeConnectionRejectedDueToNoSuitableChannelFound,
	ErrorCodeControllerBusy,
	ErrorCodeUnacceptableConnectionParameters,
	ErrorCodeDirectedAdvertisingTimeout,
	ErrorCodeConnectionTerminatedDueToMicFailure,
	ErrorCodeConnectionFailedToBeEstablished,
	ErrorCodeMacConnectionFailed,
	ErrorCodeCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging,

	BtstackConnectionToBtdaemonFailed,
	BtstackActivationFailedSystemBluetooth,
	BtstackActivationPoweronFailed,
	BtstackActivationFailedUnknown,
	BtstackNotActivated,
	BtstackBusy,
	BtstackMemoryAllocFailed,
	BtstackAclBuffersFull,

	L2capCommandRejectReasonCommandNotUnderstood,
	L2capCommandRejectReasonSignalingMtuExceeded,
	L2capCommandRejectReasonInvalidCidInRequest,
	L2capConnectionResponseResultSuccessful,
	L2capConnectionResponseResultPending,
	L2capConnectionResponseResultRefusedPsm,
	L2capConnectionResponseResultRefusedSecurity,
	L2capConnectionResponseResultRefusedResources,
	L2capConnectionResponseResultErtmNotSupported,
	L2capConnectionResponseResultRtxTimeout,
	L2capConnectionBasebandDisconnect,
	L2capServiceAlreadyRegistered,
	L2capDataLenExceedsRemoteMtu,
	L2capServiceDoesNotExist,
	L2capLocalCidDoesNotExist,
	L2capConnectionResponseUnknownError,

	RfcommMultiplexerStopped,
	RfcommChannelAlreadyRegistered,
	RfcommNoOutgoingCredits,
	RfcommAggregateFlowOff,
	RfcommDataLenExceedsMtu,

	HfpRemoteRejectsAudioConnection,

	SdpHandleAlreadyRegistered,
	SdpQueryIncomplete,
	SdpServiceNotFound,
	SdpHandleInvalid,
	SdpQueryBusy,

	AttHandleValueIndicationInProgress,
	AttHandleValueIndicationTimeout,
	AttHandleValueIndicationDisconnect,

	GattClientNotConnected,
	GattClientBusy,
	GattClientInWrongState,
	GattClientDifferentContextForAddressAlreadyExists,
	GattClientValueTooLong,
	GattClientCharacteristicNotificationNotSupported,
	GattClientCharacteristicIndicationNotSupported,

	BnepServiceAlreadyRegistered,
	BnepChannelNotConnected,
	BnepDataLenExceedsMtu,
	BnepSetupConnectionError,

	ObexUnknownError,
	ObexConnectFailed,
	ObexDisconnected,
	ObexNotFound,
	ObexNotAcceptable,
	ObexAborted,

	MeshErrorAppkeyIndexInvalid
};

std::ostream& operator<<(std::ostream& os, BleError error);

namespace btstack_map {

bool to_btstack(BleError error, uint8_t& out);
bool from_btstack_error(uint8_t code, BleError& out);

} // namespace btstack_map

} // namespace c7222

#endif // ELEC_C7222_BLE_GAP_BLE_ERROR_HPP
