#ifndef STLX_CONFIG_BCB
#define STLX_CONFIG_BCB

# define __forceinline inline
# define __assume(X)
# define __restrict
# define __thiscall
# define __noalias
# define __w64

#ifndef NTL__CRTCALL
# ifdef NTL__STLX_FORCE_CDECL
#define NTL__CRTCALL __cdecl
# else
#define NTL__CRTCALL
# endif
#endif


#define NTL__EXTERNAPI extern "C" __declspec(dllimport) __declspec(nothrow)
#define NTL__EXTERNVAR extern "C" __declspec(dllimport)
#define NTL__NORETURN __declspec(noreturn)

#ifndef STATIC_ASSERT
# define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
#endif

#ifdef _M_X64
# define __SIZEOF_POINTER__ 8
#else
# define __SIZEOF_POINTER__ 4
#endif // x64


#if __cplusplus <= 199711L

/** BCB's partial C++0x support */

// keywords:
// align (N2798+)
//#define NTL__CXX_ALIGN
// alignas( <= N2723)
//#define NTL__CXX_ALIGNAS
// alignof
#define NTL__CXX_ALIGNOF
// auto
//#define NTL__CXX_AUTO
// char16_t, char32_t
#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
// class enum
#define NTL__CXX_ENUM
// nullptr
//#define NTL__CXX_NULLPTR
// static assert
#define NTL__CXX_ASSERT
// thread_local
//#define NTL__CXX_THREADL

// syntax:
// explicit delete/default function definition
//#define NTL__CXX_EF
// explicit conversion operators
//#define NTL__CXX_EXPLICITOP
// extern templates
#define NTL__CXX_EXTPL
// initializer lists
//#define NTL__CXX_IL
// lambda
//#define NTL__CXX_LAMBDA
// rvalues
#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT
#endif 

#endif // STLX_CONFIG_BCB
