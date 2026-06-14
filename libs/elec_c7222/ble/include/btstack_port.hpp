/**
 * @file btstack_port.hpp
 * @brief C++ adapter types for BlueKitchen BTstack controller ports.
 *
 * BTstack is a C library. Platform integration is provided through C function
 * tables such as `btstack_run_loop_t`, `hci_transport_t`, and optional chipset
 * and storage drivers. This header gives the C7222 BLE layer a small C++ API
 * for passing those tables around without forcing every platform to include
 * BTstack headers.
 *
 * This header is part of the common BLE include surface because `Ble` can own
 * or reference a port object without knowing whether the active platform links
 * BTstack. The implementation that applies these tables to BTstack lives under
 * the Pico SDK platform sources and is only compiled for BTstack-capable
 * platforms.
 */
#ifndef ELEC_C7222_BLE_BTSTACK_PORT_HPP
#define ELEC_C7222_BLE_BTSTACK_PORT_HPP

#include <cstdint>
#include <utility>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @struct BtstackPortTables
 * @brief Non-owning bundle of BTstack C function tables for one controller port.
 *
 * BTstack expects the platform to provide several C interfaces before the HCI
 * stack can be initialized. Most of those interfaces are static function-table
 * objects returned by BTstack or by the board-specific controller port. This
 * structure groups the selected interfaces so `BtstackPort` and `Ble` can pass
 * them as one coherent configuration while keeping this public header free of
 * BTstack includes.
 *
 * ---
 * ### Ownership
 *
 * This structure does not own any pointed-to object. Every pointer must remain
 * valid for as long as the BTstack port can be initialized or used. In practice
 * these opaque pointers usually refer to:
 *
 * - BTstack-provided static tables, such as the FreeRTOS run loop.
 * - Platform-provided static tables, such as a UART or SPI HCI transport.
 * - Long-lived platform storage contexts, such as a TLV flash context.
 *
 * ---
 * ### Required and Optional Tables
 *
 * `run_loop` and `hci_transport` are required for a usable controller port.
 * `hci_transport_config` is required only when the selected transport needs
 * configuration. `chipset`, `tlv`, and `tlv_context` are optional and depend on
 * the controller and storage policy.
 *
 * Use `Validate()` for a basic structural check that rejects an entirely empty
 * table bundle.
 */
struct BtstackPortTables {
	/**
	 * @brief BTstack run loop used to process timers, data sources, and queued
	 * callbacks.
	 *
	 * For the C7222 RP2350 FreeRTOS-based targets this should normally point to
	 * the BTstack FreeRTOS run loop instance, for example
	 * `btstack_run_loop_freertos_get_instance()`.
	 */
	const void* run_loop = nullptr;

	/**
	 * @brief HCI transport used to exchange HCI packets with the BLE controller.
	 *
	 * This is the central controller-specific interface. It may be an existing
	 * BTstack transport such as H4/H5 over UART, or a board-specific transport
	 * for another physical connection.
	 */
	const void* hci_transport = nullptr;

	/**
	 * @brief Optional transport configuration passed to `hci_init()`.
	 *
	 * The concrete type depends on the transport. UART H4/H5 transports use
	 * `hci_transport_config_uart_t`. Transports that do not need configuration
	 * may leave this as `nullptr`.
	 */
	const void* hci_transport_config = nullptr;

	/**
	 * @brief Optional controller chipset driver.
	 *
	 * Set this when the BLE controller needs vendor-specific initialization,
	 * baud-rate-change commands, warm-start handling, or BD_ADDR commands.
	 */
	const void* chipset = nullptr;

	/**
	 * @brief Optional BTstack TLV storage implementation.
	 *
	 * Use this when bonding, link keys, or LE device database entries should be
	 * persisted. A port that intentionally disables persistence may use
	 * `btstack_tlv_none_init_instance()`.
	 */
	const void* tlv = nullptr;

	/**
	 * @brief Optional context pointer passed to the TLV implementation.
	 *
	 * Must remain valid as long as `tlv` can be used. Leave as `nullptr` for
	 * TLV implementations that do not require context.
	 */
	void* tlv_context = nullptr;

	/**
	 * @brief Create a table bundle that uses BTstack's FreeRTOS run loop.
	 *
	 * This is the common starting point for C7222 BLE ports that run BTstack
	 * from a FreeRTOS task. The returned bundle intentionally leaves transport,
	 * chipset, and storage fields empty so a concrete controller port can fill
	 * them later.
	 *
	 * @note The BTstack FreeRTOS run-loop table is not a complete port by
	 * itself. Its implementation also requires the target to provide/link
	 * BTstack's run-loop base implementation, FreeRTOS primitives, and
	 * `hal_time_ms()`.
	 *
	 * @return A table bundle with `run_loop` set to
	 * `btstack_run_loop_freertos_get_instance()` in BTstack-capable builds, or
	 * an empty invalid bundle in non-BTstack builds.
	 */
	static BtstackPortTables CreateWithFreeRtosRunLoop();

	/**
	 * @brief Create a FreeRTOS run-loop table bundle with controller tables.
	 *
	 * This helper is intended for ports that can select all BTstack tables at
	 * construction time. The HCI transport is still externally owned; this
	 * function only stores the selected pointers in the returned bundle.
	 *
	 * @param hci_transport HCI transport used by the BLE controller.
	 * @param hci_transport_config Optional transport-specific configuration.
	 * @param chipset Optional controller chipset driver.
	 * @param tlv Optional TLV storage implementation.
	 * @param tlv_context Optional context pointer for the TLV implementation.
	 * @return A table bundle with the FreeRTOS run loop and supplied controller
	 * pointers.
	 */
	static BtstackPortTables CreateWithFreeRtosRunLoop(
		const void* hci_transport,
		const void* hci_transport_config = nullptr,
		const void* chipset = nullptr,
		const void* tlv = nullptr,
		void* tlv_context = nullptr);

	/**
	 * @brief Check whether this bundle contains at least one configured table or
	 * context pointer.
	 *
	 * @return false when every member is `nullptr`; true otherwise.
	 */
	bool Validate() const {
		return run_loop != nullptr || hci_transport != nullptr ||
			   hci_transport_config != nullptr || chipset != nullptr || tlv != nullptr ||
			   tlv_context != nullptr;
	}
};

/**
 * @struct BtstackUartTransportTables
 * @brief Convenience bundle for controllers using BTstack H4/H5 UART transport.
 *
 * A UART-attached BLE controller can often reuse BTstack's H4 or H5 HCI
 * transport. In that case the board port provides a `btstack_uart_t` driver
 * and selects the matching `hci_transport_t` implementation.
 *
 * This structure is optional. Ports with SPI, USB, vendor-specific firmware
 * mailboxes, or a fully custom HCI transport can use `BtstackPortTables`
 * directly.
 */
struct BtstackUartTransportTables {
	/**
	 * @brief UART driver used by the selected H4/H5 transport.
	 *
	 * The UART driver owns the low-level send/receive callbacks and any DMA,
	 * IRQ, GPIO flow-control, or wakeup state required by the controller.
	 */
	const void* uart = nullptr;

	/**
	 * @brief H4 or H5 HCI transport created for the UART driver.
	 */
	const void* hci_transport = nullptr;

	/**
	 * @brief UART transport configuration, including baud rate and flow-control
	 * settings.
	 */
	const void* hci_transport_config = nullptr;
};

/**
 * @class BtstackPort
 * @brief Base class for platform/controller BTstack ports.
 *
 * `BtstackPort` is the C++ lifecycle object that sits above BTstack's C
 * function tables. It does not implement a controller by itself. Instead, a
 * concrete board/controller port derives from this class, supplies the required
 * `BtstackPortTables`, and performs the initialization sequence required by
 * BTstack and the selected controller.
 *
 * ---
 * ### Expected Initialization Work
 *
	 * A concrete `Init()` implementation typically performs the following steps:
	 *
	 * 1. Initialize BTstack memory.
	 * 2. Call `Apply()` to initialize the selected BTstack run loop, configure
	 *    optional TLV storage, call `hci_init()` with the selected HCI
	 *    transport, and register optional chipset support.
 *
 * The C7222 `Ble` class remains responsible for higher-level BLE setup such as
 * L2CAP, Security Manager, GAP, Attribute Server event registration, and
 * powering the controller on.
 *
 * ---
 * ### FreeRTOS Run Loop
 *
 * FreeRTOS-based boards should normally pass the BTstack FreeRTOS run loop
 * table in `BtstackPortTables::run_loop`; the run loop does not need to be
 * reimplemented by every board. The concrete port still owns board-specific
 * controller state such as UART, DMA, reset pins, interrupt callbacks, and
 * controller wake handling.
 */
class BtstackPort : public NonCopyableNonMovable {
  public:
	/**
	 * @brief Default construction is not allowed.
	 *
	 * A BTstack port is only meaningful when it is created with an explicit set
	 * of BTstack function tables.
	 */
	BtstackPort() = delete;

	/**
	 * @brief Construct a BTstack port wrapper around the selected C tables.
	 * @param tables Non-owning function-table bundle for this port. The bundle
	 * is moved into the port; the pointed-to BTstack tables remain externally
	 * owned and must outlive the port.
	 */
	explicit BtstackPort(BtstackPortTables&& tables)
		: tables_(std::move(tables)) {
	}

	/**
	 * @brief Virtual destructor for derived board/controller ports.
	 */
	virtual ~BtstackPort() = default;

	/**
	 * @brief Initialize the BTstack port.
	 *
	 * A successful call should leave BTstack ready for C7222's `Ble::TurnOn()`
	 * path to initialize BLE layers and call `hci_power_control(HCI_POWER_ON)`.
	 *
	 * @return true when port initialization completed successfully.
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Deinitialize the BTstack port and release controller resources.
	 *
	 * Implementations should be idempotent where practical. This function should
	 * undo port-level initialization but does not replace C7222's higher-level
	 * BLE shutdown sequence.
	 */
	virtual void Deinit() = 0;

	/**
	 * @brief Apply the configured BTstack tables to the BTstack C runtime.
	 *
	 * This function performs the common table-application sequence shared by
	 * BTstack-backed controller ports:
	 *
	 * - `run_loop` is passed to `btstack_run_loop_init()`.
	 * - `tlv` and `tlv_context` are passed to `btstack_tlv_set_instance()` and
	 *   `le_device_db_tlv_configure()`.
	 * - `hci_transport` and `hci_transport_config` are passed to `hci_init()`.
	 * - `chipset` is passed to `hci_set_chipset()` after `hci_init()`.
	 *
	 * `Apply()` does not call `btstack_memory_init()` and does not power on the
	 * controller. Those steps remain the responsibility of the concrete port and
	 * the higher-level `Ble` lifecycle. In non-BTstack builds, `Apply()` is not
	 * compiled into the BLE library unless a platform source provides it.
	 *
	 * @return true when the configured tables were valid and applied; false
	 * when the bundle is empty or contains dependent pointers without the table
	 * that owns them.
	 */
	bool Apply();

	/**
	 * @brief Access the complete non-owning BTstack table bundle.
	 */
	const BtstackPortTables& GetTables() const {
		return tables_;
	}

	/**
	 * @brief Access the configured BTstack run loop table.
	 */
	const void* GetRunLoop() const {
		return tables_.run_loop;
	}

	/**
	 * @brief Access the configured HCI transport table.
	 */
	const void* GetHciTransport() const {
		return tables_.hci_transport;
	}

	/**
	 * @brief Access the optional HCI transport configuration pointer.
	 */
	const void* GetHciTransportConfig() const {
		return tables_.hci_transport_config;
	}

	/**
	 * @brief Access the optional controller chipset driver.
	 */
	const void* GetChipset() const {
		return tables_.chipset;
	}

	/**
	 * @brief Access the optional TLV storage implementation.
	 */
	const void* GetTlv() const {
		return tables_.tlv;
	}

	/**
	 * @brief Access the optional TLV storage context.
	 */
	void* GetTlvContext() const {
		return tables_.tlv_context;
	}

	/**
	 * @brief Check whether the concrete port has marked itself initialized.
	 */
	bool IsInitialized() const {
		return initialized_;
	}

	/**
	 * @brief Validate the configured BTstack table bundle.
	 *
	 * This delegates to `BtstackPortTables::Validate()` and intentionally
	 * performs only the common non-empty check. Concrete ports can add stricter
	 * requirements, such as requiring both a run loop and HCI transport.
	 *
	 * @return false when the port has an entirely empty table bundle; true
	 * otherwise.
	 */
	virtual bool Validate() const {
		return tables_.Validate();
	}

  protected:
	/**
	 * @brief Replace the configured BTstack table bundle.
	 *
	 * Intended for derived classes that finish selecting transport tables during
	 * construction or early initialization. The bundle is moved into the port;
	 * the pointed-to BTstack tables remain externally owned.
	 */
	void SetTables(BtstackPortTables&& tables) {
		tables_ = std::move(tables);
	}

	/**
	 * @brief Update the initialized state after derived-class init/deinit work.
	 */
	void SetInitialized(bool initialized) {
		initialized_ = initialized;
	}

  private:
	BtstackPortTables tables_{};
	bool initialized_ = false;
};

} // namespace c7222

#endif // ELEC_C7222_BLE_BTSTACK_PORT_HPP
