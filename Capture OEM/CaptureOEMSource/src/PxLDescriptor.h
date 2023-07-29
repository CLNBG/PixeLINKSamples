#pragma once



class CPxLDescriptor
{
	friend class CPxLCamera;
	friend class CPxLCameraDoc;

public:
	CPxLDescriptor(HANDLE descriptorHandle, int updateMode);
	~CPxLDescriptor(void);

private:
	HANDLE				m_descriptorHandle;
	int					m_updateMode;
};


