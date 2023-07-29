#include "stdafx.h"
#include "PxLDescriptor.h"



/**
* Function: CPxLDescriptor
* Purpose:  Constructor
*/
CPxLDescriptor::CPxLDescriptor(HANDLE descriptorHandle, int updateMode)
	: m_descriptorHandle(descriptorHandle)
	, m_updateMode(updateMode)
{
}


/**
* Function: ~CPxLDescriptor
* Purpose:  Destructor
*/
CPxLDescriptor::~CPxLDescriptor(void)
{
}










