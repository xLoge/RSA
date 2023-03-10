#pragma once

#ifndef _RSA_
#define _RSA_

/* gmpxx.h -- C++ class wrapper for GMP types.  -*- C++ -*-

Copyright 2001-2003, 2006, 2008, 2011-2015, 2018 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#ifndef __GMP_PLUSPLUS__
#define __GMP_PLUSPLUS__

#include <iosfwd>

#include <cstring>  /* for strlen */
#include <limits>  /* numeric_limits */
#include <utility>
#include <algorithm>  /* swap */
#include <string>
#include <stdexcept>
#include <cfloat>
/* Definitions for GNU multiple precision functions.   -*- mode: c -*-

Copyright 1991, 1993-1997, 1999-2016, 2020 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#ifndef __GMP_H__

#if defined (__cplusplus)
#include <iosfwd>   /* for std::istream, std::ostream, std::string */
#include <cstdio>
#endif


/* Instantiated by configure. */
#if ! defined (__GMP_WITHIN_CONFIGURE)
#define __GMP_HAVE_HOST_CPU_FAMILY_power   0
#define __GMP_HAVE_HOST_CPU_FAMILY_powerpc 0
#define GMP_LIMB_BITS                      64
#define GMP_NAIL_BITS                      0
#endif
#define GMP_NUMB_BITS     (GMP_LIMB_BITS - GMP_NAIL_BITS)
#define GMP_NUMB_MASK     ((~ __GMP_CAST (mp_limb_t, 0)) >> GMP_NAIL_BITS)
#define GMP_NUMB_MAX      GMP_NUMB_MASK
#define GMP_NAIL_MASK     (~ GMP_NUMB_MASK)


#ifndef __GNU_MP__
#define __GNU_MP__ 6

#include <stddef.h>    /* for size_t */
#include <limits.h>

/* Instantiated by configure. */
#if ! defined (__GMP_WITHIN_CONFIGURE)
#define _LONG_LONG_LIMB 1
#define __GMP_LIBGMP_DLL  1
#endif


/* __GMP_DECLSPEC supports Windows DLL versions of libgmp, and is empty in
   all other circumstances.

   When compiling objects for libgmp, __GMP_DECLSPEC is an export directive,
   or when compiling for an application it's an import directive.  The two
   cases are differentiated by __GMP_WITHIN_GMP defined by the GMP Makefiles
   (and not defined from an application).

   __GMP_DECLSPEC_XX is similarly used for libgmpxx.  __GMP_WITHIN_GMPXX
   indicates when building libgmpxx, and in that case libgmpxx functions are
   exports, but libgmp functions which might get called are imports.

   Libtool DLL_EXPORT define is not used.

   There's no attempt to support GMP built both static and DLL.  Doing so
   would mean applications would have to tell us which of the two is going
   to be used when linking, and that seems very tedious and error prone if
   using GMP by hand, and equally tedious from a package since autoconf and
   automake don't give much help.

   __GMP_DECLSPEC is required on all documented global functions and
   variables, the various internals in gmp-impl.h etc can be left unadorned.
   But internals used by the test programs or speed measuring programs
   should have __GMP_DECLSPEC, and certainly constants or variables must
   have it or the wrong address will be resolved.

   In gcc __declspec can go at either the start or end of a prototype.

   In Microsoft C __declspec must go at the start, or after the type like
   void __declspec(...) *foo()".  There's no __dllexport or anything to
   guard against someone foolish #defining dllexport.  _export used to be
   available, but no longer.

   In Borland C _export still exists, but needs to go after the type, like
   "void _export foo();".  Would have to change the __GMP_DECLSPEC syntax to
   make use of that.  Probably more trouble than it's worth.  */

#if defined (__GNUC__)
#define __GMP_DECLSPEC_EXPORT  __declspec(__dllexport__)
#define __GMP_DECLSPEC_IMPORT  __declspec(__dllimport__)
#endif
#if defined (_MSC_VER) || defined (__BORLANDC__)
#define __GMP_DECLSPEC_EXPORT  __declspec(dllexport)
#define __GMP_DECLSPEC_IMPORT  __declspec(dllimport)
#endif
#ifdef __WATCOMC__
#define __GMP_DECLSPEC_EXPORT  __export
#define __GMP_DECLSPEC_IMPORT  __import
#endif
#ifdef __IBMC__
#define __GMP_DECLSPEC_EXPORT  _Export
#define __GMP_DECLSPEC_IMPORT  _Import
#endif

#if __GMP_LIBGMP_DLL
#ifdef __GMP_WITHIN_GMP
   /* compiling to go into a DLL libgmp */
#define __GMP_DECLSPEC  __GMP_DECLSPEC_EXPORT
#else
   /* compiling to go into an application which will link to a DLL libgmp */
#define __GMP_DECLSPEC  __GMP_DECLSPEC_IMPORT
#endif
#else
   /* all other cases */
#define __GMP_DECLSPEC
#endif


#ifdef __GMP_SHORT_LIMB
typedef unsigned int		mp_limb_t;
typedef int			mp_limb_signed_t;
#else
#ifdef _LONG_LONG_LIMB
typedef unsigned long long int	mp_limb_t;
typedef long long int		mp_limb_signed_t;
#else
typedef unsigned long int	mp_limb_t;
typedef long int		mp_limb_signed_t;
#endif
#endif
typedef unsigned long int	mp_bitcnt_t;

/* For reference, note that the name __mpz_struct gets into C++ mangled
   function names, which means although the "__" suggests an internal, we
   must leave this name for binary compatibility.  */
typedef struct
{
    int _mp_alloc;		/* Number of *limbs* allocated and pointed
                     to by the _mp_d field.  */
    int _mp_size;			/* abs(_mp_size) is the number of limbs the
                     last field points to.  If _mp_size is
                     negative this is a negative number.  */
    mp_limb_t* _mp_d;		/* Pointer to the limbs.  */
} __mpz_struct;

#endif /* __GNU_MP__ */


typedef __mpz_struct MP_INT;    /* gmp 1 source compatibility */
typedef __mpz_struct mpz_t[1];

typedef mp_limb_t* mp_ptr;
typedef const mp_limb_t* mp_srcptr;
#if defined (_CRAY) && ! defined (_CRAYMPP)
/* plain `int' is much faster (48 bits) */
#define __GMP_MP_SIZE_T_INT     1
typedef int			mp_size_t;
typedef int			mp_exp_t;
#else
#define __GMP_MP_SIZE_T_INT     0
typedef long int		mp_size_t;
typedef long int		mp_exp_t;
#endif

typedef struct
{
    __mpz_struct _mp_num;
    __mpz_struct _mp_den;
} __mpq_struct;

typedef __mpq_struct MP_RAT;    /* gmp 1 source compatibility */
typedef __mpq_struct mpq_t[1];

typedef struct
{
    int _mp_prec;			/* Max precision, in number of `mp_limb_t's.
                     Set by mpf_init and modified by
                     mpf_set_prec.  The area pointed to by the
                     _mp_d field contains `prec' + 1 limbs.  */
    int _mp_size;			/* abs(_mp_size) is the number of limbs the
                     last field points to.  If _mp_size is
                     negative this is a negative number.  */
    mp_exp_t _mp_exp;		/* Exponent, in the base of `mp_limb_t'.  */
    mp_limb_t* _mp_d;		/* Pointer to the limbs.  */
} __mpf_struct;

/* typedef __mpf_struct MP_FLOAT; */
typedef __mpf_struct mpf_t[1];

/* Available random number generation algorithms.  */
typedef enum
{
    GMP_RAND_ALG_DEFAULT = 0,
    GMP_RAND_ALG_LC = GMP_RAND_ALG_DEFAULT /* Linear congruential.  */
} gmp_randalg_t;

/* Random state struct.  */
typedef struct
{
    mpz_t _mp_seed;	  /* _mp_d member points to state of the generator. */
    gmp_randalg_t _mp_alg;  /* Currently unused. */
    union {
        void* _mp_lc;         /* Pointer to function pointers structure.  */
    } _mp_algdata;
} __gmp_randstate_struct;
typedef __gmp_randstate_struct gmp_randstate_t[1];

/* Types for function declarations in gmp files.  */
/* ??? Should not pollute user name space with these ??? */
typedef const __mpz_struct* mpz_srcptr;
typedef __mpz_struct* mpz_ptr;
typedef const __mpf_struct* mpf_srcptr;
typedef __mpf_struct* mpf_ptr;
typedef const __mpq_struct* mpq_srcptr;
typedef __mpq_struct* mpq_ptr;


#if __GMP_LIBGMP_DLL
#ifdef __GMP_WITHIN_GMPXX
/* compiling to go into a DLL libgmpxx */
#define __GMP_DECLSPEC_XX  __GMP_DECLSPEC_EXPORT
#else
/* compiling to go into a application which will link to a DLL libgmpxx */
#define __GMP_DECLSPEC_XX  __GMP_DECLSPEC_IMPORT
#endif
#else
/* all other cases */
#define __GMP_DECLSPEC_XX
#endif


#ifndef __MPN
#define __MPN(x) __gmpn_##x
#endif

/* For reference, "defined(EOF)" cannot be used here.  In g++ 2.95.4,
   <iostream> defines EOF but not FILE.  */
#if defined (FILE)                                              \
  || defined (H_STDIO)                                          \
  || defined (_H_STDIO)               /* AIX */                 \
  || defined (_STDIO_H)               /* glibc, Sun, SCO */     \
  || defined (_STDIO_H_)              /* BSD, OSF */            \
  || defined (__STDIO_H)              /* Borland */             \
  || defined (__STDIO_H__)            /* IRIX */                \
  || defined (_STDIO_INCLUDED)        /* HPUX */                \
  || defined (__dj_include_stdio_h_)  /* DJGPP */               \
  || defined (_FILE_DEFINED)          /* Microsoft */           \
  || defined (__STDIO__)              /* Apple MPW MrC */       \
  || defined (_MSL_STDIO_H)           /* Metrowerks */          \
  || defined (_STDIO_H_INCLUDED)      /* QNX4 */		\
  || defined (_ISO_STDIO_ISO_H)       /* Sun C++ */		\
  || defined (__STDIO_LOADED)         /* VMS */			\
  || defined (__DEFINED_FILE)         /* musl */
#define _GMP_H_HAVE_FILE 1
#endif

   /* In ISO C, if a prototype involving "struct obstack *" is given without
      that structure defined, then the struct is scoped down to just the
      prototype, causing a conflict if it's subsequently defined for real.  So
      only give prototypes if we've got obstack.h.  */
#if defined (_OBSTACK_H)   /* glibc <obstack.h> */
#define _GMP_H_HAVE_OBSTACK 1
#endif

      /* The prototypes for gmp_vprintf etc are provided only if va_list is defined,
         via an application having included <stdarg.h>.  Usually va_list is a typedef
         so can't be tested directly, but C99 specifies that va_start is a macro.

         <stdio.h> will define some sort of va_list for vprintf and vfprintf, but
         let's not bother trying to use that since it's not standard and since
         application uses for gmp_vprintf etc will almost certainly require the
         whole <stdarg.h> anyway.  */

#ifdef va_start
#define _GMP_H_HAVE_VA_LIST 1
#endif

         /* Test for gcc >= maj.min, as per __GNUC_PREREQ in glibc */
#if defined (__GNUC__) && defined (__GNUC_MINOR__)
#define __GMP_GNUC_PREREQ(maj, min) \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GMP_GNUC_PREREQ(maj, min)  0
#endif

/* "pure" is in gcc 2.96 and up, see "(gcc)Function Attributes".  Basically
   it means a function does nothing but examine its arguments and memory
   (global or via arguments) to generate a return value, but changes nothing
   and has no side-effects.  __GMP_NO_ATTRIBUTE_CONST_PURE lets
   tune/common.c etc turn this off when trying to write timing loops.  */
#if __GMP_GNUC_PREREQ (2,96) && ! defined (__GMP_NO_ATTRIBUTE_CONST_PURE)
#define __GMP_ATTRIBUTE_PURE   __attribute__ ((__pure__))
#else
#define __GMP_ATTRIBUTE_PURE
#endif


   /* __GMP_CAST allows us to use static_cast in C++, so our macros are clean
      to "g++ -Wold-style-cast".

      Casts in "extern inline" code within an extern "C" block don't induce
      these warnings, so __GMP_CAST only needs to be used on documented
      macros.  */

#ifdef __cplusplus
#define __GMP_CAST(type, expr)  (static_cast<type> (expr))
#else
#define __GMP_CAST(type, expr)  ((type) (expr))
#endif


      /* An empty "throw ()" means the function doesn't throw any C++ exceptions,
         this can save some stack frame info in applications.

         Currently it's given only on functions which never divide-by-zero etc,
         don't allocate memory, and are expected to never need to allocate memory.
         This leaves open the possibility of a C++ throw from a future GMP
         exceptions scheme.

         mpz_set_ui etc are omitted to leave open the lazy allocation scheme
         described in doc/tasks.html.  mpz_get_d etc are omitted to leave open
         exceptions for float overflows.

         Note that __GMP_NOTHROW must be given on any inlines the same as on their
         prototypes (for g++ at least, where they're used together).  Note also
         that g++ 3.0 demands that __GMP_NOTHROW is before other attributes like
         __GMP_ATTRIBUTE_PURE.  */

#if defined (__cplusplus)
#if __cplusplus >= 201103L
#define __GMP_NOTHROW  noexcept
#else
#define __GMP_NOTHROW  throw ()
#endif
#else
#define __GMP_NOTHROW
#endif


         /* PORTME: What other compilers have a useful "extern inline"?  "static
            inline" would be an acceptable substitute if the compiler (or linker)
            discards unused statics.  */

            /* gcc has __inline__ in all modes, including strict ansi.  Give a prototype
               for an inline too, so as to correctly specify "dllimport" on windows, in
               case the function is called rather than inlined.
               GCC 4.3 and above with -std=c99 or -std=gnu99 implements ISO C99
               inline semantics, unless -fgnu89-inline is used.  */
#ifdef __GNUC__
#if (defined __GNUC_STDC_INLINE__) || (__GNUC__ == 4 && __GNUC_MINOR__ == 2) \
  || (defined __GNUC_GNU_INLINE__ && defined __cplusplus)
#define __GMP_EXTERN_INLINE extern __inline__ __attribute__ ((__gnu_inline__))
#else
#define __GMP_EXTERN_INLINE      extern __inline__
#endif
#define __GMP_INLINE_PROTOTYPES  1
#endif

               /* DEC C (eg. version 5.9) supports "static __inline foo()", even in -std1
                  strict ANSI mode.  Inlining is done even when not optimizing (ie. -O0
                  mode, which is the default), but an unnecessary local copy of foo is
                  emitted unless -O is used.  "extern __inline" is accepted, but the
                  "extern" appears to be ignored, ie. it becomes a plain global function
                  but which is inlined within its file.  Don't know if all old versions of
                  DEC C supported __inline, but as a start let's do the right thing for
                  current versions.  */
#ifdef __DECC
#define __GMP_EXTERN_INLINE  static __inline
#endif

                  /* SCO OpenUNIX 8 cc supports "static inline foo()" but not in -Xc strict
                     ANSI mode (__STDC__ is 1 in that mode).  Inlining only actually takes
                     place under -O.  Without -O "foo" seems to be emitted whether it's used
                     or not, which is wasteful.  "extern inline foo()" isn't useful, the
                     "extern" is apparently ignored, so foo is inlined if possible but also
                     emitted as a global, which causes multiple definition errors when
                     building a shared libgmp.  */
#ifdef __SCO_VERSION__
#if __SCO_VERSION__ > 400000000 && __STDC__ != 1 \
  && ! defined (__GMP_EXTERN_INLINE)
#define __GMP_EXTERN_INLINE  static inline
#endif
#endif

                     /* Microsoft's C compiler accepts __inline */
#ifdef _MSC_VER
#define __GMP_EXTERN_INLINE  static __inline
#endif

/* Recent enough Sun C compilers want "inline" */
#if defined (__SUNPRO_C) && __SUNPRO_C >= 0x560 \
  && ! defined (__GMP_EXTERN_INLINE)
#define __GMP_EXTERN_INLINE  inline
#endif

/* Somewhat older Sun C compilers want "static inline" */
#if defined (__SUNPRO_C) && __SUNPRO_C >= 0x540 \
  && ! defined (__GMP_EXTERN_INLINE)
#define __GMP_EXTERN_INLINE  static inline
#endif


/* C++ always has "inline" and since it's a normal feature the linker should
   discard duplicate non-inlined copies, or if it doesn't then that's a
   problem for everyone, not just GMP.  */
#if defined (__cplusplus) && ! defined (__GMP_EXTERN_INLINE)
#define __GMP_EXTERN_INLINE  inline
#endif

   /* Don't do any inlining within a configure run, since if the compiler ends
      up emitting copies of the code into the object file it can end up
      demanding the various support routines (like mpn_popcount) for linking,
      making the "alloca" test and perhaps others fail.  And on hppa ia64 a
      pre-release gcc 3.2 was seen not respecting the "extern" in "extern
      __inline__", triggering this problem too.  */
#if defined (__GMP_WITHIN_CONFIGURE) && ! __GMP_WITHIN_CONFIGURE_INLINE
#undef __GMP_EXTERN_INLINE
#endif

      /* By default, don't give a prototype when there's going to be an inline
         version.  Note in particular that Cray C++ objects to the combination of
         prototype and inline.  */
#ifdef __GMP_EXTERN_INLINE
#ifndef __GMP_INLINE_PROTOTYPES
#define __GMP_INLINE_PROTOTYPES  0
#endif
#else
#define __GMP_INLINE_PROTOTYPES  1
#endif


#define __GMP_ABS(x)   ((x) >= 0 ? (x) : -(x))
#define __GMP_MAX(h,i) ((h) > (i) ? (h) : (i))


         /* __builtin_expect is in gcc 3.0, and not in 2.95. */
#if __GMP_GNUC_PREREQ (3,0)
#define __GMP_LIKELY(cond)    __builtin_expect ((cond) != 0, 1)
#define __GMP_UNLIKELY(cond)  __builtin_expect ((cond) != 0, 0)
#else
#define __GMP_LIKELY(cond)    (cond)
#define __GMP_UNLIKELY(cond)  (cond)
#endif

#ifdef _CRAY
#define __GMP_CRAY_Pragma(str)  _Pragma (str)
#else
#define __GMP_CRAY_Pragma(str)
#endif


/* Allow direct user access to numerator and denominator of an mpq_t object.  */
#define mpq_numref(Q) (&((Q)->_mp_num))
#define mpq_denref(Q) (&((Q)->_mp_den))


#if defined (__cplusplus)
extern "C" {
    using std::FILE;
#endif

#define mp_set_memory_functions __gmp_set_memory_functions
    __GMP_DECLSPEC void mp_set_memory_functions(void* (*) (size_t),
        void* (*) (void*, size_t, size_t),
        void (*) (void*, size_t)) __GMP_NOTHROW;

#define mp_get_memory_functions __gmp_get_memory_functions
    __GMP_DECLSPEC void mp_get_memory_functions(void* (**) (size_t),
        void* (**) (void*, size_t, size_t),
        void (**) (void*, size_t)) __GMP_NOTHROW;

#define mp_bits_per_limb __gmp_bits_per_limb
    __GMP_DECLSPEC extern const int mp_bits_per_limb;

#define gmp_errno __gmp_errno
    __GMP_DECLSPEC extern int gmp_errno;

#define gmp_version __gmp_version
    __GMP_DECLSPEC extern const char* const gmp_version;


    /**************** Random number routines.  ****************/

    /* obsolete */
#define gmp_randinit __gmp_randinit
    __GMP_DECLSPEC void gmp_randinit(gmp_randstate_t, gmp_randalg_t, ...);

#define gmp_randinit_default __gmp_randinit_default
    __GMP_DECLSPEC void gmp_randinit_default(gmp_randstate_t);

#define gmp_randinit_lc_2exp __gmp_randinit_lc_2exp
    __GMP_DECLSPEC void gmp_randinit_lc_2exp(gmp_randstate_t, mpz_srcptr, unsigned long int, mp_bitcnt_t);

#define gmp_randinit_lc_2exp_size __gmp_randinit_lc_2exp_size
    __GMP_DECLSPEC int gmp_randinit_lc_2exp_size(gmp_randstate_t, mp_bitcnt_t);

#define gmp_randinit_mt __gmp_randinit_mt
    __GMP_DECLSPEC void gmp_randinit_mt(gmp_randstate_t);

#define gmp_randinit_set __gmp_randinit_set
    __GMP_DECLSPEC void gmp_randinit_set(gmp_randstate_t, const __gmp_randstate_struct*);

#define gmp_randseed __gmp_randseed
    __GMP_DECLSPEC void gmp_randseed(gmp_randstate_t, mpz_srcptr);

#define gmp_randseed_ui __gmp_randseed_ui
    __GMP_DECLSPEC void gmp_randseed_ui(gmp_randstate_t, unsigned long int);

#define gmp_randclear __gmp_randclear
    __GMP_DECLSPEC void gmp_randclear(gmp_randstate_t);

#define gmp_urandomb_ui __gmp_urandomb_ui
    __GMP_DECLSPEC unsigned long gmp_urandomb_ui(gmp_randstate_t, unsigned long);

#define gmp_urandomm_ui __gmp_urandomm_ui
    __GMP_DECLSPEC unsigned long gmp_urandomm_ui(gmp_randstate_t, unsigned long);


    /**************** Formatted output routines.  ****************/

#define gmp_asprintf __gmp_asprintf
    __GMP_DECLSPEC int gmp_asprintf(char**, const char*, ...);

#define gmp_fprintf __gmp_fprintf
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC int gmp_fprintf(FILE*, const char*, ...);
#endif

#define gmp_obstack_printf __gmp_obstack_printf
#if defined (_GMP_H_HAVE_OBSTACK)
    __GMP_DECLSPEC int gmp_obstack_printf(struct obstack*, const char*, ...);
#endif

#define gmp_obstack_vprintf __gmp_obstack_vprintf
#if defined (_GMP_H_HAVE_OBSTACK) && defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_obstack_vprintf(struct obstack*, const char*, va_list);
#endif

#define gmp_printf __gmp_printf
    __GMP_DECLSPEC int gmp_printf(const char*, ...);

#define gmp_snprintf __gmp_snprintf
    __GMP_DECLSPEC int gmp_snprintf(char*, size_t, const char*, ...);

#define gmp_sprintf __gmp_sprintf
    __GMP_DECLSPEC int gmp_sprintf(char*, const char*, ...);

#define gmp_vasprintf __gmp_vasprintf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vasprintf(char**, const char*, va_list);
#endif

#define gmp_vfprintf __gmp_vfprintf
#if defined (_GMP_H_HAVE_FILE) && defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vfprintf(FILE*, const char*, va_list);
#endif

#define gmp_vprintf __gmp_vprintf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vprintf(const char*, va_list);
#endif

#define gmp_vsnprintf __gmp_vsnprintf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vsnprintf(char*, size_t, const char*, va_list);
#endif

#define gmp_vsprintf __gmp_vsprintf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vsprintf(char*, const char*, va_list);
#endif


    /**************** Formatted input routines.  ****************/

#define gmp_fscanf __gmp_fscanf
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC int gmp_fscanf(FILE*, const char*, ...);
#endif

#define gmp_scanf __gmp_scanf
    __GMP_DECLSPEC int gmp_scanf(const char*, ...);

#define gmp_sscanf __gmp_sscanf
    __GMP_DECLSPEC int gmp_sscanf(const char*, const char*, ...);

#define gmp_vfscanf __gmp_vfscanf
#if defined (_GMP_H_HAVE_FILE) && defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vfscanf(FILE*, const char*, va_list);
#endif

#define gmp_vscanf __gmp_vscanf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vscanf(const char*, va_list);
#endif

#define gmp_vsscanf __gmp_vsscanf
#if defined (_GMP_H_HAVE_VA_LIST)
    __GMP_DECLSPEC int gmp_vsscanf(const char*, const char*, va_list);
#endif


    /**************** Integer (i.e. Z) routines.  ****************/

#define _mpz_realloc __gmpz_realloc
#define mpz_realloc __gmpz_realloc
    __GMP_DECLSPEC void* _mpz_realloc(mpz_ptr, mp_size_t);

#define mpz_abs __gmpz_abs
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_abs)
    __GMP_DECLSPEC void mpz_abs(mpz_ptr, mpz_srcptr);
#endif

#define mpz_add __gmpz_add
    __GMP_DECLSPEC void mpz_add(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_add_ui __gmpz_add_ui
    __GMP_DECLSPEC void mpz_add_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_addmul __gmpz_addmul
    __GMP_DECLSPEC void mpz_addmul(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_addmul_ui __gmpz_addmul_ui
    __GMP_DECLSPEC void mpz_addmul_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_and __gmpz_and
    __GMP_DECLSPEC void mpz_and(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_array_init __gmpz_array_init
    __GMP_DECLSPEC void mpz_array_init(mpz_ptr, mp_size_t, mp_size_t);

#define mpz_bin_ui __gmpz_bin_ui
    __GMP_DECLSPEC void mpz_bin_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_bin_uiui __gmpz_bin_uiui
    __GMP_DECLSPEC void mpz_bin_uiui(mpz_ptr, unsigned long int, unsigned long int);

#define mpz_cdiv_q __gmpz_cdiv_q
    __GMP_DECLSPEC void mpz_cdiv_q(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_cdiv_q_2exp __gmpz_cdiv_q_2exp
    __GMP_DECLSPEC void mpz_cdiv_q_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_cdiv_q_ui __gmpz_cdiv_q_ui
    __GMP_DECLSPEC unsigned long int mpz_cdiv_q_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_cdiv_qr __gmpz_cdiv_qr
    __GMP_DECLSPEC void mpz_cdiv_qr(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_cdiv_qr_ui __gmpz_cdiv_qr_ui
    __GMP_DECLSPEC unsigned long int mpz_cdiv_qr_ui(mpz_ptr, mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_cdiv_r __gmpz_cdiv_r
    __GMP_DECLSPEC void mpz_cdiv_r(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_cdiv_r_2exp __gmpz_cdiv_r_2exp
    __GMP_DECLSPEC void mpz_cdiv_r_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_cdiv_r_ui __gmpz_cdiv_r_ui
    __GMP_DECLSPEC unsigned long int mpz_cdiv_r_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_cdiv_ui __gmpz_cdiv_ui
    __GMP_DECLSPEC unsigned long int mpz_cdiv_ui(mpz_srcptr, unsigned long int) __GMP_ATTRIBUTE_PURE;

#define mpz_clear __gmpz_clear
    __GMP_DECLSPEC void mpz_clear(mpz_ptr);

#define mpz_clears __gmpz_clears
    __GMP_DECLSPEC void mpz_clears(mpz_ptr, ...);

#define mpz_clrbit __gmpz_clrbit
    __GMP_DECLSPEC void mpz_clrbit(mpz_ptr, mp_bitcnt_t);

#define mpz_cmp __gmpz_cmp
    __GMP_DECLSPEC int mpz_cmp(mpz_srcptr, mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_cmp_d __gmpz_cmp_d
    __GMP_DECLSPEC int mpz_cmp_d(mpz_srcptr, double) __GMP_ATTRIBUTE_PURE;

#define _mpz_cmp_si __gmpz_cmp_si
    __GMP_DECLSPEC int _mpz_cmp_si(mpz_srcptr, signed long int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define _mpz_cmp_ui __gmpz_cmp_ui
    __GMP_DECLSPEC int _mpz_cmp_ui(mpz_srcptr, unsigned long int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_cmpabs __gmpz_cmpabs
    __GMP_DECLSPEC int mpz_cmpabs(mpz_srcptr, mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_cmpabs_d __gmpz_cmpabs_d
    __GMP_DECLSPEC int mpz_cmpabs_d(mpz_srcptr, double) __GMP_ATTRIBUTE_PURE;

#define mpz_cmpabs_ui __gmpz_cmpabs_ui
    __GMP_DECLSPEC int mpz_cmpabs_ui(mpz_srcptr, unsigned long int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_com __gmpz_com
    __GMP_DECLSPEC void mpz_com(mpz_ptr, mpz_srcptr);

#define mpz_combit __gmpz_combit
    __GMP_DECLSPEC void mpz_combit(mpz_ptr, mp_bitcnt_t);

#define mpz_congruent_p __gmpz_congruent_p
    __GMP_DECLSPEC int mpz_congruent_p(mpz_srcptr, mpz_srcptr, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_congruent_2exp_p __gmpz_congruent_2exp_p
    __GMP_DECLSPEC int mpz_congruent_2exp_p(mpz_srcptr, mpz_srcptr, mp_bitcnt_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_congruent_ui_p __gmpz_congruent_ui_p
    __GMP_DECLSPEC int mpz_congruent_ui_p(mpz_srcptr, unsigned long, unsigned long) __GMP_ATTRIBUTE_PURE;

#define mpz_divexact __gmpz_divexact
    __GMP_DECLSPEC void mpz_divexact(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_divexact_ui __gmpz_divexact_ui
    __GMP_DECLSPEC void mpz_divexact_ui(mpz_ptr, mpz_srcptr, unsigned long);

#define mpz_divisible_p __gmpz_divisible_p
    __GMP_DECLSPEC int mpz_divisible_p(mpz_srcptr, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_divisible_ui_p __gmpz_divisible_ui_p
    __GMP_DECLSPEC int mpz_divisible_ui_p(mpz_srcptr, unsigned long) __GMP_ATTRIBUTE_PURE;

#define mpz_divisible_2exp_p __gmpz_divisible_2exp_p
    __GMP_DECLSPEC int mpz_divisible_2exp_p(mpz_srcptr, mp_bitcnt_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_dump __gmpz_dump
    __GMP_DECLSPEC void mpz_dump(mpz_srcptr);

#define mpz_export __gmpz_export
    __GMP_DECLSPEC void* mpz_export(void*, size_t*, int, size_t, int, size_t, mpz_srcptr);

#define mpz_fac_ui __gmpz_fac_ui
    __GMP_DECLSPEC void mpz_fac_ui(mpz_ptr, unsigned long int);

#define mpz_2fac_ui __gmpz_2fac_ui
    __GMP_DECLSPEC void mpz_2fac_ui(mpz_ptr, unsigned long int);

#define mpz_mfac_uiui __gmpz_mfac_uiui
    __GMP_DECLSPEC void mpz_mfac_uiui(mpz_ptr, unsigned long int, unsigned long int);

#define mpz_primorial_ui __gmpz_primorial_ui
    __GMP_DECLSPEC void mpz_primorial_ui(mpz_ptr, unsigned long int);

#define mpz_fdiv_q __gmpz_fdiv_q
    __GMP_DECLSPEC void mpz_fdiv_q(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_fdiv_q_2exp __gmpz_fdiv_q_2exp
    __GMP_DECLSPEC void mpz_fdiv_q_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_fdiv_q_ui __gmpz_fdiv_q_ui
    __GMP_DECLSPEC unsigned long int mpz_fdiv_q_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_fdiv_qr __gmpz_fdiv_qr
    __GMP_DECLSPEC void mpz_fdiv_qr(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_fdiv_qr_ui __gmpz_fdiv_qr_ui
    __GMP_DECLSPEC unsigned long int mpz_fdiv_qr_ui(mpz_ptr, mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_fdiv_r __gmpz_fdiv_r
    __GMP_DECLSPEC void mpz_fdiv_r(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_fdiv_r_2exp __gmpz_fdiv_r_2exp
    __GMP_DECLSPEC void mpz_fdiv_r_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_fdiv_r_ui __gmpz_fdiv_r_ui
    __GMP_DECLSPEC unsigned long int mpz_fdiv_r_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_fdiv_ui __gmpz_fdiv_ui
    __GMP_DECLSPEC unsigned long int mpz_fdiv_ui(mpz_srcptr, unsigned long int) __GMP_ATTRIBUTE_PURE;

#define mpz_fib_ui __gmpz_fib_ui
    __GMP_DECLSPEC void mpz_fib_ui(mpz_ptr, unsigned long int);

#define mpz_fib2_ui __gmpz_fib2_ui
    __GMP_DECLSPEC void mpz_fib2_ui(mpz_ptr, mpz_ptr, unsigned long int);

#define mpz_fits_sint_p __gmpz_fits_sint_p
    __GMP_DECLSPEC int mpz_fits_sint_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_fits_slong_p __gmpz_fits_slong_p
    __GMP_DECLSPEC int mpz_fits_slong_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_fits_sshort_p __gmpz_fits_sshort_p
    __GMP_DECLSPEC int mpz_fits_sshort_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_fits_uint_p __gmpz_fits_uint_p
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_fits_uint_p)
    __GMP_DECLSPEC int mpz_fits_uint_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_fits_ulong_p __gmpz_fits_ulong_p
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_fits_ulong_p)
    __GMP_DECLSPEC int mpz_fits_ulong_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_fits_ushort_p __gmpz_fits_ushort_p
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_fits_ushort_p)
    __GMP_DECLSPEC int mpz_fits_ushort_p(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_gcd __gmpz_gcd
    __GMP_DECLSPEC void mpz_gcd(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_gcd_ui __gmpz_gcd_ui
    __GMP_DECLSPEC unsigned long int mpz_gcd_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_gcdext __gmpz_gcdext
    __GMP_DECLSPEC void mpz_gcdext(mpz_ptr, mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_get_d __gmpz_get_d
    __GMP_DECLSPEC double mpz_get_d(mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_get_d_2exp __gmpz_get_d_2exp
    __GMP_DECLSPEC double mpz_get_d_2exp(signed long int*, mpz_srcptr);

#define mpz_get_si __gmpz_get_si
    __GMP_DECLSPEC /* signed */ long int mpz_get_si(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_get_str __gmpz_get_str
    __GMP_DECLSPEC char* mpz_get_str(char*, int, mpz_srcptr);

#define mpz_get_ui __gmpz_get_ui
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_get_ui)
    __GMP_DECLSPEC unsigned long int mpz_get_ui(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_getlimbn __gmpz_getlimbn
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_getlimbn)
    __GMP_DECLSPEC mp_limb_t mpz_getlimbn(mpz_srcptr, mp_size_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_hamdist __gmpz_hamdist
    __GMP_DECLSPEC mp_bitcnt_t mpz_hamdist(mpz_srcptr, mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_import __gmpz_import
    __GMP_DECLSPEC void mpz_import(mpz_ptr, size_t, int, size_t, int, size_t, const void*);

#define mpz_init __gmpz_init
    __GMP_DECLSPEC void mpz_init(mpz_ptr) __GMP_NOTHROW;

#define mpz_init2 __gmpz_init2
    __GMP_DECLSPEC void mpz_init2(mpz_ptr, mp_bitcnt_t);

#define mpz_inits __gmpz_inits
    __GMP_DECLSPEC void mpz_inits(mpz_ptr, ...) __GMP_NOTHROW;

#define mpz_init_set __gmpz_init_set
    __GMP_DECLSPEC void mpz_init_set(mpz_ptr, mpz_srcptr);

#define mpz_init_set_d __gmpz_init_set_d
    __GMP_DECLSPEC void mpz_init_set_d(mpz_ptr, double);

#define mpz_init_set_si __gmpz_init_set_si
    __GMP_DECLSPEC void mpz_init_set_si(mpz_ptr, signed long int);

#define mpz_init_set_str __gmpz_init_set_str
    __GMP_DECLSPEC int mpz_init_set_str(mpz_ptr, const char*, int);

#define mpz_init_set_ui __gmpz_init_set_ui
    __GMP_DECLSPEC void mpz_init_set_ui(mpz_ptr, unsigned long int);

#define mpz_inp_raw __gmpz_inp_raw
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpz_inp_raw(mpz_ptr, FILE*);
#endif

#define mpz_inp_str __gmpz_inp_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpz_inp_str(mpz_ptr, FILE*, int);
#endif

#define mpz_invert __gmpz_invert
    __GMP_DECLSPEC int mpz_invert(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_ior __gmpz_ior
    __GMP_DECLSPEC void mpz_ior(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_jacobi __gmpz_jacobi
    __GMP_DECLSPEC int mpz_jacobi(mpz_srcptr, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_kronecker mpz_jacobi  /* alias */

#define mpz_kronecker_si __gmpz_kronecker_si
    __GMP_DECLSPEC int mpz_kronecker_si(mpz_srcptr, long) __GMP_ATTRIBUTE_PURE;

#define mpz_kronecker_ui __gmpz_kronecker_ui
    __GMP_DECLSPEC int mpz_kronecker_ui(mpz_srcptr, unsigned long) __GMP_ATTRIBUTE_PURE;

#define mpz_si_kronecker __gmpz_si_kronecker
    __GMP_DECLSPEC int mpz_si_kronecker(long, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_ui_kronecker __gmpz_ui_kronecker
    __GMP_DECLSPEC int mpz_ui_kronecker(unsigned long, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_lcm __gmpz_lcm
    __GMP_DECLSPEC void mpz_lcm(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_lcm_ui __gmpz_lcm_ui
    __GMP_DECLSPEC void mpz_lcm_ui(mpz_ptr, mpz_srcptr, unsigned long);

#define mpz_legendre mpz_jacobi  /* alias */

#define mpz_lucnum_ui __gmpz_lucnum_ui
    __GMP_DECLSPEC void mpz_lucnum_ui(mpz_ptr, unsigned long int);

#define mpz_lucnum2_ui __gmpz_lucnum2_ui
    __GMP_DECLSPEC void mpz_lucnum2_ui(mpz_ptr, mpz_ptr, unsigned long int);

#define mpz_millerrabin __gmpz_millerrabin
    __GMP_DECLSPEC int mpz_millerrabin(mpz_srcptr, int) __GMP_ATTRIBUTE_PURE;

#define mpz_mod __gmpz_mod
    __GMP_DECLSPEC void mpz_mod(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_mod_ui mpz_fdiv_r_ui /* same as fdiv_r because divisor unsigned */

#define mpz_mul __gmpz_mul
    __GMP_DECLSPEC void mpz_mul(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_mul_2exp __gmpz_mul_2exp
    __GMP_DECLSPEC void mpz_mul_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_mul_si __gmpz_mul_si
    __GMP_DECLSPEC void mpz_mul_si(mpz_ptr, mpz_srcptr, long int);

#define mpz_mul_ui __gmpz_mul_ui
    __GMP_DECLSPEC void mpz_mul_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_neg __gmpz_neg
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_neg)
    __GMP_DECLSPEC void mpz_neg(mpz_ptr, mpz_srcptr);
#endif

#define mpz_nextprime __gmpz_nextprime
    __GMP_DECLSPEC void mpz_nextprime(mpz_ptr, mpz_srcptr);

#define mpz_out_raw __gmpz_out_raw
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpz_out_raw(FILE*, mpz_srcptr);
#endif

#define mpz_out_str __gmpz_out_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpz_out_str(FILE*, int, mpz_srcptr);
#endif

#define mpz_perfect_power_p __gmpz_perfect_power_p
    __GMP_DECLSPEC int mpz_perfect_power_p(mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpz_perfect_square_p __gmpz_perfect_square_p
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_perfect_square_p)
    __GMP_DECLSPEC int mpz_perfect_square_p(mpz_srcptr) __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_popcount __gmpz_popcount
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_popcount)
    __GMP_DECLSPEC mp_bitcnt_t mpz_popcount(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_pow_ui __gmpz_pow_ui
    __GMP_DECLSPEC void mpz_pow_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_powm __gmpz_powm
    __GMP_DECLSPEC void mpz_powm(mpz_ptr, mpz_srcptr, mpz_srcptr, mpz_srcptr);

#define mpz_powm_sec __gmpz_powm_sec
    __GMP_DECLSPEC void mpz_powm_sec(mpz_ptr, mpz_srcptr, mpz_srcptr, mpz_srcptr);

#define mpz_powm_ui __gmpz_powm_ui
    __GMP_DECLSPEC void mpz_powm_ui(mpz_ptr, mpz_srcptr, unsigned long int, mpz_srcptr);

#define mpz_probab_prime_p __gmpz_probab_prime_p
    __GMP_DECLSPEC int mpz_probab_prime_p(mpz_srcptr, int) __GMP_ATTRIBUTE_PURE;

#define mpz_random __gmpz_random
    __GMP_DECLSPEC void mpz_random(mpz_ptr, mp_size_t);

#define mpz_random2 __gmpz_random2
    __GMP_DECLSPEC void mpz_random2(mpz_ptr, mp_size_t);

#define mpz_realloc2 __gmpz_realloc2
    __GMP_DECLSPEC void mpz_realloc2(mpz_ptr, mp_bitcnt_t);

#define mpz_remove __gmpz_remove
    __GMP_DECLSPEC mp_bitcnt_t mpz_remove(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_root __gmpz_root
    __GMP_DECLSPEC int mpz_root(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_rootrem __gmpz_rootrem
    __GMP_DECLSPEC void mpz_rootrem(mpz_ptr, mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_rrandomb __gmpz_rrandomb
    __GMP_DECLSPEC void mpz_rrandomb(mpz_ptr, gmp_randstate_t, mp_bitcnt_t);

#define mpz_scan0 __gmpz_scan0
    __GMP_DECLSPEC mp_bitcnt_t mpz_scan0(mpz_srcptr, mp_bitcnt_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_scan1 __gmpz_scan1
    __GMP_DECLSPEC mp_bitcnt_t mpz_scan1(mpz_srcptr, mp_bitcnt_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_set __gmpz_set
    __GMP_DECLSPEC void mpz_set(mpz_ptr, mpz_srcptr);

#define mpz_set_d __gmpz_set_d
    __GMP_DECLSPEC void mpz_set_d(mpz_ptr, double);

#define mpz_set_f __gmpz_set_f
    __GMP_DECLSPEC void mpz_set_f(mpz_ptr, mpf_srcptr);

#define mpz_set_q __gmpz_set_q
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_set_q)
    __GMP_DECLSPEC void mpz_set_q(mpz_ptr, mpq_srcptr);
#endif

#define mpz_set_si __gmpz_set_si
    __GMP_DECLSPEC void mpz_set_si(mpz_ptr, signed long int);

#define mpz_set_str __gmpz_set_str
    __GMP_DECLSPEC int mpz_set_str(mpz_ptr, const char*, int);

#define mpz_set_ui __gmpz_set_ui
    __GMP_DECLSPEC void mpz_set_ui(mpz_ptr, unsigned long int);

#define mpz_setbit __gmpz_setbit
    __GMP_DECLSPEC void mpz_setbit(mpz_ptr, mp_bitcnt_t);

#define mpz_size __gmpz_size
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_size)
    __GMP_DECLSPEC size_t mpz_size(mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_sizeinbase __gmpz_sizeinbase
    __GMP_DECLSPEC size_t mpz_sizeinbase(mpz_srcptr, int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_sqrt __gmpz_sqrt
    __GMP_DECLSPEC void mpz_sqrt(mpz_ptr, mpz_srcptr);

#define mpz_sqrtrem __gmpz_sqrtrem
    __GMP_DECLSPEC void mpz_sqrtrem(mpz_ptr, mpz_ptr, mpz_srcptr);

#define mpz_sub __gmpz_sub
    __GMP_DECLSPEC void mpz_sub(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_sub_ui __gmpz_sub_ui
    __GMP_DECLSPEC void mpz_sub_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_ui_sub __gmpz_ui_sub
    __GMP_DECLSPEC void mpz_ui_sub(mpz_ptr, unsigned long int, mpz_srcptr);

#define mpz_submul __gmpz_submul
    __GMP_DECLSPEC void mpz_submul(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_submul_ui __gmpz_submul_ui
    __GMP_DECLSPEC void mpz_submul_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_swap __gmpz_swap
    __GMP_DECLSPEC void mpz_swap(mpz_ptr, mpz_ptr) __GMP_NOTHROW;

#define mpz_tdiv_ui __gmpz_tdiv_ui
    __GMP_DECLSPEC unsigned long int mpz_tdiv_ui(mpz_srcptr, unsigned long int) __GMP_ATTRIBUTE_PURE;

#define mpz_tdiv_q __gmpz_tdiv_q
    __GMP_DECLSPEC void mpz_tdiv_q(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_tdiv_q_2exp __gmpz_tdiv_q_2exp
    __GMP_DECLSPEC void mpz_tdiv_q_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_tdiv_q_ui __gmpz_tdiv_q_ui
    __GMP_DECLSPEC unsigned long int mpz_tdiv_q_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_tdiv_qr __gmpz_tdiv_qr
    __GMP_DECLSPEC void mpz_tdiv_qr(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_tdiv_qr_ui __gmpz_tdiv_qr_ui
    __GMP_DECLSPEC unsigned long int mpz_tdiv_qr_ui(mpz_ptr, mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_tdiv_r __gmpz_tdiv_r
    __GMP_DECLSPEC void mpz_tdiv_r(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_tdiv_r_2exp __gmpz_tdiv_r_2exp
    __GMP_DECLSPEC void mpz_tdiv_r_2exp(mpz_ptr, mpz_srcptr, mp_bitcnt_t);

#define mpz_tdiv_r_ui __gmpz_tdiv_r_ui
    __GMP_DECLSPEC unsigned long int mpz_tdiv_r_ui(mpz_ptr, mpz_srcptr, unsigned long int);

#define mpz_tstbit __gmpz_tstbit
    __GMP_DECLSPEC int mpz_tstbit(mpz_srcptr, mp_bitcnt_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpz_ui_pow_ui __gmpz_ui_pow_ui
    __GMP_DECLSPEC void mpz_ui_pow_ui(mpz_ptr, unsigned long int, unsigned long int);

#define mpz_urandomb __gmpz_urandomb
    __GMP_DECLSPEC void mpz_urandomb(mpz_ptr, gmp_randstate_t, mp_bitcnt_t);

#define mpz_urandomm __gmpz_urandomm
    __GMP_DECLSPEC void mpz_urandomm(mpz_ptr, gmp_randstate_t, mpz_srcptr);

#define mpz_xor __gmpz_xor
#define mpz_eor __gmpz_xor
    __GMP_DECLSPEC void mpz_xor(mpz_ptr, mpz_srcptr, mpz_srcptr);

#define mpz_limbs_read __gmpz_limbs_read
    __GMP_DECLSPEC mp_srcptr mpz_limbs_read(mpz_srcptr);

#define mpz_limbs_write __gmpz_limbs_write
    __GMP_DECLSPEC mp_ptr mpz_limbs_write(mpz_ptr, mp_size_t);

#define mpz_limbs_modify __gmpz_limbs_modify
    __GMP_DECLSPEC mp_ptr mpz_limbs_modify(mpz_ptr, mp_size_t);

#define mpz_limbs_finish __gmpz_limbs_finish
    __GMP_DECLSPEC void mpz_limbs_finish(mpz_ptr, mp_size_t);

#define mpz_roinit_n __gmpz_roinit_n
    __GMP_DECLSPEC mpz_srcptr mpz_roinit_n(mpz_ptr, mp_srcptr, mp_size_t);

#define MPZ_ROINIT_N(xp, xs) {{0, (xs),(xp) }}

    /**************** Rational (i.e. Q) routines.  ****************/

#define mpq_abs __gmpq_abs
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpq_abs)
    __GMP_DECLSPEC void mpq_abs(mpq_ptr, mpq_srcptr);
#endif

#define mpq_add __gmpq_add
    __GMP_DECLSPEC void mpq_add(mpq_ptr, mpq_srcptr, mpq_srcptr);

#define mpq_canonicalize __gmpq_canonicalize
    __GMP_DECLSPEC void mpq_canonicalize(mpq_ptr);

#define mpq_clear __gmpq_clear
    __GMP_DECLSPEC void mpq_clear(mpq_ptr);

#define mpq_clears __gmpq_clears
    __GMP_DECLSPEC void mpq_clears(mpq_ptr, ...);

#define mpq_cmp __gmpq_cmp
    __GMP_DECLSPEC int mpq_cmp(mpq_srcptr, mpq_srcptr) __GMP_ATTRIBUTE_PURE;

#define _mpq_cmp_si __gmpq_cmp_si
    __GMP_DECLSPEC int _mpq_cmp_si(mpq_srcptr, long, unsigned long) __GMP_ATTRIBUTE_PURE;

#define _mpq_cmp_ui __gmpq_cmp_ui
    __GMP_DECLSPEC int _mpq_cmp_ui(mpq_srcptr, unsigned long int, unsigned long int) __GMP_ATTRIBUTE_PURE;

#define mpq_cmp_z __gmpq_cmp_z
    __GMP_DECLSPEC int mpq_cmp_z(mpq_srcptr, mpz_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpq_div __gmpq_div
    __GMP_DECLSPEC void mpq_div(mpq_ptr, mpq_srcptr, mpq_srcptr);

#define mpq_div_2exp __gmpq_div_2exp
    __GMP_DECLSPEC void mpq_div_2exp(mpq_ptr, mpq_srcptr, mp_bitcnt_t);

#define mpq_equal __gmpq_equal
    __GMP_DECLSPEC int mpq_equal(mpq_srcptr, mpq_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpq_get_num __gmpq_get_num
    __GMP_DECLSPEC void mpq_get_num(mpz_ptr, mpq_srcptr);

#define mpq_get_den __gmpq_get_den
    __GMP_DECLSPEC void mpq_get_den(mpz_ptr, mpq_srcptr);

#define mpq_get_d __gmpq_get_d
    __GMP_DECLSPEC double mpq_get_d(mpq_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpq_get_str __gmpq_get_str
    __GMP_DECLSPEC char* mpq_get_str(char*, int, mpq_srcptr);

#define mpq_init __gmpq_init
    __GMP_DECLSPEC void mpq_init(mpq_ptr);

#define mpq_inits __gmpq_inits
    __GMP_DECLSPEC void mpq_inits(mpq_ptr, ...);

#define mpq_inp_str __gmpq_inp_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpq_inp_str(mpq_ptr, FILE*, int);
#endif

#define mpq_inv __gmpq_inv
    __GMP_DECLSPEC void mpq_inv(mpq_ptr, mpq_srcptr);

#define mpq_mul __gmpq_mul
    __GMP_DECLSPEC void mpq_mul(mpq_ptr, mpq_srcptr, mpq_srcptr);

#define mpq_mul_2exp __gmpq_mul_2exp
    __GMP_DECLSPEC void mpq_mul_2exp(mpq_ptr, mpq_srcptr, mp_bitcnt_t);

#define mpq_neg __gmpq_neg
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpq_neg)
    __GMP_DECLSPEC void mpq_neg(mpq_ptr, mpq_srcptr);
#endif

#define mpq_out_str __gmpq_out_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpq_out_str(FILE*, int, mpq_srcptr);
#endif

#define mpq_set __gmpq_set
    __GMP_DECLSPEC void mpq_set(mpq_ptr, mpq_srcptr);

#define mpq_set_d __gmpq_set_d
    __GMP_DECLSPEC void mpq_set_d(mpq_ptr, double);

#define mpq_set_den __gmpq_set_den
    __GMP_DECLSPEC void mpq_set_den(mpq_ptr, mpz_srcptr);

#define mpq_set_f __gmpq_set_f
    __GMP_DECLSPEC void mpq_set_f(mpq_ptr, mpf_srcptr);

#define mpq_set_num __gmpq_set_num
    __GMP_DECLSPEC void mpq_set_num(mpq_ptr, mpz_srcptr);

#define mpq_set_si __gmpq_set_si
    __GMP_DECLSPEC void mpq_set_si(mpq_ptr, signed long int, unsigned long int);

#define mpq_set_str __gmpq_set_str
    __GMP_DECLSPEC int mpq_set_str(mpq_ptr, const char*, int);

#define mpq_set_ui __gmpq_set_ui
    __GMP_DECLSPEC void mpq_set_ui(mpq_ptr, unsigned long int, unsigned long int);

#define mpq_set_z __gmpq_set_z
    __GMP_DECLSPEC void mpq_set_z(mpq_ptr, mpz_srcptr);

#define mpq_sub __gmpq_sub
    __GMP_DECLSPEC void mpq_sub(mpq_ptr, mpq_srcptr, mpq_srcptr);

#define mpq_swap __gmpq_swap
    __GMP_DECLSPEC void mpq_swap(mpq_ptr, mpq_ptr) __GMP_NOTHROW;


    /**************** Float (i.e. F) routines.  ****************/

#define mpf_abs __gmpf_abs
    __GMP_DECLSPEC void mpf_abs(mpf_ptr, mpf_srcptr);

#define mpf_add __gmpf_add
    __GMP_DECLSPEC void mpf_add(mpf_ptr, mpf_srcptr, mpf_srcptr);

#define mpf_add_ui __gmpf_add_ui
    __GMP_DECLSPEC void mpf_add_ui(mpf_ptr, mpf_srcptr, unsigned long int);
#define mpf_ceil __gmpf_ceil
    __GMP_DECLSPEC void mpf_ceil(mpf_ptr, mpf_srcptr);

#define mpf_clear __gmpf_clear
    __GMP_DECLSPEC void mpf_clear(mpf_ptr);

#define mpf_clears __gmpf_clears
    __GMP_DECLSPEC void mpf_clears(mpf_ptr, ...);

#define mpf_cmp __gmpf_cmp
    __GMP_DECLSPEC int mpf_cmp(mpf_srcptr, mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_cmp_z __gmpf_cmp_z
    __GMP_DECLSPEC int mpf_cmp_z(mpf_srcptr, mpz_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_cmp_d __gmpf_cmp_d
    __GMP_DECLSPEC int mpf_cmp_d(mpf_srcptr, double) __GMP_ATTRIBUTE_PURE;

#define mpf_cmp_si __gmpf_cmp_si
    __GMP_DECLSPEC int mpf_cmp_si(mpf_srcptr, signed long int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_cmp_ui __gmpf_cmp_ui
    __GMP_DECLSPEC int mpf_cmp_ui(mpf_srcptr, unsigned long int) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_div __gmpf_div
    __GMP_DECLSPEC void mpf_div(mpf_ptr, mpf_srcptr, mpf_srcptr);

#define mpf_div_2exp __gmpf_div_2exp
    __GMP_DECLSPEC void mpf_div_2exp(mpf_ptr, mpf_srcptr, mp_bitcnt_t);

#define mpf_div_ui __gmpf_div_ui
    __GMP_DECLSPEC void mpf_div_ui(mpf_ptr, mpf_srcptr, unsigned long int);

#define mpf_dump __gmpf_dump
    __GMP_DECLSPEC void mpf_dump(mpf_srcptr);

#define mpf_eq __gmpf_eq
    __GMP_DECLSPEC int mpf_eq(mpf_srcptr, mpf_srcptr, mp_bitcnt_t) __GMP_ATTRIBUTE_PURE;

#define mpf_fits_sint_p __gmpf_fits_sint_p
    __GMP_DECLSPEC int mpf_fits_sint_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_fits_slong_p __gmpf_fits_slong_p
    __GMP_DECLSPEC int mpf_fits_slong_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_fits_sshort_p __gmpf_fits_sshort_p
    __GMP_DECLSPEC int mpf_fits_sshort_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_fits_uint_p __gmpf_fits_uint_p
    __GMP_DECLSPEC int mpf_fits_uint_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_fits_ulong_p __gmpf_fits_ulong_p
    __GMP_DECLSPEC int mpf_fits_ulong_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_fits_ushort_p __gmpf_fits_ushort_p
    __GMP_DECLSPEC int mpf_fits_ushort_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_floor __gmpf_floor
    __GMP_DECLSPEC void mpf_floor(mpf_ptr, mpf_srcptr);

#define mpf_get_d __gmpf_get_d
    __GMP_DECLSPEC double mpf_get_d(mpf_srcptr) __GMP_ATTRIBUTE_PURE;

#define mpf_get_d_2exp __gmpf_get_d_2exp
    __GMP_DECLSPEC double mpf_get_d_2exp(signed long int*, mpf_srcptr);

#define mpf_get_default_prec __gmpf_get_default_prec
    __GMP_DECLSPEC mp_bitcnt_t mpf_get_default_prec(void) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_get_prec __gmpf_get_prec
    __GMP_DECLSPEC mp_bitcnt_t mpf_get_prec(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_get_si __gmpf_get_si
    __GMP_DECLSPEC long mpf_get_si(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_get_str __gmpf_get_str
    __GMP_DECLSPEC char* mpf_get_str(char*, mp_exp_t*, int, size_t, mpf_srcptr);

#define mpf_get_ui __gmpf_get_ui
    __GMP_DECLSPEC unsigned long mpf_get_ui(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_init __gmpf_init
    __GMP_DECLSPEC void mpf_init(mpf_ptr);

#define mpf_init2 __gmpf_init2
    __GMP_DECLSPEC void mpf_init2(mpf_ptr, mp_bitcnt_t);

#define mpf_inits __gmpf_inits
    __GMP_DECLSPEC void mpf_inits(mpf_ptr, ...);

#define mpf_init_set __gmpf_init_set
    __GMP_DECLSPEC void mpf_init_set(mpf_ptr, mpf_srcptr);

#define mpf_init_set_d __gmpf_init_set_d
    __GMP_DECLSPEC void mpf_init_set_d(mpf_ptr, double);

#define mpf_init_set_si __gmpf_init_set_si
    __GMP_DECLSPEC void mpf_init_set_si(mpf_ptr, signed long int);

#define mpf_init_set_str __gmpf_init_set_str
    __GMP_DECLSPEC int mpf_init_set_str(mpf_ptr, const char*, int);

#define mpf_init_set_ui __gmpf_init_set_ui
    __GMP_DECLSPEC void mpf_init_set_ui(mpf_ptr, unsigned long int);

#define mpf_inp_str __gmpf_inp_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpf_inp_str(mpf_ptr, FILE*, int);
#endif

#define mpf_integer_p __gmpf_integer_p
    __GMP_DECLSPEC int mpf_integer_p(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_mul __gmpf_mul
    __GMP_DECLSPEC void mpf_mul(mpf_ptr, mpf_srcptr, mpf_srcptr);

#define mpf_mul_2exp __gmpf_mul_2exp
    __GMP_DECLSPEC void mpf_mul_2exp(mpf_ptr, mpf_srcptr, mp_bitcnt_t);

#define mpf_mul_ui __gmpf_mul_ui
    __GMP_DECLSPEC void mpf_mul_ui(mpf_ptr, mpf_srcptr, unsigned long int);

#define mpf_neg __gmpf_neg
    __GMP_DECLSPEC void mpf_neg(mpf_ptr, mpf_srcptr);

#define mpf_out_str __gmpf_out_str
#ifdef _GMP_H_HAVE_FILE
    __GMP_DECLSPEC size_t mpf_out_str(FILE*, int, size_t, mpf_srcptr);
#endif

#define mpf_pow_ui __gmpf_pow_ui
    __GMP_DECLSPEC void mpf_pow_ui(mpf_ptr, mpf_srcptr, unsigned long int);

#define mpf_random2 __gmpf_random2
    __GMP_DECLSPEC void mpf_random2(mpf_ptr, mp_size_t, mp_exp_t);

#define mpf_reldiff __gmpf_reldiff
    __GMP_DECLSPEC void mpf_reldiff(mpf_ptr, mpf_srcptr, mpf_srcptr);

#define mpf_set __gmpf_set
    __GMP_DECLSPEC void mpf_set(mpf_ptr, mpf_srcptr);

#define mpf_set_d __gmpf_set_d
    __GMP_DECLSPEC void mpf_set_d(mpf_ptr, double);

#define mpf_set_default_prec __gmpf_set_default_prec
    __GMP_DECLSPEC void mpf_set_default_prec(mp_bitcnt_t) __GMP_NOTHROW;

#define mpf_set_prec __gmpf_set_prec
    __GMP_DECLSPEC void mpf_set_prec(mpf_ptr, mp_bitcnt_t);

#define mpf_set_prec_raw __gmpf_set_prec_raw
    __GMP_DECLSPEC void mpf_set_prec_raw(mpf_ptr, mp_bitcnt_t) __GMP_NOTHROW;

#define mpf_set_q __gmpf_set_q
    __GMP_DECLSPEC void mpf_set_q(mpf_ptr, mpq_srcptr);

#define mpf_set_si __gmpf_set_si
    __GMP_DECLSPEC void mpf_set_si(mpf_ptr, signed long int);

#define mpf_set_str __gmpf_set_str
    __GMP_DECLSPEC int mpf_set_str(mpf_ptr, const char*, int);

#define mpf_set_ui __gmpf_set_ui
    __GMP_DECLSPEC void mpf_set_ui(mpf_ptr, unsigned long int);

#define mpf_set_z __gmpf_set_z
    __GMP_DECLSPEC void mpf_set_z(mpf_ptr, mpz_srcptr);

#define mpf_size __gmpf_size
    __GMP_DECLSPEC size_t mpf_size(mpf_srcptr) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpf_sqrt __gmpf_sqrt
    __GMP_DECLSPEC void mpf_sqrt(mpf_ptr, mpf_srcptr);

#define mpf_sqrt_ui __gmpf_sqrt_ui
    __GMP_DECLSPEC void mpf_sqrt_ui(mpf_ptr, unsigned long int);

#define mpf_sub __gmpf_sub
    __GMP_DECLSPEC void mpf_sub(mpf_ptr, mpf_srcptr, mpf_srcptr);

#define mpf_sub_ui __gmpf_sub_ui
    __GMP_DECLSPEC void mpf_sub_ui(mpf_ptr, mpf_srcptr, unsigned long int);

#define mpf_swap __gmpf_swap
    __GMP_DECLSPEC void mpf_swap(mpf_ptr, mpf_ptr) __GMP_NOTHROW;

#define mpf_trunc __gmpf_trunc
    __GMP_DECLSPEC void mpf_trunc(mpf_ptr, mpf_srcptr);

#define mpf_ui_div __gmpf_ui_div
    __GMP_DECLSPEC void mpf_ui_div(mpf_ptr, unsigned long int, mpf_srcptr);

#define mpf_ui_sub __gmpf_ui_sub
    __GMP_DECLSPEC void mpf_ui_sub(mpf_ptr, unsigned long int, mpf_srcptr);

#define mpf_urandomb __gmpf_urandomb
    __GMP_DECLSPEC void mpf_urandomb(mpf_t, gmp_randstate_t, mp_bitcnt_t);


    /************ Low level positive-integer (i.e. N) routines.  ************/

    /* This is ugly, but we need to make user calls reach the prefixed function. */

#define mpn_add __MPN(add)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_add)
    __GMP_DECLSPEC mp_limb_t mpn_add(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t);
#endif

#define mpn_add_1 __MPN(add_1)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_add_1)
    __GMP_DECLSPEC mp_limb_t mpn_add_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t) __GMP_NOTHROW;
#endif

#define mpn_add_n __MPN(add_n)
    __GMP_DECLSPEC mp_limb_t mpn_add_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);

#define mpn_addmul_1 __MPN(addmul_1)
    __GMP_DECLSPEC mp_limb_t mpn_addmul_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_cmp __MPN(cmp)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_cmp)
    __GMP_DECLSPEC int mpn_cmp(mp_srcptr, mp_srcptr, mp_size_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpn_zero_p __MPN(zero_p)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_zero_p)
    __GMP_DECLSPEC int mpn_zero_p(mp_srcptr, mp_size_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpn_divexact_1 __MPN(divexact_1)
    __GMP_DECLSPEC void mpn_divexact_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_divexact_by3(dst,src,size) \
  mpn_divexact_by3c (dst, src, size, __GMP_CAST (mp_limb_t, 0))

#define mpn_divexact_by3c __MPN(divexact_by3c)
    __GMP_DECLSPEC mp_limb_t mpn_divexact_by3c(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_divmod_1(qp,np,nsize,dlimb) \
  mpn_divrem_1 (qp, __GMP_CAST (mp_size_t, 0), np, nsize, dlimb)

#define mpn_divrem __MPN(divrem)
    __GMP_DECLSPEC mp_limb_t mpn_divrem(mp_ptr, mp_size_t, mp_ptr, mp_size_t, mp_srcptr, mp_size_t);

#define mpn_divrem_1 __MPN(divrem_1)
    __GMP_DECLSPEC mp_limb_t mpn_divrem_1(mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_divrem_2 __MPN(divrem_2)
    __GMP_DECLSPEC mp_limb_t mpn_divrem_2(mp_ptr, mp_size_t, mp_ptr, mp_size_t, mp_srcptr);

#define mpn_div_qr_1 __MPN(div_qr_1)
    __GMP_DECLSPEC mp_limb_t mpn_div_qr_1(mp_ptr, mp_limb_t*, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_div_qr_2 __MPN(div_qr_2)
    __GMP_DECLSPEC mp_limb_t mpn_div_qr_2(mp_ptr, mp_ptr, mp_srcptr, mp_size_t, mp_srcptr);

#define mpn_gcd __MPN(gcd)
    __GMP_DECLSPEC mp_size_t mpn_gcd(mp_ptr, mp_ptr, mp_size_t, mp_ptr, mp_size_t);

#define mpn_gcd_11 __MPN(gcd_11)
    __GMP_DECLSPEC mp_limb_t mpn_gcd_11(mp_limb_t, mp_limb_t) __GMP_ATTRIBUTE_PURE;

#define mpn_gcd_1 __MPN(gcd_1)
    __GMP_DECLSPEC mp_limb_t mpn_gcd_1(mp_srcptr, mp_size_t, mp_limb_t) __GMP_ATTRIBUTE_PURE;

#define mpn_gcdext_1 __MPN(gcdext_1)
    __GMP_DECLSPEC mp_limb_t mpn_gcdext_1(mp_limb_signed_t*, mp_limb_signed_t*, mp_limb_t, mp_limb_t);

#define mpn_gcdext __MPN(gcdext)
    __GMP_DECLSPEC mp_size_t mpn_gcdext(mp_ptr, mp_ptr, mp_size_t*, mp_ptr, mp_size_t, mp_ptr, mp_size_t);

#define mpn_get_str __MPN(get_str)
    __GMP_DECLSPEC size_t mpn_get_str(unsigned char*, int, mp_ptr, mp_size_t);

#define mpn_hamdist __MPN(hamdist)
    __GMP_DECLSPEC mp_bitcnt_t mpn_hamdist(mp_srcptr, mp_srcptr, mp_size_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpn_lshift __MPN(lshift)
    __GMP_DECLSPEC mp_limb_t mpn_lshift(mp_ptr, mp_srcptr, mp_size_t, unsigned int);

#define mpn_mod_1 __MPN(mod_1)
    __GMP_DECLSPEC mp_limb_t mpn_mod_1(mp_srcptr, mp_size_t, mp_limb_t) __GMP_ATTRIBUTE_PURE;

#define mpn_mul __MPN(mul)
    __GMP_DECLSPEC mp_limb_t mpn_mul(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t);

#define mpn_mul_1 __MPN(mul_1)
    __GMP_DECLSPEC mp_limb_t mpn_mul_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_mul_n __MPN(mul_n)
    __GMP_DECLSPEC void mpn_mul_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);

#define mpn_sqr __MPN(sqr)
    __GMP_DECLSPEC void mpn_sqr(mp_ptr, mp_srcptr, mp_size_t);

#define mpn_neg __MPN(neg)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_neg)
    __GMP_DECLSPEC mp_limb_t mpn_neg(mp_ptr, mp_srcptr, mp_size_t);
#endif

#define mpn_com __MPN(com)
    __GMP_DECLSPEC void mpn_com(mp_ptr, mp_srcptr, mp_size_t);

#define mpn_perfect_square_p __MPN(perfect_square_p)
    __GMP_DECLSPEC int mpn_perfect_square_p(mp_srcptr, mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_perfect_power_p __MPN(perfect_power_p)
    __GMP_DECLSPEC int mpn_perfect_power_p(mp_srcptr, mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_popcount __MPN(popcount)
    __GMP_DECLSPEC mp_bitcnt_t mpn_popcount(mp_srcptr, mp_size_t) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;

#define mpn_pow_1 __MPN(pow_1)
    __GMP_DECLSPEC mp_size_t mpn_pow_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_ptr);

    /* undocumented now, but retained here for upward compatibility */
#define mpn_preinv_mod_1 __MPN(preinv_mod_1)
    __GMP_DECLSPEC mp_limb_t mpn_preinv_mod_1(mp_srcptr, mp_size_t, mp_limb_t, mp_limb_t) __GMP_ATTRIBUTE_PURE;

#define mpn_random __MPN(random)
    __GMP_DECLSPEC void mpn_random(mp_ptr, mp_size_t);

#define mpn_random2 __MPN(random2)
    __GMP_DECLSPEC void mpn_random2(mp_ptr, mp_size_t);

#define mpn_rshift __MPN(rshift)
    __GMP_DECLSPEC mp_limb_t mpn_rshift(mp_ptr, mp_srcptr, mp_size_t, unsigned int);

#define mpn_scan0 __MPN(scan0)
    __GMP_DECLSPEC mp_bitcnt_t mpn_scan0(mp_srcptr, mp_bitcnt_t) __GMP_ATTRIBUTE_PURE;

#define mpn_scan1 __MPN(scan1)
    __GMP_DECLSPEC mp_bitcnt_t mpn_scan1(mp_srcptr, mp_bitcnt_t) __GMP_ATTRIBUTE_PURE;

#define mpn_set_str __MPN(set_str)
    __GMP_DECLSPEC mp_size_t mpn_set_str(mp_ptr, const unsigned char*, size_t, int);

#define mpn_sizeinbase __MPN(sizeinbase)
    __GMP_DECLSPEC size_t mpn_sizeinbase(mp_srcptr, mp_size_t, int);

#define mpn_sqrtrem __MPN(sqrtrem)
    __GMP_DECLSPEC mp_size_t mpn_sqrtrem(mp_ptr, mp_ptr, mp_srcptr, mp_size_t);

#define mpn_sub __MPN(sub)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_sub)
    __GMP_DECLSPEC mp_limb_t mpn_sub(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t);
#endif

#define mpn_sub_1 __MPN(sub_1)
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpn_sub_1)
    __GMP_DECLSPEC mp_limb_t mpn_sub_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t) __GMP_NOTHROW;
#endif

#define mpn_sub_n __MPN(sub_n)
    __GMP_DECLSPEC mp_limb_t mpn_sub_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);

#define mpn_submul_1 __MPN(submul_1)
    __GMP_DECLSPEC mp_limb_t mpn_submul_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t);

#define mpn_tdiv_qr __MPN(tdiv_qr)
    __GMP_DECLSPEC void mpn_tdiv_qr(mp_ptr, mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t);

#define mpn_and_n __MPN(and_n)
    __GMP_DECLSPEC void mpn_and_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_andn_n __MPN(andn_n)
    __GMP_DECLSPEC void mpn_andn_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_nand_n __MPN(nand_n)
    __GMP_DECLSPEC void mpn_nand_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_ior_n __MPN(ior_n)
    __GMP_DECLSPEC void mpn_ior_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_iorn_n __MPN(iorn_n)
    __GMP_DECLSPEC void mpn_iorn_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_nior_n __MPN(nior_n)
    __GMP_DECLSPEC void mpn_nior_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_xor_n __MPN(xor_n)
    __GMP_DECLSPEC void mpn_xor_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_xnor_n __MPN(xnor_n)
    __GMP_DECLSPEC void mpn_xnor_n(mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);

#define mpn_copyi __MPN(copyi)
    __GMP_DECLSPEC void mpn_copyi(mp_ptr, mp_srcptr, mp_size_t);
#define mpn_copyd __MPN(copyd)
    __GMP_DECLSPEC void mpn_copyd(mp_ptr, mp_srcptr, mp_size_t);
#define mpn_zero __MPN(zero)
    __GMP_DECLSPEC void mpn_zero(mp_ptr, mp_size_t);

#define mpn_cnd_add_n __MPN(cnd_add_n)
    __GMP_DECLSPEC mp_limb_t mpn_cnd_add_n(mp_limb_t, mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);
#define mpn_cnd_sub_n __MPN(cnd_sub_n)
    __GMP_DECLSPEC mp_limb_t mpn_cnd_sub_n(mp_limb_t, mp_ptr, mp_srcptr, mp_srcptr, mp_size_t);

#define mpn_sec_add_1 __MPN(sec_add_1)
    __GMP_DECLSPEC mp_limb_t mpn_sec_add_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_ptr);
#define mpn_sec_add_1_itch __MPN(sec_add_1_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_add_1_itch(mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_sec_sub_1 __MPN(sec_sub_1)
    __GMP_DECLSPEC mp_limb_t mpn_sec_sub_1(mp_ptr, mp_srcptr, mp_size_t, mp_limb_t, mp_ptr);
#define mpn_sec_sub_1_itch __MPN(sec_sub_1_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_sub_1_itch(mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_cnd_swap  __MPN(cnd_swap)
    __GMP_DECLSPEC void mpn_cnd_swap(mp_limb_t, volatile mp_limb_t*, volatile mp_limb_t*, mp_size_t);

#define mpn_sec_mul __MPN(sec_mul)
    __GMP_DECLSPEC void mpn_sec_mul(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t, mp_ptr);
#define mpn_sec_mul_itch __MPN(sec_mul_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_mul_itch(mp_size_t, mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_sec_sqr __MPN(sec_sqr)
    __GMP_DECLSPEC void mpn_sec_sqr(mp_ptr, mp_srcptr, mp_size_t, mp_ptr);
#define mpn_sec_sqr_itch __MPN(sec_sqr_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_sqr_itch(mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_sec_powm __MPN(sec_powm)
    __GMP_DECLSPEC void mpn_sec_powm(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_bitcnt_t, mp_srcptr, mp_size_t, mp_ptr);
#define mpn_sec_powm_itch __MPN(sec_powm_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_powm_itch(mp_size_t, mp_bitcnt_t, mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_sec_tabselect __MPN(sec_tabselect)
    __GMP_DECLSPEC void mpn_sec_tabselect(volatile mp_limb_t*, volatile const mp_limb_t*, mp_size_t, mp_size_t, mp_size_t);

#define mpn_sec_div_qr __MPN(sec_div_qr)
    __GMP_DECLSPEC mp_limb_t mpn_sec_div_qr(mp_ptr, mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_ptr);
#define mpn_sec_div_qr_itch __MPN(sec_div_qr_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_div_qr_itch(mp_size_t, mp_size_t) __GMP_ATTRIBUTE_PURE;
#define mpn_sec_div_r __MPN(sec_div_r)
    __GMP_DECLSPEC void mpn_sec_div_r(mp_ptr, mp_size_t, mp_srcptr, mp_size_t, mp_ptr);
#define mpn_sec_div_r_itch __MPN(sec_div_r_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_div_r_itch(mp_size_t, mp_size_t) __GMP_ATTRIBUTE_PURE;

#define mpn_sec_invert __MPN(sec_invert)
    __GMP_DECLSPEC int mpn_sec_invert(mp_ptr, mp_ptr, mp_srcptr, mp_size_t, mp_bitcnt_t, mp_ptr);
#define mpn_sec_invert_itch __MPN(sec_invert_itch)
    __GMP_DECLSPEC mp_size_t mpn_sec_invert_itch(mp_size_t) __GMP_ATTRIBUTE_PURE;


    /**************** mpz inlines ****************/

    /* The following are provided as inlines where possible, but always exist as
       library functions too, for binary compatibility.

       Within gmp itself this inlining generally isn't relied on, since it
       doesn't get done for all compilers, whereas if something is worth
       inlining then it's worth arranging always.

       There are two styles of inlining here.  When the same bit of code is
       wanted for the inline as for the library version, then __GMP_FORCE_foo
       arranges for that code to be emitted and the __GMP_EXTERN_INLINE
       directive suppressed, eg. mpz_fits_uint_p.  When a different bit of code
       is wanted for the inline than for the library version, then
       __GMP_FORCE_foo arranges the inline to be suppressed, eg. mpz_abs.  */

#if defined (__GMP_EXTERN_INLINE) && ! defined (__GMP_FORCE_mpz_abs)
    __GMP_EXTERN_INLINE void
        mpz_abs(mpz_ptr __gmp_w, mpz_srcptr __gmp_u)
    {
        if (__gmp_w != __gmp_u)
            mpz_set(__gmp_w, __gmp_u);
        __gmp_w->_mp_size = __GMP_ABS(__gmp_w->_mp_size);
    }
#endif

#if GMP_NAIL_BITS == 0
#define __GMPZ_FITS_UTYPE_P(z,maxval)					\
  mp_size_t  __gmp_n = z->_mp_size;					\
  mp_ptr  __gmp_p = z->_mp_d;						\
  return (__gmp_n == 0 || (__gmp_n == 1 && __gmp_p[0] <= maxval));
#else
#define __GMPZ_FITS_UTYPE_P(z,maxval)					\
  mp_size_t  __gmp_n = z->_mp_size;					\
  mp_ptr  __gmp_p = z->_mp_d;						\
  return (__gmp_n == 0 || (__gmp_n == 1 && __gmp_p[0] <= maxval)	\
	  || (__gmp_n == 2 && __gmp_p[1] <= ((mp_limb_t) maxval >> GMP_NUMB_BITS)));
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_fits_uint_p)
#if ! defined (__GMP_FORCE_mpz_fits_uint_p)
    __GMP_EXTERN_INLINE
#endif
        int
        mpz_fits_uint_p(mpz_srcptr __gmp_z) __GMP_NOTHROW
    {
        __GMPZ_FITS_UTYPE_P(__gmp_z, UINT_MAX);
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_fits_ulong_p)
#if ! defined (__GMP_FORCE_mpz_fits_ulong_p)
    __GMP_EXTERN_INLINE
#endif
        int
        mpz_fits_ulong_p(mpz_srcptr __gmp_z) __GMP_NOTHROW
    {
        __GMPZ_FITS_UTYPE_P(__gmp_z, ULONG_MAX);
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_fits_ushort_p)
#if ! defined (__GMP_FORCE_mpz_fits_ushort_p)
    __GMP_EXTERN_INLINE
#endif
        int
        mpz_fits_ushort_p(mpz_srcptr __gmp_z) __GMP_NOTHROW
    {
        __GMPZ_FITS_UTYPE_P(__gmp_z, USHRT_MAX);
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_get_ui)
#if ! defined (__GMP_FORCE_mpz_get_ui)
    __GMP_EXTERN_INLINE
#endif
        unsigned long
        mpz_get_ui(mpz_srcptr __gmp_z) __GMP_NOTHROW
    {
        mp_ptr __gmp_p = __gmp_z->_mp_d;
        mp_size_t __gmp_n = __gmp_z->_mp_size;
        mp_limb_t __gmp_l = __gmp_p[0];
        /* This is a "#if" rather than a plain "if" so as to avoid gcc warnings
           about "<< GMP_NUMB_BITS" exceeding the type size, and to avoid Borland
           C++ 6.0 warnings about condition always true for something like
           "ULONG_MAX < GMP_NUMB_MASK".  */
#if GMP_NAIL_BITS == 0 || defined (_LONG_LONG_LIMB)
           /* limb==long and no nails, or limb==longlong, one limb is enough */
        return (__gmp_n != 0 ? __gmp_l : 0);
#else
           /* limb==long and nails, need two limbs when available */
        __gmp_n = __GMP_ABS(__gmp_n);
        if (__gmp_n <= 1)
            return (__gmp_n != 0 ? __gmp_l : 0);
        else
            return __gmp_l + (__gmp_p[1] << GMP_NUMB_BITS);
#endif
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_getlimbn)
#if ! defined (__GMP_FORCE_mpz_getlimbn)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpz_getlimbn(mpz_srcptr __gmp_z, mp_size_t __gmp_n) __GMP_NOTHROW
    {
        mp_limb_t  __gmp_result = 0;
        if (__GMP_LIKELY(__gmp_n >= 0 && __gmp_n < __GMP_ABS(__gmp_z->_mp_size)))
            __gmp_result = __gmp_z->_mp_d[__gmp_n];
        return __gmp_result;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) && ! defined (__GMP_FORCE_mpz_neg)
    __GMP_EXTERN_INLINE void
        mpz_neg(mpz_ptr __gmp_w, mpz_srcptr __gmp_u)
    {
        if (__gmp_w != __gmp_u)
            mpz_set(__gmp_w, __gmp_u);
        __gmp_w->_mp_size = -__gmp_w->_mp_size;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_perfect_square_p)
#if ! defined (__GMP_FORCE_mpz_perfect_square_p)
    __GMP_EXTERN_INLINE
#endif
        int
        mpz_perfect_square_p(mpz_srcptr __gmp_a)
    {
        mp_size_t __gmp_asize;
        int       __gmp_result;

        __gmp_asize = __gmp_a->_mp_size;
        __gmp_result = (__gmp_asize >= 0);  /* zero is a square, negatives are not */
        if (__GMP_LIKELY(__gmp_asize > 0))
            __gmp_result = mpn_perfect_square_p(__gmp_a->_mp_d, __gmp_asize);
        return __gmp_result;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_popcount)
#if ! defined (__GMP_FORCE_mpz_popcount)
    __GMP_EXTERN_INLINE
#endif
        mp_bitcnt_t
        mpz_popcount(mpz_srcptr __gmp_u) __GMP_NOTHROW
    {
        mp_size_t      __gmp_usize;
        mp_bitcnt_t    __gmp_result;

        __gmp_usize = __gmp_u->_mp_size;
        __gmp_result = (__gmp_usize < 0 ? ~__GMP_CAST(mp_bitcnt_t, 0) : __GMP_CAST(mp_bitcnt_t, 0));
        if (__GMP_LIKELY(__gmp_usize > 0))
            __gmp_result = mpn_popcount(__gmp_u->_mp_d, __gmp_usize);
        return __gmp_result;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_set_q)
#if ! defined (__GMP_FORCE_mpz_set_q)
    __GMP_EXTERN_INLINE
#endif
        void
        mpz_set_q(mpz_ptr __gmp_w, mpq_srcptr __gmp_u)
    {
        mpz_tdiv_q(__gmp_w, mpq_numref(__gmp_u), mpq_denref(__gmp_u));
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpz_size)
#if ! defined (__GMP_FORCE_mpz_size)
    __GMP_EXTERN_INLINE
#endif
        size_t
        mpz_size(mpz_srcptr __gmp_z) __GMP_NOTHROW
    {
        return __GMP_ABS(__gmp_z->_mp_size);
    }
#endif


    /**************** mpq inlines ****************/

#if defined (__GMP_EXTERN_INLINE) && ! defined (__GMP_FORCE_mpq_abs)
    __GMP_EXTERN_INLINE void
        mpq_abs(mpq_ptr __gmp_w, mpq_srcptr __gmp_u)
    {
        if (__gmp_w != __gmp_u)
            mpq_set(__gmp_w, __gmp_u);
        __gmp_w->_mp_num._mp_size = __GMP_ABS(__gmp_w->_mp_num._mp_size);
    }
#endif

#if defined (__GMP_EXTERN_INLINE) && ! defined (__GMP_FORCE_mpq_neg)
    __GMP_EXTERN_INLINE void
        mpq_neg(mpq_ptr __gmp_w, mpq_srcptr __gmp_u)
    {
        if (__gmp_w != __gmp_u)
            mpq_set(__gmp_w, __gmp_u);
        __gmp_w->_mp_num._mp_size = -__gmp_w->_mp_num._mp_size;
    }
#endif


    /**************** mpn inlines ****************/

    /* The comments with __GMPN_ADD_1 below apply here too.

       The test for FUNCTION returning 0 should predict well.  If it's assumed
       {yp,ysize} will usually have a random number of bits then the high limb
       won't be full and a carry out will occur a good deal less than 50% of the
       time.

       ysize==0 isn't a documented feature, but is used internally in a few
       places.

       Producing cout last stops it using up a register during the main part of
       the calculation, though gcc (as of 3.0) on an "if (mpn_add (...))"
       doesn't seem able to move the true and false legs of the conditional up
       to the two places cout is generated.  */

#define __GMPN_AORS(cout, wp, xp, xsize, yp, ysize, FUNCTION, TEST)     \
  do {                                                                  \
    mp_size_t  __gmp_i;                                                 \
    mp_limb_t  __gmp_x;                                                 \
                                                                        \
    /* ASSERT ((ysize) >= 0); */                                        \
    /* ASSERT ((xsize) >= (ysize)); */                                  \
    /* ASSERT (MPN_SAME_OR_SEPARATE2_P (wp, xsize, xp, xsize)); */      \
    /* ASSERT (MPN_SAME_OR_SEPARATE2_P (wp, xsize, yp, ysize)); */      \
                                                                        \
    __gmp_i = (ysize);                                                  \
    if (__gmp_i != 0)                                                   \
      {                                                                 \
        if (FUNCTION (wp, xp, yp, __gmp_i))                             \
          {                                                             \
            do                                                          \
              {                                                         \
                if (__gmp_i >= (xsize))                                 \
                  {                                                     \
                    (cout) = 1;                                         \
                    goto __gmp_done;                                    \
                  }                                                     \
                __gmp_x = (xp)[__gmp_i];                                \
              }                                                         \
            while (TEST);                                               \
          }                                                             \
      }                                                                 \
    if ((wp) != (xp))                                                   \
      __GMPN_COPY_REST (wp, xp, xsize, __gmp_i);                        \
    (cout) = 0;                                                         \
  __gmp_done:                                                           \
    ;                                                                   \
  } while (0)

#define __GMPN_ADD(cout, wp, xp, xsize, yp, ysize)              \
  __GMPN_AORS (cout, wp, xp, xsize, yp, ysize, mpn_add_n,       \
               (((wp)[__gmp_i++] = (__gmp_x + 1) & GMP_NUMB_MASK) == 0))
#define __GMPN_SUB(cout, wp, xp, xsize, yp, ysize)              \
  __GMPN_AORS (cout, wp, xp, xsize, yp, ysize, mpn_sub_n,       \
               (((wp)[__gmp_i++] = (__gmp_x - 1) & GMP_NUMB_MASK), __gmp_x == 0))


       /* The use of __gmp_i indexing is designed to ensure a compile time src==dst
          remains nice and clear to the compiler, so that __GMPN_COPY_REST can
          disappear, and the load/add/store gets a chance to become a
          read-modify-write on CISC CPUs.

          Alternatives:

          Using a pair of pointers instead of indexing would be possible, but gcc
          isn't able to recognise compile-time src==dst in that case, even when the
          pointers are incremented more or less together.  Other compilers would
          very likely have similar difficulty.

          gcc could use "if (__builtin_constant_p(src==dst) && src==dst)" or
          similar to detect a compile-time src==dst.  This works nicely on gcc
          2.95.x, it's not good on gcc 3.0 where __builtin_constant_p(p==p) seems
          to be always false, for a pointer p.  But the current code form seems
          good enough for src==dst anyway.

          gcc on x86 as usual doesn't give particularly good flags handling for the
          carry/borrow detection.  It's tempting to want some multi instruction asm
          blocks to help it, and this was tried, but in truth there's only a few
          instructions to save and any gain is all too easily lost by register
          juggling setting up for the asm.  */

#if GMP_NAIL_BITS == 0
#define __GMPN_AORS_1(cout, dst, src, n, v, OP, CB)		\
  do {								\
    mp_size_t  __gmp_i;						\
    mp_limb_t  __gmp_x, __gmp_r;                                \
								\
    /* ASSERT ((n) >= 1); */					\
    /* ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, n)); */	\
								\
    __gmp_x = (src)[0];						\
    __gmp_r = __gmp_x OP (v);                                   \
    (dst)[0] = __gmp_r;						\
    if (CB (__gmp_r, __gmp_x, (v)))                             \
      {								\
	(cout) = 1;						\
	for (__gmp_i = 1; __gmp_i < (n);)                       \
	  {							\
	    __gmp_x = (src)[__gmp_i];                           \
	    __gmp_r = __gmp_x OP 1;                             \
	    (dst)[__gmp_i] = __gmp_r;                           \
	    ++__gmp_i;						\
	    if (!CB (__gmp_r, __gmp_x, 1))                      \
	      {							\
		if ((src) != (dst))				\
		  __GMPN_COPY_REST (dst, src, n, __gmp_i);      \
		(cout) = 0;					\
		break;						\
	      }							\
	  }							\
      }								\
    else							\
      {								\
	if ((src) != (dst))					\
	  __GMPN_COPY_REST (dst, src, n, 1);			\
	(cout) = 0;						\
      }								\
  } while (0)
#endif

#if GMP_NAIL_BITS >= 1
#define __GMPN_AORS_1(cout, dst, src, n, v, OP, CB)		\
  do {								\
    mp_size_t  __gmp_i;						\
    mp_limb_t  __gmp_x, __gmp_r;				\
								\
    /* ASSERT ((n) >= 1); */					\
    /* ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, n)); */	\
								\
    __gmp_x = (src)[0];						\
    __gmp_r = __gmp_x OP (v);					\
    (dst)[0] = __gmp_r & GMP_NUMB_MASK;				\
    if (__gmp_r >> GMP_NUMB_BITS != 0)				\
      {								\
	(cout) = 1;						\
	for (__gmp_i = 1; __gmp_i < (n);)			\
	  {							\
	    __gmp_x = (src)[__gmp_i];				\
	    __gmp_r = __gmp_x OP 1;				\
	    (dst)[__gmp_i] = __gmp_r & GMP_NUMB_MASK;		\
	    ++__gmp_i;						\
	    if (__gmp_r >> GMP_NUMB_BITS == 0)			\
	      {							\
		if ((src) != (dst))				\
		  __GMPN_COPY_REST (dst, src, n, __gmp_i);	\
		(cout) = 0;					\
		break;						\
	      }							\
	  }							\
      }								\
    else							\
      {								\
	if ((src) != (dst))					\
	  __GMPN_COPY_REST (dst, src, n, 1);			\
	(cout) = 0;						\
      }								\
  } while (0)
#endif

#define __GMPN_ADDCB(r,x,y) ((r) < (y))
#define __GMPN_SUBCB(r,x,y) ((x) < (y))

#define __GMPN_ADD_1(cout, dst, src, n, v)	     \
  __GMPN_AORS_1(cout, dst, src, n, v, +, __GMPN_ADDCB)
#define __GMPN_SUB_1(cout, dst, src, n, v)	     \
  __GMPN_AORS_1(cout, dst, src, n, v, -, __GMPN_SUBCB)


          /* Compare {xp,size} and {yp,size}, setting "result" to positive, zero or
             negative.  size==0 is allowed.  On random data usually only one limb will
             need to be examined to get a result, so it's worth having it inline.  */
#define __GMPN_CMP(result, xp, yp, size)                                \
  do {                                                                  \
    mp_size_t  __gmp_i;                                                 \
    mp_limb_t  __gmp_x, __gmp_y;                                        \
                                                                        \
    /* ASSERT ((size) >= 0); */                                         \
                                                                        \
    (result) = 0;                                                       \
    __gmp_i = (size);                                                   \
    while (--__gmp_i >= 0)                                              \
      {                                                                 \
        __gmp_x = (xp)[__gmp_i];                                        \
        __gmp_y = (yp)[__gmp_i];                                        \
        if (__gmp_x != __gmp_y)                                         \
          {                                                             \
            /* Cannot use __gmp_x - __gmp_y, may overflow an "int" */   \
            (result) = (__gmp_x > __gmp_y ? 1 : -1);                    \
            break;                                                      \
          }                                                             \
      }                                                                 \
  } while (0)


#if defined (__GMPN_COPY) && ! defined (__GMPN_COPY_REST)
#define __GMPN_COPY_REST(dst, src, size, start)                 \
  do {                                                          \
    /* ASSERT ((start) >= 0); */                                \
    /* ASSERT ((start) <= (size)); */                           \
    __GMPN_COPY ((dst)+(start), (src)+(start), (size)-(start)); \
  } while (0)
#endif

             /* Copy {src,size} to {dst,size}, starting at "start".  This is designed to
                keep the indexing dst[j] and src[j] nice and simple for __GMPN_ADD_1,
                __GMPN_ADD, etc.  */
#if ! defined (__GMPN_COPY_REST)
#define __GMPN_COPY_REST(dst, src, size, start)                 \
  do {                                                          \
    mp_size_t __gmp_j;                                          \
    /* ASSERT ((size) >= 0); */                                 \
    /* ASSERT ((start) >= 0); */                                \
    /* ASSERT ((start) <= (size)); */                           \
    /* ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, size)); */     \
    __GMP_CRAY_Pragma ("_CRI ivdep");                           \
    for (__gmp_j = (start); __gmp_j < (size); __gmp_j++)        \
      (dst)[__gmp_j] = (src)[__gmp_j];                          \
  } while (0)
#endif

                /* Enhancement: Use some of the smarter code from gmp-impl.h.  Maybe use
                   mpn_copyi if there's a native version, and if we don't mind demanding
                   binary compatibility for it (on targets which use it).  */

#if ! defined (__GMPN_COPY)
#define __GMPN_COPY(dst, src, size)   __GMPN_COPY_REST (dst, src, size, 0)
#endif


#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_add)
#if ! defined (__GMP_FORCE_mpn_add)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpn_add(mp_ptr __gmp_wp, mp_srcptr __gmp_xp, mp_size_t __gmp_xsize, mp_srcptr __gmp_yp, mp_size_t __gmp_ysize)
    {
        mp_limb_t  __gmp_c;
        __GMPN_ADD(__gmp_c, __gmp_wp, __gmp_xp, __gmp_xsize, __gmp_yp, __gmp_ysize);
        return __gmp_c;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_add_1)
#if ! defined (__GMP_FORCE_mpn_add_1)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpn_add_1(mp_ptr __gmp_dst, mp_srcptr __gmp_src, mp_size_t __gmp_size, mp_limb_t __gmp_n) __GMP_NOTHROW
    {
        mp_limb_t  __gmp_c;
        __GMPN_ADD_1(__gmp_c, __gmp_dst, __gmp_src, __gmp_size, __gmp_n);
        return __gmp_c;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_cmp)
#if ! defined (__GMP_FORCE_mpn_cmp)
    __GMP_EXTERN_INLINE
#endif
        int
        mpn_cmp(mp_srcptr __gmp_xp, mp_srcptr __gmp_yp, mp_size_t __gmp_size) __GMP_NOTHROW
    {
        int __gmp_result;
        __GMPN_CMP(__gmp_result, __gmp_xp, __gmp_yp, __gmp_size);
        return __gmp_result;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_zero_p)
#if ! defined (__GMP_FORCE_mpn_zero_p)
    __GMP_EXTERN_INLINE
#endif
        int
        mpn_zero_p(mp_srcptr __gmp_p, mp_size_t __gmp_n) __GMP_NOTHROW
    {
        /* if (__GMP_LIKELY (__gmp_n > 0)) */
        do {
            if (__gmp_p[--__gmp_n] != 0)
                return 0;
        } while (__gmp_n != 0);
        return 1;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_sub)
#if ! defined (__GMP_FORCE_mpn_sub)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpn_sub(mp_ptr __gmp_wp, mp_srcptr __gmp_xp, mp_size_t __gmp_xsize, mp_srcptr __gmp_yp, mp_size_t __gmp_ysize)
    {
        mp_limb_t  __gmp_c;
        __GMPN_SUB(__gmp_c, __gmp_wp, __gmp_xp, __gmp_xsize, __gmp_yp, __gmp_ysize);
        return __gmp_c;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_sub_1)
#if ! defined (__GMP_FORCE_mpn_sub_1)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpn_sub_1(mp_ptr __gmp_dst, mp_srcptr __gmp_src, mp_size_t __gmp_size, mp_limb_t __gmp_n) __GMP_NOTHROW
    {
        mp_limb_t  __gmp_c;
        __GMPN_SUB_1(__gmp_c, __gmp_dst, __gmp_src, __gmp_size, __gmp_n);
        return __gmp_c;
    }
#endif

#if defined (__GMP_EXTERN_INLINE) || defined (__GMP_FORCE_mpn_neg)
#if ! defined (__GMP_FORCE_mpn_neg)
    __GMP_EXTERN_INLINE
#endif
        mp_limb_t
        mpn_neg(mp_ptr __gmp_rp, mp_srcptr __gmp_up, mp_size_t __gmp_n)
    {
        while (*__gmp_up == 0) /* Low zero limbs are unchanged by negation. */
        {
            *__gmp_rp = 0;
            if (!--__gmp_n) /* All zero */
                return 0;
            ++__gmp_up; ++__gmp_rp;
        }

        *__gmp_rp = (-*__gmp_up) & GMP_NUMB_MASK;

        if (--__gmp_n) /* Higher limbs get complemented. */
            mpn_com(++__gmp_rp, ++__gmp_up, __gmp_n);

        return 1;
    }
#endif

#if defined (__cplusplus)
}
#endif


/* Allow faster testing for negative, zero, and positive.  */
#define mpz_sgn(Z) ((Z)->_mp_size < 0 ? -1 : (Z)->_mp_size > 0)
#define mpf_sgn(F) ((F)->_mp_size < 0 ? -1 : (F)->_mp_size > 0)
#define mpq_sgn(Q) ((Q)->_mp_num._mp_size < 0 ? -1 : (Q)->_mp_num._mp_size > 0)

/* When using GCC, optimize certain common comparisons.  */
#if defined (__GNUC__) && __GNUC__ >= 2
#define mpz_cmp_ui(Z,UI) \
  (__builtin_constant_p (UI) && (UI) == 0				\
   ? mpz_sgn (Z) : _mpz_cmp_ui (Z,UI))
#define mpz_cmp_si(Z,SI)						\
  (__builtin_constant_p ((SI) >= 0) && (SI) >= 0			\
   ? mpz_cmp_ui (Z, __GMP_CAST (unsigned long, SI))			\
   : _mpz_cmp_si (Z,SI))
#define mpq_cmp_ui(Q,NUI,DUI)					\
  (__builtin_constant_p (NUI) && (NUI) == 0 ? mpq_sgn (Q)	\
   : __builtin_constant_p ((NUI) == (DUI)) && (NUI) == (DUI)	\
   ? mpz_cmp (mpq_numref (Q), mpq_denref (Q))			\
   : _mpq_cmp_ui (Q,NUI,DUI))
#define mpq_cmp_si(q,n,d)				\
  (__builtin_constant_p ((n) >= 0) && (n) >= 0		\
   ? mpq_cmp_ui (q, __GMP_CAST (unsigned long, n), d)	\
   : _mpq_cmp_si (q, n, d))
#else
#define mpz_cmp_ui(Z,UI) _mpz_cmp_ui (Z,UI)
#define mpz_cmp_si(Z,UI) _mpz_cmp_si (Z,UI)
#define mpq_cmp_ui(Q,NUI,DUI) _mpq_cmp_ui (Q,NUI,DUI)
#define mpq_cmp_si(q,n,d)  _mpq_cmp_si(q,n,d)
#endif


/* Using "&" rather than "&&" means these can come out branch-free.  Every
   mpz_t has at least one limb allocated, so fetching the low limb is always
   allowed.  */
#define mpz_odd_p(z)   (((z)->_mp_size != 0) & __GMP_CAST (int, (z)->_mp_d[0]))
#define mpz_even_p(z)  (! mpz_odd_p (z))


   /**************** C++ routines ****************/

#ifdef __cplusplus
__GMP_DECLSPEC_XX std::ostream& operator<< (std::ostream&, mpz_srcptr);
__GMP_DECLSPEC_XX std::ostream& operator<< (std::ostream&, mpq_srcptr);
__GMP_DECLSPEC_XX std::ostream& operator<< (std::ostream&, mpf_srcptr);
__GMP_DECLSPEC_XX std::istream& operator>> (std::istream&, mpz_ptr);
__GMP_DECLSPEC_XX std::istream& operator>> (std::istream&, mpq_ptr);
__GMP_DECLSPEC_XX std::istream& operator>> (std::istream&, mpf_ptr);
#endif


/* Source-level compatibility with GMP 2 and earlier. */
#define mpn_divmod(qp,np,nsize,dp,dsize) \
  mpn_divrem (qp, __GMP_CAST (mp_size_t, 0), np, nsize, dp, dsize)

/* Source-level compatibility with GMP 1.  */
#define mpz_mdiv	mpz_fdiv_q
#define mpz_mdivmod	mpz_fdiv_qr
#define mpz_mmod	mpz_fdiv_r
#define mpz_mdiv_ui	mpz_fdiv_q_ui
#define mpz_mdivmod_ui(q,r,n,d) \
  (((r) == 0) ? mpz_fdiv_q_ui (q,n,d) : mpz_fdiv_qr_ui (q,r,n,d))
#define mpz_mmod_ui(r,n,d) \
  (((r) == 0) ? mpz_fdiv_ui (n,d) : mpz_fdiv_r_ui (r,n,d))

/* Useful synonyms, but not quite compatible with GMP 1.  */
#define mpz_div		mpz_fdiv_q
#define mpz_divmod	mpz_fdiv_qr
#define mpz_div_ui	mpz_fdiv_q_ui
#define mpz_divmod_ui	mpz_fdiv_qr_ui
#define mpz_div_2exp	mpz_fdiv_q_2exp
#define mpz_mod_2exp	mpz_fdiv_r_2exp

enum
{
    GMP_ERROR_NONE = 0,
    GMP_ERROR_UNSUPPORTED_ARGUMENT = 1,
    GMP_ERROR_DIVISION_BY_ZERO = 2,
    GMP_ERROR_SQRT_OF_NEGATIVE = 4,
    GMP_ERROR_INVALID_ARGUMENT = 8
};

/* Define CC and CFLAGS which were used to build this version of GMP */
#define __GMP_CC "compile cl.exe"
#define __GMP_CFLAGS "-Xcompiler -nologo -Xcompiler -W3 -Xcompiler -utf-8 -Xcompiler -MP -Xcompiler -MD -Xcompiler -O2 -Xcompiler -Oi -Xcompiler -Gy -Xcompiler -Z7"

/* Major version number is the value of __GNU_MP__ too, above. */
#define __GNU_MP_VERSION            6
#define __GNU_MP_VERSION_MINOR      2
#define __GNU_MP_VERSION_PATCHLEVEL 1
#define __GNU_MP_RELEASE (__GNU_MP_VERSION * 10000 + __GNU_MP_VERSION_MINOR * 100 + __GNU_MP_VERSION_PATCHLEVEL)

#define __GMP_H__
#endif /* __GMP_H__ */

// wrapper for gcc's __builtin_constant_p
// __builtin_constant_p has been in gcc since forever,
// but g++-3.4 miscompiles it.
#if __GMP_GNUC_PREREQ(4, 2)
#define __GMPXX_CONSTANT(X) __builtin_constant_p(X)
#else
#define __GMPXX_CONSTANT(X) false
#endif
#define __GMPXX_CONSTANT_TRUE(X) (__GMPXX_CONSTANT(X) && (X))

// Use C++11 features
#ifndef __GMPXX_USE_CXX11
#if __cplusplus >= 201103L
#define __GMPXX_USE_CXX11 1
#else
#define __GMPXX_USE_CXX11 0
#endif
#endif

#if __GMPXX_USE_CXX11
#define __GMPXX_NOEXCEPT noexcept
#include <type_traits> // for common_type
#else
#define __GMPXX_NOEXCEPT
#endif

// Max allocations for plain types when converted to GMP types
#if GMP_NAIL_BITS != 0 && ! defined _LONG_LONG_LIMB
#define __GMPZ_ULI_LIMBS 2
#else
#define __GMPZ_ULI_LIMBS 1
#endif

#define __GMPXX_BITS_TO_LIMBS(n)  (((n) + (GMP_NUMB_BITS - 1)) / GMP_NUMB_BITS)
#define __GMPZ_DBL_LIMBS __GMPXX_BITS_TO_LIMBS(DBL_MAX_EXP)+1
#define __GMPQ_NUM_DBL_LIMBS __GMPZ_DBL_LIMBS
#define __GMPQ_DEN_DBL_LIMBS __GMPXX_BITS_TO_LIMBS(DBL_MANT_DIG+1-DBL_MIN_EXP)+1
// The final +1s are a security margin. The current implementation of
// mpq_set_d seems to need it for the denominator.

inline void __mpz_set_ui_safe(mpz_ptr p, unsigned long l)
{
    p->_mp_size = (l != 0);
    p->_mp_d[0] = l & GMP_NUMB_MASK;
#if __GMPZ_ULI_LIMBS > 1
    l >>= GMP_NUMB_BITS;
    p->_mp_d[1] = l;
    p->_mp_size += (l != 0);
#endif
}

inline void __mpz_set_si_safe(mpz_ptr p, long l)
{
    if (l < 0)
    {
        __mpz_set_ui_safe(p, -static_cast<unsigned long>(l));
        mpz_neg(p, p);
    }
    else
        __mpz_set_ui_safe(p, l);
    // Note: we know the high bit of l is 0 so we could do slightly better
}

// Fake temporary variables
#define __GMPXX_TMPZ_UI							\
  mpz_t temp;								\
  mp_limb_t limbs[__GMPZ_ULI_LIMBS];					\
  temp->_mp_d = limbs;							\
  __mpz_set_ui_safe (temp, l)
#define __GMPXX_TMPZ_SI							\
  mpz_t temp;								\
  mp_limb_t limbs[__GMPZ_ULI_LIMBS];					\
  temp->_mp_d = limbs;							\
  __mpz_set_si_safe (temp, l)
#define __GMPXX_TMPZ_D							\
  mpz_t temp;								\
  mp_limb_t limbs[__GMPZ_DBL_LIMBS];					\
  temp->_mp_d = limbs;							\
  temp->_mp_alloc = __GMPZ_DBL_LIMBS;					\
  mpz_set_d (temp, d)

#define __GMPXX_TMPQ_UI							\
  mpq_t temp;								\
  mp_limb_t limbs[__GMPZ_ULI_LIMBS+1];					\
  mpq_numref(temp)->_mp_d = limbs;					\
  __mpz_set_ui_safe (mpq_numref(temp), l);				\
  mpq_denref(temp)->_mp_d = limbs + __GMPZ_ULI_LIMBS;			\
  mpq_denref(temp)->_mp_size = 1;					\
  mpq_denref(temp)->_mp_d[0] = 1
#define __GMPXX_TMPQ_SI							\
  mpq_t temp;								\
  mp_limb_t limbs[__GMPZ_ULI_LIMBS+1];					\
  mpq_numref(temp)->_mp_d = limbs;					\
  __mpz_set_si_safe (mpq_numref(temp), l);				\
  mpq_denref(temp)->_mp_d = limbs + __GMPZ_ULI_LIMBS;			\
  mpq_denref(temp)->_mp_size = 1;					\
  mpq_denref(temp)->_mp_d[0] = 1
#define __GMPXX_TMPQ_D							\
  mpq_t temp;								\
  mp_limb_t limbs[__GMPQ_NUM_DBL_LIMBS + __GMPQ_DEN_DBL_LIMBS];		\
  mpq_numref(temp)->_mp_d = limbs;					\
  mpq_numref(temp)->_mp_alloc = __GMPQ_NUM_DBL_LIMBS;			\
  mpq_denref(temp)->_mp_d = limbs + __GMPQ_NUM_DBL_LIMBS;		\
  mpq_denref(temp)->_mp_alloc = __GMPQ_DEN_DBL_LIMBS;			\
  mpq_set_d (temp, d)

inline unsigned long __gmpxx_abs_ui(signed long l)
{
    return l >= 0 ? static_cast<unsigned long>(l)
        : -static_cast<unsigned long>(l);
}

/**************** Function objects ****************/
/* Any evaluation of a __gmp_expr ends up calling one of these functions
   all intermediate functions being inline, the evaluation should optimize
   to a direct call to the relevant function, thus yielding no overhead
   over the C interface. */

struct __gmp_unary_plus
{
    static void eval(mpz_ptr z, mpz_srcptr w) { mpz_set(z, w); }
    static void eval(mpq_ptr q, mpq_srcptr r) { mpq_set(q, r); }
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_set(f, g); }
};

struct __gmp_unary_minus
{
    static void eval(mpz_ptr z, mpz_srcptr w) { mpz_neg(z, w); }
    static void eval(mpq_ptr q, mpq_srcptr r) { mpq_neg(q, r); }
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_neg(f, g); }
};

struct __gmp_unary_com
{
    static void eval(mpz_ptr z, mpz_srcptr w) { mpz_com(z, w); }
};

struct __gmp_binary_plus
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_add(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        // Ideally, those checks should happen earlier so that the tree
        // generated for a+0+b would just be sum(a,b).
        if (__GMPXX_CONSTANT(l) && l == 0)
        {
            if (z != w) mpz_set(z, w);
        }
        else
            mpz_add_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        if (l >= 0)
            eval(z, w, static_cast<unsigned long>(l));
        else
            mpz_sub_ui(z, w, -static_cast<unsigned long>(l));
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_add(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpq_srcptr s)
    {
        mpq_add(q, r, s);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, unsigned long int l)
    {
        if (__GMPXX_CONSTANT(l) && l == 0)
        {
            if (q != r) mpq_set(q, r);
        }
        else if (__GMPXX_CONSTANT(l) && l == 1)
        {
            mpz_add(mpq_numref(q), mpq_numref(r), mpq_denref(r));
            if (q != r) mpz_set(mpq_denref(q), mpq_denref(r));
        }
        else
        {
            if (q == r)
                mpz_addmul_ui(mpq_numref(q), mpq_denref(q), l);
            else
            {
                mpz_mul_ui(mpq_numref(q), mpq_denref(r), l);
                mpz_add(mpq_numref(q), mpq_numref(q), mpq_numref(r));
                mpz_set(mpq_denref(q), mpq_denref(r));
            }
        }
    }
    static void eval(mpq_ptr q, unsigned long int l, mpq_srcptr r)
    {
        eval(q, r, l);
    }
    static inline void eval(mpq_ptr q, mpq_srcptr r, signed long int l);
    // defined after __gmp_binary_minus
    static void eval(mpq_ptr q, signed long int l, mpq_srcptr r)
    {
        eval(q, r, l);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, double d)
    {
        __GMPXX_TMPQ_D;    mpq_add(q, r, temp);
    }
    static void eval(mpq_ptr q, double d, mpq_srcptr r)
    {
        eval(q, r, d);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpz_srcptr z)
    {
        if (q == r)
            mpz_addmul(mpq_numref(q), mpq_denref(q), z);
        else
        {
            mpz_mul(mpq_numref(q), mpq_denref(r), z);
            mpz_add(mpq_numref(q), mpq_numref(q), mpq_numref(r));
            mpz_set(mpq_denref(q), mpq_denref(r));
        }
    }
    static void eval(mpq_ptr q, mpz_srcptr z, mpq_srcptr r)
    {
        eval(q, r, z);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, mpf_srcptr h)
    {
        mpf_add(f, g, h);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, unsigned long int l)
    {
        mpf_add_ui(f, g, l);
    }
    static void eval(mpf_ptr f, unsigned long int l, mpf_srcptr g)
    {
        mpf_add_ui(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, signed long int l)
    {
        if (l >= 0)
            mpf_add_ui(f, g, l);
        else
            mpf_sub_ui(f, g, -static_cast<unsigned long>(l));
    }
    static void eval(mpf_ptr f, signed long int l, mpf_srcptr g)
    {
        eval(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, double d)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_add(f, g, temp);
        mpf_clear(temp);
    }
    static void eval(mpf_ptr f, double d, mpf_srcptr g)
    {
        eval(f, g, d);
    }
};

struct __gmp_binary_minus
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_sub(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        if (__GMPXX_CONSTANT(l) && l == 0)
        {
            if (z != w) mpz_set(z, w);
        }
        else
            mpz_sub_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        if (__GMPXX_CONSTANT(l) && l == 0)
        {
            mpz_neg(z, w);
        }
        else
            mpz_ui_sub(z, l, w);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        if (l >= 0)
            eval(z, w, static_cast<unsigned long>(l));
        else
            mpz_add_ui(z, w, -static_cast<unsigned long>(l));
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        if (l >= 0)
            eval(z, static_cast<unsigned long>(l), w);
        else
        {
            mpz_add_ui(z, w, -static_cast<unsigned long>(l));
            mpz_neg(z, z);
        }
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_sub(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        __GMPXX_TMPZ_D;    mpz_sub(z, temp, w);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpq_srcptr s)
    {
        mpq_sub(q, r, s);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, unsigned long int l)
    {
        if (__GMPXX_CONSTANT(l) && l == 0)
        {
            if (q != r) mpq_set(q, r);
        }
        else if (__GMPXX_CONSTANT(l) && l == 1)
        {
            mpz_sub(mpq_numref(q), mpq_numref(r), mpq_denref(r));
            if (q != r) mpz_set(mpq_denref(q), mpq_denref(r));
        }
        else
        {
            if (q == r)
                mpz_submul_ui(mpq_numref(q), mpq_denref(q), l);
            else
            {
                mpz_mul_ui(mpq_numref(q), mpq_denref(r), l);
                mpz_sub(mpq_numref(q), mpq_numref(r), mpq_numref(q));
                mpz_set(mpq_denref(q), mpq_denref(r));
            }
        }
    }
    static void eval(mpq_ptr q, unsigned long int l, mpq_srcptr r)
    {
        eval(q, r, l); mpq_neg(q, q);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, signed long int l)
    {
        if (l >= 0)
            eval(q, r, static_cast<unsigned long>(l));
        else
            __gmp_binary_plus::eval(q, r, -static_cast<unsigned long>(l));
    }
    static void eval(mpq_ptr q, signed long int l, mpq_srcptr r)
    {
        eval(q, r, l); mpq_neg(q, q);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, double d)
    {
        __GMPXX_TMPQ_D;    mpq_sub(q, r, temp);
    }
    static void eval(mpq_ptr q, double d, mpq_srcptr r)
    {
        __GMPXX_TMPQ_D;    mpq_sub(q, temp, r);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpz_srcptr z)
    {
        if (q == r)
            mpz_submul(mpq_numref(q), mpq_denref(q), z);
        else
        {
            mpz_mul(mpq_numref(q), mpq_denref(r), z);
            mpz_sub(mpq_numref(q), mpq_numref(r), mpq_numref(q));
            mpz_set(mpq_denref(q), mpq_denref(r));
        }
    }
    static void eval(mpq_ptr q, mpz_srcptr z, mpq_srcptr r)
    {
        eval(q, r, z); mpq_neg(q, q);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, mpf_srcptr h)
    {
        mpf_sub(f, g, h);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, unsigned long int l)
    {
        mpf_sub_ui(f, g, l);
    }
    static void eval(mpf_ptr f, unsigned long int l, mpf_srcptr g)
    {
        mpf_ui_sub(f, l, g);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, signed long int l)
    {
        if (l >= 0)
            mpf_sub_ui(f, g, l);
        else
            mpf_add_ui(f, g, -static_cast<unsigned long>(l));
    }
    static void eval(mpf_ptr f, signed long int l, mpf_srcptr g)
    {
        if (l >= 0)
            mpf_sub_ui(f, g, l);
        else
            mpf_add_ui(f, g, -static_cast<unsigned long>(l));
        mpf_neg(f, f);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, double d)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_sub(f, g, temp);
        mpf_clear(temp);
    }
    static void eval(mpf_ptr f, double d, mpf_srcptr g)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_sub(f, temp, g);
        mpf_clear(temp);
    }
};

// defined here so it can reference __gmp_binary_minus
inline void
__gmp_binary_plus::eval(mpq_ptr q, mpq_srcptr r, signed long int l)
{
    if (l >= 0)
        eval(q, r, static_cast<unsigned long>(l));
    else
        __gmp_binary_minus::eval(q, r, -static_cast<unsigned long>(l));
}

struct __gmp_binary_lshift
{
    static void eval(mpz_ptr z, mpz_srcptr w, mp_bitcnt_t l)
    {
        if (__GMPXX_CONSTANT(l) && (l == 0))
        {
            if (z != w) mpz_set(z, w);
        }
        else
            mpz_mul_2exp(z, w, l);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, mp_bitcnt_t l)
    {
        if (__GMPXX_CONSTANT(l) && (l == 0))
        {
            if (q != r) mpq_set(q, r);
        }
        else
            mpq_mul_2exp(q, r, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, mp_bitcnt_t l)
    {
        mpf_mul_2exp(f, g, l);
    }
};

struct __gmp_binary_rshift
{
    static void eval(mpz_ptr z, mpz_srcptr w, mp_bitcnt_t l)
    {
        if (__GMPXX_CONSTANT(l) && (l == 0))
        {
            if (z != w) mpz_set(z, w);
        }
        else
            mpz_fdiv_q_2exp(z, w, l);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, mp_bitcnt_t l)
    {
        if (__GMPXX_CONSTANT(l) && (l == 0))
        {
            if (q != r) mpq_set(q, r);
        }
        else
            mpq_div_2exp(q, r, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, mp_bitcnt_t l)
    {
        mpf_div_2exp(f, g, l);
    }
};

struct __gmp_binary_multiplies
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_mul(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        // gcc-3.3 doesn't have __builtin_ctzl. Don't bother optimizing for old gcc.
#if __GMP_GNUC_PREREQ(3, 4)
        if (__GMPXX_CONSTANT(l) && (l & (l - 1)) == 0)
        {
            if (l == 0)
            {
                z->_mp_size = 0;
            }
            else
            {
                __gmp_binary_lshift::eval(z, w, __builtin_ctzl(l));
            }
        }
        else
#endif
            mpz_mul_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            eval(z, w, static_cast<unsigned long>(l));
        else if (__GMPXX_CONSTANT_TRUE(l <= 0))
        {
            eval(z, w, -static_cast<unsigned long>(l));
            mpz_neg(z, z);
        }
        else
            mpz_mul_si(z, w, l);
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_mul(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpq_srcptr s)
    {
        mpq_mul(q, r, s);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, unsigned long int l)
    {
#if __GMP_GNUC_PREREQ(3, 4)
        if (__GMPXX_CONSTANT(l) && (l & (l - 1)) == 0)
        {
            if (l == 0)
            {
                mpq_set_ui(q, 0, 1);
            }
            else
            {
                __gmp_binary_lshift::eval(q, r, __builtin_ctzl(l));
            }
        }
        else
#endif
        {
            __GMPXX_TMPQ_UI;
            mpq_mul(q, r, temp);
        }
    }
    static void eval(mpq_ptr q, unsigned long int l, mpq_srcptr r)
    {
        eval(q, r, l);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, signed long int l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            eval(q, r, static_cast<unsigned long>(l));
        else if (__GMPXX_CONSTANT_TRUE(l <= 0))
        {
            eval(q, r, -static_cast<unsigned long>(l));
            mpq_neg(q, q);
        }
        else
        {
            __GMPXX_TMPQ_SI;
            mpq_mul(q, r, temp);
        }
    }
    static void eval(mpq_ptr q, signed long int l, mpq_srcptr r)
    {
        eval(q, r, l);
    }
    static void eval(mpq_ptr q, mpq_srcptr r, double d)
    {
        __GMPXX_TMPQ_D;    mpq_mul(q, r, temp);
    }
    static void eval(mpq_ptr q, double d, mpq_srcptr r)
    {
        eval(q, r, d);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, mpf_srcptr h)
    {
        mpf_mul(f, g, h);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, unsigned long int l)
    {
        mpf_mul_ui(f, g, l);
    }
    static void eval(mpf_ptr f, unsigned long int l, mpf_srcptr g)
    {
        mpf_mul_ui(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, signed long int l)
    {
        if (l >= 0)
            mpf_mul_ui(f, g, l);
        else
        {
            mpf_mul_ui(f, g, -static_cast<unsigned long>(l));
            mpf_neg(f, f);
        }
    }
    static void eval(mpf_ptr f, signed long int l, mpf_srcptr g)
    {
        eval(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, double d)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_mul(f, g, temp);
        mpf_clear(temp);
    }
    static void eval(mpf_ptr f, double d, mpf_srcptr g)
    {
        eval(f, g, d);
    }
};

struct __gmp_binary_divides
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_tdiv_q(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
#if __GMP_GNUC_PREREQ(3, 4)
        // Don't optimize division by 0...
        if (__GMPXX_CONSTANT(l) && (l & (l - 1)) == 0 && l != 0)
        {
            if (l == 1)
            {
                if (z != w) mpz_set(z, w);
            }
            else
                mpz_tdiv_q_2exp(z, w, __builtin_ctzl(l));
            // warning: do not use rshift (fdiv)
        }
        else
#endif
            mpz_tdiv_q_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        if (mpz_sgn(w) >= 0)
        {
            if (mpz_fits_ulong_p(w))
                mpz_set_ui(z, l / mpz_get_ui(w));
            else
                mpz_set_ui(z, 0);
        }
        else
        {
            mpz_neg(z, w);
            if (mpz_fits_ulong_p(z))
            {
                mpz_set_ui(z, l / mpz_get_ui(z));
                mpz_neg(z, z);
            }
            else
                mpz_set_ui(z, 0);
        }
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        if (l >= 0)
            eval(z, w, static_cast<unsigned long>(l));
        else
        {
            eval(z, w, -static_cast<unsigned long>(l));
            mpz_neg(z, z);
        }
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        if (mpz_fits_slong_p(w))
            mpz_set_si(z, l / mpz_get_si(w));
        else
        {
            /* if w is bigger than a long then the quotient must be zero, unless
               l==LONG_MIN and w==-LONG_MIN in which case the quotient is -1 */
            mpz_set_si(z, (mpz_cmpabs_ui(w, __gmpxx_abs_ui(l)) == 0 ? -1 : 0));
        }
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_tdiv_q(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        __GMPXX_TMPZ_D;    mpz_tdiv_q(z, temp, w);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, mpq_srcptr s)
    {
        mpq_div(q, r, s);
    }

    static void eval(mpq_ptr q, mpq_srcptr r, unsigned long int l)
    {
#if __GMP_GNUC_PREREQ(3, 4)
        if (__GMPXX_CONSTANT(l) && (l & (l - 1)) == 0 && l != 0)
            __gmp_binary_rshift::eval(q, r, __builtin_ctzl(l));
        else
#endif
        {
            __GMPXX_TMPQ_UI;
            mpq_div(q, r, temp);
        }
    }
    static void eval(mpq_ptr q, unsigned long int l, mpq_srcptr r)
    {
        if (__GMPXX_CONSTANT_TRUE(l == 0))
            mpq_set_ui(q, 0, 1);
        else if (__GMPXX_CONSTANT_TRUE(l == 1))
            mpq_inv(q, r);
        else
        {
            __GMPXX_TMPQ_UI;
            mpq_div(q, temp, r);
        }
    }
    static void eval(mpq_ptr q, mpq_srcptr r, signed long int l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            eval(q, r, static_cast<unsigned long>(l));
        else if (__GMPXX_CONSTANT_TRUE(l <= 0))
        {
            eval(q, r, -static_cast<unsigned long>(l));
            mpq_neg(q, q);
        }
        else
        {
            __GMPXX_TMPQ_SI;
            mpq_div(q, r, temp);
      }
  }
    static void eval(mpq_ptr q, signed long int l, mpq_srcptr r)
    {
        if (__GMPXX_CONSTANT_TRUE(l == 0))
            mpq_set_ui(q, 0, 1);
        else if (__GMPXX_CONSTANT_TRUE(l == 1))
            mpq_inv(q, r);
        else if (__GMPXX_CONSTANT_TRUE(l == -1))
        {
            mpq_inv(q, r);
            mpq_neg(q, q);
        }
        else
        {
            __GMPXX_TMPQ_SI;
            mpq_div(q, temp, r);
        }
    }
    static void eval(mpq_ptr q, mpq_srcptr r, double d)
    {
        __GMPXX_TMPQ_D;    mpq_div(q, r, temp);
    }
    static void eval(mpq_ptr q, double d, mpq_srcptr r)
    {
        __GMPXX_TMPQ_D;    mpq_div(q, temp, r);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, mpf_srcptr h)
    {
        mpf_div(f, g, h);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, unsigned long int l)
    {
        mpf_div_ui(f, g, l);
    }
    static void eval(mpf_ptr f, unsigned long int l, mpf_srcptr g)
    {
        mpf_ui_div(f, l, g);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, signed long int l)
    {
        if (l >= 0)
            mpf_div_ui(f, g, l);
        else
        {
            mpf_div_ui(f, g, -static_cast<unsigned long>(l));
            mpf_neg(f, f);
        }
    }
    static void eval(mpf_ptr f, signed long int l, mpf_srcptr g)
    {
        if (l >= 0)
            mpf_ui_div(f, l, g);
        else
        {
            mpf_ui_div(f, -static_cast<unsigned long>(l), g);
            mpf_neg(f, f);
        }
    }
    static void eval(mpf_ptr f, mpf_srcptr g, double d)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_div(f, g, temp);
        mpf_clear(temp);
    }
    static void eval(mpf_ptr f, double d, mpf_srcptr g)
    {
        mpf_t temp;
        mpf_init2(temp, 8 * sizeof(double));
        mpf_set_d(temp, d);
        mpf_div(f, temp, g);
        mpf_clear(temp);
    }
};

struct __gmp_binary_modulus
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_tdiv_r(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        mpz_tdiv_r_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        if (mpz_sgn(w) >= 0)
        {
            if (mpz_fits_ulong_p(w))
                mpz_set_ui(z, l % mpz_get_ui(w));
            else
                mpz_set_ui(z, l);
        }
        else
        {
            mpz_neg(z, w);
            if (mpz_fits_ulong_p(z))
                mpz_set_ui(z, l % mpz_get_ui(z));
            else
                mpz_set_ui(z, l);
        }
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        mpz_tdiv_r_ui(z, w, __gmpxx_abs_ui(l));
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        if (mpz_fits_slong_p(w))
            mpz_set_si(z, l % mpz_get_si(w));
        else
        {
            /* if w is bigger than a long then the remainder is l unchanged,
               unless l==LONG_MIN and w==-LONG_MIN in which case it's 0 */
            mpz_set_si(z, mpz_cmpabs_ui(w, __gmpxx_abs_ui(l)) == 0 ? 0 : l);
        }
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_tdiv_r(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        __GMPXX_TMPZ_D;    mpz_tdiv_r(z, temp, w);
    }
};

struct __gmp_binary_and
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_and(z, w, v);
    }

    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        __GMPXX_TMPZ_UI;   mpz_and(z, w, temp);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        __GMPXX_TMPZ_SI;   mpz_and(z, w, temp);
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_and(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }
};

struct __gmp_binary_ior
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_ior(z, w, v);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        __GMPXX_TMPZ_UI;   mpz_ior(z, w, temp);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        __GMPXX_TMPZ_SI;   mpz_ior(z, w, temp);
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_ior(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }
};

struct __gmp_binary_xor
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_xor(z, w, v);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        __GMPXX_TMPZ_UI;   mpz_xor(z, w, temp);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        __GMPXX_TMPZ_SI;   mpz_xor(z, w, temp);
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_xor(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }
};

struct __gmp_cmp_function
{
    static int eval(mpz_srcptr z, mpz_srcptr w) { return mpz_cmp(z, w); }

    static int eval(mpz_srcptr z, unsigned long int l)
    {
        return mpz_cmp_ui(z, l);
    }
    static int eval(unsigned long int l, mpz_srcptr z)
    {
        return -mpz_cmp_ui(z, l);
    }
    static int eval(mpz_srcptr z, signed long int l)
    {
        return mpz_cmp_si(z, l);
    }
    static int eval(signed long int l, mpz_srcptr z)
    {
        return -mpz_cmp_si(z, l);
    }
    static int eval(mpz_srcptr z, double d)
    {
        return mpz_cmp_d(z, d);
    }
    static int eval(double d, mpz_srcptr z)
    {
        return -mpz_cmp_d(z, d);
    }

    static int eval(mpq_srcptr q, mpq_srcptr r) { return mpq_cmp(q, r); }

    static int eval(mpq_srcptr q, unsigned long int l)
    {
        return mpq_cmp_ui(q, l, 1);
    }
    static int eval(unsigned long int l, mpq_srcptr q)
    {
        return -mpq_cmp_ui(q, l, 1);
    }
    static int eval(mpq_srcptr q, signed long int l)
    {
        return mpq_cmp_si(q, l, 1);
    }
    static int eval(signed long int l, mpq_srcptr q)
    {
        return -mpq_cmp_si(q, l, 1);
    }
    static int eval(mpq_srcptr q, double d)
    {
        __GMPXX_TMPQ_D;    return mpq_cmp(q, temp);
    }
    static int eval(double d, mpq_srcptr q)
    {
        __GMPXX_TMPQ_D;    return mpq_cmp(temp, q);
    }
    static int eval(mpq_srcptr q, mpz_srcptr z)
    {
        return mpq_cmp_z(q, z);
    }
    static int eval(mpz_srcptr z, mpq_srcptr q)
    {
        return -mpq_cmp_z(q, z);
    }

    static int eval(mpf_srcptr f, mpf_srcptr g) { return mpf_cmp(f, g); }

    static int eval(mpf_srcptr f, unsigned long int l)
    {
        return mpf_cmp_ui(f, l);
    }
    static int eval(unsigned long int l, mpf_srcptr f)
    {
        return -mpf_cmp_ui(f, l);
    }
    static int eval(mpf_srcptr f, signed long int l)
    {
        return mpf_cmp_si(f, l);
    }
    static int eval(signed long int l, mpf_srcptr f)
    {
        return -mpf_cmp_si(f, l);
    }
    static int eval(mpf_srcptr f, double d)
    {
        return mpf_cmp_d(f, d);
    }
    static int eval(double d, mpf_srcptr f)
    {
        return -mpf_cmp_d(f, d);
    }
    static int eval(mpf_srcptr f, mpz_srcptr z)
    {
        return mpf_cmp_z(f, z);
    }
    static int eval(mpz_srcptr z, mpf_srcptr f)
    {
        return -mpf_cmp_z(f, z);
    }
    static int eval(mpf_srcptr f, mpq_srcptr q)
    {
        mpf_t qf;
        mpf_init(qf); /* Should we use the precision of f?  */
        mpf_set_q(qf, q);
        int ret = eval(f, qf);
        mpf_clear(qf);
        return ret;
    }
    static int eval(mpq_srcptr q, mpf_srcptr f)
    {
        return -eval(f, q);
    }
};

struct __gmp_binary_equal
{
    static bool eval(mpz_srcptr z, mpz_srcptr w) { return mpz_cmp(z, w) == 0; }

    static bool eval(mpz_srcptr z, unsigned long int l)
    {
        return mpz_cmp_ui(z, l) == 0;
    }
    static bool eval(unsigned long int l, mpz_srcptr z)
    {
        return eval(z, l);
    }
    static bool eval(mpz_srcptr z, signed long int l)
    {
        return mpz_cmp_si(z, l) == 0;
    }
    static bool eval(signed long int l, mpz_srcptr z)
    {
        return eval(z, l);
    }
    static bool eval(mpz_srcptr z, double d)
    {
        return mpz_cmp_d(z, d) == 0;
    }
    static bool eval(double d, mpz_srcptr z)
    {
        return eval(z, d);
    }

    static bool eval(mpq_srcptr q, mpq_srcptr r)
    {
        return mpq_equal(q, r) != 0;
    }

    static bool eval(mpq_srcptr q, unsigned long int l)
    {
        return ((__GMPXX_CONSTANT(l) && l == 0) ||
            mpz_cmp_ui(mpq_denref(q), 1) == 0) &&
            mpz_cmp_ui(mpq_numref(q), l) == 0;
    }
    static bool eval(unsigned long int l, mpq_srcptr q)
    {
        return eval(q, l);
    }
    static bool eval(mpq_srcptr q, signed long int l)
    {
        return ((__GMPXX_CONSTANT(l) && l == 0) ||
            mpz_cmp_ui(mpq_denref(q), 1) == 0) &&
            mpz_cmp_si(mpq_numref(q), l) == 0;
    }
    static bool eval(signed long int l, mpq_srcptr q)
    {
        return eval(q, l);
    }
    static bool eval(mpq_srcptr q, double d)
    {
        __GMPXX_TMPQ_D;    return mpq_equal(q, temp) != 0;
    }
    static bool eval(double d, mpq_srcptr q)
    {
        return eval(q, d);
    }
    static bool eval(mpq_srcptr q, mpz_srcptr z)
    {
        return mpz_cmp_ui(mpq_denref(q), 1) == 0 && mpz_cmp(mpq_numref(q), z) == 0;
    }
    static bool eval(mpz_srcptr z, mpq_srcptr q)
    {
        return eval(q, z);
    }

    static bool eval(mpf_srcptr f, mpf_srcptr g) { return mpf_cmp(f, g) == 0; }

    static bool eval(mpf_srcptr f, unsigned long int l)
    {
        return mpf_cmp_ui(f, l) == 0;
    }
    static bool eval(unsigned long int l, mpf_srcptr f)
    {
        return eval(f, l);
    }
    static bool eval(mpf_srcptr f, signed long int l)
    {
        return mpf_cmp_si(f, l) == 0;
    }
    static bool eval(signed long int l, mpf_srcptr f)
    {
        return eval(f, l);
    }
    static bool eval(mpf_srcptr f, double d)
    {
        return mpf_cmp_d(f, d) == 0;
    }
    static bool eval(double d, mpf_srcptr f)
    {
        return eval(f, d);
    }
    static bool eval(mpf_srcptr f, mpz_srcptr z)
    {
        return mpf_cmp_z(f, z) == 0;
    }
    static bool eval(mpz_srcptr z, mpf_srcptr f)
    {
        return eval(f, z);
    }
    static bool eval(mpf_srcptr f, mpq_srcptr q)
    {
        return __gmp_cmp_function::eval(f, q) == 0;
    }
    static bool eval(mpq_srcptr q, mpf_srcptr f)
    {
        return eval(f, q);
    }
};

struct __gmp_binary_less
{
    static bool eval(mpz_srcptr z, mpz_srcptr w) { return mpz_cmp(z, w) < 0; }

    static bool eval(mpz_srcptr z, unsigned long int l)
    {
        return mpz_cmp_ui(z, l) < 0;
    }
    static bool eval(unsigned long int l, mpz_srcptr z)
    {
        return mpz_cmp_ui(z, l) > 0;
    }
    static bool eval(mpz_srcptr z, signed long int l)
    {
        return mpz_cmp_si(z, l) < 0;
    }
    static bool eval(signed long int l, mpz_srcptr z)
    {
        return mpz_cmp_si(z, l) > 0;
    }
    static bool eval(mpz_srcptr z, double d)
    {
        return mpz_cmp_d(z, d) < 0;
    }
    static bool eval(double d, mpz_srcptr z)
    {
        return mpz_cmp_d(z, d) > 0;
    }

    static bool eval(mpq_srcptr q, mpq_srcptr r) { return mpq_cmp(q, r) < 0; }

    static bool eval(mpq_srcptr q, unsigned long int l)
    {
        return mpq_cmp_ui(q, l, 1) < 0;
    }
    static bool eval(unsigned long int l, mpq_srcptr q)
    {
        return mpq_cmp_ui(q, l, 1) > 0;
    }
    static bool eval(mpq_srcptr q, signed long int l)
    {
        return mpq_cmp_si(q, l, 1) < 0;
    }
    static bool eval(signed long int l, mpq_srcptr q)
    {
        return mpq_cmp_si(q, l, 1) > 0;
    }
    static bool eval(mpq_srcptr q, double d)
    {
        __GMPXX_TMPQ_D;    return mpq_cmp(q, temp) < 0;
    }
    static bool eval(double d, mpq_srcptr q)
    {
        __GMPXX_TMPQ_D;    return mpq_cmp(temp, q) < 0;
    }
    static bool eval(mpq_srcptr q, mpz_srcptr z)
    {
        return mpq_cmp_z(q, z) < 0;
    }
    static bool eval(mpz_srcptr z, mpq_srcptr q)
    {
        return mpq_cmp_z(q, z) > 0;
    }

    static bool eval(mpf_srcptr f, mpf_srcptr g) { return mpf_cmp(f, g) < 0; }

    static bool eval(mpf_srcptr f, unsigned long int l)
    {
        return mpf_cmp_ui(f, l) < 0;
    }
    static bool eval(unsigned long int l, mpf_srcptr f)
    {
        return mpf_cmp_ui(f, l) > 0;
    }
    static bool eval(mpf_srcptr f, signed long int l)
    {
        return mpf_cmp_si(f, l) < 0;
    }
    static bool eval(signed long int l, mpf_srcptr f)
    {
        return mpf_cmp_si(f, l) > 0;
    }
    static bool eval(mpf_srcptr f, double d)
    {
        return mpf_cmp_d(f, d) < 0;
    }
    static bool eval(double d, mpf_srcptr f)
    {
        return mpf_cmp_d(f, d) > 0;
    }
    static bool eval(mpf_srcptr f, mpz_srcptr z)
    {
        return mpf_cmp_z(f, z) < 0;
    }
    static bool eval(mpz_srcptr z, mpf_srcptr f)
    {
        return mpf_cmp_z(f, z) > 0;
    }
    static bool eval(mpf_srcptr f, mpq_srcptr q)
    {
        return __gmp_cmp_function::eval(f, q) < 0;
    }
    static bool eval(mpq_srcptr q, mpf_srcptr f)
    {
        return __gmp_cmp_function::eval(q, f) < 0;
    }
};

struct __gmp_binary_greater
{
    template <class T, class U>
    static inline bool eval(T t, U u) { return __gmp_binary_less::eval(u, t); }
};

struct __gmp_unary_increment
{
    static void eval(mpz_ptr z) { mpz_add_ui(z, z, 1); }
    static void eval(mpq_ptr q)
    {
        mpz_add(mpq_numref(q), mpq_numref(q), mpq_denref(q));
    }
    static void eval(mpf_ptr f) { mpf_add_ui(f, f, 1); }
};

struct __gmp_unary_decrement
{
    static void eval(mpz_ptr z) { mpz_sub_ui(z, z, 1); }
    static void eval(mpq_ptr q)
    {
        mpz_sub(mpq_numref(q), mpq_numref(q), mpq_denref(q));
    }
    static void eval(mpf_ptr f) { mpf_sub_ui(f, f, 1); }
};

struct __gmp_abs_function
{
    static void eval(mpz_ptr z, mpz_srcptr w) { mpz_abs(z, w); }
    static void eval(mpq_ptr q, mpq_srcptr r) { mpq_abs(q, r); }
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_abs(f, g); }
};

struct __gmp_trunc_function
{
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_trunc(f, g); }
};

struct __gmp_floor_function
{
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_floor(f, g); }
};

struct __gmp_ceil_function
{
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_ceil(f, g); }
};

struct __gmp_sqrt_function
{
    static void eval(mpz_ptr z, mpz_srcptr w) { mpz_sqrt(z, w); }
    static void eval(mpf_ptr f, mpf_srcptr g) { mpf_sqrt(f, g); }
};

struct __gmp_hypot_function
{
    static void eval(mpf_ptr f, mpf_srcptr g, mpf_srcptr h)
    {
        mpf_t temp;
        mpf_init2(temp, mpf_get_prec(f));
        mpf_mul(temp, g, g);
        mpf_mul(f, h, h);
        mpf_add(f, f, temp);
        mpf_sqrt(f, f);
        mpf_clear(temp);
    }

    static void eval(mpf_ptr f, mpf_srcptr g, unsigned long int l)
    {
        mpf_t temp;
        mpf_init2(temp, mpf_get_prec(f));
        mpf_mul(temp, g, g);
        mpf_set_ui(f, l);
        mpf_mul_ui(f, f, l);
        mpf_add(f, f, temp);
        mpf_clear(temp);
        mpf_sqrt(f, f);
    }
    static void eval(mpf_ptr f, unsigned long int l, mpf_srcptr g)
    {
        eval(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, signed long int l)
    {
        eval(f, g, __gmpxx_abs_ui(l));
    }
    static void eval(mpf_ptr f, signed long int l, mpf_srcptr g)
    {
        eval(f, g, l);
    }
    static void eval(mpf_ptr f, mpf_srcptr g, double d)
    {
        mpf_t temp;
        mpf_init2(temp, mpf_get_prec(f));
        mpf_mul(temp, g, g);
        mpf_set_d(f, d);
        mpf_mul(f, f, f);
        mpf_add(f, f, temp);
        mpf_sqrt(f, f);
        mpf_clear(temp);
    }
    static void eval(mpf_ptr f, double d, mpf_srcptr g)
    {
        eval(f, g, d);
    }
};

struct __gmp_sgn_function
{
    static int eval(mpz_srcptr z) { return mpz_sgn(z); }
    static int eval(mpq_srcptr q) { return mpq_sgn(q); }
    static int eval(mpf_srcptr f) { return mpf_sgn(f); }
};

struct __gmp_gcd_function
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_gcd(z, w, v);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        mpz_gcd_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        eval(z, w, __gmpxx_abs_ui(l));
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_gcd(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }
};

struct __gmp_lcm_function
{
    static void eval(mpz_ptr z, mpz_srcptr w, mpz_srcptr v)
    {
        mpz_lcm(z, w, v);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, unsigned long int l)
    {
        mpz_lcm_ui(z, w, l);
    }
    static void eval(mpz_ptr z, unsigned long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, signed long int l)
    {
        eval(z, w, __gmpxx_abs_ui(l));
    }
    static void eval(mpz_ptr z, signed long int l, mpz_srcptr w)
    {
        eval(z, w, l);
    }
    static void eval(mpz_ptr z, mpz_srcptr w, double d)
    {
        __GMPXX_TMPZ_D;    mpz_lcm(z, w, temp);
    }
    static void eval(mpz_ptr z, double d, mpz_srcptr w)
    {
        eval(z, w, d);
    }
};

struct __gmp_rand_function
{
    static void eval(mpz_ptr z, gmp_randstate_t s, mp_bitcnt_t l)
    {
        mpz_urandomb(z, s, l);
    }
    static void eval(mpz_ptr z, gmp_randstate_t s, mpz_srcptr w)
    {
        mpz_urandomm(z, s, w);
    }
    static void eval(mpf_ptr f, gmp_randstate_t s, mp_bitcnt_t prec)
    {
        mpf_urandomb(f, s, prec);
    }
};

struct __gmp_fac_function
{
    static void eval(mpz_ptr z, unsigned long l) { mpz_fac_ui(z, l); }
    static void eval(mpz_ptr z, signed long l)
    {
        if (l < 0)
            throw std::domain_error("factorial(negative)");
        eval(z, static_cast<unsigned long>(l));
    }
    static void eval(mpz_ptr z, mpz_srcptr w)
    {
        if (!mpz_fits_ulong_p(w))
        {
            if (mpz_sgn(w) < 0)
                throw std::domain_error("factorial(negative)");
            else
                throw std::bad_alloc(); // or std::overflow_error ("factorial")?
        }
        eval(z, mpz_get_ui(w));
    }
    static void eval(mpz_ptr z, double d)
    {
        __GMPXX_TMPZ_D;    eval(z, temp);
    }
};

struct __gmp_primorial_function
{
    static void eval(mpz_ptr z, unsigned long l) { mpz_primorial_ui(z, l); }
    static void eval(mpz_ptr z, signed long l)
    {
        if (l < 0)
            throw std::domain_error("primorial(negative)");
        eval(z, static_cast<unsigned long>(l));
    }
    static void eval(mpz_ptr z, mpz_srcptr w)
    {
        if (!mpz_fits_ulong_p(w))
        {
            if (mpz_sgn(w) < 0)
                throw std::domain_error("primorial(negative)");
            else
                throw std::bad_alloc(); // or std::overflow_error ("primorial")?
        }
        eval(z, mpz_get_ui(w));
    }
    static void eval(mpz_ptr z, double d)
    {
        __GMPXX_TMPZ_D;    eval(z, temp);
    }
};

struct __gmp_fib_function
{
    static void eval(mpz_ptr z, unsigned long l) { mpz_fib_ui(z, l); }
    static void eval(mpz_ptr z, signed long l)
    {
        if (l < 0)
        {
            eval(z, -static_cast<unsigned long>(l));
            if ((l & 1) == 0)
                mpz_neg(z, z);
        }
        else
            eval(z, static_cast<unsigned long>(l));
    }
    static void eval(mpz_ptr z, mpz_srcptr w)
    {
        if (!mpz_fits_slong_p(w))
            throw std::bad_alloc(); // or std::overflow_error ("fibonacci")?
        eval(z, mpz_get_si(w));
    }
    static void eval(mpz_ptr z, double d)
    {
        __GMPXX_TMPZ_D;    eval(z, temp);
    }
};


/**************** Auxiliary classes ****************/

/* this is much the same as gmp_allocated_string in gmp-impl.h
   since gmp-impl.h is not publicly available, I redefine it here
   I use a different name to avoid possible clashes */

extern "C" {
    typedef void (*__gmp_freefunc_t) (void*, size_t);
}
struct __gmp_alloc_cstring
{
    char* str;
    __gmp_alloc_cstring(char* s) { str = s; }
    ~__gmp_alloc_cstring()
    {
        __gmp_freefunc_t freefunc;
        mp_get_memory_functions(NULL, NULL, &freefunc);
        (*freefunc) (str, std::strlen(str) + 1);
    }
};


// general expression template class
template <class T, class U>
class __gmp_expr;


// templates for resolving expression types
template <class T>
struct __gmp_resolve_ref
{
    typedef T ref_type;
};

template <class T, class U>
struct __gmp_resolve_ref<__gmp_expr<T, U> >
{
    typedef const __gmp_expr<T, U>& ref_type;
};


template <class T, class U = T>
struct __gmp_resolve_expr;

template <>
struct __gmp_resolve_expr<mpz_t>
{
    typedef mpz_t value_type;
    typedef mpz_ptr ptr_type;
    typedef mpz_srcptr srcptr_type;
};

template <>
struct __gmp_resolve_expr<mpq_t>
{
    typedef mpq_t value_type;
    typedef mpq_ptr ptr_type;
    typedef mpq_srcptr srcptr_type;
};

template <>
struct __gmp_resolve_expr<mpf_t>
{
    typedef mpf_t value_type;
    typedef mpf_ptr ptr_type;
    typedef mpf_srcptr srcptr_type;
};

template <>
struct __gmp_resolve_expr<mpz_t, mpq_t>
{
    typedef mpq_t value_type;
};

template <>
struct __gmp_resolve_expr<mpq_t, mpz_t>
{
    typedef mpq_t value_type;
};

template <>
struct __gmp_resolve_expr<mpz_t, mpf_t>
{
    typedef mpf_t value_type;
};

template <>
struct __gmp_resolve_expr<mpf_t, mpz_t>
{
    typedef mpf_t value_type;
};

template <>
struct __gmp_resolve_expr<mpq_t, mpf_t>
{
    typedef mpf_t value_type;
};

template <>
struct __gmp_resolve_expr<mpf_t, mpq_t>
{
    typedef mpf_t value_type;
};

#if __GMPXX_USE_CXX11
namespace std {
    template <class T, class U, class V, class W>
    struct common_type <__gmp_expr<T, U>, __gmp_expr<V, W> >
    {
    private:
        typedef typename __gmp_resolve_expr<T, V>::value_type X;
    public:
        typedef __gmp_expr<X, X> type;
    };

    template <class T, class U>
    struct common_type <__gmp_expr<T, U> >
    {
        typedef __gmp_expr<T, T> type;
    };

#define __GMPXX_DECLARE_COMMON_TYPE(typ)	\
  template <class T, class U>			\
  struct common_type <__gmp_expr<T, U>, typ >	\
  {						\
    typedef __gmp_expr<T, T> type;		\
  };						\
						\
  template <class T, class U>			\
  struct common_type <typ, __gmp_expr<T, U> >	\
  {						\
    typedef __gmp_expr<T, T> type;		\
  }

    __GMPXX_DECLARE_COMMON_TYPE(signed char);
    __GMPXX_DECLARE_COMMON_TYPE(unsigned char);
    __GMPXX_DECLARE_COMMON_TYPE(signed int);
    __GMPXX_DECLARE_COMMON_TYPE(unsigned int);
    __GMPXX_DECLARE_COMMON_TYPE(signed short int);
    __GMPXX_DECLARE_COMMON_TYPE(unsigned short int);
    __GMPXX_DECLARE_COMMON_TYPE(signed long int);
    __GMPXX_DECLARE_COMMON_TYPE(unsigned long int);
    __GMPXX_DECLARE_COMMON_TYPE(float);
    __GMPXX_DECLARE_COMMON_TYPE(double);
#undef __GMPXX_DECLARE_COMMON_TYPE
}
#endif

// classes for evaluating unary and binary expressions
template <class T, class Op>
struct __gmp_unary_expr
{
    typename __gmp_resolve_ref<T>::ref_type val;

    __gmp_unary_expr(const T& v) : val(v) { }
private:
    __gmp_unary_expr();
};

template <class T, class U, class Op>
struct __gmp_binary_expr
{
    typename __gmp_resolve_ref<T>::ref_type val1;
    typename __gmp_resolve_ref<U>::ref_type val2;

    __gmp_binary_expr(const T& v1, const U& v2) : val1(v1), val2(v2) { }
private:
    __gmp_binary_expr();
};



/**************** Macros for in-class declarations ****************/
/* This is just repetitive code that is easier to maintain if it's written
   only once */

#define __GMPP_DECLARE_COMPOUND_OPERATOR(fun)                         \
  template <class T, class U>                                         \
  __gmp_expr<value_type, value_type> & fun(const __gmp_expr<T, U> &);

#define __GMPN_DECLARE_COMPOUND_OPERATOR(fun) \
  __gmp_expr & fun(signed char);              \
  __gmp_expr & fun(unsigned char);            \
  __gmp_expr & fun(signed int);               \
  __gmp_expr & fun(unsigned int);             \
  __gmp_expr & fun(signed short int);         \
  __gmp_expr & fun(unsigned short int);       \
  __gmp_expr & fun(signed long int);          \
  __gmp_expr & fun(unsigned long int);        \
  __gmp_expr & fun(float);                    \
  __gmp_expr & fun(double);                   \
  /* __gmp_expr & fun(long double); */

#define __GMP_DECLARE_COMPOUND_OPERATOR(fun) \
__GMPP_DECLARE_COMPOUND_OPERATOR(fun)        \
__GMPN_DECLARE_COMPOUND_OPERATOR(fun)

#define __GMP_DECLARE_COMPOUND_OPERATOR_UI(fun) \
  __gmp_expr & fun(mp_bitcnt_t);

#define __GMP_DECLARE_INCREMENT_OPERATOR(fun) \
  inline __gmp_expr & fun();                  \
  inline __gmp_expr fun(int);

#define __GMPXX_DEFINE_ARITHMETIC_CONSTRUCTORS		\
  __gmp_expr(signed char c) { init_si(c); }		\
  __gmp_expr(unsigned char c) { init_ui(c); }		\
  __gmp_expr(signed int i) { init_si(i); }		\
  __gmp_expr(unsigned int i) { init_ui(i); }		\
  __gmp_expr(signed short int s) { init_si(s); }	\
  __gmp_expr(unsigned short int s) { init_ui(s); }	\
  __gmp_expr(signed long int l) { init_si(l); }		\
  __gmp_expr(unsigned long int l) { init_ui(l); }	\
  __gmp_expr(float f) { init_d(f); }			\
  __gmp_expr(double d) { init_d(d); }

#define __GMPXX_DEFINE_ARITHMETIC_ASSIGNMENTS		\
  __gmp_expr & operator=(signed char c) { assign_si(c); return *this; } \
  __gmp_expr & operator=(unsigned char c) { assign_ui(c); return *this; } \
  __gmp_expr & operator=(signed int i) { assign_si(i); return *this; } \
  __gmp_expr & operator=(unsigned int i) { assign_ui(i); return *this; } \
  __gmp_expr & operator=(signed short int s) { assign_si(s); return *this; } \
  __gmp_expr & operator=(unsigned short int s) { assign_ui(s); return *this; } \
  __gmp_expr & operator=(signed long int l) { assign_si(l); return *this; } \
  __gmp_expr & operator=(unsigned long int l) { assign_ui(l); return *this; } \
  __gmp_expr & operator=(float f) { assign_d(f); return *this; } \
  __gmp_expr & operator=(double d) { assign_d(d); return *this; }

#define __GMPP_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                 \
template <class U>                                                           \
static __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >          \
fun(const __gmp_expr<T, U> &expr);

#define __GMPNN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, bigtype) \
static inline __gmp_expr<T, __gmp_unary_expr<bigtype, eval_fun> >            \
fun(type expr);

#define __GMPNS_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, signed long)
#define __GMPNU_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, unsigned long)
#define __GMPND_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, double)

#define __GMPN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                 \
__GMPNS_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed char)           \
__GMPNU_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned char)         \
__GMPNS_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed int)            \
__GMPNU_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned int)          \
__GMPNS_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed short int)      \
__GMPNU_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned short int)    \
__GMPNS_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed long int)       \
__GMPNU_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned long int)     \
__GMPND_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, float)                 \
__GMPND_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, double)

#define __GMP_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                  \
__GMPP_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                         \
__GMPN_DECLARE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)

   /**************** mpz_class -- wrapper for mpz_t ****************/

template <>
class __gmp_expr<mpz_t, mpz_t>
{
private:
    typedef mpz_t value_type;
    value_type mp;

    // Helper functions used for all arithmetic types
    void assign_ui(unsigned long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l == 0))
            mp->_mp_size = 0;
        else
            mpz_set_ui(mp, l);
    }
    void assign_si(signed long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            assign_ui(l);
        else if (__GMPXX_CONSTANT_TRUE(l <= 0))
        {
            assign_ui(-static_cast<unsigned long>(l));
            mpz_neg(mp, mp);
        }
        else
            mpz_set_si(mp, l);
    }
    void assign_d(double d)
    {
        mpz_set_d(mp, d);
    }

    void init_ui(unsigned long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l == 0))
            mpz_init(mp);
        else
            mpz_init_set_ui(mp, l);
    }
    void init_si(signed long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            init_ui(l);
        else if (__GMPXX_CONSTANT_TRUE(l <= 0))
        {
            init_ui(-static_cast<unsigned long>(l));
            mpz_neg(mp, mp);
        }
        else
            mpz_init_set_si(mp, l);
    }
    void init_d(double d)
    {
        mpz_init_set_d(mp, d);
    }

public:
    mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }

    // constructors and destructor
    __gmp_expr() __GMPXX_NOEXCEPT { mpz_init(mp); }

    __gmp_expr(const __gmp_expr& z) { mpz_init_set(mp, z.mp); }
#if __GMPXX_USE_CXX11
    __gmp_expr(__gmp_expr&& z) noexcept
    {
        *mp = *z.mp; mpz_init(z.mp);
    }
#endif
    template <class T>
    __gmp_expr(const __gmp_expr<mpz_t, T>& expr)
    {
        mpz_init(mp); __gmp_set_expr(mp, expr);
    }
    template <class T, class U>
    explicit __gmp_expr(const __gmp_expr<T, U>& expr)
    {
        mpz_init(mp); __gmp_set_expr(mp, expr);
    }

    __GMPXX_DEFINE_ARITHMETIC_CONSTRUCTORS

        explicit __gmp_expr(const char* s, int base = 0)
    {
        if (mpz_init_set_str(mp, s, base) != 0)
        {
            mpz_clear(mp);
            throw std::invalid_argument("mpz_set_str");
        }
    }
    explicit __gmp_expr(const std::string& s, int base = 0)
    {
        if (mpz_init_set_str(mp, s.c_str(), base) != 0)
        {
            mpz_clear(mp);
            throw std::invalid_argument("mpz_set_str");
        }
    }

    explicit __gmp_expr(mpz_srcptr z) { mpz_init_set(mp, z); }

    ~__gmp_expr() { mpz_clear(mp); }

    void swap(__gmp_expr& z) __GMPXX_NOEXCEPT { std::swap(*mp, *z.mp); }

    // assignment operators
    __gmp_expr& operator=(const __gmp_expr& z)
    {
        mpz_set(mp, z.mp); return *this;
    }
#if __GMPXX_USE_CXX11
    __gmp_expr& operator=(__gmp_expr&& z) noexcept
    {
        swap(z); return *this;
    }
#endif
    template <class T, class U>
    __gmp_expr<value_type, value_type>& operator=(const __gmp_expr<T, U>& expr)
    {
        __gmp_set_expr(mp, expr); return *this;
    }

    __GMPXX_DEFINE_ARITHMETIC_ASSIGNMENTS

        __gmp_expr& operator=(const char* s)
    {
        if (mpz_set_str(mp, s, 0) != 0)
            throw std::invalid_argument("mpz_set_str");
        return *this;
    }
    __gmp_expr& operator=(const std::string& s)
    {
        if (mpz_set_str(mp, s.c_str(), 0) != 0)
            throw std::invalid_argument("mpz_set_str");
        return *this;
    }

    // string input/output functions
    int set_str(const char* s, int base)
    {
        return mpz_set_str(mp, s, base);
    }
    int set_str(const std::string& s, int base)
    {
        return mpz_set_str(mp, s.c_str(), base);
    }
    std::string get_str(int base = 10) const
    {
        __gmp_alloc_cstring temp(mpz_get_str(0, base, mp));
        return std::string(temp.str);
    }

    // conversion functions
    mpz_srcptr __get_mp() const { return mp; }
    mpz_ptr __get_mp() { return mp; }
    mpz_srcptr get_mpz_t() const { return mp; }
    mpz_ptr get_mpz_t() { return mp; }

    signed long int get_si() const { return mpz_get_si(mp); }
    unsigned long int get_ui() const { return mpz_get_ui(mp); }
    double get_d() const { return mpz_get_d(mp); }

    // bool fits_schar_p() const { return mpz_fits_schar_p(mp); }
    // bool fits_uchar_p() const { return mpz_fits_uchar_p(mp); }
    bool fits_sint_p() const { return mpz_fits_sint_p(mp); }
    bool fits_uint_p() const { return mpz_fits_uint_p(mp); }
    bool fits_sshort_p() const { return mpz_fits_sshort_p(mp); }
    bool fits_ushort_p() const { return mpz_fits_ushort_p(mp); }
    bool fits_slong_p() const { return mpz_fits_slong_p(mp); }
    bool fits_ulong_p() const { return mpz_fits_ulong_p(mp); }
    // bool fits_float_p() const { return mpz_fits_float_p(mp); }
    // bool fits_double_p() const { return mpz_fits_double_p(mp); }
    // bool fits_ldouble_p() const { return mpz_fits_ldouble_p(mp); }

#if __GMPXX_USE_CXX11
    explicit operator bool() const { return mp->_mp_size != 0; }
#endif

    // member operators
    __GMP_DECLARE_COMPOUND_OPERATOR(operator+=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator-=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator*=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator/=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator%=)

        __GMP_DECLARE_COMPOUND_OPERATOR(operator&=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator|=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator^=)

        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator<<=)
        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator>>=)

        __GMP_DECLARE_INCREMENT_OPERATOR(operator++)
        __GMP_DECLARE_INCREMENT_OPERATOR(operator--)

        __GMP_DECLARE_UNARY_STATIC_MEMFUN(mpz_t, factorial, __gmp_fac_function)
        __GMP_DECLARE_UNARY_STATIC_MEMFUN(mpz_t, primorial, __gmp_primorial_function)
        __GMP_DECLARE_UNARY_STATIC_MEMFUN(mpz_t, fibonacci, __gmp_fib_function)
};

typedef __gmp_expr<mpz_t, mpz_t> mpz_class;


/**************** mpq_class -- wrapper for mpq_t ****************/

template <>
class __gmp_expr<mpq_t, mpq_t>
{
private:
    typedef mpq_t value_type;
    value_type mp;

    // Helper functions used for all arithmetic types
    void assign_ui(unsigned long l) { mpq_set_ui(mp, l, 1); }
    void assign_si(signed long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            assign_ui(l);
        else
            mpq_set_si(mp, l, 1);
    }
    void assign_d(double d) { mpq_set_d(mp, d); }

    void init_ui(unsigned long l) { mpq_init(mp); get_num() = l; }
    void init_si(signed long l) { mpq_init(mp); get_num() = l; }
    void init_d(double d) { mpq_init(mp); assign_d(d); }

public:
    mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }
    void canonicalize() { mpq_canonicalize(mp); }

    // constructors and destructor
    __gmp_expr() { mpq_init(mp); }

    __gmp_expr(const __gmp_expr& q)
    {
        mpz_init_set(mpq_numref(mp), mpq_numref(q.mp));
        mpz_init_set(mpq_denref(mp), mpq_denref(q.mp));
    }
#if __GMPXX_USE_CXX11
    __gmp_expr(__gmp_expr&& q)
    {
        *mp = *q.mp; mpq_init(q.mp);
    }
#endif
    template <class T>
    __gmp_expr(const __gmp_expr<mpz_t, T>& expr)
    {
        mpq_init(mp); __gmp_set_expr(mp, expr);
    }
    template <class T>
    __gmp_expr(const __gmp_expr<mpq_t, T>& expr)
    {
        mpq_init(mp); __gmp_set_expr(mp, expr);
    }
    template <class T, class U>
    explicit __gmp_expr(const __gmp_expr<T, U>& expr)
    {
        mpq_init(mp); __gmp_set_expr(mp, expr);
    }

    __GMPXX_DEFINE_ARITHMETIC_CONSTRUCTORS

        explicit __gmp_expr(const char* s, int base = 0)
    {
        mpq_init(mp);
        // If s is the literal 0, we meant to call another constructor.
        // If s just happens to evaluate to 0, we would crash, so whatever.
        if (s == 0)
        {
            // Don't turn mpq_class(0,0) into 0
            mpz_set_si(mpq_denref(mp), base);
        }
        else if (mpq_set_str(mp, s, base) != 0)
        {
            mpq_clear(mp);
            throw std::invalid_argument("mpq_set_str");
        }
    }
    explicit __gmp_expr(const std::string& s, int base = 0)
    {
        mpq_init(mp);
        if (mpq_set_str(mp, s.c_str(), base) != 0)
        {
            mpq_clear(mp);
            throw std::invalid_argument("mpq_set_str");
        }
    }
    explicit __gmp_expr(mpq_srcptr q)
    {
        mpz_init_set(mpq_numref(mp), mpq_numref(q));
        mpz_init_set(mpq_denref(mp), mpq_denref(q));
    }

    __gmp_expr(const mpz_class& num, const mpz_class& den)
    {
        mpz_init_set(mpq_numref(mp), num.get_mpz_t());
        mpz_init_set(mpq_denref(mp), den.get_mpz_t());
    }

    ~__gmp_expr() { mpq_clear(mp); }

    void swap(__gmp_expr& q) __GMPXX_NOEXCEPT { std::swap(*mp, *q.mp); }

    // assignment operators
    __gmp_expr& operator=(const __gmp_expr& q)
    {
        mpq_set(mp, q.mp); return *this;
    }
#if __GMPXX_USE_CXX11
    __gmp_expr& operator=(__gmp_expr&& q) noexcept
    {
        swap(q); return *this;
    }
    __gmp_expr& operator=(mpz_class&& z) noexcept
    {
        get_num() = std::move(z); get_den() = 1u; return *this;
    }
#endif
    template <class T, class U>
    __gmp_expr<value_type, value_type>& operator=(const __gmp_expr<T, U>& expr)
    {
        __gmp_set_expr(mp, expr); return *this;
    }

    __GMPXX_DEFINE_ARITHMETIC_ASSIGNMENTS

        __gmp_expr& operator=(const char* s)
    {
        if (mpq_set_str(mp, s, 0) != 0)
            throw std::invalid_argument("mpq_set_str");
        return *this;
    }
    __gmp_expr& operator=(const std::string& s)
    {
        if (mpq_set_str(mp, s.c_str(), 0) != 0)
            throw std::invalid_argument("mpq_set_str");
        return *this;
    }

    // string input/output functions
    int set_str(const char* s, int base)
    {
        return mpq_set_str(mp, s, base);
    }
    int set_str(const std::string& s, int base)
    {
        return mpq_set_str(mp, s.c_str(), base);
    }
    std::string get_str(int base = 10) const
    {
        __gmp_alloc_cstring temp(mpq_get_str(0, base, mp));
        return std::string(temp.str);
    }

    // conversion functions

    // casting a reference to an mpz_t to mpz_class & is a dirty hack,
    // but works because the internal representation of mpz_class is
    // exactly an mpz_t
    const mpz_class& get_num() const
    {
        return reinterpret_cast<const mpz_class&>(*mpq_numref(mp));
    }
    mpz_class& get_num()
    {
        return reinterpret_cast<mpz_class&>(*mpq_numref(mp));
    }
    const mpz_class& get_den() const
    {
        return reinterpret_cast<const mpz_class&>(*mpq_denref(mp));
    }
    mpz_class& get_den()
    {
        return reinterpret_cast<mpz_class&>(*mpq_denref(mp));
    }

    mpq_srcptr __get_mp() const { return mp; }
    mpq_ptr __get_mp() { return mp; }
    mpq_srcptr get_mpq_t() const { return mp; }
    mpq_ptr get_mpq_t() { return mp; }

    mpz_srcptr get_num_mpz_t() const { return mpq_numref(mp); }
    mpz_ptr get_num_mpz_t() { return mpq_numref(mp); }
    mpz_srcptr get_den_mpz_t() const { return mpq_denref(mp); }
    mpz_ptr get_den_mpz_t() { return mpq_denref(mp); }

    double get_d() const { return mpq_get_d(mp); }

#if __GMPXX_USE_CXX11
    explicit operator bool() const { return mpq_numref(mp)->_mp_size != 0; }
#endif

    // compound assignments
    __GMP_DECLARE_COMPOUND_OPERATOR(operator+=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator-=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator*=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator/=)

        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator<<=)
        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator>>=)

        __GMP_DECLARE_INCREMENT_OPERATOR(operator++)
        __GMP_DECLARE_INCREMENT_OPERATOR(operator--)
};

typedef __gmp_expr<mpq_t, mpq_t> mpq_class;


/**************** mpf_class -- wrapper for mpf_t ****************/

template <>
class __gmp_expr<mpf_t, mpf_t>
{
private:
    typedef mpf_t value_type;
    value_type mp;

    // Helper functions used for all arithmetic types
    void assign_ui(unsigned long l) { mpf_set_ui(mp, l); }
    void assign_si(signed long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            assign_ui(l);
        else
            mpf_set_si(mp, l);
    }
    void assign_d(double d) { mpf_set_d(mp, d); }

    void init_ui(unsigned long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l == 0))
            mpf_init(mp);
        else
            mpf_init_set_ui(mp, l);
    }
    void init_si(signed long l)
    {
        if (__GMPXX_CONSTANT_TRUE(l >= 0))
            init_ui(l);
        else
            mpf_init_set_si(mp, l);
    }
    void init_d(double d) { mpf_init_set_d(mp, d); }

public:
    mp_bitcnt_t get_prec() const { return mpf_get_prec(mp); }

    void set_prec(mp_bitcnt_t prec) { mpf_set_prec(mp, prec); }
    void set_prec_raw(mp_bitcnt_t prec) { mpf_set_prec_raw(mp, prec); }

    // constructors and destructor
    __gmp_expr() { mpf_init(mp); }

    __gmp_expr(const __gmp_expr& f)
    {
        mpf_init2(mp, f.get_prec()); mpf_set(mp, f.mp);
    }
#if __GMPXX_USE_CXX11
    __gmp_expr(__gmp_expr&& f)
    {
        *mp = *f.mp; mpf_init2(f.mp, get_prec());
    }
#endif
    __gmp_expr(const __gmp_expr& f, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set(mp, f.mp);
    }
    template <class T, class U>
    __gmp_expr(const __gmp_expr<T, U>& expr)
    {
        mpf_init2(mp, expr.get_prec()); __gmp_set_expr(mp, expr);
    }
    template <class T, class U>
    __gmp_expr(const __gmp_expr<T, U>& expr, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); __gmp_set_expr(mp, expr);
    }

    __GMPXX_DEFINE_ARITHMETIC_CONSTRUCTORS

        __gmp_expr(signed char c, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_si(mp, c);
    }
    __gmp_expr(unsigned char c, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_ui(mp, c);
    }

    __gmp_expr(signed int i, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_si(mp, i);
    }
    __gmp_expr(unsigned int i, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_ui(mp, i);
    }

    __gmp_expr(signed short int s, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_si(mp, s);
    }
    __gmp_expr(unsigned short int s, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_ui(mp, s);
    }

    __gmp_expr(signed long int l, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_si(mp, l);
    }
    __gmp_expr(unsigned long int l, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_ui(mp, l);
    }

    __gmp_expr(float f, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_d(mp, f);
    }
    __gmp_expr(double d, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set_d(mp, d);
    }
    // __gmp_expr(long double ld) { mpf_init_set_d(mp, ld); }
    // __gmp_expr(long double ld, mp_bitcnt_t prec)
    // { mpf_init2(mp, prec); mpf_set_d(mp, ld); }

    explicit __gmp_expr(const char* s)
    {
        if (mpf_init_set_str(mp, s, 0) != 0)
        {
            mpf_clear(mp);
            throw std::invalid_argument("mpf_set_str");
        }
    }
    __gmp_expr(const char* s, mp_bitcnt_t prec, int base = 0)
    {
        mpf_init2(mp, prec);
        if (mpf_set_str(mp, s, base) != 0)
        {
            mpf_clear(mp);
            throw std::invalid_argument("mpf_set_str");
        }
    }
    explicit __gmp_expr(const std::string& s)
    {
        if (mpf_init_set_str(mp, s.c_str(), 0) != 0)
        {
            mpf_clear(mp);
            throw std::invalid_argument("mpf_set_str");
        }
    }
    __gmp_expr(const std::string& s, mp_bitcnt_t prec, int base = 0)
    {
        mpf_init2(mp, prec);
        if (mpf_set_str(mp, s.c_str(), base) != 0)
        {
            mpf_clear(mp);
            throw std::invalid_argument("mpf_set_str");
        }
    }

    explicit __gmp_expr(mpf_srcptr f)
    {
        mpf_init2(mp, mpf_get_prec(f)); mpf_set(mp, f);
    }
    __gmp_expr(mpf_srcptr f, mp_bitcnt_t prec)
    {
        mpf_init2(mp, prec); mpf_set(mp, f);
    }

    ~__gmp_expr() { mpf_clear(mp); }

    void swap(__gmp_expr& f) __GMPXX_NOEXCEPT { std::swap(*mp, *f.mp); }

    // assignment operators
    __gmp_expr& operator=(const __gmp_expr& f)
    {
        mpf_set(mp, f.mp); return *this;
    }
#if __GMPXX_USE_CXX11
    __gmp_expr& operator=(__gmp_expr&& f) noexcept
    {
        swap(f); return *this;
    }
#endif
    template <class T, class U>
    __gmp_expr<value_type, value_type>& operator=(const __gmp_expr<T, U>& expr)
    {
        __gmp_set_expr(mp, expr); return *this;
    }

    __GMPXX_DEFINE_ARITHMETIC_ASSIGNMENTS

        __gmp_expr& operator=(const char* s)
    {
        if (mpf_set_str(mp, s, 0) != 0)
            throw std::invalid_argument("mpf_set_str");
        return *this;
    }
    __gmp_expr& operator=(const std::string& s)
    {
        if (mpf_set_str(mp, s.c_str(), 0) != 0)
            throw std::invalid_argument("mpf_set_str");
        return *this;
    }

    // string input/output functions
    int set_str(const char* s, int base)
    {
        return mpf_set_str(mp, s, base);
    }
    int set_str(const std::string& s, int base)
    {
        return mpf_set_str(mp, s.c_str(), base);
    }
    std::string get_str(mp_exp_t& expo, int base = 10, size_t size = 0) const
    {
        __gmp_alloc_cstring temp(mpf_get_str(0, &expo, base, size, mp));
        return std::string(temp.str);
    }

    // conversion functions
    mpf_srcptr __get_mp() const { return mp; }
    mpf_ptr __get_mp() { return mp; }
    mpf_srcptr get_mpf_t() const { return mp; }
    mpf_ptr get_mpf_t() { return mp; }

    signed long int get_si() const { return mpf_get_si(mp); }
    unsigned long int get_ui() const { return mpf_get_ui(mp); }
    double get_d() const { return mpf_get_d(mp); }

    // bool fits_schar_p() const { return mpf_fits_schar_p(mp); }
    // bool fits_uchar_p() const { return mpf_fits_uchar_p(mp); }
    bool fits_sint_p() const { return mpf_fits_sint_p(mp); }
    bool fits_uint_p() const { return mpf_fits_uint_p(mp); }
    bool fits_sshort_p() const { return mpf_fits_sshort_p(mp); }
    bool fits_ushort_p() const { return mpf_fits_ushort_p(mp); }
    bool fits_slong_p() const { return mpf_fits_slong_p(mp); }
    bool fits_ulong_p() const { return mpf_fits_ulong_p(mp); }
    // bool fits_float_p() const { return mpf_fits_float_p(mp); }
    // bool fits_double_p() const { return mpf_fits_double_p(mp); }
    // bool fits_ldouble_p() const { return mpf_fits_ldouble_p(mp); }

#if __GMPXX_USE_CXX11
    explicit operator bool() const { return mpf_sgn(mp) != 0; }
#endif

    // compound assignments
    __GMP_DECLARE_COMPOUND_OPERATOR(operator+=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator-=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator*=)
        __GMP_DECLARE_COMPOUND_OPERATOR(operator/=)

        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator<<=)
        __GMP_DECLARE_COMPOUND_OPERATOR_UI(operator>>=)

        __GMP_DECLARE_INCREMENT_OPERATOR(operator++)
        __GMP_DECLARE_INCREMENT_OPERATOR(operator--)
};

typedef __gmp_expr<mpf_t, mpf_t> mpf_class;



/**************** User-defined literals ****************/

#if __GMPXX_USE_CXX11
inline mpz_class operator"" _mpz(const char* s)
{
    return mpz_class(s);
}

inline mpq_class operator"" _mpq(const char* s)
{
    mpq_class q;
    q.get_num() = s;
    return q;
}

inline mpf_class operator"" _mpf(const char* s)
{
    return mpf_class(s);
}
#endif

/**************** I/O operators ****************/

// these should (and will) be provided separately

template <class T, class U>
inline std::ostream& operator<<
(std::ostream& o, const __gmp_expr<T, U>& expr)
{
    __gmp_expr<T, T> const& temp(expr);
    return o << temp.__get_mp();
}

template <class T>
inline std::istream& operator>>(std::istream& i, __gmp_expr<T, T>& expr)
{
    return i >> expr.__get_mp();
}

/*
// you might want to uncomment this
inline std::istream & operator>>(std::istream &i, mpq_class &q)
{
  i >> q.get_mpq_t();
  q.canonicalize();
  return i;
}
*/


/**************** Functions for type conversion ****************/

inline void __gmp_set_expr(mpz_ptr z, const mpz_class& w)
{
    mpz_set(z, w.get_mpz_t());
}

template <class T>
inline void __gmp_set_expr(mpz_ptr z, const __gmp_expr<mpz_t, T>& expr)
{
    expr.eval(z);
}

template <class T>
inline void __gmp_set_expr(mpz_ptr z, const __gmp_expr<mpq_t, T>& expr)
{
    mpq_class const& temp(expr);
    mpz_set_q(z, temp.get_mpq_t());
}

template <class T>
inline void __gmp_set_expr(mpz_ptr z, const __gmp_expr<mpf_t, T>& expr)
{
    mpf_class const& temp(expr);
    mpz_set_f(z, temp.get_mpf_t());
}

inline void __gmp_set_expr(mpq_ptr q, const mpz_class& z)
{
    mpq_set_z(q, z.get_mpz_t());
}

template <class T>
inline void __gmp_set_expr(mpq_ptr q, const __gmp_expr<mpz_t, T>& expr)
{
    __gmp_set_expr(mpq_numref(q), expr);
    mpz_set_ui(mpq_denref(q), 1);
}

inline void __gmp_set_expr(mpq_ptr q, const mpq_class& r)
{
    mpq_set(q, r.get_mpq_t());
}

template <class T>
inline void __gmp_set_expr(mpq_ptr q, const __gmp_expr<mpq_t, T>& expr)
{
    expr.eval(q);
}

template <class T>
inline void __gmp_set_expr(mpq_ptr q, const __gmp_expr<mpf_t, T>& expr)
{
    mpf_class const& temp(expr);
    mpq_set_f(q, temp.get_mpf_t());
}

template <class T>
inline void __gmp_set_expr(mpf_ptr f, const __gmp_expr<mpz_t, T>& expr)
{
    mpz_class const& temp(expr);
    mpf_set_z(f, temp.get_mpz_t());
}

template <class T>
inline void __gmp_set_expr(mpf_ptr f, const __gmp_expr<mpq_t, T>& expr)
{
    mpq_class const& temp(expr);
    mpf_set_q(f, temp.get_mpq_t());
}

inline void __gmp_set_expr(mpf_ptr f, const mpf_class& g)
{
    mpf_set(f, g.get_mpf_t());
}

template <class T>
inline void __gmp_set_expr(mpf_ptr f, const __gmp_expr<mpf_t, T>& expr)
{
    expr.eval(f);
}


/* Temporary objects */

template <class T>
class __gmp_temp
{
    __gmp_expr<T, T> val;
public:
    template<class U, class V>
    __gmp_temp(U const& u, V) : val(u) {}
    typename __gmp_resolve_expr<T>::srcptr_type
        __get_mp() const { return val.__get_mp(); }
};

template <>
class __gmp_temp <mpf_t>
{
    mpf_class val;
public:
    template<class U>
    __gmp_temp(U const& u, mpf_ptr res) : val(u, mpf_get_prec(res)) {}
    mpf_srcptr __get_mp() const { return val.__get_mp(); }
};

/**************** Specializations of __gmp_expr ****************/
/* The eval() method of __gmp_expr<T, U> evaluates the corresponding
   expression and assigns the result to its argument, which is either an
   mpz_t, mpq_t, or mpf_t as specified by the T argument.
   Compound expressions are evaluated recursively (temporaries are created
   to hold intermediate values), while for simple expressions the eval()
   method of the appropriate function object (available as the Op argument
   of either __gmp_unary_expr<T, Op> or __gmp_binary_expr<T, U, Op>) is
   called. */


   /**************** Unary expressions ****************/
   /* cases:
      - simple:   argument is mp*_class, that is, __gmp_expr<T, T>
      - compound: argument is __gmp_expr<T, U> (with U not equal to T) */


      // simple expressions

template <class T, class Op>
class __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, T>, Op> >
{
private:
    typedef __gmp_expr<T, T> val_type;

    __gmp_unary_expr<val_type, Op> expr;
public:
    explicit __gmp_expr(const val_type& val) : expr(val) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        Op::eval(p, expr.val.__get_mp());
    }
    const val_type& get_val() const { return expr.val; }
    mp_bitcnt_t get_prec() const { return expr.val.get_prec(); }
};


// simple expressions, U is a built-in numerical type

template <class T, class U, class Op>
class __gmp_expr<T, __gmp_unary_expr<U, Op> >
{
private:
    typedef U val_type;

    __gmp_unary_expr<val_type, Op> expr;
public:
    explicit __gmp_expr(const val_type& val) : expr(val) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        Op::eval(p, expr.val);
    }
    const val_type& get_val() const { return expr.val; }
    mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }
};


// compound expressions

template <class T, class U, class Op>
class __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, Op> >
{
private:
    typedef __gmp_expr<T, U> val_type;

    __gmp_unary_expr<val_type, Op> expr;
public:
    explicit __gmp_expr(const val_type& val) : expr(val) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        expr.val.eval(p); Op::eval(p, p);
    }
    const val_type& get_val() const { return expr.val; }
    mp_bitcnt_t get_prec() const { return expr.val.get_prec(); }
};


/**************** Binary expressions ****************/
/* simple:
   - arguments are both mp*_class
   - one argument is mp*_class, one is a built-in type
   compound:
   - one is mp*_class, one is __gmp_expr<T, U>
   - one is __gmp_expr<T, U>, one is built-in
   - both arguments are __gmp_expr<...> */


   // simple expressions

template <class T, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, T>, __gmp_expr<T, T>, Op> >
{
private:
    typedef __gmp_expr<T, T> val1_type;
    typedef __gmp_expr<T, T> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        Op::eval(p, expr.val1.__get_mp(), expr.val2.__get_mp());
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};


// simple expressions, U is a built-in numerical type

template <class T, class U, class Op>
class __gmp_expr<T, __gmp_binary_expr<__gmp_expr<T, T>, U, Op> >
{
private:
    typedef __gmp_expr<T, T> val1_type;
    typedef U val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        Op::eval(p, expr.val1.__get_mp(), expr.val2);
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const { return expr.val1.get_prec(); }
};

template <class T, class U, class Op>
class __gmp_expr<T, __gmp_binary_expr<U, __gmp_expr<T, T>, Op> >
{
private:
    typedef U val1_type;
    typedef __gmp_expr<T, T> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        Op::eval(p, expr.val1, expr.val2.__get_mp());
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const { return expr.val2.get_prec(); }
};


// compound expressions, one argument is a subexpression

template <class T, class U, class V, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, T>, __gmp_expr<U, V>, Op> >
{
private:
    typedef __gmp_expr<T, T> val1_type;
    typedef __gmp_expr<U, V> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        if (p != expr.val1.__get_mp())
        {
            __gmp_set_expr(p, expr.val2);
            Op::eval(p, expr.val1.__get_mp(), p);
        }
        else
        {
            __gmp_temp<T> temp(expr.val2, p);
            Op::eval(p, expr.val1.__get_mp(), temp.__get_mp());
        }
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};

template <class T, class U, class V, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<U, V>, __gmp_expr<T, T>, Op> >
{
private:
    typedef __gmp_expr<U, V> val1_type;
    typedef __gmp_expr<T, T> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        if (p != expr.val2.__get_mp())
        {
            __gmp_set_expr(p, expr.val1);
            Op::eval(p, p, expr.val2.__get_mp());
        }
        else
        {
            __gmp_temp<T> temp(expr.val1, p);
            Op::eval(p, temp.__get_mp(), expr.val2.__get_mp());
        }
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};

template <class T, class U, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, T>, __gmp_expr<T, U>, Op> >
{
private:
    typedef __gmp_expr<T, T> val1_type;
    typedef __gmp_expr<T, U> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        if (p != expr.val1.__get_mp())
        {
            __gmp_set_expr(p, expr.val2);
            Op::eval(p, expr.val1.__get_mp(), p);
        }
        else
        {
            __gmp_temp<T> temp(expr.val2, p);
            Op::eval(p, expr.val1.__get_mp(), temp.__get_mp());
        }
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};

template <class T, class U, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<T, T>, Op> >
{
private:
    typedef __gmp_expr<T, U> val1_type;
    typedef __gmp_expr<T, T> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        if (p != expr.val2.__get_mp())
        {
            __gmp_set_expr(p, expr.val1);
            Op::eval(p, p, expr.val2.__get_mp());
        }
        else
        {
            __gmp_temp<T> temp(expr.val1, p);
            Op::eval(p, temp.__get_mp(), expr.val2.__get_mp());
        }
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};


// one argument is a subexpression, one is a built-in

template <class T, class U, class V, class Op>
class __gmp_expr<T, __gmp_binary_expr<__gmp_expr<T, U>, V, Op> >
{
private:
    typedef __gmp_expr<T, U> val1_type;
    typedef V val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        expr.val1.eval(p);
        Op::eval(p, p, expr.val2);
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const { return expr.val1.get_prec(); }
};

template <class T, class U, class V, class Op>
class __gmp_expr<T, __gmp_binary_expr<U, __gmp_expr<T, V>, Op> >
{
private:
    typedef U val1_type;
    typedef __gmp_expr<T, V> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        expr.val2.eval(p);
        Op::eval(p, expr.val1, p);
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const { return expr.val2.get_prec(); }
};


// both arguments are subexpressions

template <class T, class U, class V, class W, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, Op> >
{
private:
    typedef __gmp_expr<T, U> val1_type;
    typedef __gmp_expr<V, W> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        __gmp_temp<T> temp2(expr.val2, p);
        expr.val1.eval(p);
        Op::eval(p, p, temp2.__get_mp());
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};

template <class T, class U, class V, class W, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<U, V>, __gmp_expr<T, W>, Op> >
{
private:
    typedef __gmp_expr<U, V> val1_type;
    typedef __gmp_expr<T, W> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        __gmp_temp<T> temp1(expr.val1, p);
        expr.val2.eval(p);
        Op::eval(p, temp1.__get_mp(), p);
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};

template <class T, class U, class V, class Op>
class __gmp_expr
    <T, __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<T, V>, Op> >
{
private:
    typedef __gmp_expr<T, U> val1_type;
    typedef __gmp_expr<T, V> val2_type;

    __gmp_binary_expr<val1_type, val2_type, Op> expr;
public:
    __gmp_expr(const val1_type& val1, const val2_type& val2)
        : expr(val1, val2) { }
    void eval(typename __gmp_resolve_expr<T>::ptr_type p) const
    {
        __gmp_temp<T> temp2(expr.val2, p);
        expr.val1.eval(p);
        Op::eval(p, p, temp2.__get_mp());
    }
    const val1_type& get_val1() const { return expr.val1; }
    const val2_type& get_val2() const { return expr.val2; }
    mp_bitcnt_t get_prec() const
    {
        mp_bitcnt_t prec1 = expr.val1.get_prec(),
            prec2 = expr.val2.get_prec();
        return (prec1 > prec2) ? prec1 : prec2;
    }
};


/**************** Special cases ****************/

/* Some operations (i.e., add and subtract) with mixed mpz/mpq arguments
   can be done directly without first converting the mpz to mpq.
   Appropriate specializations of __gmp_expr are required. */


#define __GMPZQ_DEFINE_EXPR(eval_fun)                                       \
                                                                            \
template <>                                                                 \
class __gmp_expr<mpq_t, __gmp_binary_expr<mpz_class, mpq_class, eval_fun> > \
{                                                                           \
private:                                                                    \
  typedef mpz_class val1_type;                                              \
  typedef mpq_class val2_type;                                              \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  { eval_fun::eval(q, expr.val1.get_mpz_t(), expr.val2.get_mpq_t()); }      \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <>                                                                 \
class __gmp_expr<mpq_t, __gmp_binary_expr<mpq_class, mpz_class, eval_fun> > \
{                                                                           \
private:                                                                    \
  typedef mpq_class val1_type;                                              \
  typedef mpz_class val2_type;                                              \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  { eval_fun::eval(q, expr.val1.get_mpq_t(), expr.val2.get_mpz_t()); }      \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T>                                                          \
class __gmp_expr                                                            \
<mpq_t, __gmp_binary_expr<mpz_class, __gmp_expr<mpq_t, T>, eval_fun> >      \
{                                                                           \
private:                                                                    \
  typedef mpz_class val1_type;                                              \
  typedef __gmp_expr<mpq_t, T> val2_type;                                   \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpq_class temp(expr.val2);                                              \
    eval_fun::eval(q, expr.val1.get_mpz_t(), temp.get_mpq_t());             \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T>                                                          \
class __gmp_expr                                                            \
<mpq_t, __gmp_binary_expr<mpq_class, __gmp_expr<mpz_t, T>, eval_fun> >      \
{                                                                           \
private:                                                                    \
  typedef mpq_class val1_type;                                              \
  typedef __gmp_expr<mpz_t, T> val2_type;                                   \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpz_class temp(expr.val2);                                              \
    eval_fun::eval(q, expr.val1.get_mpq_t(), temp.get_mpz_t());             \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T>                                                          \
class __gmp_expr                                                            \
<mpq_t, __gmp_binary_expr<__gmp_expr<mpz_t, T>, mpq_class, eval_fun> >      \
{                                                                           \
private:                                                                    \
  typedef __gmp_expr<mpz_t, T> val1_type;                                   \
  typedef mpq_class val2_type;                                              \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpz_class temp(expr.val1);                                              \
    eval_fun::eval(q, temp.get_mpz_t(), expr.val2.get_mpq_t());             \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T>                                                          \
class __gmp_expr                                                            \
<mpq_t, __gmp_binary_expr<__gmp_expr<mpq_t, T>, mpz_class, eval_fun> >      \
{                                                                           \
private:                                                                    \
  typedef __gmp_expr<mpq_t, T> val1_type;                                   \
  typedef mpz_class val2_type;                                              \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpq_class temp(expr.val1);                                              \
    eval_fun::eval(q, temp.get_mpq_t(), expr.val2.get_mpz_t());             \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T, class U>                                                 \
class __gmp_expr<mpq_t, __gmp_binary_expr                                   \
<__gmp_expr<mpz_t, T>, __gmp_expr<mpq_t, U>, eval_fun> >                    \
{                                                                           \
private:                                                                    \
  typedef __gmp_expr<mpz_t, T> val1_type;                                   \
  typedef __gmp_expr<mpq_t, U> val2_type;                                   \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpz_class temp1(expr.val1);                                             \
    expr.val2.eval(q);                                                      \
    eval_fun::eval(q, temp1.get_mpz_t(), q);                                \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};                                                                          \
                                                                            \
template <class T, class U>                                                 \
class __gmp_expr<mpq_t, __gmp_binary_expr                                   \
<__gmp_expr<mpq_t, T>, __gmp_expr<mpz_t, U>, eval_fun> >                    \
{                                                                           \
private:                                                                    \
  typedef __gmp_expr<mpq_t, T> val1_type;                                   \
  typedef __gmp_expr<mpz_t, U> val2_type;                                   \
                                                                            \
  __gmp_binary_expr<val1_type, val2_type, eval_fun> expr;                   \
public:                                                                     \
  __gmp_expr(const val1_type &val1, const val2_type &val2)                  \
    : expr(val1, val2) { }                                                  \
  void eval(mpq_ptr q) const                                                \
  {                                                                         \
    mpz_class temp2(expr.val2);                                             \
    expr.val1.eval(q);                                             \
    eval_fun::eval(q, q, temp2.get_mpz_t());                \
  }                                                                         \
  const val1_type & get_val1() const { return expr.val1; }                  \
  const val2_type & get_val2() const { return expr.val2; }                  \
  mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }           \
};


__GMPZQ_DEFINE_EXPR(__gmp_binary_plus)
__GMPZQ_DEFINE_EXPR(__gmp_binary_minus)



/**************** Macros for defining functions ****************/
/* Results of operators and functions are instances of __gmp_expr<T, U>.
   T determines the numerical type of the expression: it can be either
   mpz_t, mpq_t, or mpf_t.  When the arguments of a binary
   expression have different numerical types, __gmp_resolve_expr is used
   to determine the "larger" type.
   U is either __gmp_unary_expr<V, Op> or __gmp_binary_expr<V, W, Op>,
   where V and W are the arguments' types -- they can in turn be
   expressions, thus allowing to build compound expressions to any
   degree of complexity.
   Op is a function object that must have an eval() method accepting
   appropriate arguments.
   Actual evaluation of a __gmp_expr<T, U> object is done when it gets
   assigned to an mp*_class ("lazy" evaluation): this is done by calling
   its eval() method. */


   // non-member unary operators and functions

#define __GMP_DEFINE_UNARY_FUNCTION(fun, eval_fun)                           \
                                                                             \
template <class T, class U>                                                  \
inline __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >          \
fun(const __gmp_expr<T, U> &expr)                                            \
{                                                                            \
  return __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >(expr); \
}

// variant that only works for one of { mpz, mpq, mpf }

#define __GMP_DEFINE_UNARY_FUNCTION_1(T, fun, eval_fun)                      \
                                                                             \
template <class U>                                                           \
inline __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >          \
fun(const __gmp_expr<T, U> &expr)                                            \
{                                                                            \
  return __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >(expr); \
}

#define __GMP_DEFINE_UNARY_TYPE_FUNCTION(type, fun, eval_fun) \
                                                              \
template <class T, class U>                                   \
inline type fun(const __gmp_expr<T, U> &expr)                 \
{                                                             \
  __gmp_expr<T, T> const& temp(expr); \
  return eval_fun::eval(temp.__get_mp());                     \
}


// non-member binary operators and functions

#define __GMPP_DEFINE_BINARY_FUNCTION(fun, eval_fun)                   \
                                                                       \
template <class T, class U, class V, class W>                          \
inline __gmp_expr<typename __gmp_resolve_expr<T, V>::value_type,       \
__gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, eval_fun> >      \
fun(const __gmp_expr<T, U> &expr1, const __gmp_expr<V, W> &expr2)      \
{                                                                      \
  return __gmp_expr<typename __gmp_resolve_expr<T, V>::value_type,     \
     __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<V, W>, eval_fun> > \
    (expr1, expr2);                                                    \
}

#define __GMPNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, bigtype)       \
                                                                           \
template <class T, class U>                                                \
inline __gmp_expr                                                          \
<T, __gmp_binary_expr<__gmp_expr<T, U>, bigtype, eval_fun> >               \
fun(const __gmp_expr<T, U> &expr, type t)                                  \
{                                                                          \
  return __gmp_expr                                                        \
    <T, __gmp_binary_expr<__gmp_expr<T, U>, bigtype, eval_fun> >(expr, t); \
}                                                                          \
                                                                           \
template <class T, class U>                                                \
inline __gmp_expr                                                          \
<T, __gmp_binary_expr<bigtype, __gmp_expr<T, U>, eval_fun> >               \
fun(type t, const __gmp_expr<T, U> &expr)                                  \
{                                                                          \
  return __gmp_expr                                                        \
    <T, __gmp_binary_expr<bigtype, __gmp_expr<T, U>, eval_fun> >(t, expr); \
}

#define __GMPNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, type)          \
__GMPNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, signed long int)

#define __GMPNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, type)            \
__GMPNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, unsigned long int)

#define __GMPND_DEFINE_BINARY_FUNCTION(fun, eval_fun, type) \
__GMPNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, double)

#define __GMPNLD_DEFINE_BINARY_FUNCTION(fun, eval_fun, type)     \
__GMPNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, long double)

#define __GMPN_DEFINE_BINARY_FUNCTION(fun, eval_fun)              \
__GMPNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed char)        \
__GMPNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned char)      \
__GMPNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed int)         \
__GMPNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned int)       \
__GMPNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed short int)   \
__GMPNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned short int) \
__GMPNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed long int)    \
__GMPNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned long int)  \
__GMPND_DEFINE_BINARY_FUNCTION(fun, eval_fun, float)              \
__GMPND_DEFINE_BINARY_FUNCTION(fun, eval_fun, double)             \
/* __GMPNLD_DEFINE_BINARY_FUNCTION(fun, eval_fun, long double) */

#define __GMP_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
__GMPP_DEFINE_BINARY_FUNCTION(fun, eval_fun)        \
__GMPN_DEFINE_BINARY_FUNCTION(fun, eval_fun)

// variant that only works for one of { mpz, mpq, mpf }

#define __GMPP_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun)              \
                                                                       \
template <class U, class W>                                            \
inline __gmp_expr<T,                                                   \
__gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<T, W>, eval_fun> >      \
fun(const __gmp_expr<T, U> &expr1, const __gmp_expr<T, W> &expr2)      \
{                                                                      \
  return __gmp_expr<T,                                                 \
     __gmp_binary_expr<__gmp_expr<T, U>, __gmp_expr<T, W>, eval_fun> > \
    (expr1, expr2);                                                    \
}

#define __GMPNN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type, bigtype)  \
                                                                           \
template <class U>                                                         \
inline __gmp_expr                                                          \
<T, __gmp_binary_expr<__gmp_expr<T, U>, bigtype, eval_fun> >               \
fun(const __gmp_expr<T, U> &expr, type t)                                  \
{                                                                          \
  return __gmp_expr                                                        \
    <T, __gmp_binary_expr<__gmp_expr<T, U>, bigtype, eval_fun> >(expr, t); \
}                                                                          \
                                                                           \
template <class U>                                                         \
inline __gmp_expr                                                          \
<T, __gmp_binary_expr<bigtype, __gmp_expr<T, U>, eval_fun> >               \
fun(type t, const __gmp_expr<T, U> &expr)                                  \
{                                                                          \
  return __gmp_expr                                                        \
    <T, __gmp_binary_expr<bigtype, __gmp_expr<T, U>, eval_fun> >(t, expr); \
}

#define __GMPNS_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type)          \
__GMPNN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type, signed long int)

#define __GMPNU_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type)          \
__GMPNN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type, unsigned long int)

#define __GMPND_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type) \
__GMPNN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type, double)

#define __GMPNLD_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type)     \
__GMPNN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, type, long double)

#define __GMPN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun)              \
__GMPNS_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, signed char)        \
__GMPNU_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, unsigned char)      \
__GMPNS_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, signed int)         \
__GMPNU_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, unsigned int)       \
__GMPNS_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, signed short int)   \
__GMPNU_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, unsigned short int) \
__GMPNS_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, signed long int)    \
__GMPNU_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, unsigned long int)  \
__GMPND_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, float)              \
__GMPND_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, double)             \
/* __GMPNLD_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun, long double) */

#define __GMP_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun) \
__GMPP_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun)        \
__GMPN_DEFINE_BINARY_FUNCTION_1(T, fun, eval_fun)


#define __GMP_DEFINE_BINARY_FUNCTION_UI(fun, eval_fun)                 \
                                                                       \
template <class T, class U>                                            \
inline __gmp_expr                                                      \
<T, __gmp_binary_expr<__gmp_expr<T, U>, mp_bitcnt_t, eval_fun> > \
fun(const __gmp_expr<T, U> &expr, mp_bitcnt_t l)                 \
{                                                                      \
  return __gmp_expr<T, __gmp_binary_expr                               \
    <__gmp_expr<T, U>, mp_bitcnt_t, eval_fun> >(expr, l);        \
}


#define __GMPP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun)         \
                                                                        \
template <class T, class U, class V, class W>                           \
inline type fun(const __gmp_expr<T, U> &expr1,                          \
		const __gmp_expr<V, W> &expr2)                          \
{                                                                       \
  __gmp_expr<T, T> const& temp1(expr1);                                 \
  __gmp_expr<V, V> const& temp2(expr2);                                 \
  return eval_fun::eval(temp1.__get_mp(), temp2.__get_mp());            \
}

#define __GMPNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun,   \
					    type2, bigtype)        \
                                                                   \
template <class T, class U>                                        \
inline type fun(const __gmp_expr<T, U> &expr, type2 t)             \
{                                                                  \
  __gmp_expr<T, T> const& temp(expr);      \
  return eval_fun::eval(temp.__get_mp(), static_cast<bigtype>(t)); \
}                                                                  \
                                                                   \
template <class T, class U>                                        \
inline type fun(type2 t, const __gmp_expr<T, U> &expr)             \
{                                                                  \
  __gmp_expr<T, T> const& temp(expr);      \
  return eval_fun::eval(static_cast<bigtype>(t), temp.__get_mp()); \
}

#define __GMPNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun,                \
				    type2, signed long int)

#define __GMPNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun,                \
				    type2, unsigned long int)

#define __GMPND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2, double)

#define __GMPNLD_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2)     \
__GMPNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2, long double)

#define __GMPN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun)              \
__GMPNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed char)        \
__GMPNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned char)      \
__GMPNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed int)         \
__GMPNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned int)       \
__GMPNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed short int)   \
__GMPNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned short int) \
__GMPNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed long int)    \
__GMPNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned long int)  \
__GMPND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, float)              \
__GMPND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, double)             \
/* __GMPNLD_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, long double) */

#define __GMP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
__GMPP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun)        \
__GMPN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun)


// member operators

#define __GMPP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun)                 \
                                                                             \
template <class T, class U>                                                  \
inline type##_class & type##_class::fun(const __gmp_expr<T, U> &expr)        \
{                                                                            \
  __gmp_set_expr(mp, __gmp_expr<type##_t, __gmp_binary_expr                  \
		 <type##_class, __gmp_expr<T, U>, eval_fun> >(*this, expr)); \
  return *this;                                                              \
}

#define __GMPNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun,    \
					 type2, bigtype)         \
                                                                 \
inline type##_class & type##_class::fun(type2 t)                 \
{                                                                \
  __gmp_set_expr(mp, __gmp_expr<type##_t, __gmp_binary_expr      \
		 <type##_class, bigtype, eval_fun> >(*this, t)); \
  return *this;                                                  \
}

#define __GMPNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun,                \
				 type2, signed long int)

#define __GMPNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun,                \
				 type2, unsigned long int)

#define __GMPND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__GMPNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2, double)

#define __GMPNLD_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2)     \
__GMPNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2, long double)

#define __GMPN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun)              \
__GMPNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed char)        \
__GMPNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned char)      \
__GMPNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed int)         \
__GMPNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned int)       \
__GMPNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed short int)   \
__GMPNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned short int) \
__GMPNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed long int)    \
__GMPNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned long int)  \
__GMPND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, float)              \
__GMPND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, double)             \
/* __GMPNLD_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, long double) */

#define __GMP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
__GMPP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun)        \
__GMPN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun)

#define __GMPZ_DEFINE_COMPOUND_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR(mpz, fun, eval_fun)

#define __GMPQ_DEFINE_COMPOUND_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR(mpq, fun, eval_fun)

#define __GMPF_DEFINE_COMPOUND_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR(mpf, fun, eval_fun)



#define __GMP_DEFINE_COMPOUND_OPERATOR_UI(type, fun, eval_fun)  \
                                                                \
inline type##_class & type##_class::fun(mp_bitcnt_t l)    \
{                                                               \
  __gmp_set_expr(mp, __gmp_expr<type##_t, __gmp_binary_expr     \
    <type##_class, mp_bitcnt_t, eval_fun> >(*this, l));   \
  return *this;                                                 \
}

#define __GMPZ_DEFINE_COMPOUND_OPERATOR_UI(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR_UI(mpz, fun, eval_fun)

#define __GMPQ_DEFINE_COMPOUND_OPERATOR_UI(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR_UI(mpq, fun, eval_fun)

#define __GMPF_DEFINE_COMPOUND_OPERATOR_UI(fun, eval_fun) \
__GMP_DEFINE_COMPOUND_OPERATOR_UI(mpf, fun, eval_fun)



#define __GMP_DEFINE_INCREMENT_OPERATOR(type, fun, eval_fun) \
                                                             \
inline type##_class & type##_class::fun()                    \
{                                                            \
  eval_fun::eval(mp);                                        \
  return *this;                                              \
}                                                            \
                                                             \
inline type##_class type##_class::fun(int)                   \
{                                                            \
  type##_class temp(*this);                                  \
  eval_fun::eval(mp);                                        \
  return temp;                                               \
}

#define __GMPZ_DEFINE_INCREMENT_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_INCREMENT_OPERATOR(mpz, fun, eval_fun)

#define __GMPQ_DEFINE_INCREMENT_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_INCREMENT_OPERATOR(mpq, fun, eval_fun)

#define __GMPF_DEFINE_INCREMENT_OPERATOR(fun, eval_fun) \
__GMP_DEFINE_INCREMENT_OPERATOR(mpf, fun, eval_fun)


#define __GMPP_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                  \
template <class U>                                                           \
__gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >          \
fun(const __gmp_expr<T, U> &expr)                                            \
{                                                                            \
  return __gmp_expr<T, __gmp_unary_expr<__gmp_expr<T, U>, eval_fun> >(expr); \
}

#define __GMPNN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, bigtype)  \
inline __gmp_expr<T, __gmp_unary_expr<bigtype, eval_fun> >                   \
fun(type expr)                                                               \
{                                                                            \
  return __gmp_expr<T, __gmp_unary_expr<bigtype, eval_fun> >(expr);          \
}

#define __GMPNS_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, signed long)
#define __GMPNU_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, unsigned long)
#define __GMPND_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type)  \
__GMPNN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, type, double)

#define __GMPN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                 \
__GMPNS_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed char)           \
__GMPNU_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned char)         \
__GMPNS_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed int)            \
__GMPNU_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned int)          \
__GMPNS_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed short int)      \
__GMPNU_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned short int)    \
__GMPNS_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, signed long int)       \
__GMPNU_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, unsigned long int)     \
__GMPND_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, float)                 \
__GMPND_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun, double)                \

#define __GMP_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                  \
__GMPP_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                         \
__GMPN_DEFINE_UNARY_STATIC_MEMFUN(T, fun, eval_fun)                         \


/**************** Arithmetic operators and functions ****************/

// non-member operators and functions

__GMP_DEFINE_UNARY_FUNCTION(operator+, __gmp_unary_plus)
__GMP_DEFINE_UNARY_FUNCTION(operator-, __gmp_unary_minus)
__GMP_DEFINE_UNARY_FUNCTION_1(mpz_t, operator~, __gmp_unary_com)

__GMP_DEFINE_BINARY_FUNCTION(operator+, __gmp_binary_plus)
__GMP_DEFINE_BINARY_FUNCTION(operator-, __gmp_binary_minus)
__GMP_DEFINE_BINARY_FUNCTION(operator*, __gmp_binary_multiplies)
__GMP_DEFINE_BINARY_FUNCTION(operator/, __gmp_binary_divides)
__GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, operator%, __gmp_binary_modulus)
__GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, operator&, __gmp_binary_and)
__GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, operator|, __gmp_binary_ior)
__GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, operator^, __gmp_binary_xor)

__GMP_DEFINE_BINARY_FUNCTION_UI(operator<<, __gmp_binary_lshift)
__GMP_DEFINE_BINARY_FUNCTION_UI(operator>>, __gmp_binary_rshift)

__GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator==, __gmp_binary_equal)
__GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator!=, !__gmp_binary_equal)
__GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator<, __gmp_binary_less)
    __GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator<=, !__gmp_binary_greater)
    __GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator>, __gmp_binary_greater)
    __GMP_DEFINE_BINARY_TYPE_FUNCTION(bool, operator>=, !__gmp_binary_less)

    __GMP_DEFINE_UNARY_FUNCTION(abs, __gmp_abs_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpf_t, trunc, __gmp_trunc_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpf_t, floor, __gmp_floor_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpf_t, ceil, __gmp_ceil_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpf_t, sqrt, __gmp_sqrt_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpz_t, sqrt, __gmp_sqrt_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpz_t, factorial, __gmp_fac_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpz_t, primorial, __gmp_primorial_function)
    __GMP_DEFINE_UNARY_FUNCTION_1(mpz_t, fibonacci, __gmp_fib_function)
    __GMP_DEFINE_BINARY_FUNCTION_1(mpf_t, hypot, __gmp_hypot_function)
    __GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, gcd, __gmp_gcd_function)
    __GMP_DEFINE_BINARY_FUNCTION_1(mpz_t, lcm, __gmp_lcm_function)

    __GMP_DEFINE_UNARY_TYPE_FUNCTION(int, sgn, __gmp_sgn_function)
    __GMP_DEFINE_BINARY_TYPE_FUNCTION(int, cmp, __gmp_cmp_function)

    template <class T>
void swap(__gmp_expr<T, T>& x, __gmp_expr<T, T>& y) __GMPXX_NOEXCEPT
{
    x.swap(y);
}

// member operators for mpz_class

__GMPZ_DEFINE_COMPOUND_OPERATOR(operator+=, __gmp_binary_plus)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator-=, __gmp_binary_minus)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator*=, __gmp_binary_multiplies)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator/=, __gmp_binary_divides)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator%=, __gmp_binary_modulus)

__GMPZ_DEFINE_COMPOUND_OPERATOR(operator&=, __gmp_binary_and)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator|=, __gmp_binary_ior)
__GMPZ_DEFINE_COMPOUND_OPERATOR(operator^=, __gmp_binary_xor)

__GMPZ_DEFINE_COMPOUND_OPERATOR_UI(operator<<=, __gmp_binary_lshift)
__GMPZ_DEFINE_COMPOUND_OPERATOR_UI(operator>>=, __gmp_binary_rshift)

__GMPZ_DEFINE_INCREMENT_OPERATOR(operator++, __gmp_unary_increment)
__GMPZ_DEFINE_INCREMENT_OPERATOR(operator--, __gmp_unary_decrement)

__GMP_DEFINE_UNARY_STATIC_MEMFUN(mpz_t, mpz_class::factorial, __gmp_fac_function)
__GMP_DEFINE_UNARY_STATIC_MEMFUN(mpz_t, mpz_class::primorial, __gmp_primorial_function)
__GMP_DEFINE_UNARY_STATIC_MEMFUN(mpz_t, mpz_class::fibonacci, __gmp_fib_function)

// member operators for mpq_class

__GMPQ_DEFINE_COMPOUND_OPERATOR(operator+=, __gmp_binary_plus)
__GMPQ_DEFINE_COMPOUND_OPERATOR(operator-=, __gmp_binary_minus)
__GMPQ_DEFINE_COMPOUND_OPERATOR(operator*=, __gmp_binary_multiplies)
__GMPQ_DEFINE_COMPOUND_OPERATOR(operator/=, __gmp_binary_divides)

__GMPQ_DEFINE_COMPOUND_OPERATOR_UI(operator<<=, __gmp_binary_lshift)
__GMPQ_DEFINE_COMPOUND_OPERATOR_UI(operator>>=, __gmp_binary_rshift)

__GMPQ_DEFINE_INCREMENT_OPERATOR(operator++, __gmp_unary_increment)
__GMPQ_DEFINE_INCREMENT_OPERATOR(operator--, __gmp_unary_decrement)

// member operators for mpf_class

__GMPF_DEFINE_COMPOUND_OPERATOR(operator+=, __gmp_binary_plus)
__GMPF_DEFINE_COMPOUND_OPERATOR(operator-=, __gmp_binary_minus)
__GMPF_DEFINE_COMPOUND_OPERATOR(operator*=, __gmp_binary_multiplies)
__GMPF_DEFINE_COMPOUND_OPERATOR(operator/=, __gmp_binary_divides)

__GMPF_DEFINE_COMPOUND_OPERATOR_UI(operator<<=, __gmp_binary_lshift)
__GMPF_DEFINE_COMPOUND_OPERATOR_UI(operator>>=, __gmp_binary_rshift)

__GMPF_DEFINE_INCREMENT_OPERATOR(operator++, __gmp_unary_increment)
__GMPF_DEFINE_INCREMENT_OPERATOR(operator--, __gmp_unary_decrement)



/**************** Class wrapper for gmp_randstate_t ****************/

class __gmp_urandomb_value { };
class __gmp_urandomm_value { };

template <>
class __gmp_expr<mpz_t, __gmp_urandomb_value>
{
private:
    __gmp_randstate_struct* state;
    mp_bitcnt_t bits;
public:
    __gmp_expr(gmp_randstate_t s, mp_bitcnt_t l) : state(s), bits(l) { }
    void eval(mpz_ptr z) const { __gmp_rand_function::eval(z, state, bits); }
    mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }
};

template <>
class __gmp_expr<mpz_t, __gmp_urandomm_value>
{
private:
    __gmp_randstate_struct* state;
    mpz_class range;
public:
    __gmp_expr(gmp_randstate_t s, const mpz_class& z) : state(s), range(z) { }
    void eval(mpz_ptr z) const
    {
        __gmp_rand_function::eval(z, state, range.get_mpz_t());
    }
    mp_bitcnt_t get_prec() const { return mpf_get_default_prec(); }
};

template <>
class __gmp_expr<mpf_t, __gmp_urandomb_value>
{
private:
    __gmp_randstate_struct* state;
    mp_bitcnt_t bits;
public:
    __gmp_expr(gmp_randstate_t s, mp_bitcnt_t l) : state(s), bits(l) { }
    void eval(mpf_ptr f) const
    {
        __gmp_rand_function::eval(f, state,
            (bits > 0) ? bits : mpf_get_prec(f));
    }
    mp_bitcnt_t get_prec() const
    {
        if (bits == 0)
            return mpf_get_default_prec();
        else
            return bits;
    }
};

extern "C" {
    typedef void __gmp_randinit_default_t(gmp_randstate_t);
    typedef void __gmp_randinit_lc_2exp_t(gmp_randstate_t, mpz_srcptr, unsigned long int, mp_bitcnt_t);
    typedef int __gmp_randinit_lc_2exp_size_t(gmp_randstate_t, mp_bitcnt_t);
}

class gmp_randclass
{
private:
    gmp_randstate_t state;

    // copy construction and assignment not allowed
    gmp_randclass(const gmp_randclass&);
    void operator=(const gmp_randclass&);
public:
    // constructors and destructor
    gmp_randclass(gmp_randalg_t alg, unsigned long int size)
    {
        switch (alg)
        {
        case GMP_RAND_ALG_LC: // no other cases for now
        default:
            gmp_randinit(state, alg, size);
            break;
        }
    }

    // gmp_randinit_default
    gmp_randclass(__gmp_randinit_default_t* f) { f(state); }

    // gmp_randinit_lc_2exp
    gmp_randclass(__gmp_randinit_lc_2exp_t* f,
        mpz_class z, unsigned long int l1, mp_bitcnt_t l2)
    {
        f(state, z.get_mpz_t(), l1, l2);
    }

    // gmp_randinit_lc_2exp_size
    gmp_randclass(__gmp_randinit_lc_2exp_size_t* f,
        mp_bitcnt_t size)
    {
        if (f(state, size) == 0)
            throw std::length_error("gmp_randinit_lc_2exp_size");
    }

    ~gmp_randclass() { gmp_randclear(state); }

    // initialize
    void seed(); // choose a random seed some way (?)
    void seed(unsigned long int s) { gmp_randseed_ui(state, s); }
    void seed(const mpz_class& z) { gmp_randseed(state, z.get_mpz_t()); }

    // get random number
    __gmp_expr<mpz_t, __gmp_urandomb_value> get_z_bits(mp_bitcnt_t l)
    {
        return __gmp_expr<mpz_t, __gmp_urandomb_value>(state, l);
    }
    __gmp_expr<mpz_t, __gmp_urandomb_value> get_z_bits(const mpz_class& z)
    {
        return get_z_bits(z.get_ui());
    }
    // FIXME: z.get_bitcnt_t() ?

    __gmp_expr<mpz_t, __gmp_urandomm_value> get_z_range(const mpz_class& z)
    {
        return __gmp_expr<mpz_t, __gmp_urandomm_value>(state, z);
    }

    __gmp_expr<mpf_t, __gmp_urandomb_value> get_f(mp_bitcnt_t prec = 0)
    {
        return __gmp_expr<mpf_t, __gmp_urandomb_value>(state, prec);
    }
};


/**************** Specialize std::numeric_limits ****************/

namespace std {
    template <> class numeric_limits<mpz_class>
    {
    public:
        static const bool is_specialized = true;
        static mpz_class min() { return mpz_class(); }
        static mpz_class max() { return mpz_class(); }
        static mpz_class lowest() { return mpz_class(); }
        static const int digits = 0;
        static const int digits10 = 0;
        static const int max_digits10 = 0;
        static const bool is_signed = true;
        static const bool is_integer = true;
        static const bool is_exact = true;
        static const int radix = 2;
        static mpz_class epsilon() { return mpz_class(); }
        static mpz_class round_error() { return mpz_class(); }
        static const int min_exponent = 0;
        static const int min_exponent10 = 0;
        static const int max_exponent = 0;
        static const int max_exponent10 = 0;
        static const bool has_infinity = false;
        static const bool has_quiet_NaN = false;
        static const bool has_signaling_NaN = false;
        static const float_denorm_style has_denorm = denorm_absent;
        static const bool has_denorm_loss = false;
        static mpz_class infinity() { return mpz_class(); }
        static mpz_class quiet_NaN() { return mpz_class(); }
        static mpz_class signaling_NaN() { return mpz_class(); }
        static mpz_class denorm_min() { return mpz_class(); }
        static const bool is_iec559 = false;
        static const bool is_bounded = false;
        static const bool is_modulo = false;
        static const bool traps = false;
        static const bool tinyness_before = false;
        static const float_round_style round_style = round_toward_zero;
    };

    template <> class numeric_limits<mpq_class>
    {
    public:
        static const bool is_specialized = true;
        static mpq_class min() { return mpq_class(); }
        static mpq_class max() { return mpq_class(); }
        static mpq_class lowest() { return mpq_class(); }
        static const int digits = 0;
        static const int digits10 = 0;
        static const int max_digits10 = 0;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = true;
        static const int radix = 2;
        static mpq_class epsilon() { return mpq_class(); }
        static mpq_class round_error() { return mpq_class(); }
        static const int min_exponent = 0;
        static const int min_exponent10 = 0;
        static const int max_exponent = 0;
        static const int max_exponent10 = 0;
        static const bool has_infinity = false;
        static const bool has_quiet_NaN = false;
        static const bool has_signaling_NaN = false;
        static const float_denorm_style has_denorm = denorm_absent;
        static const bool has_denorm_loss = false;
        static mpq_class infinity() { return mpq_class(); }
        static mpq_class quiet_NaN() { return mpq_class(); }
        static mpq_class signaling_NaN() { return mpq_class(); }
        static mpq_class denorm_min() { return mpq_class(); }
        static const bool is_iec559 = false;
        static const bool is_bounded = false;
        static const bool is_modulo = false;
        static const bool traps = false;
        static const bool tinyness_before = false;
        static const float_round_style round_style = round_toward_zero;
    };

    template <> class numeric_limits<mpf_class>
    {
    public:
        static const bool is_specialized = true;
        static mpf_class min() { return mpf_class(); }
        static mpf_class max() { return mpf_class(); }
        static mpf_class lowest() { return mpf_class(); }
        static const int digits = 0;
        static const int digits10 = 0;
        static const int max_digits10 = 0;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = false;
        static const int radix = 2;
        static mpf_class epsilon() { return mpf_class(); }
        static mpf_class round_error() { return mpf_class(); }
        static const int min_exponent = 0;
        static const int min_exponent10 = 0;
        static const int max_exponent = 0;
        static const int max_exponent10 = 0;
        static const bool has_infinity = false;
        static const bool has_quiet_NaN = false;
        static const bool has_signaling_NaN = false;
        static const float_denorm_style has_denorm = denorm_absent;
        static const bool has_denorm_loss = false;
        static mpf_class infinity() { return mpf_class(); }
        static mpf_class quiet_NaN() { return mpf_class(); }
        static mpf_class signaling_NaN() { return mpf_class(); }
        static mpf_class denorm_min() { return mpf_class(); }
        static const bool is_iec559 = false;
        static const bool is_bounded = false;
        static const bool is_modulo = false;
        static const bool traps = false;
        static const bool tinyness_before = false;
        static const float_round_style round_style = round_indeterminate;
    };
}


/**************** #undef all private macros ****************/

#undef __GMPP_DECLARE_COMPOUND_OPERATOR
#undef __GMPN_DECLARE_COMPOUND_OPERATOR
#undef __GMP_DECLARE_COMPOUND_OPERATOR
#undef __GMP_DECLARE_COMPOUND_OPERATOR_UI
#undef __GMP_DECLARE_INCREMENT_OPERATOR
#undef __GMPXX_DEFINE_ARITHMETIC_CONSTRUCTORS
#undef __GMPXX_DEFINE_ARITHMETIC_ASSIGNMENTS

#undef __GMPZQ_DEFINE_EXPR

#undef __GMP_DEFINE_UNARY_FUNCTION_1
#undef __GMP_DEFINE_UNARY_FUNCTION
#undef __GMP_DEFINE_UNARY_TYPE_FUNCTION

#undef __GMPP_DEFINE_BINARY_FUNCTION
#undef __GMPNN_DEFINE_BINARY_FUNCTION
#undef __GMPNS_DEFINE_BINARY_FUNCTION
#undef __GMPNU_DEFINE_BINARY_FUNCTION
#undef __GMPND_DEFINE_BINARY_FUNCTION
#undef __GMPNLD_DEFINE_BINARY_FUNCTION
#undef __GMPN_DEFINE_BINARY_FUNCTION
#undef __GMP_DEFINE_BINARY_FUNCTION

#undef __GMP_DEFINE_BINARY_FUNCTION_UI

#undef __GMPP_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPNN_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPNS_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPNU_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPND_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPNLD_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMPN_DEFINE_BINARY_TYPE_FUNCTION
#undef __GMP_DEFINE_BINARY_TYPE_FUNCTION

#undef __GMPZ_DEFINE_COMPOUND_OPERATOR

#undef __GMPP_DEFINE_COMPOUND_OPERATOR
#undef __GMPNN_DEFINE_COMPOUND_OPERATOR
#undef __GMPNS_DEFINE_COMPOUND_OPERATOR
#undef __GMPNU_DEFINE_COMPOUND_OPERATOR
#undef __GMPND_DEFINE_COMPOUND_OPERATOR
#undef __GMPNLD_DEFINE_COMPOUND_OPERATOR
#undef __GMPN_DEFINE_COMPOUND_OPERATOR
#undef __GMP_DEFINE_COMPOUND_OPERATOR

#undef __GMPQ_DEFINE_COMPOUND_OPERATOR
#undef __GMPF_DEFINE_COMPOUND_OPERATOR

#undef __GMP_DEFINE_COMPOUND_OPERATOR_UI
#undef __GMPZ_DEFINE_COMPOUND_OPERATOR_UI
#undef __GMPQ_DEFINE_COMPOUND_OPERATOR_UI
#undef __GMPF_DEFINE_COMPOUND_OPERATOR_UI

#undef __GMP_DEFINE_INCREMENT_OPERATOR
#undef __GMPZ_DEFINE_INCREMENT_OPERATOR
#undef __GMPQ_DEFINE_INCREMENT_OPERATOR
#undef __GMPF_DEFINE_INCREMENT_OPERATOR

#undef __GMPXX_CONSTANT_TRUE
#undef __GMPXX_CONSTANT

#endif /* __GMP_PLUSPLUS__ */

#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <mutex>
#include <limits>

#ifndef _NODISCARD
    #define _NODISCARD [[nodiscard]]
#endif

namespace RSA
{
    static constexpr uint32_t Auto = static_cast<uint32_t>(-1);

    template<class, bool>
    class basic_rsa;

    // char, no throw
    using RSA = basic_rsa<char, false>;
    // char, throw
    using tRSA = basic_rsa<char, true>;

    // wchar_t, no throw
    using wRSA = basic_rsa<wchar_t, false>;
    // wchar_t, throw
    using wtRSA = basic_rsa<wchar_t, true>;

#ifdef __cpp_char8_t
    // char8_t, no throw
    using u8RSA = basic_rsa<char8_t, false>;
    // char8_t, throw
    using u8tRSA = basic_rsa<char8_t, true>;
#endif

    // char16_t, no throw
    using u16RSA = basic_rsa<char16_t, false>;
    // char16_t, throw
    using u16tRSA = basic_rsa<char16_t, true>;

    // char32_t, no throw
    using u32RSA = basic_rsa<char32_t, false>;
    // char32_t, throw
    using u32tRSA = basic_rsa<char32_t, true>;

    typedef mpz_class number_t;

    namespace detail
    {
        // * Decimal to number
        template <class Ty = int64_t>
        _NODISCARD constexpr Ty dton(const char* str) {
            constexpr Ty _max = std::numeric_limits<Ty>::max();

            bool _negative = false;

            if (*str == '-') {
                _negative = true;
                ++str;
            }

            Ty result = 0;
            while (*str)
            {
                if (*str < '0' || *str > '9')
                    throw std::invalid_argument("only number strings");

                const Ty temp = result * 10 + (*str++ - '0');

                if (temp < result)
                    return _max;

                result = temp;
            }

            if (_negative)
                result *= -1;

            return result;
        }

        // * Hex to number
        template <class Ty = int64_t>
        _NODISCARD constexpr Ty hton(const char* str)
        {
            constexpr Ty _max = std::numeric_limits<Ty>::max();

            bool _negative = false;

            if (*str == '-') {
                _negative = true;
                ++str;
            }

            Ty result = 0;
            while (*str)
            {
                Ty temp = result * 16;

                if (*str >= '0' && *str <= '9') {
                    temp += *str++ - '0';
                }
                else if (*str >= 'a' && *str <= 'f') {
                    temp += 10 + *str++ - 'a';
                }
                else if (*str >= 'A' && *str <= 'F') {
                    temp += 10 + *str++ - 'A';
                }
                else {
                    throw std::invalid_argument("only hex number strings");
                }

                if (temp < result) {
                    return _max;
                }

                result = temp;
            }

            if (_negative)
                result *= -1;

            return result;
        }
    }

    template<class _char, bool _throw>
    void print(basic_rsa<_char, _throw> _rsa) {
        std::cout << _rsa;
    }

    template <class char_type = char, bool throw_errors = false>
    class basic_rsa
    {
    private:

        constexpr static inline auto one_in_block_size = std::numeric_limits<char_type>::digits10 + 2;
        constexpr static inline auto char_size = sizeof(char_type);
        const static inline auto thread_count = (std::thread::hardware_concurrency() == 0) ? 1 : std::thread::hardware_concurrency();

        typedef std::tuple<number_t, number_t, number_t, number_t, uint32_t, uint32_t> export_t;
        using string = std::basic_string<char_type>;
        using string_view = std::basic_string_view<char_type>;

    protected:
        constexpr static inline uint32_t DEFAULT_BITS = static_cast<uint32_t>(4096);
        constexpr static inline uint32_t DEFAULT_TRYS = static_cast<uint32_t>(-1);
        constexpr static inline uint32_t MIN_BLOCKSIZE = static_cast<uint32_t>(one_in_block_size);

    private:
        bool m_setupdone = false;
        uint32_t m_bits = DEFAULT_BITS;
        uint32_t m_trys = DEFAULT_TRYS;

    public:
        uint32_t e = 0;
        number_t p, q, n, d;

        std::tuple<uint32_t&, number_t&> public_key{ e, n };
        std::tuple<number_t&, number_t&> private_key{ d, n };

    public:
        constexpr basic_rsa() :m_bits(3072), m_trys(17) {

        }

        constexpr basic_rsa(const uint32_t _bits, const uint32_t _trys = DEFAULT_TRYS) {
            set(_bits, _trys);
        }

        // Import values from exported RSA file
        constexpr basic_rsa(const std::string_view& _file) {
            import_file(_file);
        }

        // Import values from exported RSA file
        constexpr basic_rsa(std::ifstream& _file) {
            import_file(_file);
        }

        // Import values from other RSA class
        constexpr basic_rsa(const export_t& _key) {
            import_key(_key);
        }

        // Check if setup() is already done
        _NODISCARD constexpr operator bool() const noexcept
        {
            return m_setupdone;
        }

        // Check if setup() is already done
        _NODISCARD constexpr bool setupdone() const noexcept
        {
            return m_setupdone;
        }

        // The bits of the generated primes
        _NODISCARD constexpr uint32_t keysize() const noexcept
        {
            return m_bits;
        }

        // The current amouth of Miller Rabin tests
        _NODISCARD constexpr uint32_t trys() const noexcept
        {
            return m_trys;
        }

        // How likely the numbers generated are prime in percent
        _NODISCARD constexpr double_t precision() const noexcept
        {
            return 100.0 - (std::pow(2, static_cast<int64_t>(m_trys) * -1) * 100.0);
        }

        // Set new keysize, blocksize and trys
        constexpr void set(uint32_t _bits = DEFAULT_BITS, uint32_t _trys = DEFAULT_TRYS) noexcept(!throw_errors)
        {
            if (_bits == 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("key size can`t be 0");
                }

                _bits = DEFAULT_BITS;
            }

            if (_trys == 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("can`t have 0 trys");
                }

                _trys = DEFAULT_TRYS;
            }

            if (_bits % 8 != 0) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("keysize has to be dividable by 8");
                }

                _bits = (8 - _bits % 8);
            }

            m_bits = _bits;

            if (_trys == DEFAULT_TRYS) {
                m_trys = get_rabin_trys(_bits);
            }
            else {
                m_trys = _trys;
            }
        }

        // Set the size of the key
        constexpr void set_keysize(const uint32_t _bits)
        {
            set(_bits, m_trys);
        }

        // Set how many Miller Rabin tests to perform
        constexpr void set_trys(const uint32_t _trys)
        {
            set(m_bits, _trys);
        }

        // Import file from a filepath
        bool import_file(const std::string_view& _filename)
        {
            std::ifstream _file(_filename.data());
            return import_file(_file);
        }

        // Import file from an ifstream
        bool import_file(std::ifstream& _file)
        {
            if (_file.good() == false) {
                return false;
            }

            std::string _line;

            do
            {
                if (!std::getline(_file, _line)) {
                    _file.close();
                    return false;
                }
            } while (_line != "-----BEGIN EXPORT-----");

            if (std::getline(_file, _line))
                p.assign(_line);

            if (std::getline(_file, _line))
                q.assign(_line);

            if (std::getline(_file, _line))
                n.assign(_line);

            if (std::getline(_file, _line))
                d.assign(_line);

            if (std::getline(_file, _line))
                e = detail::dton<uint32_t>(_line.c_str());

            if (std::getline(_file, _line))
                m_bits = detail::dton<uint32_t>(_line.c_str());

            _file.close();

            set(m_bits);

            m_setupdone = true;

            return true;
        }

        // Import setup from other RSA class export
        void import_key(const export_t& _key)
        {
            p = std::get<0>(_key);
            q = std::get<1>(_key);
            n = std::get<2>(_key);
            d = std::get<3>(_key);
            e = std::get<4>(_key);
            m_bits = std::get<5>(_key);

            set(m_bits);

            m_setupdone = true;
        }

        // Export current setup to a filepath
        bool export_file(const std::string_view& _filename)
        {
            std::ofstream _file(_filename.data());
            return export_file(_file);
        }

        // Export current setup to an ostream
        bool export_file(std::ofstream& _file)
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you try to export a key.");
            }

            if (_file.good() == false) {
                return false;
            }

            _file << "-----BEGIN EXPORT-----\n";

            _file << p.str() << '\n';
            _file << q.str() << '\n';
            _file << n.str() << '\n';
            _file << d.str() << '\n';
            _file << std::to_string(e) << '\n';
            _file << std::to_string(m_bits) << '\n';

            _file << "-----END EXPORT-----\n";

            _file.close();

            return true;
        }

        // Export current setup
        _NODISCARD const export_t export_key()
        {
            return { p, q, n, d, e, m_bits };
        }

        // Generate the primes
        void setup() noexcept
        {
            m_setupdone = false;

            while (true)
            {
                const auto pair = generate_prime_pair(m_bits, m_trys);

                p = pair.first;
                q = pair.second;

                n = p * q;

                const number_t phi = (p - 1) * (q - 1);

                e = 65537;

                if (gcd(e, phi) != 1) {
                    e = 2;
                    while (gcd(e, phi) != 1) {
                        ++e;
                    }
                }

                d = inverse_mod(e, phi);

                if ((e * d) % phi == 1) {
                    break;
                }
            }

            m_setupdone = true;
        }

        // Encrypts a string
        _NODISCARD std::vector<number_t> encrypt(const string_view& _str, const std::tuple<uint32_t&, number_t&>& _public_key)
        {
            check_setup();

            if (_str.empty()) {
                if constexpr (throw_errors) {
                    throw std::invalid_argument("Can`t Encrypt empty string");
                }
                return { };
            }

            const number_t e = std::get<0>(_public_key);
            const number_t& n = std::get<1>(_public_key);
            auto blocks = create_blocks(_str, n, m_bits);
            std::vector<std::future<void>> threads;

            for (auto& block : blocks) {
                threads.push_back(std::async(std::launch::async, mpz_powm, block.get_mpz_t(), block.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t()));
            }

            for (const auto& thread : threads) {
                thread.wait();
            }

            return blocks;
        }

        // Decrypts the encrypted blocks
        _NODISCARD string decrypt(std::vector<number_t>& _encrypted) const
        {
            check_setup();

            auto decrypt_block = [this](number_t& _block) noexcept -> string
            {
                mpz_powm(_block.get_mpz_t(), _block.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());

                const std::string decrypted = _block.get_str();

                string result; result.reserve(decrypted.size());

                for (uint32_t i = 0; decrypted.at(i) != '0' || i > decrypted.size();)
                {
                    uint8_t _len = decrypted.at(i++) - '0';

                    if (decrypted.at(i) == '0' && _len > 1) {
                        const auto _chr = decrypted.substr(++i, --_len);
                        const auto _dec = detail::dton<char_type>(_chr.data()) * (-1);
                        result.push_back(_dec);
                    }
                    else {
                        const auto _chr = decrypted.substr(i, _len);
                        const auto _dec = detail::dton<char_type>(_chr.data());
                        result.push_back(_dec);
                    }

                    i += _len;
                }

                return result;
            };

            std::vector<std::future<string>> threads;

            for (auto& block : _encrypted) {
                threads.push_back(std::async(std::launch::async, decrypt_block, std::ref(block)));
            }

            string decrypted;

            for (auto& thread : threads) {
                decrypted.append(thread.get());
            }

            return decrypted;
        }

    private:
        // Check if setup() has been called
        void __forceinline check_setup() const
        {
            if (m_setupdone == false) {
                throw std::exception("You have to call setup() before you use the class");
            }
        }

        // Creates the blocks for encryption
        _NODISCARD static std::vector<number_t> create_blocks(const string_view& _str, const number_t& _n, const uint32_t _keysize) noexcept
        {
            uint32_t blocksize = std::max(_keysize, MIN_BLOCKSIZE);

            std::vector<number_t> blocks;
            std::string msg; msg.reserve(blocksize * char_size);
            uint32_t i = 0;

            auto check_for_n = [_n, &blocksize](const number_t& _check) noexcept -> bool
            {
                if (_n < _check) {
                    blocksize -= one_in_block_size;
                    return false;
                }
                else {
                    return true;
                }
            };

            auto push_block = [&msg, &blocks, &i, check_for_n]() noexcept -> bool
            {
                number_t block(msg);

                const bool valid = check_for_n(block);

                if (valid) {
                    blocks.push_back(block);
                }
                else {
                    i = 0;
                    blocks.clear();
                }

                msg.clear();

                return valid;
            };

            while (true)
            {
                const std::string    num = std::to_string(_str.at(i));
                const bool      negative = (num.front() == '-');
                const uint32_t  nextsize = msg.size() + num.size() + static_cast<int32_t>(negative) + 1;

                if (nextsize > blocksize) {
                    msg.push_back('0');
                    if (!push_block())
                        continue;
                }

                msg.append(std::to_string(num.size()));

                if (negative) {
                    msg.push_back('0');
                    msg.append(num.substr(1));
                }
                else {
                    msg.append(num);
                }

                if (++i == _str.size()) {
                    msg.push_back('0');
                    if (!push_block()) {
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }

            return blocks;
        }

        // Calculate the extended Euclidean algorithm
        _NODISCARD static number_t egcd(const number_t& a, const number_t& b, number_t& x, number_t& y) noexcept
        {
            if (a == 0) {
                x = 0;
                y = 1;
                return b;
            }

            number_t x1, y1;
            number_t gcd = egcd(b % a, a, x1, y1);

            x = y1 - (b / a) * x1;
            y = x1;

            return gcd;
        };

        // Calculate modular multiplicative inverse of e and phi
        _NODISCARD static number_t inverse_mod(const number_t& e, const number_t& phi) noexcept
        {
            number_t x = 0, y = 0;
            if (egcd(e, phi, x, y) == 1) {
                return ((x % phi) + phi) % phi;
            }
            return 0;
        }

        // Generate random primelike string with X bits
        _NODISCARD static std::string random_primish_number(const uint32_t _bits) noexcept
        {
            std::mt19937_64 gen(std::random_device{ }());
            std::uniform_int_distribution<int32_t> dist('0', '9');

            const uint32_t bytes = _bits / 8;

            std::string str(bytes, 0);

            for (char& ch : str) {
                ch = static_cast<char>(dist(gen));
            }

            while (str.front() == '0') {
                str.front() = static_cast<char>(dist(gen));
            }

            while (str.back() != '1' && str.back() != '3' && str.back() != '7' && str.back() != '9') {
                str.back() = static_cast<char>(dist(gen));
            }

            return str;
        }

        // Generate 2 primes p and q
        _NODISCARD static std::pair<number_t, number_t> generate_prime_pair(const uint32_t _bits, const uint32_t _trys) noexcept
        {
            auto search_thread = [_trys](const uint32_t bits, number_t& X, std::mutex& lock) noexcept -> void
            {
                number_t possible_prime;

                while (X == 0)
                {
                    possible_prime.set_str(random_primish_number(bits), 10);

                    if (is_prime(possible_prime, _trys))
                    {
                        lock.lock();
                        {
                            if (X == 0)
                                X = possible_prime;
                        }
                        lock.unlock();

                        return;
                    }
                }
            };

            static const uint32_t threads_per = thread_count / 2;

            number_t p = 0, q = 0;

            std::mt19937_64 mt(std::random_device{ }());
            std::uniform_int_distribution<int32_t> dist(((_bits / 64) * -1) - 8, (_bits / 64) + 8);

            const int32_t _SP = _bits + dist(mt), _SQ = _bits + dist(mt);

            std::mutex p_lock;
            std::mutex q_lock;

            std::vector<std::future<void>> threads;

            for (uint32_t i = 0; i < threads_per; ++i) {
                threads.push_back(std::async(std::launch::async, search_thread, _SP, std::ref(p), std::ref(p_lock)));
                threads.push_back(std::async(std::launch::async, search_thread, _SQ, std::ref(q), std::ref(q_lock)));
            }

            for (const auto& thread : threads) {
                thread.wait();
            }

            return std::make_pair(std::move(p), std::move(q));
        }

        // Check if X number is prime with Y Miller Rabin tests
        _NODISCARD static bool is_prime(const number_t& _num, const uint32_t _trys) noexcept
        {
            if (_num == 2 || _num == 3)
                return true;

            if (_num <= 1 || _num % 2 == 0 || _num % 3 == 0)
                return false;

            for (uint32_t i = 5; i < 1000; i += 6) {
                if (_num % i == 0 || _num % (i + 2) == 0)
                    return false;
            }

            const bool res = mpz_millerrabin(_num.get_mpz_t(), _trys);

            return res;
        }

        // Get a good amouth of Miller Rabin test for X bits
        _NODISCARD static uint32_t get_rabin_trys(const uint32_t _bits) noexcept
        {
            return static_cast<uint32_t>(
                _bits <= 1024 ? 16 :
                _bits <= 2048 ? 32 :
                _bits <= 4096 ? 64 : 96
                );
        }

        // Output for RSA class
        template<class _char, bool _throw, class ostream = std::ostream>
        friend ostream& operator<<(ostream& out, const basic_rsa<_char, _throw>& rsa)
        {
            out << "Charset  : " << typeid(_char).name() << '\n';
            out << "Keysize  : " << rsa.keysize() << '\n';
            out << "Trys     : " << rsa.trys() << '\n';
            out << "Precision: " << rsa.precision() << '%' << '\n';
            out << "Setupdone: " << (rsa.setupdone() == true ? "true" : "false") << "\n\n";
            out << "P: " << rsa.p << "\n\n";
            out << "Q: " << rsa.q << "\n\n";
            out << "N: " << rsa.n << "\n\n";
            out << "E: " << rsa.e << "\n\n";
            out << "D: " << rsa.d << "\n\n";
            return out;
        }
    };
}

#endif
