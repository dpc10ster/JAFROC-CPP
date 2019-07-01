// JAFROC_CORE.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "JAFROC_CORE.h"


// This is an example of an exported variable
JAFROC_CORE_API int nJAFROC_CORE=0;

// This is an example of an exported function.
JAFROC_CORE_API int fnJAFROC_CORE(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see JAFROC_CORE.h for the class definition
CJAFROC_CORE::CJAFROC_CORE()
{
	return;
}
