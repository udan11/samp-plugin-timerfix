#include "redirect.h"

void redirect(AMX *amx, char *from, ucell to, AMX_NATIVE *store) {
	AMX_HEADER *hdr = (AMX_HEADER*) amx->base;
	AMX_FUNCSTUB *func;
	for (int idx = 0, num = NUMENTRIES(hdr, natives, libraries); idx != num; ++idx) {
		func = GETENTRY(hdr, natives, idx);
		if (!strcmp(from, GETENTRYNAME(hdr, func))) {
			if (store) {
				*store = (AMX_NATIVE) func->address;
			}
			func->address = to;
			return;
		}
	}
}