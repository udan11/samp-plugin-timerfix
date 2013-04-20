#pragma once

#include <cstring>

#include "amx/amx.h"

// From "amx.c", part of the PAWN language runtime:
// http://code.google.com/p/pawnscript/source/browse/trunk/amx/amx.c

#define USENAMETABLE(hdr) \
	((hdr)->defsize==sizeof(AMX_FUNCSTUBNT))

#define NUMENTRIES(hdr,field,nextfield) \
	(unsigned)(((hdr)->nextfield - (hdr)->field) / (hdr)->defsize)

#define GETENTRY(hdr,table,index) \
	(AMX_FUNCSTUB *)((unsigned char*)(hdr) + (unsigned)(hdr)->table + (unsigned)index*(hdr)->defsize)

#define GETENTRYNAME(hdr,entry) \
	(USENAMETABLE(hdr) ? \
		(char *)((unsigned char*)(hdr) + (unsigned)((AMX_FUNCSTUBNT*)(entry))->nameofs) : \
		((AMX_FUNCSTUB*)(entry))->name)

extern void redirect(AMX *amx, char* from, ucell to, AMX_NATIVE *store);