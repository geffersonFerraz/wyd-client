// Compilado apenas no build do Linux (CMakeLists.txt); nao faz parte do TMProject.vcxproj.
// O codigo descompilado tem membros sem inicializacao que dependem de memoria zerada,
// como ponteiros testados com if (m_pX). Sob o Wine, a memoria reaproveitada vem com lixo.
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <new>

void* operator new(std::size_t size)
{
	void* p = std::calloc(1, size ? size : 1);
	if (!p)
		throw std::bad_alloc();
	return p;
}

void* operator new[](std::size_t size)
{
	return ::operator new(size);
}

void* operator new(std::size_t size, std::align_val_t alignment)
{
	void* p = _aligned_malloc(size ? size : 1, static_cast<std::size_t>(alignment));
	if (!p)
		throw std::bad_alloc();
	std::memset(p, 0, size);
	return p;
}

void* operator new[](std::size_t size, std::align_val_t alignment)
{
	return ::operator new(size, alignment);
}

void operator delete(void* p, std::align_val_t) noexcept { _aligned_free(p); }
void operator delete[](void* p, std::align_val_t) noexcept { _aligned_free(p); }
void operator delete(void* p, std::size_t, std::align_val_t) noexcept { _aligned_free(p); }
void operator delete[](void* p, std::size_t, std::align_val_t) noexcept { _aligned_free(p); }
