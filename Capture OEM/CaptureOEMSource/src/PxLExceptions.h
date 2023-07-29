#pragma once


// PxLExceptions.h
#include <string>
#include <stdexcept>
#include "Common.h"



/******************************************************************************
*  Class:   PxLException
*  Purpose: This is the base class for all exceptions in the pxl namespace.
******************************************************************************/
class PxLException : public std::runtime_error
{
public:
	explicit PxLException(int retCode, char const* msg)
		: std::runtime_error(msg ? msg : ReturnCodeAsString(retCode))
		, m_retCode(retCode)
	{
	}
	virtual ~PxLException() {}
	virtual int GetReturnCode() const { return m_retCode; }
private:
	int m_retCode;
};



/******************************************************************************
*  Function: PxLErrorCheck
*  Purpose:  Global error checker for PixeLINK API calls. Pass in the return
*            code, and this function will throw an appropriate C++ exception
*            if the code indicates failure.
*            This function will not throw an exception if the return code is
*            one of the qualified success codes - 
*            eg: ApiSuccessParametersChanged - unless you explicitly request
*            that it do so by passing true as the second parameter.
*            You can also pass a string containing extra information to be
*            stored in the exception object - for example, you may want to
*            give the name of the class and/or function that you called this
*            function from, to help trace the souce of the error.
******************************************************************************/
void
PxLErrorCheck(int returnCode, char const* msg = NULL, bool throwOnQualifiedSuccess = false);



