#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CBuffer  
{
public:
	void Erase();
	CBuffer(DWORD size, bool AutoDelete = true);
	CBuffer(void* buffer, DWORD length);
	virtual ~CBuffer();

	union PointerUnion{
		BYTE*		b;
		char*		c;
		WORD*		w;
		short int*	s;
		DWORD*		d;
		int*		i;
		float*		f;
		double*		r;	
		void*		v;
	}ptr;

	int	ByteLen;	

private:
	bool	m_bAutoDelete;
};