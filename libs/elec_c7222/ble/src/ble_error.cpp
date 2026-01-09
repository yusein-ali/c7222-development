#include "ble_error.hpp"
#include "ble_address.hpp"

namespace c7222 {

std::ostream& operator<<(std::ostream& os, BleError error) {
	switch (error) {
		case BleError::Success:
			return os << "Success";
		case BleError::UnknownHciCommand:
			return os << "UnknownHciCommand";
		case BleError::UnknownConnectionIdentifier:
			return os << "UnknownConnectionIdentifier";
		case BleError::HardwareFailure:
			return os << "HardwareFailure";
		case BleError::PageTimeout:
			return os << "PageTimeout";
		case BleError::AuthenticationFailure:
			return os << "AuthenticationFailure";
		case BleError::PinOrKeyMissing:
			return os << "PinOrKeyMissing";
		case BleError::MemoryCapacityExceeded:
			return os << "MemoryCapacityExceeded";
		case BleError::ConnectionTimeout:
			return os << "ConnectionTimeout";
		case BleError::ConnectionLimitExceeded:
			return os << "ConnectionLimitExceeded";
		case BleError::SynchronousConnectionLimitToADeviceExceeded:
			return os << "SynchronousConnectionLimitToADeviceExceeded";
		case BleError::AclConnectionAlreadyExists:
			return os << "AclConnectionAlreadyExists";
		case BleError::CommandDisallowed:
			return os << "CommandDisallowed";
		case BleError::ConnectionRejectedDueToLimitedResources:
			return os << "ConnectionRejectedDueToLimitedResources";
		case BleError::ConnectionRejectedDueToSecurityReasons:
			return os << "ConnectionRejectedDueToSecurityReasons";
		case BleError::ConnectionRejectedDueToUnacceptableBdAddr:
			return os << "ConnectionRejectedDueToUnacceptableBdAddr";
		case BleError::UnsupportedFeatureOrParameterValue:
			return os << "UnsupportedFeatureOrParameterValue";
		case BleError::InvalidHciCommandParameters:
			return os << "InvalidHciCommandParameters";
		case BleError::RemoteUserTerminatedConnection:
			return os << "RemoteUserTerminatedConnection";
		case BleError::ConnectionAcceptTimeoutExceeded:
			return os << "ConnectionAcceptTimeoutExceeded";
		case BleError::RemoteDeviceTerminatedConnectionDueToLowResources:
			return os << "RemoteDeviceTerminatedConnectionDueToLowResources";
		case BleError::RemoteDeviceTerminatedConnectionDueToPowerOff:
			return os << "RemoteDeviceTerminatedConnectionDueToPowerOff";
		case BleError::ConnectionTerminatedByLocalHost:
			return os << "ConnectionTerminatedByLocalHost";
		case BleError::RepeatedAttempts:
			return os << "RepeatedAttempts";
		case BleError::PairingNotAllowed:
			return os << "PairingNotAllowed";
		case BleError::UnknownLmpPdu:
			return os << "UnknownLmpPdu";
		case BleError::UnsupportedRemoteFeatureUnsupportedLmpFeature:
			return os << "UnsupportedRemoteFeatureUnsupportedLmpFeature";
		case BleError::ScoOffsetRejected:
			return os << "ScoOffsetRejected";
		case BleError::ScoIntervalRejected:
			return os << "ScoIntervalRejected";
		case BleError::ScoAirModeRejected:
			return os << "ScoAirModeRejected";
		case BleError::InvalidLmpParametersInvalidLlParameters:
			return os << "InvalidLmpParametersInvalidLlParameters";
		case BleError::UnspecifiedError:
			return os << "UnspecifiedError";
		case BleError::UnsupportedLmpParameterValueUnsupportedLlParameterValue:
			return os << "UnsupportedLmpParameterValueUnsupportedLlParameterValue";
		case BleError::RoleChangeNotAllowed:
			return os << "RoleChangeNotAllowed";
		case BleError::LmpResponseTimeoutLlResponseTimeout:
			return os << "LmpResponseTimeoutLlResponseTimeout";
		case BleError::LmpErrorTransactionCollision:
			return os << "LmpErrorTransactionCollision";
		case BleError::LmpPduNotAllowed:
			return os << "LmpPduNotAllowed";
		case BleError::EncryptionModeNotAcceptable:
			return os << "EncryptionModeNotAcceptable";
		case BleError::LinkKeyCannotBeChanged:
			return os << "LinkKeyCannotBeChanged";
		case BleError::RequestedQosNotSupported:
			return os << "RequestedQosNotSupported";
		case BleError::InstantPassed:
			return os << "InstantPassed";
		case BleError::PairingWithUnitKeyNotSupported:
			return os << "PairingWithUnitKeyNotSupported";
		case BleError::DifferentTransactionCollision:
			return os << "DifferentTransactionCollision";
		case BleError::Reserved:
			return os << "Reserved";
		case BleError::QosUnacceptableParameter:
			return os << "QosUnacceptableParameter";
		case BleError::QosRejected:
			return os << "QosRejected";
		case BleError::ChannelClassificationNotSupported:
			return os << "ChannelClassificationNotSupported";
		case BleError::InsufficientSecurity:
			return os << "InsufficientSecurity";
		case BleError::ParameterOutOfMandatoryRange:
			return os << "ParameterOutOfMandatoryRange";
		case BleError::RoleSwitchPending:
			return os << "RoleSwitchPending";
		case BleError::ReservedSlotViolation:
			return os << "ReservedSlotViolation";
		case BleError::RoleSwitchFailed:
			return os << "RoleSwitchFailed";
		case BleError::ExtendedInquiryResponseTooLarge:
			return os << "ExtendedInquiryResponseTooLarge";
		case BleError::SecureSimplePairingNotSupportedByHost:
			return os << "SecureSimplePairingNotSupportedByHost";
		case BleError::HostBusyPairing:
			return os << "HostBusyPairing";
		case BleError::ConnectionRejectedDueToNoSuitableChannelFound:
			return os << "ConnectionRejectedDueToNoSuitableChannelFound";
		case BleError::ControllerBusy:
			return os << "ControllerBusy";
		case BleError::UnacceptableConnectionParameters:
			return os << "UnacceptableConnectionParameters";
		case BleError::DirectedAdvertisingTimeout:
			return os << "DirectedAdvertisingTimeout";
		case BleError::ConnectionTerminatedDueToMicFailure:
			return os << "ConnectionTerminatedDueToMicFailure";
		case BleError::ConnectionFailedToBeEstablished:
			return os << "ConnectionFailedToBeEstablished";
		case BleError::MacConnectionFailed:
			return os << "MacConnectionFailed";
		case BleError::CoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging:
			return os << "CoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging";
		case BleError::BtstackConnectionToBtdaemonFailed:
			return os << "BtstackConnectionToBtdaemonFailed";
		case BleError::BtstackActivationFailedSystemBluetooth:
			return os << "BtstackActivationFailedSystemBluetooth";
		case BleError::BtstackActivationPoweronFailed:
			return os << "BtstackActivationPoweronFailed";
		case BleError::BtstackActivationFailedUnknown:
			return os << "BtstackActivationFailedUnknown";
		case BleError::BtstackNotActivated:
			return os << "BtstackNotActivated";
		case BleError::BtstackBusy:
			return os << "BtstackBusy";
		case BleError::BtstackMemoryAllocFailed:
			return os << "BtstackMemoryAllocFailed";
		case BleError::BtstackAclBuffersFull:
			return os << "BtstackAclBuffersFull";
		case BleError::L2capCommandRejectReasonCommandNotUnderstood:
			return os << "L2capCommandRejectReasonCommandNotUnderstood";
		case BleError::L2capCommandRejectReasonSignalingMtuExceeded:
			return os << "L2capCommandRejectReasonSignalingMtuExceeded";
		case BleError::L2capCommandRejectReasonInvalidCidInRequest:
			return os << "L2capCommandRejectReasonInvalidCidInRequest";
		case BleError::L2capConnectionResponseResultSuccessful:
			return os << "L2capConnectionResponseResultSuccessful";
		case BleError::L2capConnectionResponseResultPending:
			return os << "L2capConnectionResponseResultPending";
		case BleError::L2capConnectionResponseResultRefusedPsm:
			return os << "L2capConnectionResponseResultRefusedPsm";
		case BleError::L2capConnectionResponseResultRefusedSecurity:
			return os << "L2capConnectionResponseResultRefusedSecurity";
		case BleError::L2capConnectionResponseResultRefusedResources:
			return os << "L2capConnectionResponseResultRefusedResources";
		case BleError::L2capConnectionResponseResultErtmNotSupported:
			return os << "L2capConnectionResponseResultErtmNotSupported";
		case BleError::L2capConnectionResponseResultRtxTimeout:
			return os << "L2capConnectionResponseResultRtxTimeout";
		case BleError::L2capConnectionBasebandDisconnect:
			return os << "L2capConnectionBasebandDisconnect";
		case BleError::L2capServiceAlreadyRegistered:
			return os << "L2capServiceAlreadyRegistered";
		case BleError::L2capDataLenExceedsRemoteMtu:
			return os << "L2capDataLenExceedsRemoteMtu";
		case BleError::L2capServiceDoesNotExist:
			return os << "L2capServiceDoesNotExist";
		case BleError::L2capLocalCidDoesNotExist:
			return os << "L2capLocalCidDoesNotExist";
		case BleError::L2capConnectionResponseUnknownError:
			return os << "L2capConnectionResponseUnknownError";
		case BleError::RfcommMultiplexerStopped:
			return os << "RfcommMultiplexerStopped";
		case BleError::RfcommChannelAlreadyRegistered:
			return os << "RfcommChannelAlreadyRegistered";
		case BleError::RfcommNoOutgoingCredits:
			return os << "RfcommNoOutgoingCredits";
		case BleError::RfcommAggregateFlowOff:
			return os << "RfcommAggregateFlowOff";
		case BleError::RfcommDataLenExceedsMtu:
			return os << "RfcommDataLenExceedsMtu";
		case BleError::HfpRemoteRejectsAudioConnection:
			return os << "HfpRemoteRejectsAudioConnection";
		case BleError::SdpHandleAlreadyRegistered:
			return os << "SdpHandleAlreadyRegistered";
		case BleError::SdpQueryIncomplete:
			return os << "SdpQueryIncomplete";
		case BleError::SdpServiceNotFound:
			return os << "SdpServiceNotFound";
		case BleError::SdpHandleInvalid:
			return os << "SdpHandleInvalid";
		case BleError::SdpQueryBusy:
			return os << "SdpQueryBusy";
		case BleError::AttHandleValueIndicationInProgress:
			return os << "AttHandleValueIndicationInProgress";
		case BleError::AttHandleValueIndicationTimeout:
			return os << "AttHandleValueIndicationTimeout";
		case BleError::AttHandleValueIndicationDisconnect:
			return os << "AttHandleValueIndicationDisconnect";
		case BleError::GattClientNotConnected:
			return os << "GattClientNotConnected";
		case BleError::GattClientBusy:
			return os << "GattClientBusy";
		case BleError::GattClientInWrongState:
			return os << "GattClientInWrongState";
		case BleError::GattClientDifferentContextForAddressAlreadyExists:
			return os << "GattClientDifferentContextForAddressAlreadyExists";
		case BleError::GattClientValueTooLong:
			return os << "GattClientValueTooLong";
		case BleError::GattClientCharacteristicNotificationNotSupported:
			return os << "GattClientCharacteristicNotificationNotSupported";
		case BleError::GattClientCharacteristicIndicationNotSupported:
			return os << "GattClientCharacteristicIndicationNotSupported";
		case BleError::BnepServiceAlreadyRegistered:
			return os << "BnepServiceAlreadyRegistered";
		case BleError::BnepChannelNotConnected:
			return os << "BnepChannelNotConnected";
		case BleError::BnepDataLenExceedsMtu:
			return os << "BnepDataLenExceedsMtu";
		case BleError::BnepSetupConnectionError:
			return os << "BnepSetupConnectionError";
		case BleError::ObexUnknownError:
			return os << "ObexUnknownError";
		case BleError::ObexConnectFailed:
			return os << "ObexConnectFailed";
		case BleError::ObexDisconnected:
			return os << "ObexDisconnected";
		case BleError::ObexNotFound:
			return os << "ObexNotFound";
		case BleError::ObexNotAcceptable:
			return os << "ObexNotAcceptable";
		case BleError::ObexAborted:
			return os << "ObexAborted";
		case BleError::MeshErrorAppkeyIndexInvalid:
			return os << "MeshErrorAppkeyIndexInvalid";
	}
	return os << "BleError(" << static_cast<unsigned int>(error) << ")";
}


} // namespace c7222
