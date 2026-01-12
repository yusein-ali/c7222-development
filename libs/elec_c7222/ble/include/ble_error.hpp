/**
 * @file ble_error.hpp
 * @brief BLE error codes.
 */
#ifndef ELEC_C7222_BLE_ERROR_H_
#define ELEC_C7222_BLE_ERROR_H_

#include <cstdint>
#include <ostream>

namespace c7222 {

/**
 * @brief BLE error codes.
 */
enum class BleError : uint8_t {

	kSuccess,
	kUnknownHciCommand,
	kUnknownConnectionIdentifier,
	kHardwareFailure,
	kPageTimeout,
	kAuthenticationFailure,
	kPinOrKeyMissing,
	kMemoryCapacityExceeded,
	kConnectionTimeout,
	kConnectionLimitExceeded,
	kSynchronousConnectionLimitToADeviceExceeded,
	kAclConnectionAlreadyExists,
	kCommandDisallowed,
	kConnectionRejectedDueToLimitedResources,
	kConnectionRejectedDueToSecurityReasons,
	kConnectionRejectedDueToUnacceptableBdAddr,
	kUnsupportedFeatureOrParameterValue,
	kInvalidHciCommandParameters,
	kRemoteUserTerminatedConnection,
	kConnectionAcceptTimeoutExceeded,
	kRemoteDeviceTerminatedConnectionDueToLowResources,
	kRemoteDeviceTerminatedConnectionDueToPowerOff,
	kConnectionTerminatedByLocalHost,
	kRepeatedAttempts,
	kPairingNotAllowed,
	kUnknownLmpPdu,
	kUnsupportedRemoteFeatureUnsupportedLmpFeature,
	kScoOffsetRejected,
	kScoIntervalRejected,
	kScoAirModeRejected,
	kInvalidLmpParametersInvalidLlParameters,
	kUnspecifiedError,
	kUnsupportedLmpParameterValueUnsupportedLlParameterValue,
	kRoleChangeNotAllowed,
	kLmpResponseTimeoutLlResponseTimeout,
	kLmpErrorTransactionCollision,
	kLmpPduNotAllowed,
	kEncryptionModeNotAcceptable,
	kLinkKeyCannotBeChanged,
	kRequestedQosNotSupported,
	kInstantPassed,
	kPairingWithUnitKeyNotSupported,
	kDifferentTransactionCollision,
	kReserved,
	kQosUnacceptableParameter,
	kQosRejected,
	kChannelClassificationNotSupported,
	kInsufficientSecurity,
	kParameterOutOfMandatoryRange,
	kRoleSwitchPending,
	kReservedSlotViolation,
	kRoleSwitchFailed,
	kExtendedInquiryResponseTooLarge,
	kSecureSimplePairingNotSupportedByHost,
	kHostBusyPairing,
	kConnectionRejectedDueToNoSuitableChannelFound,
	kControllerBusy,
	kUnacceptableConnectionParameters,
	kDirectedAdvertisingTimeout,
	kConnectionTerminatedDueToMicFailure,
	kConnectionFailedToBeEstablished,
	kMacConnectionFailed,
	kCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging,

	kBtstackConnectionToBtdaemonFailed,
	kBtstackActivationFailedSystemBluetooth,
	kBtstackActivationPoweronFailed,
	kBtstackActivationFailedUnknown,
	kBtstackNotActivated,
	kBtstackBusy,
	kBtstackMemoryAllocFailed,
	kBtstackAclBuffersFull,

	kL2capCommandRejectReasonCommandNotUnderstood,
	kL2capCommandRejectReasonSignalingMtuExceeded,
	kL2capCommandRejectReasonInvalidCidInRequest,
	kL2capConnectionResponseResultSuccessful,
	kL2capConnectionResponseResultPending,
	kL2capConnectionResponseResultRefusedPsm,
	kL2capConnectionResponseResultRefusedSecurity,
	kL2capConnectionResponseResultRefusedResources,
	kL2capConnectionResponseResultErtmNotSupported,
	kL2capConnectionResponseResultRtxTimeout,
	kL2capConnectionBasebandDisconnect,
	kL2capServiceAlreadyRegistered,
	kL2capDataLenExceedsRemoteMtu,
	kL2capServiceDoesNotExist,
	kL2capLocalCidDoesNotExist,
	kL2capConnectionResponseUnknownError,

	kRfcommMultiplexerStopped,
	kRfcommChannelAlreadyRegistered,
	kRfcommNoOutgoingCredits,
	kRfcommAggregateFlowOff,
	kRfcommDataLenExceedsMtu,

	kHfpRemoteRejectsAudioConnection,

	kSdpHandleAlreadyRegistered,
	kSdpQueryIncomplete,
	kSdpServiceNotFound,
	kSdpHandleInvalid,
	kSdpQueryBusy,

	kAttHandleValueIndicationInProgress,
	kAttHandleValueIndicationTimeout,
	kAttHandleValueIndicationDisconnect,

	kGattClientNotConnected,
	kGattClientBusy,
	kGattClientInWrongState,
	kGattClientDifferentContextForAddressAlreadyExists,
	kGattClientValueTooLong,
	kGattClientCharacteristicNotificationNotSupported,
	kGattClientCharacteristicIndicationNotSupported,

	kBnepServiceAlreadyRegistered,
	kBnepChannelNotConnected,
	kBnepDataLenExceedsMtu,
	kBnepSetupConnectionError,

	kObexUnknownError,
	kObexConnectFailed,
	kObexDisconnected,
	kObexNotFound,
	kObexNotAcceptable,
	kObexAborted,

	kMeshErrorAppkeyIndexInvalid
};

std::ostream& operator<<(std::ostream& os, BleError error);

namespace btstack_map {

bool to_btstack(BleError error, uint8_t& out);
bool from_btstack_error(uint8_t code, BleError& out);

} // namespace btstack_map

} // namespace c7222

#endif // ELEC_C7222_BLE_ERROR_H_
