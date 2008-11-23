#include <Rdefines.h>
#include "common.h"
#include <ffi.h>
#include <dlfcn.h>

void ffi_PrintCIF(SEXP cif) {
  SEXP p = R_ExternalPtrProtected(cif);
  SEXP r = VECTOR_ELT(p,0);
  SEXP a = VECTOR_ELT(p,1);
  int  i;
  ffi_PrintTypeCode(r);Rprintf(" (*FUN)(");
  for(i=0;i<length(a);i++) {
    ffi_PrintTypeCode(VECTOR_ELT(a,i));
    if(i+1<length(a)) Rprintf(",");
  }
  Rprintf(")\n");
}

SEXP FFI_makeCIF(SEXP retval,SEXP types) {
  ffi_cif   *cif      = (ffi_cif*)malloc(sizeof(ffi_cif)+length(types)*sizeof(ffi_type*));
  ffi_type **typespec = (ffi_type**)( ((char*)cif)+sizeof(ffi_cif));
  int        i;
  SEXP       p,ret;
  PROTECT(p   = allocVector(VECSXP,2));
  SET_VECTOR_ELT(p,0,retval);
  SET_VECTOR_ELT(p,1,types);
  PROTECT(ret = R_MakeExternalPtr(cif,R_NilValue,p));
  for(i=0;i<length(types);i++) {
    SEXP type = VECTOR_ELT(types,i);
    if(TYPEOF(type) != EXTPTRSXP &&
       R_ExternalPtrTag(type) != FFI_TypeTag)
      error("Not a type description?");
    typespec[i] = R_ExternalPtrAddr(VECTOR_ELT(types,i));
  }
  ffi_prep_cif(R_ExternalPtrAddr(ret),FFI_DEFAULT_ABI,length(types),
	       R_ExternalPtrAddr(retval),typespec);
  UNPROTECT(2);
  return ret;
}

SEXP FFI_SummaryCIF(SEXP cif) {
  return R_ExternalPtrProtected(cif);
}

SEXP FFI_Call(SEXP recv,SEXP sig,SEXP args) {
  void (*FUN)() = NULL;
  char *vmax = vmaxget();
  void *handle = NULL;
  /* Get the function pointer */
  switch(TYPEOF(recv)) {
  case EXTPTRSXP:
    FUN = R_ExternalPtrAddr(recv);
    break;
  case STRSXP:
    if(length(recv)==1) {
      FUN = dlsym(NULL,CHAR(STRING_ELT(recv,0)));
      if(NULL == FUN)
	error("Pointer doesn't point to a function?");
    } else {
      handle = dlopen(CHAR(STRING_ELT(recv,0)),RTLD_NOW|RTLD_LOCAL);
      if(NULL == handle)
	error("Unable to load library %s",CHAR(STRING_ELT(recv,0)));
      FUN    = dlsym(handle,CHAR(STRING_ELT(recv,1)));
    }
    if(NULL == FUN)
      error("Unable to bind symbol %s",CHAR(STRING_ELT(recv,length(recv)-1)));
    break;
  default:
    error("No receiver function address");
    break;
  }
  
  /* Get the signature for the call */
  ffi_cif *cif = R_ExternalPtrAddr(sig);
  if(cif->nargs != length(args))
    error("this signature specifies %d arguments",cif->nargs);
  
  /* Sort out the arguments */
  int    i;
  void **values = (void**)R_alloc(sizeof(void),length(args));
  for(i=0;i<length(args);i++)
    values[i] = ffi_make_arg(TYPE_ELT(sig,i),cif->arg_types[i]->type,VECTOR_ELT(args,i));

  /* Make the call and return */
  ret_value retval;
  SEXP      rval;
  ffi_call(cif,FUN,&retval,values);
  PROTECT(rval = ffi_fromType(TYPE_RET(sig),cif->rtype->type,&retval));
  setAttrib(rval,install("ffi.args"),args);
  vmaxset(vmax);
  if(handle != NULL) dlclose(handle);
  UNPROTECT(1);
  return rval;
}
