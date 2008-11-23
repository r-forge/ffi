#ifndef _h_common_
#define _h_common_

#include <stdlib.h>
#include <Rinternals.h>
#include <fficonfig.h>
#include <ffi.h>

extern SEXP FFI_TypeSymbol;
extern SEXP FFI_TypeTag;


extern SEXP FFI_TypeVoid;
extern SEXP FFI_TypeFloat;
extern SEXP FFI_TypeDouble;
#ifdef HAVE_LONG_DOUBLE
extern SEXP FFI_TypeLongDouble;
#endif
extern SEXP FFI_TypeSigned8;
extern SEXP FFI_TypeSigned16;
extern SEXP FFI_TypeSigned32;
extern SEXP FFI_TypeSigned64;
extern SEXP FFI_TypeUnsigned8;
extern SEXP FFI_TypeUnsigned16;
extern SEXP FFI_TypeUnsigned32;
extern SEXP FFI_TypeUnsigned64;
extern SEXP FFI_TypePointer;

typedef union {
  ffi_sarg       sint;
  ffi_arg        uint;
  signed short   ssht;
  unsigned short usht;
  signed char    schr;
  unsigned char  uchr;
  signed long    slng;
  unsigned long  ulng;
  float          flt;
  double         dbl;
#ifdef HAVE_LONG_DOUBLE
  long double    ldbl;
#endif 
  void*          ptr;
} ret_value;

/* coerce.c */
SEXP FFI_asType(SEXP,SEXP);
SEXP FFI_fromCStringType(SEXP);
SEXP FFI_fromPtr(SEXP,SEXP,SEXP);
SEXP FFI_fromType(SEXP,SEXP);
SEXP ffi_fromType(SEXP,unsigned short,ret_value*);
void* ffi_make_arg(SEXP,unsigned short,SEXP);
void ffi_make_ret(SEXP,unsigned short,SEXP,ret_value*);

/* args.c */
SEXP      FFI_asTypeCode(SEXP);
void      ffi_PrintTypeCode(SEXP);
SEXP      FFI_TypeCodePrint(SEXP);
SEXP      FFI_TypeCodePointer(SEXP);
SEXP      FFI_TypeCodeDesc(SEXP);

/* call.c */
void ffi_PrintCIF(SEXP);
SEXP FFI_makeCIF(SEXP,SEXP);
SEXP FFI_SummaryCIF(SEXP);
SEXP FFI_Call(SEXP,SEXP,SEXP);

/* closure.c */
SEXP FFI_MakeClosure(SEXP,SEXP);


/* Type Macros */
#define TYPE_FFI(X)   ((ffi_type*)R_ExternalPtrAddr(X))
#define TYPE_DESC(X)  (R_ExternalPtrProtected(X))
#define TYPE_RET(X)   VECTOR_ELT(R_ExternalPtrProtected((X)),0)
#define TYPE_ELT(X,I) VECTOR_ELT(VECTOR_ELT(R_ExternalPtrProtected((X)),1),I)


#endif
