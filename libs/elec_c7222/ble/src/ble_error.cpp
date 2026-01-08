#include "ble_error.hpp"
#include "ble_address.hpp"

namespace c7222 {

std::ostream& operator<<(std::ostream& os, BleError error) {
	switch (error) {
		case BleError::ErrorCodeSuccess:
			return os << "ErrorCodeSuccess";
		case BleError::ErrorCodeUnknownHciCommand:
			return os << "ErrorCodeUnknownHciCommand";
		case BleError::ErrorCodeUnknownConnectionIdentifier:
			return os << "ErrorCodeUnknownConnectionIdentifier";
		case BleError::ErrorCodeHardwareFailure:
			return os << "ErrorCodeHardwareFailure";
		case BleError::ErrorCodePageTimeout:
			return os << "ErrorCodePageTimeout";
		case BleError::ErrorCodeAuthenticationFailure:
			return os << "ErrorCodeAuthenticationFailure";
		case BleError::ErrorCodePinOrKeyMissing:
			return os << "ErrorCodePinOrKeyMissing";
		case BleError::ErrorCodeMemoryCapacityExceeded:
			return os << "ErrorCodeMemoryCapacityExceeded";
		case BleError::ErrorCodeConnectionTimeout:
			return os << "ErrorCodeConnectionTimeout";
		case BleError::ErrorCodeConnectionLimitExceeded:
			return os << "ErrorCodeConnectionLimitExceeded";
		case BleError::ErrorCodeSynchronousConnectionLimitToADeviceExceeded:
			return os << "ErrorCodeSynchronousConnectionLimitToADeviceExceeded";
		case BleError::ErrorCodeAclConnectionAlreadyExists:
			return os << "ErrorCodeAclConnectionAlreadyExists";
		case BleError::ErrorCodeCommandDisallowed:
			return os << "ErrorCodeCommandDisallowed";
		case BleError::ErrorCodeConnectionRejectedDueToLimitedResources:
			return os << "ErrorCodeConnectionRejectedDueToLimitedResources";
		case BleError::ErrorCodeConnectionRejectedDueToSecurityReasons:
			return os << "ErrorCodeConnectionRejectedDueToSecurityReasons";
		case BleError::ErrorCodeConnectionRejectedDueToUnacceptableBdAddr:
			return os << "ErrorCodeConnectionRejectedDueToUnacceptableBdAddr";
		case BleError::ErrorCodeUnsupportedFeatureOrParameterValue:
			return os << "ErrorCodeUnsupportedFeatureOrParameterValue";
		case BleError::ErrorCodeInvalidHciCommandParameters:
			return os << "ErrorCodeInvalidHciCommandParameters";
		case BleError::ErrorCodeRemoteUserTerminatedConnection:
			return os << "ErrorCodeRemoteUserTerminatedConnection";
		case BleError::ErrorCodeConnectionAcceptTimeoutExceeded:
			return os << "ErrorCodeConnectionAcceptTimeoutExceeded";
		case BleError::ErrorCodeRemoteDeviceTerminatedConnectionDueToLowResources:
			return os << "ErrorCodeRemoteDeviceTerminatedConnectionDueToLowResources";
		case BleError::ErrorCodeRemoteDeviceTerminatedConnectionDueToPowerOff:
			return os << "ErrorCodeRemoteDeviceTerminatedConnectionDueToPowerOff";
		case BleError::ErrorCodeConnectionTerminatedByLocalHost:
			return os << "ErrorCodeConnectionTerminatedByLocalHost";
		case BleError::ErrorCodeRepeatedAttempts:
			return os << "ErrorCodeRepeatedAttempts";
		case BleError::ErrorCodePairingNotAllowed:
			return os << "ErrorCodePairingNotAllowed";
		case BleError::ErrorCodeUnknownLmpPdu:
			return os << "ErrorCodeUnknownLmpPdu";
		case BleError::ErrorCodeUnsupportedRemoteFeatureUnsupportedLmpFeature:
			return os << "ErrorCodeUnsupportedRemoteFeatureUnsupportedLmpFeature";
		case BleError::ErrorCodeScoOffsetRejected:
			return os << "ErrorCodeScoOffsetRejected";
		case BleError::ErrorCodeScoIntervalRejected:
			return os << "ErrorCodeScoIntervalRejected";
		case BleError::ErrorCodeScoAirModeRejected:
			return os << "ErrorCodeScoAirModeRejected";
		case BleError::ErrorCodeInvalidLmpParametersInvalidLlParameters:
			return os << "ErrorCodeInvalidLmpParametersInvalidLlParameters";
		case BleError::ErrorCodeUnspecifiedError:
			return os << "ErrorCodeUnspecifiedError";
		case BleError::ErrorCodeUnsupportedLmpParameterValueUnsupportedLlParameterValue:
			return os << "ErrorCodeUnsupportedLmpParameterValueUnsupportedLlParameterValue";
		case BleError::ErrorCodeRoleChangeNotAllowed:
			return os << "ErrorCodeRoleChangeNotAllowed";
		case BleError::ErrorCodeLmpResponseTimeoutLlResponseTimeout:
			return os << "ErrorCodeLmpResponseTimeoutLlResponseTimeout";
		case BleError::ErrorCodeLmpErrorTransactionCollision:
			return os << "ErrorCodeLmpErrorTransactionCollision";
		case BleError::ErrorCodeLmpPduNotAllowed:
			return os << "ErrorCodeLmpPduNotAllowed";
		case BleError::ErrorCodeEncryptionModeNotAcceptable:
			return os << "ErrorCodeEncryptionModeNotAcceptable";
		case BleError::ErrorCodeLinkKeyCannotBeChanged:
			return os << "ErrorCodeLinkKeyCannotBeChanged";
		case BleError::ErrorCodeRequestedQosNotSupported:
			return os << "ErrorCodeRequestedQosNotSupported";
		case BleError::ErrorCodeInstantPassed:
			return os << "ErrorCodeInstantPassed";
		case BleError::ErrorCodePairingWithUnitKeyNotSupported:
			return os << "ErrorCodePairingWithUnitKeyNotSupported";
		case BleError::ErrorCodeDifferentTransactionCollision:
			return os << "ErrorCodeDifferentTransactionCollision";
		case BleError::ErrorCodeReserved:
			return os << "ErrorCodeReserved";
		case BleError::ErrorCodeQosUnacceptableParameter:
			return os << "ErrorCodeQosUnacceptableParameter";
		case BleError::ErrorCodeQosRejected:
			return os << "ErrorCodeQosRejected";
		case BleError::ErrorCodeChannelClassificationNotSupported:
			return os << "ErrorCodeChannelClassificationNotSupported";
		case BleError::ErrorCodeInsufficientSecurity:
			return os << "ErrorCodeInsufficientSecurity";
		case BleError::ErrorCodeParameterOutOfMandatoryRange:
			return os << "ErrorCodeParameterOutOfMandatoryRange";
		case BleError::ErrorCodeRoleSwitchPending:
			return os << "ErrorCodeRoleSwitchPending";
		case BleError::ErrorCodeReservedSlotViolation:
			return os << "ErrorCodeReservedSlotViolation";
		case BleError::ErrorCodeRoleSwitchFailed:
			return os << "ErrorCodeRoleSwitchFailed";
		case BleError::ErrorCodeExtendedInquiryResponseTooLarge:
			return os << "ErrorCodeExtendedInquiryResponseTooLarge";
		case BleError::ErrorCodeSecureSimplePairingNotSupportedByHost:
			return os << "ErrorCodeSecureSimplePairingNotSupportedByHost";
		case BleError::ErrorCodeHostBusyPairing:
			return os << "ErrorCodeHostBusyPairing";
		case BleError::ErrorCodeConnectionRejectedDueToNoSuitableChannelFound:
			return os << "ErrorCodeConnectionRejectedDueToNoSuitableChannelFound";
		case BleError::ErrorCodeControllerBusy:
			return os << "ErrorCodeControllerBusy";
		case BleError::ErrorCodeUnacceptableConnectionParameters:
			return os << "ErrorCodeUnacceptableConnectionParameters";
		case BleError::ErrorCodeDirectedAdvertisingTimeout:
			return os << "ErrorCodeDirectedAdvertisingTimeout";
		case BleError::ErrorCodeConnectionTerminatedDueToMicFailure:
			return os << "ErrorCodeConnectionTerminatedDueToMicFailure";
		case BleError::ErrorCodeConnectionFailedToBeEstablished:
			return os << "ErrorCodeConnectionFailedToBeEstablished";
		case BleError::ErrorCodeMacConnectionFailed:
			return os << "ErrorCodeMacConnectionFailed";
		case BleError::ErrorCodeCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging:
			return os << "ErrorCodeCoarseClockAdjustmentRejectedButWillTryToAdjustUsingClockDragging";
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
