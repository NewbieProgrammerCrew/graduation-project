#include "pch.h"
#include "Allocator.h"

/*------------------
	BaseAllocator
------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
