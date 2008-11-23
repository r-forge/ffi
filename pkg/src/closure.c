#include <Rdefines.h>
#include "common.h"
#include <ffi.h>
#include <dlfcn.h>

void ffi_function(ffi_cif *cif,void*retVal,void**args,void*userData) {
  SEXP closInfo = (SEXP)userData;
  SEXP FUN      = R_ExternalPtrProtected(closInfo);
  SEXP sig      = getAttrib(FUN,FFI_TypeSymbol);
  SEXP call,c,ret;
  int  i,err;
  if(FUN == R_NilValue || FUN == NULL || TYPEOF(FUN) != CLOSXP)
    error("Closure is not bound to a function");
  PROTECT(call = c = allocList(cif->nargs+1));
  SET_TYPEOF(c,LANGSXP);
  SETCAR(c,FUN);c = CDR(c);
  for(i=0;i<cif->nargs;i++) {
    SEXP arg;
    PROTECT(arg = ffi_fromType(TYPE_ELT(sig,i),cif->arg_types[i]->type,(ret_value*)args[i]));
    SETCAR(c,arg);c = CDR(c);
    UNPROTECT(1);
  }
  PROTECT(ret = R_tryEval(call,R_GlobalEnv,&err));
  if(err)
    error("Closure Error");
  ffi_make_ret(TYPE_RET(sig),cif->rtype->type,ret,retVal);
  UNPROTECT(2);
}

SEXP FFI_MakeClosure(SEXP FUN,SEXP CIF) {
  if(CIF == R_NilValue)
    CIF = getAttrib(FUN,FFI_TypeSymbol);
  else
    setAttrib(FUN,FFI_TypeSymbol,CIF);
  if(CIF == R_NilValue || CIF == R_UnboundValue)
    error("unable to get function's call information");
  SEXP ret;
  ffi_closure *clos = malloc(sizeof(ffi_closure));
  PROTECT(ret = R_MakeExternalPtr(clos,R_NilValue,FUN));
  ffi_prep_closure(clos,
		   (ffi_cif*)R_ExternalPtrAddr(CIF),
		   ffi_function,
		   ret);
  UNPROTECT(1);
  return ret;
}
