// PxLExceptions.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PxLExceptions.h"


/*******************************************************************************
*  Function: PxLErrorCheck
*  Purpose:  Global error checker for Api functions.
*            Converts API return codes into C++ exceptions.
*******************************************************************************/
void
PxLErrorCheck(int returnCode, 
			  char const* msg /*=NULL*/,				// Optional extra information
			  bool throwOnQualifiedSuccess /*=false*/)
{
	if (FAILED(returnCode)
		|| ( returnCode != ApiSuccess
			  && throwOnQualifiedSuccess) )
	{
		// Hint: The following line is a good place to set a breakpoint when
		// you're trying to track down hard-to-find errors.
		throw PxLException(returnCode, msg);
	}
	// else: Success code - do nothing.
}



