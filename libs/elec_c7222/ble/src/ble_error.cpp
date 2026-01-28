#include "ble_error.hpp"

namespace c7222 {

std::ostream& operator<<(std::ostream& os, BleError error) {
	switch (error) {
		case BleError::kSuccess:
			return os << "Success";
		case BleError::kUnknownHciCommand:
			return os << "UnknownHciCommand";
		case BleError::kUnknownConnectionIdentifier:
			return os << "UnknownConnectionIdentifier";
		case BleError::kHardwareFailure:
			return os << "HardwareFailure";
		case BleError::kPageTimeout:
			return os << "PageTimeout";
		case BleError::kAuthenticationFailure:
			return os << "AuthenticationFailure";
		case BleError::kPinOrKeyMissing:
			return os << "PinOrKeyMissing";
		case BleError::kMemoryCapacityExceeded:
			return os << "MemoryCapacityExceeded";
		case BleError::kConnectionTimeout:
			return os << "ConnectionTimeout";
		case BleError::kConnectionLimitExceeded:
			return os << "ConnectionLimitExceeded";
		case BleError::kSynchronousConnectionLimitToADeviceExceeded:
			return os << "SynchronousConnectionLimitToADeviceExceeded";
		case BleError::kAclConnectionAlreadyExists:
			return os << "AclConnectionAlreadyExists";
		case BleError::kCommandDisallowed:
			return os << "CommandDisallowed";
		case BleError::kConnectionRejectedDueToLimitedResources:
			return os << "ConnectionRejectedDueToLimitedResources";
		case BleError::kConnectionRejectedDueToSecurityReasons:
			return os << "ConnectionRejectedDueToSecurityReasons";
		case BleError::kConnectionRejectedDueToUnacceptableBdAddr:
			return os << "ConnectionRejectedDueToUnacceptableBdAddr";
		case BleError::kUnsupportedFeatureOrParameterValue:
			return os << "UnsupportedFeatureOrParameterValue";
		case BleError::kInvalidHciCommandParameters:
			return os << "InvalidHciCommandParameters";
		case BleError::kRemoteUserTerminatedConnection:
			return os << "RemoteUserTerminatedConnection";
		case BleError::kConnectionAcceptTimeoutExceeded:
			return os << "ConnectionAcceptTimeoutExceeded";
		case BleError::kRemoteDeviceTerminatedConnectionDueToLowResources:
			return os << "RemoteDeviceTerminatedConnectionDueToLowResources";
		case BleError::kRemoteDeviceTerminatedConnectionDueToPowerOff:
			return os << "RemoteDeviceTerminatedConnectionDueToPowerOff";
		case BleError::kConnectionTerminatedByLocalHost:
			return os << "ConnectionTerminatedByLocalHost";
		case BleError::kRepeatedAttempts:
			return os << "RepeatedAttempts";
		case BleError::kPairingNotAllowed:
			return os << "PairingNotAllowed";
		case BleError::kUnknownLmpPdu:
			return os << "UnknownLmpPdu";
		case BleError::kUnsupportedRemoteFeatureUnsupportedLmpFeature:
			return os << "UnsupportedRemoteFeatureUnsupportedLmpFeature";
		case BleError::kScoOffsetRejected:
			return os << "ScoOffsetRejected";
		case BleError::kScoIntervalRejected:
			return os << "ScoIntervalRejected";
		case BleError::kScoAirModeRejected:
			return os << "ScoAirModeRejected";
		case BleError::kInvalidLmpParametersInvalidLlParameters:
			return os << "InvalidLmpParametersInvalidLlParameters";
		case BleError::kUnspecifiedError:
			return os << "UnspecifiedError";
		case BleError::kUnsupportedLmpParameterValueUnsupportedLlParameterValue:
			return os << "UnsupportedLmpParameterValueUnsupportedLlParameterValue";
		case BleError::kRoleChangeNotAllowed:
			return os << "RoleChangeNotAllowed";
		case BleError::kLmpResponseTimeoutLlResponseTimeout:
			return os << "LmpResponseTimeoutLlResponseTimeout";
		case BleError::kLmpErrorTransactionCollision:
			return os << "LmpErrorTransactionCollision";
		case BleError::kLmpPduNotAllowed:
			return os << "LmpPduNotAllowed";
		case BleError::kEncryptionModeNotAcceptable:
			return os << "EncryptionModeNotAcceptable";
		case BleError::kLinkKeyCannotBeChanged:
			return os << "LinkKeyCannotBeChanged";
		case BleError::kRequestedQosNotSupported:
			return os << "RequestedQosNotSupported";
		case BleError::kInstantPassed:
			return os << "InstantPassed";
		case BleError::kPairingWithUnitKeyNotSupported:
			return os << "PairingWithUnitKeyNotSupported";
		case BleError::kDifferentTransactionCollision:
			return os << "DifferentTransactionCollision";
		case BleError::kReserved:
			return os << "Reserved";
		case BleError::kQosUnacceptableParameter:
			return os << "QosUnacceptableParameter";
		case BleError::kQosRejected:
			return os << "QosRejected";
		case BleError::kChannelClassificationNotSupported:
			return os << "ChannelClassificationNotSupported";
		case BleError::kInsufficientSecurity:
			return os << "InsufficientSecurity";
		case BleError::kParameterOutOfMandatoryRange:
			return os << "ParameterOutOfMandatoryRange";
		case BleError::kRoleSwitchPending:
			return os << "RoleSwitchPending";
		case BleError::kReservedSlotViolation:
			return os << "ReservedSlotViolation";
		case BleError::kRoleSwitchFailed:
			return os << "RoleSwitchFailed";
		case BleError::kExtendedInquiryResponseTooLarge:
			return os << "ExtendedInquiryResponseTooLarge";
		case BleError::kSecureSimplePairingNotSupportedByHost:
			return os << "SecureSimplePairingNotSupportedByHost";
		case BleError::kHostBusyPairing:
			return os << "HostBusyPairing";
		case BleError::kConnectionRejectedDueToNoSuitableChannelFound:
			return os << "ConnectionRejectedDueToNoSuitableChannelFound";
		case BleError::kControllerBusy:
			return os << "ControllerBusy";
		case BleError::kUnacceptableConnectionParameters:
			return os << "UnacceptableConnectionParameters";
		case BleError::kDirectedAdvertisingTimeout:
			return os << "DirectedAdvertisingTimeout";
		case BleError::kConnectionTerminatedDueToMicFailure:
			return os << "ConnectionTerminatedDueToMicFailure";
		case BleError::kConnectionFailedToBeEstablished:
			return os << "ConnectionFailedToBeEstablished";
		case BleError::kMacConnectionFailed:
			return os << "MacConnectionFailed";
		case BleError::kCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging:
			return os << "CoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging";
		case BleError::kBtstackConnectionToBtdaemonFailed:
			return os << "BtstackConnectionToBtdaemonFailed";
		case BleError::kBtstackActivationFailedSystemBluetooth:
			return os << "BtstackActivationFailedSystemBluetooth";
		case BleError::kBtstackActivationPoweronFailed:
			return os << "BtstackActivationPoweronFailed";
		case BleError::kBtstackActivationFailedUnknown:
			return os << "BtstackActivationFailedUnknown";
		case BleError::kBtstackNotActivated:
			return os << "BtstackNotActivated";
		case BleError::kBtstackBusy:
			return os << "BtstackBusy";
		case BleError::kBtstackMemoryAllocFailed:
			return os << "BtstackMemoryAllocFailed";
		case BleError::kBtstackAclBuffersFull:
			return os << "BtstackAclBuffersFull";
		case BleError::kL2capCommandRejectReasonCommandNotUnderstood:
			return os << "L2capCommandRejectReasonCommandNotUnderstood";
		case BleError::kL2capCommandRejectReasonSignalingMtuExceeded:
			return os << "L2capCommandRejectReasonSignalingMtuExceeded";
		case BleError::kL2capCommandRejectReasonInvalidCidInRequest:
			return os << "L2capCommandRejectReasonInvalidCidInRequest";
		case BleError::kL2capConnectionResponseResultSuccessful:
			return os << "L2capConnectionResponseResultSuccessful";
		case BleError::kL2capConnectionResponseResultPending:
			return os << "L2capConnectionResponseResultPending";
		case BleError::kL2capConnectionResponseResultRefusedPsm:
			return os << "L2capConnectionResponseResultRefusedPsm";
		case BleError::kL2capConnectionResponseResultRefusedSecurity:
			return os << "L2capConnectionResponseResultRefusedSecurity";
		case BleError::kL2capConnectionResponseResultRefusedResources:
			return os << "L2capConnectionResponseResultRefusedResources";
		case BleError::kL2capConnectionResponseResultErtmNotSupported:
			return os << "L2capConnectionResponseResultErtmNotSupported";
		case BleError::kL2capConnectionResponseResultRtxTimeout:
			return os << "L2capConnectionResponseResultRtxTimeout";
		case BleError::kL2capConnectionBasebandDisconnect:
			return os << "L2capConnectionBasebandDisconnect";
		case BleError::kL2capServiceAlreadyRegistered:
			return os << "L2capServiceAlreadyRegistered";
		case BleError::kL2capDataLenExceedsRemoteMtu:
			return os << "L2capDataLenExceedsRemoteMtu";
		case BleError::kL2capServiceDoesNotExist:
			return os << "L2capServiceDoesNotExist";
		case BleError::kL2capLocalCidDoesNotExist:
			return os << "L2capLocalCidDoesNotExist";
		case BleError::kL2capConnectionResponseUnknownError:
			return os << "L2capConnectionResponseUnknownError";
		case BleError::kRfcommMultiplexerStopped:
			return os << "RfcommMultiplexerStopped";
		case BleError::kRfcommChannelAlreadyRegistered:
			return os << "RfcommChannelAlreadyRegistered";
		case BleError::kRfcommNoOutgoingCredits:
			return os << "RfcommNoOutgoingCredits";
		case BleError::kRfcommAggregateFlowOff:
			return os << "RfcommAggregateFlowOff";
		case BleError::kRfcommDataLenExceedsMtu:
			return os << "RfcommDataLenExceedsMtu";
		case BleError::kHfpRemoteRejectsAudioConnection:
			return os << "HfpRemoteRejectsAudioConnection";
		case BleError::kSdpHandleAlreadyRegistered:
			return os << "SdpHandleAlreadyRegistered";
		case BleError::kSdpQueryIncomplete:
			return os << "SdpQueryIncomplete";
		case BleError::kSdpServiceNotFound:
			return os << "SdpServiceNotFound";
		case BleError::kSdpHandleInvalid:
			return os << "SdpHandleInvalid";
		case BleError::kSdpQueryBusy:
			return os << "SdpQueryBusy";
		case BleError::kAttHandleValueIndicationInProgress:
			return os << "AttHandleValueIndicationInProgress";
		case BleError::kAttHandleValueIndicationTimeout:
			return os << "AttHandleValueIndicationTimeout";
		case BleError::kAttHandleValueIndicationDisconnect:
			return os << "AttHandleValueIndicationDisconnect";
		case BleError::kAttErrorReadNotPermitted:
			return os << "AttErrorReadNotPermitted";
		case BleError::kAttErrorWriteNotPermitted:
			return os << "AttErrorWriteNotPermitted";
		case BleError::kAttErrorInsufficientAuthentication:
			return os << "AttErrorInsufficientAuthentication";
		case BleError::kAttErrorInsufficientAuthorization:
			return os << "AttErrorInsufficientAuthorization";
		case BleError::kAttErrorInvalidAttrValueLength:
			return os << "AttErrorInvalidAttrValueLength";
		case BleError::kAttErrorInsufficientEncryption:
			return os << "AttErrorInsufficientEncryption";
		case BleError::kGattClientNotConnected:
			return os << "GattClientNotConnected";
		case BleError::kGattClientBusy:
			return os << "GattClientBusy";
		case BleError::kGattClientInWrongState:
			return os << "GattClientInWrongState";
		case BleError::kGattClientDifferentContextForAddressAlreadyExists:
			return os << "GattClientDifferentContextForAddressAlreadyExists";
		case BleError::kGattClientValueTooLong:
			return os << "GattClientValueTooLong";
		case BleError::kGattClientCharacteristicNotificationNotSupported:
			return os << "GattClientCharacteristicNotificationNotSupported";
		case BleError::kGattClientCharacteristicIndicationNotSupported:
			return os << "GattClientCharacteristicIndicationNotSupported";
		case BleError::kBnepServiceAlreadyRegistered:
			return os << "BnepServiceAlreadyRegistered";
		case BleError::kBnepChannelNotConnected:
			return os << "BnepChannelNotConnected";
		case BleError::kBnepDataLenExceedsMtu:
			return os << "BnepDataLenExceedsMtu";
		case BleError::kBnepSetupConnectionError:
			return os << "BnepSetupConnectionError";
		case BleError::kObexUnknownError:
			return os << "ObexUnknownError";
		case BleError::kObexConnectFailed:
			return os << "ObexConnectFailed";
		case BleError::kObexDisconnected:
			return os << "ObexDisconnected";
		case BleError::kObexNotFound:
			return os << "ObexNotFound";
		case BleError::kObexNotAcceptable:
			return os << "ObexNotAcceptable";
		case BleError::kObexAborted:
			return os << "ObexAborted";
		case BleError::kMeshErrorAppkeyIndexInvalid:
			return os << "MeshErrorAppkeyIndexInvalid";
	}
	return os << "BleError(" << static_cast<unsigned int>(error) << ")";
}


} // namespace c7222
