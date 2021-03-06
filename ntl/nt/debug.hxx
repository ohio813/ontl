/**\file*********************************************************************
 *                                                                     \brief
 *  Debugger support
 *
 ****************************************************************************
 */
#ifndef NTL__NT_DEBUG
#define NTL__NT_DEBUG
#pragma once

#include "basedef.hxx"
#include "string.hxx"
#include "../stlx/cstdlib.hxx"

namespace ntl {
  namespace nt {

    struct dpfltr
    {
      enum type
      {
        ihvdriver     = 77,
        ihvideo       = 78,
        ihvaudio      = 79,
        ihvnetwork    = 80,
        ihvstreaming  = 81,
        ihvbus        = 82,
      };

      enum level
      {
        error   = 0,
        warning = 2,
        trace   = 3,
        info    = 4,
        __identifier(default) = error
      };
    };

    NTL__EXTERNAPI
      ntstatus
      DbgPrint(
      const char  Format[],
      ...
      );

    NTL__EXTERNAPI
      ntstatus
      DbgPrintEx(
      dpfltr::type  ComponentId,
      dpfltr::level Level,
      const char    Format[],
      ...
      );



    //warning C4100: 'XXX' : unreferenced formal parameter
#pragma warning(push)
#pragma warning(disable:4100)

#ifdef NTL__DEBUG
#	define KdBreakPoint() intrinsic::debugbreak()
# ifndef KdPrint
#	  define KdPrint(X) DbgPrint X
# endif
# ifndef KdPrintEx
#	  define KdPrintEx(X) DbgPrintEx X
# endif
#else
#	define KdBreakPoint()
# ifndef KdPrint
# 	define KdPrint(X) __noop
# endif
# ifndef KdPrintEx
# 	define KdPrintEx(X) __noop
# endif
#endif

    template <dpfltr::level DefaultLevel  = dpfltr::__identifier(default),
      dpfltr::type  Type          = dpfltr::ihvdriver>
    struct dbgprint
    {
      static const dpfltr::type type = Type;

      void printf(const char msg[]) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((msg));
#else
        KdPrintEx((type, DefaultLevel, msg));
#endif
      }

      template <typename T>
      void printf(const char format[], T arg) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((format, arg));
#else
        KdPrintEx((type, DefaultLevel, format, arg));
#endif
      }

      template <typename T1, typename T2>
      void printf(const char format[], T1 arg1, T2 arg2) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((format, arg1, arg2));
#else
        KdPrintEx((type, DefaultLevel, format, arg1, arg2));
#endif
      }

      template <typename T1, typename T2, typename T3>
      void printf(const char format[], T1 arg1, T2 arg2, T3 arg3) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((format, arg1, arg2, arg3));
#else
        KdPrintEx((type, DefaultLevel, format, arg1, arg2, arg3));
#endif
      }

      template <typename T1, typename T2, typename T3, typename T4>
      void printf(const char format[], T1 arg1, T2 arg2, T3 arg3, T4 arg4) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((format, arg1, arg2, arg3, arg4));
#else
        KdPrintEx((type, DefaultLevel, format, arg1, arg2, arg3, arg4));
#endif
      }

      template <typename T1, typename T2, typename T3, typename T4, typename T5>
      void printf(const char format[], T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) const
      {
#ifdef NTL__DEBUG_2K
        KdPrint((format, arg1, arg2, arg3, arg4, arg5));
#else
        KdPrintEx((type, DefaultLevel, format, arg1, arg2, arg3, arg4, arg5));
#endif
      }

      void operator()(const char msg[]) const
      {
        printf(msg);
      }

      void operator()(const wchar_t msg[]) const
      {
        printf("%ws", msg);
      }

      void operator()(const char msg[], int i) const
      {
        printf("%s%i", msg, i);
      }

      void operator()(const char msg[], unsigned u) const
      {
        printf("%s%u", msg, u);
      }

      void operator()(const char msg[], long i) const
      {
        printf("%s%i", msg, i);
      }

      void operator()(const char msg[], int64_t i) const
      {
        printf("%s%I64i", msg, i);
      }

      void operator()(const char msg[], uint64_t i) const
      {
        printf("%s%I64u", msg, i);
      }

      void operator()(const char msg[], unsigned long u) const
      {
        printf("%s%u", msg, u);
      }

      void operator()(const char msg[], const void* p) const
      {
        printf("%s%p", msg, p);
      }

      void operator()(const char msg[], char c) const
      {
        printf("%s`%c'", msg, c);
      }

      void operator()(char c) const
      {
        printf("%c", c);
      }

      void operator()(const char msg[], wchar_t c) const
      {
        printf("%s`%C'", msg, c);
      }

      void operator()(const char msg[], const char msg2[]) const
      {
        printf("%s`%s'", msg, msg2);
      }

      void operator()(const char msg[], const wchar_t msg2[]) const
      {
        printf("%s`%S'", msg, msg2);
      }

      void operator()(const char msg[], const const_unicode_string & msg2) const
      {
        printf("%s`%wZ'", msg, &msg2);
      }

      void operator()(const char msg[], const unicode_string & msg2) const
      {
        printf("%s`%wZ'", msg, &msg2);
      }
    };

  } // namespace nt
  namespace intrinsic {
#ifndef __ICL
    extern "C" void __debugbreak();

#ifdef _MSC_VER
# pragma intrinsic(__debugbreak)
#endif
#endif

    static __forceinline
      void debugbreak()
    {
      __debugbreak();
    }



  }//namespace intrinsic
  namespace nt {

#pragma warning(pop)

    namespace dbg {

#ifdef NTL_DBG_FILTER
      const dbgprint<dpfltr::__identifier(default)>   error;
      const dbgprint<dpfltr::warning> warning;
      const dbgprint<dpfltr::trace>   trace;
      const dbgprint<dpfltr::info>    info;
  #else
  #ifndef __BCPLUSPLUS__
      const dbgprint<dpfltr::__identifier(default)>   error;
      const dbgprint<dpfltr::__identifier(default)>   warning;
      const dbgprint<dpfltr::__identifier(default)>   trace;
      const dbgprint<dpfltr::__identifier(default)>   info;
  #else
      const dbgprint<dpfltr::__identifier(default)>   error = {};
      const dbgprint<dpfltr::__identifier(default)>   warning = {};
      const dbgprint<dpfltr::__identifier(default)>   trace = {};
      const dbgprint<dpfltr::__identifier(default)>   info = {};
  #endif
#endif

      static __forceinline
        void bp()
      {
      #ifdef NTL__DEBUG
        intrinsic::debugbreak();
      #endif
      }

    }//namespace dbg


  }//namespace nt
}//namespace ntl

#ifdef _M_X64
# pragma comment(linker, "/alternatename:abort=debug_abort")
#else
# pragma comment(linker, "/alternatename:_abort=_debug_abort")
#endif

extern "C" inline void __cdecl debug_abort()
{
  ntl::intrinsic::debugbreak();
}

//static const void* __dummyref = reinterpret_cast<const void*>(debug_abort);

#endif // NTL__NT_DEBUG
