#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CPtrFIFO : public CPtrList
{
public:
	void* Consume();
	void Add(void *newElement);
	CPtrFIFO();
	virtual ~CPtrFIFO();

	CSemaphore* m_pSemaphore;
};