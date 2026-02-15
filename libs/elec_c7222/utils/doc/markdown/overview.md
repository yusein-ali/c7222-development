# Utils Overview

This document summarizes the `libs/elec_c7222/utils` utilities layer. These helpers provide small, reusable building blocks used across the course codebase.

## Purpose

The utilities module exists to:

1. Provide safe base classes for copy/move semantics.
2. Provide lightweight C++ utility primitives used by multiple library modules.

## Utility Classes

### `NonCopyable`, `NonMovable`, `NonCopyableNonMovable`, `MovableOnly`, `CopyableOnly`

Location: `libs/elec_c7222/utils/include/non_copyable.hpp`

Purpose:
1. `NonCopyable`: disables copy operations while allowing move.
2. `NonMovable`: disables move operations while allowing copy.
3. `NonCopyableNonMovable`: disables both copy and move.
4. `MovableOnly`: allows move but forbids copy (convenience base).
5. `CopyableOnly`: allows copy but forbids move (convenience base).

These base classes are intended for hardware handles, singleton-like services, and objects that must not be duplicated or relocated.

