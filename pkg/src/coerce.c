#include <Rdefines.h>
#include "common.h"
#include <ffi.h>

int align(int off,int a) {
  int r=off%a;
  if(r) off += a-r;
  return off;
}

void* ffi_dataptr(SEXP arg) {
	void* ptr;
	switch(TYPEOF(arg)) {
		case EXTPTRSXP:ptr = R_ExternalPtrAddr(arg);break;
		case RAWSXP:ptr    = RAW(arg);break;
		case NILSXP:ptr    = NULL;break;
		case STRSXP:ptr    = CHAR(STRING_ELT(arg,0));break;
		case INTSXP:ptr    = INTEGER(arg);break;
		case REALSXP:ptr   = REAL(arg);break;
		case LGLSXP:ptr    = LOGICAL(arg);break;
	}	
	return ptr;
}

SEXP FFI_asStructType(SEXP argType,SEXP sxp) {
  SEXP out;int i,off;
  SEXP desc = TYPE_DESC(argType);
  ffi_type *typ = TYPE_FFI(argType);
  PROTECT(out = allocVector(RAWSXP,typ->size));
  memset(RAW(out),0,typ->size);

  off = 0;
  for(i=0;i<length(desc);i++) {
    SEXP local = FFI_asType(VECTOR_ELT(desc,i),VECTOR_ELT(sxp,i));
    off = align(off,typ->elements[i]->alignment);
    memcpy(&(RAW(out)[off]),ffi_dataptr(local),typ->elements[i]->size);
    off += typ->elements[i]->size;
  }
  UNPROTECT(1);
  return out;
}
SEXP FFI_fromStructType(SEXP argType,SEXP sxp,int n) {
  error("not yet implemented");
  return R_NilValue;
}

#define ALLOC_RAW(TYPE) PROTECT(raw = allocVector(RAWSXP,sizeof(TYPE)*length(sxp)))


SEXP FFI_asFloatType(SEXP sxp) {
  SEXP raw;int i;float *f;
  PROTECT(sxp = coerceVector(sxp,REALSXP));
  ALLOC_RAW(float);
  f   = (float*)RAW(raw);
  for(i=0;i<length(sxp);i++) f[i] = REAL(sxp)[i];
  UNPROTECT(2);
  return raw;
}
SEXP FFI_fromFloatType(SEXP sxp,int n) {
  SEXP out;int i;
  float *f = (float*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(REALSXP,n));
  for(i=0;i<n;i++) REAL(out)[i] = f[i];
  UNPROTECT(1);
  return out;
}

SEXP FFI_fromDoubleType(SEXP sxp,int n) {
  SEXP out;
  double *f = (double*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(REALSXP,n));
  memcpy(REAL(out),f,sizeof(double)*n);
  UNPROTECT(1);
  return out;
}

#ifdef HAVE_LONG_DOUBLE
SEXP FFI_asLongDoubleType(SEXP sxp) {
  SEXP raw;int i;long double *f;
  PROTECT(sxp = coerceVector(sxp,REALSXP));
  ALLOC_RAW(long double);
  f   = (long double*)RAW(raw);
  for(i=0;i<length(sxp);i++) f[i] = REAL(sxp)[i];
  UNPROTECT(2);
  return raw;
}
SEXP FFI_fromLongDoubleType(SEXP sxp,int n) {
  SEXP out;int i;
  long double *f = (long double*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(REALSXP,n));
  for(i=0;i<n;i++) REAL(out)[i] = f[i];
  UNPROTECT(1);
  return out;
}
#endif

SEXP FFI_asCharType(SEXP sxp) {
  SEXP raw;int i;
  if(TYPEOF(sxp) == STRSXP) {
    char *str = CHAR(STRING_ELT(sxp,0));
    PROTECT(raw = allocVector(RAWSXP,strlen(str)+1));
    memcpy(RAW(raw),str,sizeof(char)*(strlen(str)+1));
    UNPROTECT(1);
    return raw;
  } else {
    PROTECT(sxp = coerceVector(sxp,INTSXP));
    ALLOC_RAW(char);
    for(i=0;i<length(sxp);i++) RAW(raw)[i] = INTEGER(sxp)[i];
    UNPROTECT(2);
    return raw;
  }
}
SEXP FFI_fromCharType(SEXP sxp,int n) {
  SEXP out;int i;
  char *f = (char*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(RAWSXP,n));
  for(i=0;i<n;i++) RAW(out)[i] = f[i];
  UNPROTECT(1);
  return out;
}
SEXP FFI_fromCStringType(SEXP sxp) {
  SEXP out;
  char *str = (char*)R_ExternalPtrAddr(sxp);
  return ScalarString(mkChar(str));
}

SEXP FFI_asShortType(SEXP sxp) {
  SEXP raw;int i;short *s;
  PROTECT(sxp = coerceVector(sxp,INTSXP));
  ALLOC_RAW(short);
  s = (short*)RAW(sxp);
  for(i=0;i<length(sxp);i++) s[i] = INTEGER(sxp)[i];
  UNPROTECT(2);
  return raw;
}
SEXP FFI_fromShortType(SEXP sxp,int n) {
  SEXP out;int i;
  short *f = (short*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(INTSXP,n));
  for(i=0;i<n;i++) INTEGER(out)[i] = f[i];
  UNPROTECT(1);
  return out;  
}

SEXP FFI_fromIntType(SEXP sxp,int n) {
  SEXP out;
  int *i = (int*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(INTSXP,n));
  memcpy(INTEGER(out),i,sizeof(int)*n);
  UNPROTECT(1);
  return out;
}


SEXP FFI_asLongType(SEXP sxp) {
  SEXP raw;int i;long *l;
  PROTECT(sxp = coerceVector(sxp,INTSXP));
  PROTECT(raw = allocVector(RAWSXP,sizeof(long)*length(sxp)));
  l = (long*)RAW(sxp);
  for(i=0;i<length(sxp);i++) l[i] = INTEGER(sxp)[i];
  UNPROTECT(2);
  return raw;
}
SEXP FFI_fromLongType(SEXP sxp,int n) {
  SEXP out;int i;
  long *f = (long*)R_ExternalPtrAddr(sxp);
  PROTECT(out = allocVector(INTSXP,n));
  for(i=0;i<n;i++) INTEGER(out)[i] = f[i];
  UNPROTECT(1);
  return out;  
}

SEXP FFI_asPointerType(SEXP sxp) {
  return (TYPEOF(sxp)==CLOSXP)?FFI_MakeClosure(sxp,R_NilValue):sxp;
}

SEXP FFI_ScalarStruct(SEXP type,char *val) {
  SEXP ret,desc = TYPE_DESC(type);
  ffi_type *typ = TYPE_FFI(type);
  int i,off;
  PROTECT(ret = allocVector(VECSXP,length(desc)));
  off = 0;
  for(i=0;i<length(desc);i++) {
    SEXP      local = VECTOR_ELT(desc,i);
    off = align(off,typ->elements[i]->alignment);
    SET_VECTOR_ELT(ret,i,ffi_fromType(local,typ->elements[i]->type,(ret_value*)&val[off]));
    off += typ->elements[i]->size;
  }
  UNPROTECT(1);
  return ret;
}

SEXP ffi_fromType(SEXP typ,unsigned short argType,ret_value *retval) {
  switch(argType) {
  case FFI_TYPE_VOID:return R_NilValue;
  case FFI_TYPE_FLOAT:return ScalarReal(retval->flt);
  case FFI_TYPE_DOUBLE:return ScalarReal(retval->dbl);
#ifdef HAVE_LONG_DOUBLE
  case FFI_TYPE_LONGDOUBLE:return ScalarReal(retval->dbl);
#endif HAVE_LONG_DOUBLE
  case FFI_TYPE_INT:
  case FFI_TYPE_SINT32:return ScalarInteger(retval->sint);
  case FFI_TYPE_UINT32:return ScalarInteger(retval->uint);
  case FFI_TYPE_SINT8:return ScalarInteger(retval->schr);
  case FFI_TYPE_UINT8:return ScalarInteger(retval->uchr);
  case FFI_TYPE_SINT16:return ScalarInteger(retval->ssht);
  case FFI_TYPE_UINT16:return ScalarInteger(retval->usht);
  case FFI_TYPE_UINT64:return ScalarInteger(retval->slng);
  case FFI_TYPE_SINT64:return ScalarInteger(retval->ulng);
  case FFI_TYPE_STRUCT:return FFI_ScalarStruct(typ,(char*)retval);
  case FFI_TYPE_POINTER:
    return R_MakeExternalPtr(retval->ptr,R_NilValue,R_NilValue);
  default:
    error("unsupported type %d",argType);
  }
}



SEXP FFI_asType(SEXP typeCode,SEXP sxp) {
  if(TYPEOF(typeCode) != EXTPTRSXP ||
     R_ExternalPtrTag(typeCode) != FFI_TypeTag)
    error("typeCode is not a type");
  switch(TYPE_FFI(typeCode)->type) {
  case FFI_TYPE_VOID:return R_NilValue;
  case FFI_TYPE_FLOAT:return FFI_asFloatType(sxp);
  case FFI_TYPE_DOUBLE:return coerceVector(sxp,REALSXP);
#ifdef HAVE_LONG_DOUBLE
  case FFI_TYPE_LONGDOUBLE:return FFI_asLongDoubleType(sxp);
#endif HAVE_LONG_DOUBLE
  case FFI_TYPE_INT:
  case FFI_TYPE_UINT32:
  case FFI_TYPE_SINT32:return coerceVector(sxp,INTSXP);
  case FFI_TYPE_SINT8:
  case FFI_TYPE_UINT8:return FFI_asCharType(sxp);
  case FFI_TYPE_SINT16:
  case FFI_TYPE_UINT16:return FFI_asShortType(sxp);
  case FFI_TYPE_UINT64:
  case FFI_TYPE_SINT64:return FFI_asLongType(sxp);
  case FFI_TYPE_POINTER:return FFI_asPointerType(sxp);
  case FFI_TYPE_STRUCT:return FFI_asStructType(typeCode,sxp);
  default:
    error("unsupported type");
  }
  return R_NilValue;
}
SEXP FFI_fromType(SEXP typeCode,SEXP sxp) {
  if(TYPEOF(typeCode) != EXTPTRSXP || 
     R_ExternalPtrTag(typeCode) != FFI_TypeTag)
    error("typeCode is not a type");
  char* dataptr = CHAR(sxp);
  switch(TYPE_FFI(typeCode)->type) {
  case FFI_TYPE_STRUCT:return FFI_ScalarStruct(typeCode,dataptr);
  case FFI_TYPE_POINTER:
    return R_MakeExternalPtr(((void**)dataptr)[0],R_NilValue,R_NilValue);
  }
}

SEXP FFI_fromPtr(SEXP argType,SEXP sxp,SEXP N) {
  if(TYPEOF(sxp) != EXTPTRSXP)
    error("object to convert must be an external pointer");
  int n = asInteger(N);
  if(length(argType) > 1 || TYPEOF(argType) == VECSXP)
    return FFI_fromStructType(argType,sxp,n);
  switch(asInteger(argType)) {
  case FFI_TYPE_FLOAT:return FFI_fromFloatType(sxp,n);
  case FFI_TYPE_DOUBLE:return FFI_fromDoubleType(sxp,n);
#ifdef HAVE_LONG_DOUBLE
  case FFI_TYPE_LONGDOUBLE:return FFI_fromLongDoubleType(sxp,n);
#endif
  case FFI_TYPE_INT:
  case FFI_TYPE_UINT32:
  case FFI_TYPE_SINT32:return FFI_fromIntType(sxp,n);
  case FFI_TYPE_SINT8:
  case FFI_TYPE_UINT8:return FFI_fromCharType(sxp,n);
  case FFI_TYPE_SINT16:
  case FFI_TYPE_UINT16:return FFI_fromShortType(sxp,n);
  case FFI_TYPE_SINT64:
  case FFI_TYPE_UINT64:return FFI_fromLongType(sxp,n);
    /* Passthrough type */
  case FFI_TYPE_POINTER:return (SEXP)R_ExternalPtrAddr(sxp);
  }
}

void *ffi_make_ptr(SEXP typ,SEXP arg) {
	void*ptr;
	/* We have to be somewhat more careful these days as R enforces RAW() against non-RAWSXP types */
	arg = FFI_asType(typ,arg);
	switch(TYPEOF(arg)) {
		case EXTPTRSXP:ptr = R_ExternalPtrAddr(arg);break;
		case RAWSXP:ptr    = RAW(arg);break;
		case NILSXP:ptr    = NULL;break;
		case STRSXP:ptr    = CHAR(STRING_ELT(arg,0));break;
		case INTSXP:ptr    = INTEGER(arg);break;
		case REALSXP:ptr   = REAL(arg);break;
	}	
	return ptr;
}

void* ffi_make_arg(SEXP typ,unsigned short type,SEXP arg) {
	void* ptr = ffi_make_ptr(typ,arg);
	/* If the argument type is an FFI_TYPE_POINTER then we need an extra indirection */
	if(type == FFI_TYPE_POINTER) {
		char **temp = (char**)R_alloc(sizeof(char*),1); /* alloca? */
		temp[0]     = (char*)ptr;
		return temp;
	} 
	return ptr;
}

void ffi_make_ret(SEXP typ,unsigned short type,SEXP arg,ret_value*rv) {
  	void*ptr;SEXP temp;ffi_type *l = TYPE_FFI(typ);
	ptr = ffi_make_ptr(typ,arg);
	switch(type) {
		case FFI_TYPE_FLOAT:     rv->flt = ((float*)ptr)[0];break;
		case FFI_TYPE_DOUBLE:    rv->dbl = ((double*)ptr)[0];break;
#ifdef HAVE_LONG_DOUBLE
		case FFI_TYPE_LONGDOUBLE:rv->ldbl = ((long double*)ptr)[0];break;
#endif
		case FFI_TYPE_INT:
		case FFI_TYPE_SINT32:    rv->sint = ((int*)ptr)[0];break;
		case FFI_TYPE_UINT32:    rv->uint = ((unsigned int*)ptr)[0];break;
		case FFI_TYPE_SINT8:     rv->schr = ((char*)ptr)[0];break;
		case FFI_TYPE_UINT8:     rv->uint = ((unsigned char*)ptr)[0];break;
		case FFI_TYPE_SINT16:    rv->ssht = ((short*)ptr)[0];break;
		case FFI_TYPE_UINT16:    rv->usht = ((unsigned short*)ptr)[0];break;
		case FFI_TYPE_SINT64:    rv->slng = ((long*)ptr)[0];break;
		case FFI_TYPE_UINT64:    rv->ulng = ((unsigned long*)ptr)[0];break;
		case FFI_TYPE_STRUCT:    
			memcpy((char*)rv,ptr,l->size);
		break;
		/* Passthrough type */
		case FFI_TYPE_POINTER:   rv->ptr  = ptr;break;
	}
}
