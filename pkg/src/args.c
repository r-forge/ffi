#include <Rdefines.h>
#include "common.h"
#include <ffi.h>


void ffi_PrintTypeCode(SEXP typecode) {
  int i;
  ffi_type *typ = TYPE_FFI(typecode);
  SEXP desc;
  switch(typ->type) {
  case FFI_TYPE_VOID:      Rprintf("void");break;
  case FFI_TYPE_FLOAT:     Rprintf("float");break;
  case FFI_TYPE_DOUBLE:    Rprintf("double");break;
#ifdef HAVE_LONG_DOUBLE
  case FFI_TYPE_LONGDOUBLE:Rprintf("long double");break;
#endif 
  case FFI_TYPE_INT:
  case FFI_TYPE_SINT32:    Rprintf("sint32");break;
  case FFI_TYPE_UINT32:    Rprintf("uint32");break;
  case FFI_TYPE_SINT8:     Rprintf("sint8");break;
  case FFI_TYPE_UINT8:     Rprintf("uint8");break;
  case FFI_TYPE_SINT16:    Rprintf("sint16");break;
  case FFI_TYPE_UINT16:    Rprintf("uint16");break;
  case FFI_TYPE_UINT64:    Rprintf("sint64");break;
  case FFI_TYPE_SINT64:    Rprintf("uint64");break;
  case FFI_TYPE_POINTER:   
    desc = TYPE_DESC(typecode);
    if(desc == R_NilValue || desc == NULL) Rprintf("void"); else ffi_PrintTypeCode(desc);
    Rprintf("*");
    break;
  case FFI_TYPE_STRUCT:
    desc = TYPE_DESC(typecode);
    Rprintf("{");
    for(i=0;i<length(desc);i++) {
      ffi_PrintTypeCode(VECTOR_ELT(desc,i));
      if(i+1<length(desc)) Rprintf(",");
    }
    Rprintf("}");
    break;
  }
}

static SEXP IntegerAsType(int i) {
  switch(i) {
  case FFI_TYPE_VOID:return FFI_TypeVoid;
  case FFI_TYPE_FLOAT:return FFI_TypeFloat;
  case FFI_TYPE_DOUBLE:return FFI_TypeDouble;
#ifdef HAVE_LONG_DOUBLE
  case FFI_TYPE_LONGDOUBLE:return FFI_TypeLongDouble;
#endif
  case FFI_TYPE_INT:
  case FFI_TYPE_SINT32:return FFI_TypeSigned32;
  case FFI_TYPE_UINT32:return FFI_TypeUnsigned32;
  case FFI_TYPE_SINT8:return FFI_TypeSigned8;
  case FFI_TYPE_UINT8:return FFI_TypeUnsigned8;
  case FFI_TYPE_SINT16:return FFI_TypeSigned16;
  case FFI_TYPE_UINT16:return FFI_TypeUnsigned16;
  case FFI_TYPE_UINT64:return FFI_TypeUnsigned64;
  case FFI_TYPE_SINT64:return FFI_TypeSigned64;
  case FFI_TYPE_POINTER:return FFI_TypePointer;
  }
  error("unable to convert integer to type");
}

static SEXP StringAsType(char *s) {
  switch(*s) {
  case 'v':case 'V':return FFI_TypeVoid;
  case 'f':return FFI_TypeFloat;
  case 'd':return FFI_TypeDouble;
#ifdef HAVE_LONG_DOUBLE
  case 'D':return FFI_TypeLongDouble;
#else
  case 'D':error("long double not supported");return R_NilValue;
#endif
  case '*':case 'p':case '^':return FFI_TypePointer;
  case 'i':return FFI_TypeSigned32;
  case 'I':return FFI_TypeUnsigned32;
  case 'l':return FFI_TypeSigned64;
  case 'L':return FFI_TypeUnsigned64;
  case 'c':return FFI_TypeSigned8;
  case 'C':return FFI_TypeUnsigned8;
  case 'h':return FFI_TypeSigned16;
  case 'H':return FFI_TypeUnsigned16;
  case 's':case 'S':
    s++;
    switch(atoi(s)) {
    case 8:return FFI_TypeSigned8;
    case 16:return FFI_TypeSigned16;
    case 32:return FFI_TypeSigned32;
    case 64:return FFI_TypeSigned64;
    }
    break;
  case 'u':case 'U':
    s++;
    switch(atoi(s)) {
    case 8:return FFI_TypeUnsigned8;
    case 16:return FFI_TypeUnsigned16;
    case 32:return FFI_TypeUnsigned32;
    case 64:return FFI_TypeUnsigned64;
    }
    break;
  }
  error("Unable to convert %s to a type",s);
  return R_NilValue;		/* -Wall */
}

static SEXP ScalarAsType(SEXP s) {
  if(length(s)>1)
    warning("Only first element is used for type information");
  switch(TYPEOF(s)) {
  case INTSXP: return IntegerAsType(INTEGER(s)[0]);
  case REALSXP:return IntegerAsType(REAL(s)[0]);
  case STRSXP: return StringAsType(CHAR(STRING_ELT(s,0)));
  case NILSXP: return FFI_TypeVoid;
  default:
    error("cannot convert to Type");
  }
  return R_NilValue;
}

extern int align(int,int);

static SEXP VectorAsType(SEXP v) {
  SEXP      s,r;
  ffi_type *f;
  int       i;
  PROTECT(s = allocVector(VECSXP,length(v)));
  PROTECT(r = R_MakeExternalPtr(malloc(sizeof(ffi_type)+length(v)*sizeof(ffi_type*)),
				FFI_TypeTag,s));
  /* Should register finalizer */
  f = (ffi_type*)R_ExternalPtrAddr(r);
  f->elements = (ffi_type**)(f+1);
  f->type     = FFI_TYPE_STRUCT;
  f->alignment= sizeof(ffi_sarg); /* Need to align on boundary */
  f->size     = 0;
  for(i=0;i<length(v);i++) {
    SEXP m = FFI_asTypeCode(VECTOR_ELT(v,i));
    SET_VECTOR_ELT(s,i,m);
    f->elements[i] = TYPE_FFI(m);
    f->size = align(f->size,f->elements[i]->alignment)+f->elements[i]->size;
  }
  UNPROTECT(2);
  return r;
}

SEXP FFI_asTypeCode(SEXP s) {
  /* No need to convert the already converted */
  if(TYPEOF(s)==EXTPTRSXP &&
     R_ExternalPtrTag(s) == FFI_TypeTag) return s;
  if(TYPEOF(s)==VECSXP) return VectorAsType(s);
  return ScalarAsType(s);
}

SEXP FFI_TypeCodePrint(SEXP s) {
  ffi_PrintTypeCode(s);Rprintf("\n");
  return R_NilValue;
}

SEXP FFI_TypeCodePointer(SEXP s) {
  /* Make sure its a typecode */
  s = FFI_asTypeCode(s);
  return R_MakeExternalPtr(&ffi_type_pointer,FFI_TypeTag,s);
}

SEXP FFI_TypeCodeDesc(SEXP s) {
  ffi_type *typ = TYPE_FFI(s);
  if(typ && typ->type == FFI_TYPE_STRUCT)
    return TYPE_DESC(s);
}
