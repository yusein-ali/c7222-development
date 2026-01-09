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
	Success,
	UnknownHciCommand,
	UnknownConnectionIdentifier,
	HardwareFailure,
	PageTimeout,
	AuthenticationFailure,
	PinOrKeyMissing,
	MemoryCapacityExceeded,
	ConnectionTimeout,
	ConnectionLimitExceeded,
	SynchronousConnectionLimitToADeviceExceeded,
	AclConnectionAlreadyExists,
	CommandDisallowed,
	ConnectionRejectedDueToLimitedResources,
	ConnectionRejectedDueToSecurityReasons,
	ConnectionRejectedDueToUnacceptableBdAddr,
	UnsupportedFeatureOrParameterValue,
	InvalidHciCommandParameters,
	RemoteUserTerminatedConnection,
	ConnectionAcceptTimeoutExceeded,
	RemoteDeviceTerminatedConnectionDueToLowResources,
	RemoteDeviceTerminatedConnectionDueToPowerOff,
	ConnectionTerminatedByLocalHost,
	RepeatedAttempts,
	PairingNotAllowed,
	UnknownLmpPdu,
	UnsupportedRemoteFeatureUnsupportedLmpFeature,
	ScoOffsetRejected,
	ScoIntervalRejected,
	ScoAirModeRejected,
	InvalidLmpParametersInvalidLlParameters,
	UnspecifiedError,
	UnsupportedLmpParameterValueUnsupportedLlParameterValue,
	RoleChangeNotAllowed,
	LmpResponseTimeoutLlResponseTimeout,
	LmpErrorTransactionCollision,
	LmpPduNotAllowed,
	EncryptionModeNotAcceptable,
	LinkKeyCannotBeChanged,
	RequestedQosNotSupported,
	InstantPassed,
	PairingWithUnitKeyNotSupported,
	DifferentTransactionCollision,
	Reserved,
	QosUnacceptableParameter,
	QosRejected,
	ChannelClassificationNotSupported,
	InsufficientSecurity,
	ParameterOutOfMandatoryRange,
	RoleSwitchPending,
	ReservedSlotViolation,
	RoleSwitchFailed,
	ExtendedInquiryResponseTooLarge,
	SecureSimplePairingNotSupportedByHost,
	HostBusyPairing,
	ConnectionRejectedDueToNoSuitableChannelFound,
	ControllerBusy,
	UnacceptableConnectionParameters,
	DirectedAdvertisingTimeout,
	ConnectionTerminatedDueToMicFailure,
	ConnectionFailedToBeEstablished,
	MacConnectionFailed,
	CoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging,

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
