#include "stdafx.h"
#include "getmfcc.h"
#include<iostream>
#include<fstream>
#include<string>
#include<cstdio>
#include<cmath>
#include<vector>
#include <complex> 
#include <bitset> 
#include <conio.h> 

using namespace std;

typedef struct _TWavHeader
{
	int rId;    //标志符（RIFF）
	int rLen;   //数据大小,包括数据头的大小和音频文件的大小
	int wId;    //格式类型（"WAVE"）
	int fId;    //"fmt"

	int fLen;   //Sizeof(WAVEFORMATEX)

	short wFormatTag;       //编码格式，包括WAVE_FORMAT_PCM，WAVEFORMAT_ADPCM等
	short nChannels;        //声道数，单声道为1，双声道为2
	int nSamplesPerSec;   //采样频率
	int nAvgBytesPerSec;  //每秒的数据量
	short nBlockAlign;      //块对齐
	short wBitsPerSample;   //WAVE文件的采样大小
	int dId;              //"data"
	int wSampleLength;    //音频数据的大小
}TWavHeader;

const int FS = 16;
const int FrmLen = 20*FS;
const unsigned long FFTLen = 512;
const double PI = 3.1415926536;
const int FiltNum = 25;
const int PCEP = 13;
const int GmmNum = 8;
const double infinite = 1e+32;
double Hamming[FrmLen];

vector<float>xishu;

typedef struct _GmmPara
{
	double weight;  //存放权值
	double mean[PCEP];  //存放均值
	double variance[PCEP];  //存放方差
}GmmPara;

GmmPara  para[GmmNum];

void InitHamming();
void HammingWindow(short* buf, float* data);
void compute_fft(float *buffer, vector<complex<float> >& vecList);
void FFT(const unsigned long & ulN, vector<complex<float> >& vecList);
void InitFilt(float *FiltCoe1, float *FiltCoe2, int *Num);
void CFilt(float *spdata, float *FiltCoe1, float *FiltCoe2, int *Num, float *En, vector<complex<float> >& vecList);
void MFCC(float *En);
void EM();
float CalculateLikelihood();

int getmfcc(CString intpath, CString Name)
{
	const char* path =intpath.GetBuffer(sizeof(intpath));
	TWavHeader waveheader;
	FILE *sourcefile;
	short buffer[FrmLen];
	float data[FrmLen];
	int index = 0, count = 0;
	float energy = 0.0, sum = 0.0;
	float FiltCoe1[FFTLen / 2 + 1];  //左系数
	float FiltCoe2[FFTLen / 2 + 1];  //右系数
	int Num[FFTLen / 2 + 1];     //决定每个点属于哪一个滤波器
	float En[FiltNum + 1];         //频带能量

	vector<complex<float> > vecList;
	ofstream outfile1(Name);
	sourcefile = fopen(path, "rb");
	fread(&waveheader, sizeof(struct _TWavHeader), 1, sourcefile);

	InitHamming();//初始化汉明窗
	InitFilt(FiltCoe1, FiltCoe2, Num); //初始化MEL滤波系数

	while (fread(buffer, sizeof(short), FrmLen, sourcefile) == FrmLen)
	{
		HammingWindow(buffer, data);
		compute_fft(data, vecList);
		CFilt(data, FiltCoe1, FiltCoe2, Num, En, vecList);
		MFCC(En);
		vecList.clear();
		index++;
	}
	cout << index << endl;
	int length = xishu.size();
	for (int i = 0; i<length; ++i)
	{
		outfile1 << xishu[i] << ' ';
		if ((i + 1) % 13 == 0)
			outfile1 << endl;
	}
	::MessageBox(NULL, _T("已经提取好了MFCC特征"), _T("OK"), NULL);
	return 0;

}

void InitHamming()
{
	float twopi;
	int i;
	twopi = 8.0F*atan(1.0F);
	for (i = 0; i<FrmLen; i++)
	{
		Hamming[i] = (float)(0.54 - 0.46*cos((float)i*twopi / (float)(FrmLen - 1)));
	}
}

void HammingWindow(short* buf, float* data)
{
	int i;
	for (i = 0; i<FrmLen; i++)
	{
		data[i] = buf[i] * Hamming[i];
	}
}



void compute_fft(float *data, vector<complex<float> >& vecList)
{
	for (int i = 0; i<FFTLen; ++i)
	{
		if (i<FrmLen)
		{
			complex<float> temp(data[i]);
			vecList.push_back(temp);
		}
		else
		{
			complex<float> temp(0);
			vecList.push_back(temp);
		}
	}
	FFT(FFTLen, vecList);
}
void FFT(const unsigned long & ulN, vector<complex<float> >& vecList)
{
	//得到幂数 

	unsigned long ulPower = 0; //幂数 
	unsigned long ulN1 = ulN - 1;
	while (ulN1 > 0)
	{
		ulPower++;
		ulN1 /= 2;
	}
	//反序 

	bitset<sizeof(unsigned long)* 8> bsIndex; //二进制容器 
	unsigned long ulIndex; //反转后的序号 
	unsigned long ulK;
	for (unsigned long p = 0; p < ulN; p++)
	{
		ulIndex = 0;
		ulK = 1;
		bsIndex = bitset<sizeof(unsigned long)* 8>(p);
		for (unsigned long j = 0; j < ulPower; j++)
		{
			ulIndex += bsIndex.test(ulPower - j - 1) ? ulK : 0;
			ulK *= 2;
		}

		if (ulIndex > p)
		{
			complex<float> c = vecList[p];
			vecList[p] = vecList[ulIndex];
			vecList[ulIndex] = c;
		}
	}

	//计算旋转因子 

	vector<complex<float> > vecW;
	for (unsigned long i = 0; i < ulN / 2; i++)
	{
		vecW.push_back(complex<float>(cos(2 * i * PI / ulN), -1 * sin(2 * i * PI / ulN)));
	}

	//计算FFT 

	unsigned long ulGroupLength = 1; //段的长度 
	unsigned long ulHalfLength = 0; //段长度的一半 
	unsigned long ulGroupCount = 0; //段的数量 
	complex<float> cw; //WH(x) 
	complex<float> c1; //G(x) + WH(x) 
	complex<float> c2; //G(x) - WH(x) 
	for (unsigned long b = 0; b < ulPower; b++)
	{
		ulHalfLength = ulGroupLength;
		ulGroupLength *= 2;
		for (unsigned long j = 0; j < ulN; j += ulGroupLength)
		{
			for (unsigned long k = 0; k < ulHalfLength; k++)
			{
				cw = vecW[k * ulN / ulGroupLength] * vecList[j + k + ulHalfLength];
				c1 = vecList[j + k] + cw;
				c2 = vecList[j + k] - cw;
				vecList[j + k] = c1;
				vecList[j + k + ulHalfLength] = c2;
			}
		}
	}
}

/*
设置滤波器参数
输入参数：无
输出参数：*FiltCoe1---三角形滤波器左边的系数
*FiltCoe2---三角形滤波器右边的系数
*Num     ---决定每个点属于哪一个滤波器
*/
void InitFilt(float *FiltCoe1, float *FiltCoe2, int *Num)
{
	int i, j;
	float Freq;
	int FiltFreq[FiltNum + 1] = { 0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000,
		1149, 1320, 1516, 1741, 2000, 2297, 2639, 3031, 3482, 4000,
		4595, 5278, 6063, 6964, 8001 };//滤波器的中心频率
	int BW[FiltNum + 1] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 124,
		160, 184, 211, 242, 278, 320, 367, 422, 484, 556,
		639, 734, 843, 969, 1112 };//滤波器的带宽
	for (i = 0; i <= FFTLen / 2; i++)
	{
		Num[i] = 0;
	}

	for (i = 0; i <= FFTLen / 2; i++)
	{
		Freq = FS * 1000.0F * (float)(i) / (float)(FFTLen);
		for (j = 0; j <FiltNum; j++)
		{
			if (Freq >= (float)FiltFreq[j] && Freq <= (float)FiltFreq[j + 1])
			{
				Num[i] = j;
				if (j == 0)
				{
					FiltCoe1[i] = 0.0F;
				}
				else
				{
					FiltCoe1[i] = ((float)(FiltFreq[j] + BW[j]) - Freq) / (float)(BW[j]);
				}
				FiltCoe2[i] = (Freq - (float)(FiltFreq[j + 1] - BW[j + 1])) / (float)(BW[j + 1]);
				FiltCoe1[i] = FiltCoe1[i] * FiltCoe1[i];
				FiltCoe2[i] = FiltCoe2[i] * FiltCoe2[i];
				break;
			}
		}
	}

}


/*
根据滤波器参数计算频带能量
输入参数：*spdata  ---预处理之后的一帧语音信号
*FiltCoe1---三角形滤波器左边的系数
*FiltCoe2---三角形滤波器右边的系数
*Num     ---决定每个点属于哪一个滤波器

输出参数：*En      ---输出对数频带能量
*/
void CFilt(float *spdata, float *FiltCoe1, float *FiltCoe2, int *Num, float *En, vector<complex<float> >& vecList)
{

	float temp = 0;
	int id, id1, id2;

	for (id = 0; id <= FiltNum; id++)
	{
		En[id] = 0.0F;
	}
	for (id = 0; id < FFTLen / 2; id++)
	{
		temp = vecList[id].real()*vecList[id].real() + vecList[id].imag()*vecList[id].imag();
		id1 = Num[id];
		id2 = id1 + 1;
		En[id1] = En[id1] + FiltCoe1[id] * temp;
		En[id2] = En[id2] + FiltCoe2[id] * temp;
	}
	for (id = 1; id <= FiltNum; id++)
	{
		if (En[id] != 0)
			En[id] = (float)log(En[id]);
	}
}

/*
计算MFCC系数
输入参数：*En ---对数频带能量
*/

void MFCC(float *En)
{
	int idcep, iden;
	float Cep[13];

	for (idcep = 0; idcep < PCEP; idcep++)
	{
		Cep[idcep] = 0.0;

		for (iden = 1; iden <= FiltNum; iden++)
		{
			Cep[idcep] = Cep[idcep] + En[iden] * (float)cos((idcep + 1) * (iden - 0.5F) * PI / (FiltNum));
		}
		Cep[idcep] = Cep[idcep] / 10.0F;
		xishu.push_back(Cep[idcep]);
	}
}

void EM()
{
	float result1 = 0.0, result2;
	int TotalFrame = xishu.size() / PCEP;
	int i, j, k, index, count = 0;
	float temp, temp1, temp2, sum, sum1;
	double poslimit = 1e-100;
	float twopi = 8.0F*atan(1.0F);
	float mid[GmmNum], mid1[GmmNum], mid2[GmmNum][PCEP], mid3[GmmNum][PCEP];
	result2 = CalculateLikelihood();
	cout << result2 << endl;
	while (fabs(result1 - result2) / TotalFrame>0.0005)
	{
		count++;
		for (i = 0; i<GmmNum; ++i)
		{
			mid1[i] = 0.0;
			for (j = 0; j<PCEP; ++j)
			{
				mid2[i][j] = 0.0;
				mid3[i][j] = 0.0;
			}
		}
		for (i = 0; i<TotalFrame; ++i)
		{
			sum1 = 0.0;
			for (j = 0; j<GmmNum; ++j)
			{
				sum = 0.0;
				temp2 = 1.0;
				for (k = 0; k<PCEP; ++k)
				{
					index = i*PCEP + k;
					temp = xishu[index] - para[j].mean[k];
					temp1 = temp*temp;
					temp1 = temp1 / (2 * para[j].variance[k]);
					sum -= temp1;
					temp2 /= sqrt(para[j].variance[k]);
				}
				sum = exp(sum)*temp2 / sqrt(twopi);
				if (sum == 0)
					sum = poslimit;
				mid[j] = sum*para[j].weight;
				sum1 += mid[j];
			}
			for (j = 0; j<GmmNum; ++j)
			{
				mid1[j] += mid[j] / sum1;
				for (k = 0; k<PCEP; ++k)
				{
					index = i*PCEP + k;
					mid2[j][k] += mid[j] * xishu[index] / sum1;
					mid3[j][k] += mid[j] * (xishu[index] - para[j].mean[k])*(xishu[index] - para[j].mean[k]) / sum1;
				}
			}
		}
		for (i = 0; i<GmmNum; ++i)
		{
			para[i].weight = mid1[i] / TotalFrame;
			for (j = 0; j<PCEP; ++j)
			{
				para[i].mean[j] = mid2[i][j] / mid1[i];
				para[i].variance[j] = mid3[i][j] / mid1[i];
			}
		}
		result1 = result2;
		result2 = CalculateLikelihood();
		cout << result2 << endl;
	}
	cout << count << endl;
}

float CalculateLikelihood()
{
	int i, j, k, index;
	float temp, temp1, temp2, sum, sum1, sum2 = 0.0;
	double poslimit = 1e-100;
	float twopi = 8.0F*atan(1.0F);
	int TotalFrame = xishu.size() / PCEP;
	for (i = 0; i<TotalFrame; ++i)
	{
		sum1 = 0.0;
		for (j = 0; j<GmmNum; ++j)
		{
			sum = 0.0;
			temp2 = 1.0;
			for (k = 0; k<PCEP; ++k)
			{
				index = i*PCEP + k;
				temp = xishu[index] - para[j].mean[k];
				temp1 = temp*temp;
				temp1 = temp1 / (2 * para[j].variance[k]);
				sum -= temp1;
				temp2 /= sqrt(para[j].variance[k]);
			}
			sum = exp(sum)*temp2 / sqrt(twopi);
			if (sum == 0)
				sum = poslimit;
			sum1 += sum*para[j].weight;
		}
		sum2 += log(sum1);
	}
	return sum2;
}