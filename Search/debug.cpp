#include "pch.h"
#include "debug.h"

#include <stdio.h>

FILE* fp_in, * fp_out, * fp_err;
void DebugConsole::Show() {
	AllocConsole();
	freopen_s(&fp_in, "CONIN$", "r", stdin);
	freopen_s(&fp_out, "CONOUT$", "w", stdout);
	freopen_s(&fp_err, "CONOUT$", "w", stdout);
}

void DebugConsole::Close() {
	fclose(fp_in);
	fclose(fp_out);
	fclose(fp_err);
	FreeConsole();
}

