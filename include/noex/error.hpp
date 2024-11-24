#pragma once

namespace noex {

enum ErrorNo : int {
    OK = 0,
    STR_ALLOCATION_ERROR,  // Failed to allocate memory for string.
    STR_BOUNDARY_ERROR,  // Accessed out-of-bounds with substr or [].
    STR_FORMAT_ERROR,  // Failed to convert number to string.
    VEC_ALLOCATION_ERROR,
    VEC_BOUNDARY_ERROR,
    EXTERNAL_ALLOCATION_ERROR,  // User defined allocation errors.
    EXTERNAL_BOUDARY_ERROR,  // User defined boundary errors.
    EXTERNAL_ERROR,  // User defined errors.
    ERROR_MAX,
};

// Returns the error status for tuwString.
ErrorNo GetErrorNo() noexcept;

void SetErrorNo(ErrorNo err) noexcept;

// Set noex::OK to the error status.
void ClearErrorNo() noexcept;

}  // namespace noex
