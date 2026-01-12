/**
 * @file non_copyable.hpp
 * @brief Base classes to control copy/move semantics.
 *
 * These types are intended to be used as lightweight base classes to constrain
 * how derived types can be copied or moved.
 */
#ifndef BLE_NON_COPYABLE_HPP
#define BLE_NON_COPYABLE_HPP

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 */
namespace c7222 {

/**
 * @class NonCopyable
 * @brief Disable copy operations while keeping move operations enabled.
 *
 * Intended use:
 * - Use as a base class when a type owns a unique resource (hardware handle,
 *   mutex, file descriptor) and copying would be unsafe or ambiguous.
 *
 * Base class usage:
 * - Inherit publicly if you want derived types to be non-copyable but movable.
 */
class NonCopyable {
  protected:
	NonCopyable() = default;
	~NonCopyable() = default;

  public:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&&) = default;
};

/**
 * @class NonMovable
 * @brief Disable move operations while keeping copy operations enabled.
 *
 * Intended use:
 * - Use as a base class when the object must stay at a stable address (e.g.,
 *   it registers its `this` pointer with hardware/RTOS or exposes internal
 *   pointers that must remain valid).
 *
 * Base class usage:
 * - Inherit publicly if you want derived types to be copyable but not movable.
 */
class NonMovable {
  protected:
	NonMovable() = default;
	~NonMovable() = default;

  public:
	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;
	NonMovable(NonMovable&&) = delete;
	NonMovable& operator=(NonMovable&&) = delete;
};

/**
 * @class NonCopyableNonMovable
 * @brief Disable both copy and move operations.
 *
 * Intended use:
 * - Use as a base class for singleton-like services, hardware peripherals, or
 *   any object that should never be duplicated or relocated.
 *
 * Base class usage:
 * - Inherit publicly when a derived type must be neither copyable nor movable.
 */
class NonCopyableNonMovable : public NonCopyable, public NonMovable {
  protected:
	NonCopyableNonMovable() = default;
	~NonCopyableNonMovable() = default;
};

/**
 * @class MovableOnly
 * @brief Convenience base that allows move but forbids copy.
 *
 * Intended use:
 * - Use as a base class for types that are transferable but must not be copied.
 *
 * Base class usage:
 * - Inherit publicly to advertise move-only semantics in derived classes.
 */
class MovableOnly : public NonCopyable {
  public:
	// Re-enable default move operations
	MovableOnly(MovableOnly&&) = default;
	MovableOnly& operator=(MovableOnly&&) = default;

	// Default constructor is required because defining move ops hides it
	MovableOnly() = default;
};

/**
 * @class CopyableOnly
 * @brief Convenience base that allows copy but forbids move.
 *
 * Intended use:
 * - Use as a base class for types that must remain at a stable address but can
 *   be duplicated safely (e.g., configuration objects that are registered by
 *   reference elsewhere).
 *
 * Base class usage:
 * - Inherit publicly to advertise copy-only semantics in derived classes.
 */
class CopyableOnly : public NonMovable {
  public:
	// Re-enable default copy operations
	CopyableOnly(const CopyableOnly&) = default;
	CopyableOnly& operator=(const CopyableOnly&) = default;

	// Default constructor is required because defining copy ops hides it
	CopyableOnly() = default;
};
} // namespace c7222

#endif // BLE_NON_COPYABLE_HPP
