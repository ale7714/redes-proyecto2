/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "centro.h"

bool_t
xdr_str (XDR *xdrs, str *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MAXLEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_datos (XDR *xdrs, datos *objp)
{
	register int32_t *buf;

	 if (!xdr_str (xdrs, &objp->nombre))
		 return FALSE;
	 if (!xdr_str (xdrs, &objp->respuesta))
		 return FALSE;
	return TRUE;
}
