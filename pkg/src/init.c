#include <R.h>
#include "common.h"
#include <R_ext/Rdynload.h>
#include "ffi.h"

#define CALLFN(name,n) {#name,(DL_FUNC)&name,n}

SEXP FFI_TypeSymbol;
SEXP FFI_TypeTag;

/* Basic Atomic Types */
SEXP FFI_TypeVoid;
SEXP FFI_TypeFloat;
SEXP FFI_TypeDouble;
#ifdef HAVE_LONG_DOUBLE
SEXP FFI_TypeLongDouble;
#endif
SEXP FFI_TypeSigned8;
SEXP FFI_TypeSigned16;
SEXP FFI_TypeSigned32;
SEXP FFI_TypeSigned64;
SEXP FFI_TypeUnsigned8;
SEXP FFI_TypeUnsigned16;
SEXP FFI_TypeUnsigned32;
SEXP FFI_TypeUnsigned64;
SEXP FFI_TypePointer;

#define ATOMIC_TYPE(X) R_MakeExternalPtr(&(X),FFI_TypeTag,R_NilValue)


static R_CallMethodDef CallEntries[] = {
  CALLFN(FFI_asType,2),
  CALLFN(FFI_Call,3),
  CALLFN(FFI_asTypeCode,1),
  CALLFN(FFI_makeCIF,2),
  CALLFN(FFI_SummaryCIF,1),
  CALLFN(FFI_MakeClosure,2),
  CALLFN(FFI_fromCStringType,1),
  CALLFN(FFI_fromPtr,3),
  CALLFN(FFI_fromType,2),
  CALLFN(FFI_TypeCodePrint,1),
  CALLFN(FFI_TypeCodePointer,1),
  CALLFN(FFI_TypeCodeDesc,1),
  {NULL,NULL,0}
};

void R_init_ffi(DllInfo *dll) {
  R_registerRoutines(dll,NULL,CallEntries,NULL,NULL);
  R_useDynamicSymbols(dll,FALSE);

  FFI_TypeSymbol = install("ffi.type");
  FFI_TypeTag    = install("FFI_TYPE_TAG");
  
  FFI_TypeVoid       = ATOMIC_TYPE(ffi_type_void);
  FFI_TypeFloat      = ATOMIC_TYPE(ffi_type_float);
  FFI_TypeDouble     = ATOMIC_TYPE(ffi_type_double);
#ifdef HAVE_LONG_DOUBLE
  FFI_TypeLongDouble = ATOMIC_TYPE(ffi_type_longdouble);
#endif 
  FFI_TypeSigned8    = ATOMIC_TYPE(ffi_type_sint8);
  FFI_TypeSigned16   = ATOMIC_TYPE(ffi_type_sint16);
  FFI_TypeSigned32   = ATOMIC_TYPE(ffi_type_sint32);
  FFI_TypeSigned64   = ATOMIC_TYPE(ffi_type_sint64);
  FFI_TypeUnsigned8  = ATOMIC_TYPE(ffi_type_uint8);
  FFI_TypeUnsigned16 = ATOMIC_TYPE(ffi_type_uint16);
  FFI_TypeUnsigned32 = ATOMIC_TYPE(ffi_type_uint32);
  FFI_TypeUnsigned64 = ATOMIC_TYPE(ffi_type_uint64);
  FFI_TypePointer    = ATOMIC_TYPE(ffi_type_pointer);
}

