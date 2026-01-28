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
 * @enum BleError
 * @brief BLE error codes used across HCI/L2CAP/ATT/GATT and BTstack helpers.
 *
 * Values are aligned with BTstack and Bluetooth error/status codes where applicable.
 */
enum class BleError : int {

	/**
	 * @brief Generic HCI status and controller errors.
	 */
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

	/**
	 * @brief BTstack framework errors.
	 */
	kBtstackConnectionToBtdaemonFailed,
	kBtstackActivationFailedSystemBluetooth,
	kBtstackActivationPoweronFailed,
	kBtstackActivationFailedUnknown,
	kBtstackNotActivated,
	kBtstackBusy,
	kBtstackMemoryAllocFailed,
	kBtstackAclBuffersFull,

	/**
	 * @brief L2CAP errors and results.
	 */
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

	/**
	 * @brief RFCOMM errors.
	 */
	kRfcommMultiplexerStopped,
	kRfcommChannelAlreadyRegistered,
	kRfcommNoOutgoingCredits,
	kRfcommAggregateFlowOff,
	kRfcommDataLenExceedsMtu,

	/**
	 * @brief HFP errors.
	 */
	kHfpRemoteRejectsAudioConnection,

	/**
	 * @brief SDP errors.
	 */
	kSdpHandleAlreadyRegistered,
	kSdpQueryIncomplete,
	kSdpServiceNotFound,
	kSdpHandleInvalid,
	kSdpQueryBusy,

	/**
	 * @brief ATT indication flow control errors.
	 */
	kAttHandleValueIndicationInProgress,
	kAttHandleValueIndicationTimeout,
	kAttHandleValueIndicationDisconnect,

	/**
	 * @brief ATT Error: Read Not Permitted (0x02 from spec).
	 */
	kAttErrorReadNotPermitted,
	/**
	 * @brief ATT Error: Write Not Permitted (0x03 from spec).
	 */
	kAttErrorWriteNotPermitted,
	/**
	 * @brief ATT Error: Insufficient Authentication (0x05 from spec).
	 */
	kAttErrorInsufficientAuthentication,
	/**
	 * @brief ATT Error: Insufficient Authorization (0x08 from spec).
	 */
	kAttErrorInsufficientAuthorization,
	/**
	 * @brief ATT Error: Invalid Attribute Value Length (0x0D from spec).
	 */
	kAttErrorInvalidAttrValueLength,
	/**
	 * @brief ATT Error: Insufficient Encryption (0x0F from spec).
	 */
	kAttErrorInsufficientEncryption,

	/**
	 * @brief GATT client errors.
	 */
	kGattClientNotConnected,
	kGattClientBusy,
	kGattClientInWrongState,
	kGattClientDifferentContextForAddressAlreadyExists,
	kGattClientValueTooLong,
	kGattClientCharacteristicNotificationNotSupported,
	kGattClientCharacteristicIndicationNotSupported,

	/**
	 * @brief BNEP errors.
	 */
	kBnepServiceAlreadyRegistered,
	kBnepChannelNotConnected,
	kBnepDataLenExceedsMtu,
	kBnepSetupConnectionError,

	/**
	 * @brief OBEX errors.
	 */
	kObexUnknownError,
	kObexConnectFailed,
	kObexDisconnected,
	kObexNotFound,
	kObexNotAcceptable,
	kObexAborted,

	/**
	 * @brief Mesh errors.
	 */
	kMeshErrorAppkeyIndexInvalid
};

/**
 * @brief Stream output helper for BleError.
 */
std::ostream& operator<<(std::ostream& os, BleError error);

}  // namespace c7222

#endif  // ELEC_C7222_BLE_ERROR_H_
