// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

/**
* Makes a type stack-only by making it non-copyable, non-movable by deleting copy/move constructors and assignment/move 
* operators, and by deleting the 'new' and 'delete' operators.
* 
* The macro should be placed in the public section of the type for better compiler diagnostic messages.
* Example usage:
*
*    class FMyClassName
*    {
*    public:
*        PSI_STACK_ONLY(FMyClassName)
*        FMyClassName() = default;
*    };
*    
* Based on: UE_NONCOPYABLE
*/
#define PSI_STACK_ONLY(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName(TypeName&&) = delete; \
    TypeName& operator=(const TypeName&) = delete; \
    TypeName& operator=(TypeName&&) = delete; \
    void* operator new(size_t) = delete; \
    void* operator new[](size_t) = delete; \
    void operator delete(void*) = delete; \
    void operator delete[](void*) = delete;