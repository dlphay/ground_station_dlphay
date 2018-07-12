
// DllUseDemoView.cpp : CDllUseDemoView 类的实现
//

#include "stdafx.h"
#include <time.h>
#include <nmmintrin.h>
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DllUseDemo.h"
#endif

#include "DllUseDemoDoc.h"
#include "DllUseDemoView.h"
#include "UAVInfo.h"

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

// opencv2.4.13
#include "opencv2/core/core.hpp"   
#include "opencv2/features2d/features2d.hpp"   
#include "opencv2/highgui/highgui.hpp"   
#include "opencv2/legacy/legacy.hpp"
#include <iostream>   
#include <vector>  
#include <time.h>
#include "cv.h"  
#include "opencv2/opencv.hpp"  

// GPU
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/nonfree/gpu.hpp"

/******************************************************************************
                          *** 命名空间 ***
******************************************************************************/
using namespace std;
using namespace cv;
using namespace cv::gpu;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  ISOK_FUNCRETURN  BOOL
#define  NO_FUNCTURN   void
/**************************************************************************************/
/***************************    绘图与更新   ******************************************/
/**************************************************************************************/
#include <Vfw.h>//显示用到的库
#pragma comment(lib,"Vfw32.lib")
CDllUseDemoView* g_pView = NULL;
unsigned char* pRGB;//用于绘图的内存
unsigned char* pRGB_camera;//用于摄像头部分绘图的内存

int DisplayFlag=0;//是否更新了图像

int is_UpdatedisplayFlag = 0;//是否更新初始图像

/**************************************************************************************/
/***************************  线程函数的声明  ******************************************/
/**************************************************************************************/
void Thread_display(void* param);
void Thread_DLL(void* param);

#include "./lib/imagetrans.h"
#pragma comment(lib,"./lib/imagetrans.lib")
// unsigned char* _Image = new unsigned char[1920 * 1080 * 3];

long long IMAGE_NUM = 640 * 352 * 3;
long long DEMO_NUM = 1200 * 750 * 3;
int IMAGE_WIDTH = 640;
int IMAGE_HEIGHT = 352;

int QQQQ = 5;

IplImage* camera_img;
cv::Mat img_mat;
unsigned char* _Image = new unsigned char[IMAGE_NUM];


/**************************************************************************************/
/*******************************    去雾    *******************************************/
/**************************************************************************************/
bool dehazeflag = 0;
void correct_border(unsigned char* src);
int stride = 3 * 640;
//去雾
typedef int(__stdcall *Dehaze)(unsigned char *Src, unsigned char *Dest, int Width, int Height, int Stride,
	int BlockSize, int GuideRadius, int MaxAtom, float Omega, float T0, float Gamma);
//blocksize用于计算暗影通道时的矩形半径；guideradius导向滤波的半径；
//omega控制去雾程度的一个参数，建议取值范围[0.75,1]值越大，去雾越明显；
//t0控制最小透射率的一个参数，建议取值范围[0.01,0.2]；gama调整亮度的参数，建议取值范围[0.7,1]。

Dehaze pfFuncInDll = NULL;
HINSTANCE hinst = LoadLibraryA("ImageMaster.dll");

/**************************************************************************************/
/*******************************    检测    *******************************************/
/**************************************************************************************/

//是否检测标志位
bool detectionflag = 0;
int DETECTION_FLAG = 0;

/*****************************************************
               *** orb检测宏定义 ***
*****************************************************/

#define GRAYSCALE  0

//orb匹配点系数
#define COEFF_DISTANCE 0.63

//累计计算的帧数
#define COUNT_sumNUM  4

//要检测几次？
#define COUNT_jiance_NUM  10

//间隔帧数
#define INTERVAL_IMAGE  7

//是否是第一次的检测   是0   不是1
#define FIRST_IS    0
#define FIRST_NOT    1

//最大要初始化的帧数
#define NUM_MAX_INITDETECTOR  (INTERVAL_IMAGE + COUNT_sumNUM - 1)

// 初始化要进行差分累加的图像计数
int num_Init_detector = 0;
int num_Init_detector_FLAG = 0;

char *path_image[] =
{
	"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a0.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a1.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a2.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a3.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a4.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a5.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a6.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a7.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a8.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a9.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a10.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a11.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a12.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a13.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a14.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a15.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a16.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a17.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a18.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\a19.bmp"
};


char *descriptors_image[] =
{
	"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d0.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d1.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d2.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d3.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d4.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d5.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d6.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d7.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d8.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d9.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d10.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d11.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\d12.bmp"
};

char *savee_image[] =
{
	"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e0.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e1.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e2.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e3.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e4.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e5.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e6.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e7.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e8.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e9.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e10.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e11.bmp"
	,"E:\\地面站\\DllUseDemo(debug_x64)\\data\\e12.bmp"
};

/******************************************************************************
*** 图像本地路径 ***
******************************************************************************/
/*
char *path_image[] = {
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3702.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3705.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3708.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3711.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3712.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3713.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3714.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3715.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3716.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3717.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3718.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3719.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3720.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3721.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3722.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3723.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3724.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3725.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3726.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3727.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3728.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3729.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3730.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3731.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3732.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3733.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3734.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3735.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3736.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3737.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3738.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3739.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3740.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3741.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3742.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3743.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3744.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3745.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3746.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3747.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3748.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3749.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3750.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3751.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3752.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3753.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3754.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3755.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3756.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3757.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3758.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3759.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3760.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3761.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3762.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3763.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3764.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3765.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3766.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3767.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3768.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3769.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3770.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3771.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3772.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3773.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3774.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3775.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3776.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3777.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3778.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3779.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3780.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3781.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3782.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3783.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3784.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3785.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3786.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3787.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3788.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3789.jpg",
	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3790.jpg"
};

char *result_image[] = {
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3702.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3705.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3708.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3711.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3712.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3713.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3714.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3715.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3716.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3717.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3718.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3719.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3720.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3721.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3722.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3723.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3724.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3725.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3726.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3727.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3728.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3729.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3730.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3731.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3732.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3733.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3734.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3735.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3736.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3737.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3738.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3739.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3740.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3741.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3742.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3743.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3744.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3745.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3746.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3747.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3748.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3749.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3750.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3751.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3752.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3753.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3754.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3755.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3756.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3757.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3758.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3759.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3760.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3761.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3762.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3763.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3764.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3765.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3766.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3767.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3768.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3769.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3770.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3771.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3772.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3773.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3774.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3775.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3776.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3777.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3778.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3779.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3780.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3781.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3782.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3783.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3784.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3785.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3786.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3787.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3788.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3789.jpg",
	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3790.jpg"
};
*/


char *sum_image[] = {
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3700.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3701.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3702.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3703.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3704.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3705.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3706.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3707.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3708.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3709.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3710.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3711.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\SUM.mpg3712.jpg"
};

char *quzao_image[] = {
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3700.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3701.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3702.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3703.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3704.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3705.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3706.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3707.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3708.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3709.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3710.jpg","E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3711.jpg",
	"E:\\地面站\\DllUseDemo(debug_x64)\\DllUseDemo\\data\\capture\\QUZAO.mpg3712.jpg"
};

/******************************************************************************
              *** orb相关变量定义 ***
******************************************************************************/

// 置信度约束：
int CONFIDENCE_FLAG_NUM = 0;  //当前稳定的约束点有几个？ 
CvPoint *fram_confidence = new CvPoint[1000];




ORB_GPU orb_gpu;


GpuMat img_gpu[15];
//vector<KeyPoint> KeyPoints_1, KeyPoints_2;
vector<KeyPoint> KeyPoints[12];

//GpuMat Descriptors_1, Descriptors_2;
GpuMat Descriptors[12];

BruteForceMatcher_GPU<Hamming> matcher;
vector<DMatch> matches;
double max_dist;   //计算距离
double min_dist;
vector<DMatch> good_matches;
std::vector<Point2f> obj;
std::vector<Point2f> scene;

/******************************************************************************
                        *** 图像相关变量定义 ***
******************************************************************************/

int I_count = 0;
GpuMat xformed;
GpuMat save;
GpuMat img_A;
IplImage* ipl_save[4];
IplImage* ipl_img[4];
IplImage* SUM_image;
IplImage* SUM_image1;
IplImage* SUM_TEMP;
IplImage* SUM_image2_quzao;  //去噪
IplImage* SUM_image3_smooth;  //平滑
IplImage* SUM_image4_candy;   //边缘
cv::Mat SUM_image5_blob;      //blob
IplImage* ipl_save_TEMP;  //用于检测临时存放

/******************************************************************************
					*** 去噪尺度计算 ***
******************************************************************************/

int SCALE_NOISE;
int STRIDE_SLIDE_WINDOWS;
IplImage* BGR_image_cpu;
int COUNT;
int red_point_x;
int red_point_y;

/******************************************************************************
*** 检测坐标点计算变量定义 ***
******************************************************************************/
int n = 0;
CvPoint pt1, pt2;
CvPoint pt3, pt4;
CvPoint pt5, pt6;
CvPoint pt7, pt8;

/******************************************************************************
*** 其他变量定义 ***
******************************************************************************/

int is_first = FIRST_IS;  //第一次
int x, y, z, t, c;   //循环变量定义

//sift 跟踪
struct feature * fffff;


/**************************************************************************************/
/******************************* 函数块声明 *******************************************/
/**************************************************************************************/
NO_FUNCTURN f_dehaze_Init_DlphaY();
NO_FUNCTURN f_dehaze_DlphaY(unsigned char* src);

NO_FUNCTURN f_correct_DlphaY(unsigned char* src);

/*************************************
           *** 检测 ***
**************************************/
// 链表内存释放 
NO_FUNCTURN CLEAR_VECTOR();
NO_FUNCTURN f_gpu_init();
NO_FUNCTURN f_conv_UncharPTR_to_Gpumat();

// 实时帧的检测
NO_FUNCTURN f_detector_DlphaY(unsigned char* src);


/*************************************
      *** blob ***
**************************************/

vector<KeyPoint> keypoints_blob;
SimpleBlobDetector::Params params;



/**************************************************************************************/
/*******************************添加代码结束*******************************************/
/**************************************************************************************/
// CDllUseDemoView

IMPLEMENT_DYNCREATE(CDllUseDemoView, CScrollView)

BEGIN_MESSAGE_MAP(CDllUseDemoView, CScrollView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_QUWU_OK, &CDllUseDemoView::Ondehaze)
	ON_COMMAND(ID_QUWU_HUIFU, &CDllUseDemoView::Onhuifu)
	ON_COMMAND(ID_CAMERA, &CDllUseDemoView::Detector_CAMERA)
	ON_COMMAND(ID_ORB_CAMERA, &CDllUseDemoView::Detector_ORB_CAMERA)
	ON_COMMAND(ID_ORB_LOCAL, &CDllUseDemoView::Detector_ORB_LOCAL)
	ON_COMMAND(ANA_CONFIDENCE, &CDllUseDemoView::Detector_ANA_CONFIDENCE)
	ON_COMMAND(PARA_OVERFIT, &CDllUseDemoView::Detector_PARA_OVERFIT)
	// 连通域 Detector_CONNECTED_DOMAIN
	// hogsvm Detector_HOGSVM
	// Detector_HOUGH
	ON_COMMAND(ID_32792, &CDllUseDemoView::Detector_CONNECTED_DOMAIN)
	ON_COMMAND(ID_32797, &CDllUseDemoView::Detector_HOGSVM)
	ON_COMMAND(ID_32798, &CDllUseDemoView::Detector_HOUGH)
	// 跟踪
	// Tracker_tld
	// Tracker_meanshift
	// Tracker_particle
	// Tracker_mtilti
	ON_COMMAND(ID_32793, &CDllUseDemoView::Tracker_tld)
	ON_COMMAND(ID_32794, &CDllUseDemoView::Tracker_meanshift)
	ON_COMMAND(ID_32795, &CDllUseDemoView::Tracker_particle)
	ON_COMMAND(ID_32796, &CDllUseDemoView::Tracker_mtilti)
	// 识别
	// Detector_yolo
	// Detector_hog_svm
	ON_COMMAND(ID_32799, &CDllUseDemoView::Detector_yolo)
	ON_COMMAND(ID_32800, &CDllUseDemoView::Detector_hog_svm)
END_MESSAGE_MAP()

// CDllUseDemoView 构造/析构

CDllUseDemoView::CDllUseDemoView()
{
	// TODO: 在此处添加构造代码
}

CDllUseDemoView::~CDllUseDemoView()
{
}

BOOL CDllUseDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CDllUseDemoView 绘制


// clockwise 为true则顺时针旋转，否则为逆时针旋转  
IplImage* rotateImage(IplImage* src, int angle, bool clockwise)
{
	angle = abs(angle) % 180;
	if (angle > 90)
	{
		angle = 90 - (angle % 90);
	}
	IplImage* dst = NULL;
	int width =
		(double)(src->height * sin(angle * CV_PI / 180.0)) +
		(double)(src->width * cos(angle * CV_PI / 180.0)) + 1;
	int height =
		(double)(src->height * cos(angle * CV_PI / 180.0)) +
		(double)(src->width * sin(angle * CV_PI / 180.0)) + 1;
	int tempLength = sqrt((double)src->width * src->width + src->height * src->height) + 10;
	int tempX = (tempLength + 1) / 2 - src->width / 2;
	int tempY = (tempLength + 1) / 2 - src->height / 2;
	int flag = -1;

	dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);
	cvZero(dst);
	IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), src->depth, src->nChannels);
	cvZero(temp);

	cvSetImageROI(temp, cvRect(tempX, tempY, src->width, src->height));
	cvCopy(src, temp, NULL);
	cvResetImageROI(temp);

	if (clockwise)
		flag = 1;

	float m[6];
	int w = temp->width;
	int h = temp->height;
	m[0] = (float)cos(flag * angle * CV_PI / 180.);
	m[1] = (float)sin(flag * angle * CV_PI / 180.);
	m[3] = -m[1];
	m[4] = m[0];
	// 将旋转中心移至图像中间  
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	//  
	CvMat M = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(temp, dst, &M);
	cvReleaseImage(&temp);
	return dst;
}


void CDllUseDemoView::OnDraw(CDC* /*pDC*/)
{
	CDllUseDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: 在此处为本机数据添加绘制代码
}

//用于显示的线程

void CDllUseDemoView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
	/**************************************************************************************/
	/***************************此处为添加的代码******************************************/
	/**************************************************************************************/

	g_pView=this;
	m_pdispThread = (HANDLE)_beginthread(Thread_display, 0, this);
	m_pCapThread= (HANDLE)_beginthread(Thread_DLL, 0, this);
	/**************************************************************************************/
	/*******************************添加代码结束*******************************************/
	/**************************************************************************************/
}

void CDllUseDemoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDllUseDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDllUseDemoView 诊断

#ifdef _DEBUG
void CDllUseDemoView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDllUseDemoView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDllUseDemoDoc* CDllUseDemoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDllUseDemoDoc)));
	return (CDllUseDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CDllUseDemoView 消息处理程序



/**************************************************************************************/
/***************************     显示线程    ******************************************/
/**************************************************************************************/
void Thread_display(void* param) 
{
	//f_gpu_init();
	HDRAWDIB hdd = DrawDibOpen();
	pRGB   = new unsigned char[1281*721*3];
	//pRGB = new unsigned char[IMAGE_NUM];


	unsigned char* _Idemo = new unsigned char[1920*1080*3]; //用于存放初始化图像
	unsigned char* _Icamera = new unsigned char[1920*1080*3]; //用于相机缓冲
	//unsigned char* _Ilocal = new unsigned char[640 * 352 * 3]; //用于本地视频
	unsigned char* _Ilocal = new unsigned char[1920 * 1080 * 3]; //用于本地视频
	//记得每次改过之后进行重新
	pRGB = new unsigned char[1920 * 1080 * 3];

	/*************************************************************/
	/***********************  算法初始化  ************************/
	/*************************************************************/

	CvCapture* cap_camera;


	//cap_camera = cvCaptureFromCAM(0);  //本地usb
	cap_camera = cvCaptureFromCAM(0);  //本地usb
	cvSetCaptureProperty(cap_camera, CV_CAP_PROP_FRAME_WIDTH, 1280);
	cvSetCaptureProperty(cap_camera, CV_CAP_PROP_FRAME_HEIGHT, 720);

	Mat Demo_image_pRGB = imread("E:\\地面站\\test.jpg", 1);
	cv::flip(Demo_image_pRGB, Demo_image_pRGB, 0);
	cv::VideoCapture cap_local("E:\\video\\test_car01_0.mp4");

	f_gpu_init();

	// DisplayFlag == 0 --- 不进行更新
	// DisplayFlag == 1 --- 更新摄像头显示。
	// DisplayFlag == 2 --- 更新摄像头实时检测。
	// DisplayFlag == 3 --- 更新本地视频实时检测。

	// blob初始化
	params.filterByArea = true;
	params.minArea = 2;
	params.maxArea = 100;
	SimpleBlobDetector blobDetector(params);
	blobDetector.create("SimpleBlob");

	while(1)
	{

		// DETECTION_FLAG == 0 --- 不进行更新
		// DETECTION_FLAG == 1 --- 更新摄像头显示
		// DETECTION_FLAG == 2 --- 更新摄像头实时检测
		// DETECTION_FLAG == 3 --- 更新本地视频实时检测

		if (0 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;
			IMAGE_WIDTH = Demo_image_pRGB.cols;
			IMAGE_HEIGHT = Demo_image_pRGB.rows;
			IMAGE_NUM = Demo_image_pRGB.cols*Demo_image_pRGB.rows * 3;
			delete []pRGB;
			pRGB = new unsigned char[IMAGE_NUM];
			_Idemo = (unsigned char *)Demo_image_pRGB.data;
			memcpy(pRGB, _Idemo, IMAGE_NUM);	
		}

		// 更新摄像头显示
		if (1 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;
			camera_img = cvQueryFrame(cap_camera);//捕获相机的视频帧，并进行解码操作
			if (camera_img->origin == IPL_ORIGIN_TL)//如果图片原点在左上角，将其沿X轴翻转，使左下角
			{
				cvFlip(camera_img, camera_img);
			}

			IMAGE_WIDTH = camera_img[0].width;
			IMAGE_HEIGHT = camera_img[0].height;
			IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;

			delete[]pRGB;
			pRGB = new unsigned char[IMAGE_NUM]; 
			_Icamera = (unsigned char *)camera_img[0].imageData;
			memcpy(pRGB, _Icamera, IMAGE_NUM);
		}

		// 更新摄像头实时检测
		if (2 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;


			if (num_Init_detector_FLAG == 1)
			{
				num_Init_detector = 0;
				num_Init_detector_FLAG = 0;
				CLEAR_VECTOR();
			}

			camera_img = cvQueryFrame(cap_camera);//捕获相机的视频帧，并进行解码操作
			if (camera_img->origin == IPL_ORIGIN_TL)//如果图片原点在左上角，将其沿X轴翻转，使左下角
			{
				cvFlip(camera_img, camera_img);
			}
			IplImage *SUM_image6_blob = camera_img;

			img_mat = cvarrToMat(camera_img);
			//img_mat = img_mat(Range(45, 340), Range(69, 503));
			cv::imwrite(path_image[num_Init_detector % 20], img_mat);

			if (num_Init_detector <= 11)
			{
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				GpuMat img_gpu_temp(img_mat);
				img_gpu[num_Init_detector] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[num_Init_detector], Descriptors[num_Init_detector]);
				num_Init_detector++;

				//Sleep(100);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}
			if (num_Init_detector == 12)
			{
				num_Init_detector++;

				for (int aa = 0; aa < 4; aa++)
				{
					matcher.match(Descriptors[aa], Descriptors[aa + 8], matches, GpuMat());

					// 计算距离
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}

					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[aa][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[aa + 8][good_matches[i].trainIdx].pt);
					}

					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[aa], xformed, H_p, img_gpu[aa].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[aa + 8], xformed, save, GpuMat(), 0);


					save.download(save_cpu);

					cv::imwrite(descriptors_image[aa % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[aa] = cvCloneImage(&ipl_save_d);
					ipl_img[aa] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}


				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 60;
					keypoints_blob[i].pt.y += 10;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++], SUM_image6_blob);
			}
			if (num_Init_detector == 13)
			{
				num_Init_detector++;
				continue;

			}

			// 优化的
			if (num_Init_detector == 14)
			{
				// 开始计时
				int a;
				clock_t start, finish;
				double totaltime;
				start = clock();
				/*************************************************************************************************************************************/

				//Mat yyyy;

				// 优化部分
				// cv::imwrite(path_image[19], img_mat); 

				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				// 开始清除数据！
				for (int i = 0; i < 10; i++)
				{
					img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
					KeyPoints[i] = KeyPoints[i + 1];
					Descriptors[i] = Descriptors[i + 1];
				}
				KeyPoints[11].clear();

				// di 11 每次将关键点和描述信息重新更新
				GpuMat img_gpu_temp(img_mat);
				img_gpu[11] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);

				for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
				{
					ipl_save[i] = ipl_save[i + 1];
				}

				{  // 重新计算
					matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
					// 计算距离
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}
					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
					}
					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

					save.download(save_cpu);

					// 优化部分
					finish = clock();
					totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
					cv::imwrite("E:\\地面站\\DllUseDemo(debug_x64)(优化速度)\\test.jpg", save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[3] = cvCloneImage(&ipl_save_d);
					ipl_img[3] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}

				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);


				SUM_image3_smooth = SUM_image1;
				//去除
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);


				// blob
				blobDetector.detect(SUM_image4_candy, keypoints_blob);
				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}
				//更新显示

				//SUM_image6_blob = SUM_image;
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				//IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 1;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				//_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, (unsigned char *)SUM_image6_blob[0].imageData, IMAGE_NUM);

				/*************************************************************************************************************************************/
				//finish = clock();
				//totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
				//int b;

			}

			//最开始的，没有经过优化的
			//if (num_Init_detector == 14)
			//{
			//	// 开始计时
			//	int a;
			//	clock_t start, finish;
			//	double totaltime;
			//	start = clock();
			//	/*************************************************************************************************************************************/

			//	//Mat yyyy;
			//	cv::imwrite(path_image[19], img_mat);
			//	cvtColor(img_mat, img_mat, CV_BGR2GRAY);
			//	// 开始清除数据！
			//	for (int i = 0; i < 10; i++)
			//	{
			//		img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
			//		KeyPoints[i] = KeyPoints[i + 1];
			//		Descriptors[i] = Descriptors[i + 1];
			//	}
			//	KeyPoints[11].clear();

			//	// di 11 每次将关键点和描述信息重新更新
			//	GpuMat img_gpu_temp(img_mat);
			//	img_gpu[11] = img_gpu_temp;
			//	orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);




			//	for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
			//	{
			//		ipl_save[i] = ipl_save[i + 1];
			//	}

			//	{  // 重新计算
			//		matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
			//		// 计算距离
			//		for (int i = 0; i < Descriptors[3].rows; i++)
			//		{
			//			double dist = matches[i].distance;
			//			if (i == 0)
			//			{
			//				min_dist = dist;
			//				max_dist = dist;
			//			}
			//			if (dist < min_dist) min_dist = dist;
			//			if (dist > max_dist) max_dist = dist;
			//		}

			//		// 获得好的匹配点
			//		for (int i = 0; i < Descriptors[3].rows; i++)
			//		{
			//			if (matches[i].distance < COEFF_DISTANCE * max_dist)
			//			{
			//				good_matches.push_back(matches[i]);
			//			}
			//		}
			//		for (size_t i = 0; i < good_matches.size(); ++i)
			//		{
			//			// get the keypoints from the good matches
			//			obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
			//			scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
			//		}
			//		const Mat H_p = findHomography(obj, scene, CV_RANSAC);
			//		gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
			//		Mat save_cpu;
			//		Mat img_cpu;
			//		xformed.download(save_cpu);
			//		gpu::threshold(xformed, img_A, 0, 255, 0);
			//		gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

			//		save.download(save_cpu);

			//		cv::imwrite(descriptors_image[3 % 100], save_cpu);

			//		img_A.download(img_cpu);

			//		IplImage ipl_save_d = save_cpu;
			//		IplImage ipl_img_d = img_cpu;
			//		ipl_save[3] = cvCloneImage(&ipl_save_d);
			//		ipl_img[3] = cvCloneImage(&ipl_img_d);

			//		// 清除向量！！！
			//		CLEAR_VECTOR();
			//	}





			//	SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
			//	for (y = 0; y<SUM_image->height; y++)
			//		for (x = 0; x<SUM_image->width; x++)
			//		{
			//			c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
			//				+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

			//			if (c<255)
			//				((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
			//			else
			//			{
			//				c = 255;
			//				((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
			//			}
			//		}

			//	cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

			//											  //去噪
			//	SUM_TEMP = SUM_image;
			//	SUM_image1 = SUM_image;
			//	cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
			//		CV_MOP_OPEN, //CV_MOP_CLOSE,
			//		1);
			//	SUM_image2_quzao = SUM_image1;
			//	cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
			//	// 
			//	cvSaveImage(savee_image[1], SUM_image1);
			//	SUM_image3_smooth = SUM_image1;



			//	for (y = 0; y<(SUM_image3_smooth->height); y++)
			//		for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
			//		{
			//			c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
			//			if (c > 0)
			//			{
			//				SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
			//			}
			//		}
			//	for (y = 0; y<(SUM_image3_smooth->height); y++)
			//		for (x = 0; x<(20); x++)
			//		{
			//			c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
			//			if (c > 0)
			//			{
			//				SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
			//			}
			//		}
			//	for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
			//		for (x = 0; x<(SUM_image3_smooth->width); x++)
			//		{
			//			c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
			//			if (c > 0)
			//			{
			//				SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
			//			}
			//		}
			//	for (y = 0; y<20; y++)
			//		for (x = 0; x<(SUM_image3_smooth->width); x++)
			//		{
			//			c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
			//			if (c > 0)
			//			{
			//				SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
			//			}
			//		}

			//	cvSaveImage(savee_image[2], SUM_image3_smooth);

			//	SUM_image4_candy = SUM_image3_smooth;
			//	cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
			//	cvSaveImage(savee_image[3], SUM_image4_candy);



			//	// blob
			//	params.filterByArea = true;
			//	params.minArea = 2;
			//	params.maxArea = 100;
			//	SimpleBlobDetector blobDetector(params);
			//	blobDetector.create("SimpleBlob");
			//	blobDetector.detect(SUM_image4_candy, keypoints_blob);



			//	//cvSaveImage(savee_image[4], SUM_image4_candy);

			//	for (unsigned int i = 0; i < keypoints_blob.size(); i++)
			//	{
			//		keypoints_blob[i].pt.x += 69;
			//		keypoints_blob[i].pt.y += 45;
			//	}

			//	CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
			//	for (int i = 0; i < keypoints_blob.size(); i++)
			//	{
			//		fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
			//		fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

			//		fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
			//		fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

			//		cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
			//	}


			//	//cvSaveImage(savee_image[5], SUM_image4_candy);
			//	//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
			//	// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
			//	//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
			//	//cv::imwrite(savee_image[4], SUM_image5_blob);

			//	cvSaveImage(savee_image[QQQQ++ % 13], SUM_image6_blob);
			//	SUM_image6_blob = rotateImage(SUM_image6_blob, 180, 1);
			//	//cvTranspose(SUM_image6_blob, SUM_image6_blob);



			//	//Sleep(10);

			//	//更新显示
			//	IMAGE_WIDTH = SUM_image6_blob[0].width;
			//	IMAGE_HEIGHT = SUM_image6_blob[0].height;
			//	IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
			//	delete[]pRGB;
			//	pRGB = new unsigned char[IMAGE_NUM];
			//	_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
			//	memcpy(pRGB, _Ilocal, IMAGE_NUM);

			//	/*************************************************************************************************************************************/
			//	finish = clock();
			//	totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
			//	int b;

			//}
			

		}

		// 更新本地视频实时检测
		if (3 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;

			if (num_Init_detector_FLAG == 1)
			{
				num_Init_detector = 0;
				num_Init_detector_FLAG = 0;
				CLEAR_VECTOR();
			}

			cap_local.read(img_mat); //shipin

			cv::flip(img_mat, img_mat, 0);
			IplImage SUM_image6_blob_temp = img_mat;
			IplImage *SUM_image6_blob = cvCloneImage(&SUM_image6_blob_temp);

			cv::flip(img_mat, img_mat, 0);
			cv::flip(img_mat, img_mat, 0);
			// 77 49
			img_mat = img_mat(Range(45, 340), Range(69, 503));
			cv::imwrite(path_image[num_Init_detector % 20], img_mat);


			if (num_Init_detector <= 11)
			{
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				GpuMat img_gpu_temp(img_mat);
				img_gpu[num_Init_detector] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[num_Init_detector], Descriptors[num_Init_detector]);
				num_Init_detector++;

				Sleep(100);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}

			if (num_Init_detector == 12)
			{
				num_Init_detector++;

				for (int aa = 0; aa < 4; aa++)
				{
					matcher.match(Descriptors[aa], Descriptors[aa + 8], matches, GpuMat());

					// 计算距离
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}

					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[aa][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[aa + 8][good_matches[i].trainIdx].pt);
					}

					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[aa], xformed, H_p, img_gpu[aa].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[aa + 8], xformed, save, GpuMat(), 0);


					save.download(save_cpu);

					cv::imwrite(descriptors_image[aa % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[aa] = cvCloneImage(&ipl_save_d);
					ipl_img[aa] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}


				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 60;
					keypoints_blob[i].pt.y += 10;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++], SUM_image6_blob);
			}
			if (num_Init_detector == 13)
			{
				num_Init_detector++;
				continue;

			}

			if (num_Init_detector == 14)
			{
				//Mat yyyy;
				cv::imwrite(path_image[19], img_mat);
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				// 开始清除数据！
				for (int i = 0; i < 10; i++)
				{
					img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
					KeyPoints[i] = KeyPoints[i + 1];
					Descriptors[i] = Descriptors[i + 1];
				}
				KeyPoints[11].clear();

				// di 11 每次将关键点和描述信息重新更新
				GpuMat img_gpu_temp(img_mat);
				img_gpu[11] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);

				for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
				{
					ipl_save[i] = ipl_save[i + 1];
				}

				{  // 重新计算
					matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
					// 计算距离
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}
					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
					}
					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

					save.download(save_cpu);

					cv::imwrite(descriptors_image[3 % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[3] = cvCloneImage(&ipl_save_d);
					ipl_img[3] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}
			
				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

				//去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 69;
					keypoints_blob[i].pt.y += 45;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++ % 13], SUM_image6_blob);
				SUM_image6_blob = rotateImage(SUM_image6_blob, 180, 1);
				//cvTranspose(SUM_image6_blob, SUM_image6_blob);

				if (QQQQ > 55)  DETECTION_FLAG = 0;


				Sleep(10);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete []pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}


		}

		// 邻域置信度分析
		if (4 == DETECTION_FLAG)
		{

			is_UpdatedisplayFlag = 1;

			if (num_Init_detector_FLAG == 1)
			{
				num_Init_detector = 0;
				num_Init_detector_FLAG = 0;
				CLEAR_VECTOR();
			}

			cap_local.read(img_mat); //shipin

			cv::flip(img_mat, img_mat, 0);
			IplImage SUM_image6_blob_temp = img_mat;
			IplImage *SUM_image6_blob = cvCloneImage(&SUM_image6_blob_temp);

			cv::flip(img_mat, img_mat, 0);
			cv::flip(img_mat, img_mat, 0);
			// 77 49
			img_mat = img_mat(Range(45, 340), Range(69, 503));
			cv::imwrite(path_image[num_Init_detector % 20], img_mat);


			if (num_Init_detector <= 11)
			{
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				GpuMat img_gpu_temp(img_mat);
				img_gpu[num_Init_detector] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[num_Init_detector], Descriptors[num_Init_detector]);
				num_Init_detector++;

				Sleep(100);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}

			if (num_Init_detector == 12)
			{
				num_Init_detector++;

				for (int aa = 0; aa < 4; aa++)
				{
					matcher.match(Descriptors[aa], Descriptors[aa + 8], matches, GpuMat());

					// 计算距离
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}

					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[aa][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[aa + 8][good_matches[i].trainIdx].pt);
					}

					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[aa], xformed, H_p, img_gpu[aa].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[aa + 8], xformed, save, GpuMat(), 0);


					save.download(save_cpu);

					cv::imwrite(descriptors_image[aa % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[aa] = cvCloneImage(&ipl_save_d);
					ipl_img[aa] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}


				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 60;
					keypoints_blob[i].pt.y += 10;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++], SUM_image6_blob);
			}
			if (num_Init_detector == 13)
			{
				num_Init_detector++;
				continue;

			}

			if (num_Init_detector == 14)
			{
				//Mat yyyy;
				cv::imwrite(path_image[19], img_mat);
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				// 开始清除数据！
				for (int i = 0; i < 10; i++)
				{
					img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
					KeyPoints[i] = KeyPoints[i + 1];
					Descriptors[i] = Descriptors[i + 1];
				}
				KeyPoints[11].clear();

				// di 11 每次将关键点和描述信息重新更新
				GpuMat img_gpu_temp(img_mat);
				img_gpu[11] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);

				for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
				{
					ipl_save[i] = ipl_save[i + 1];
				}

				{  // 重新计算
					matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
					// 计算距离
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}
					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
					}
					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

					save.download(save_cpu);

					cv::imwrite(descriptors_image[3 % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[3] = cvCloneImage(&ipl_save_d);
					ipl_img[3] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}

				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 69;
					keypoints_blob[i].pt.y += 45;
				}

				//if (CONFIDENCE_FLAG_NUM == 0 && keypoints_blob.size() == 4)
				if (CONFIDENCE_FLAG_NUM == 0 && keypoints_blob.size() >= 4)
				{
					for (int i = 0; i < keypoints_blob.size(); i++)
					{
						fram_confidence[i].x = keypoints_blob[i].pt.x;
						fram_confidence[i].y = keypoints_blob[i].pt.y;

					}
					CONFIDENCE_FLAG_NUM = keypoints_blob.size();
				}

				int ooooo = 0;
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					for (int j = 0; j < CONFIDENCE_FLAG_NUM; j++)
					{
						// 进行距离的约束！！！
						if ((fram_confidence[j].x - keypoints_blob[i].pt.x) * (fram_confidence[j].x - keypoints_blob[i].pt.x)
							+ (fram_confidence[j].y - keypoints_blob[i].pt.y) * (fram_confidence[j].y - keypoints_blob[i].pt.y) < 150)
						{
							//如果是在范围内就进行更新赋值
							fram_confidence[j].x = keypoints_blob[i].pt.x;
							fram_confidence[j].y = keypoints_blob[i].pt.y;
						}

						//if ((fram_confidence[j].x - keypoints_blob[i].pt.x) * (fram_confidence[j].x - keypoints_blob[i].pt.x)
						//	+ (fram_confidence[j].y - keypoints_blob[i].pt.y) * (fram_confidence[j].y - keypoints_blob[i].pt.y) > 200)
						//{
						//	ooooo++;
						//}

					    //如果都没有，证明这个是一个新点！
						if(ooooo == CONFIDENCE_FLAG_NUM)
						{
							//如果不在范围内，就进行
							fram_confidence[++CONFIDENCE_FLAG_NUM].x = keypoints_blob[i].pt.x;
							fram_confidence[++CONFIDENCE_FLAG_NUM].y = keypoints_blob[i].pt.y;
							ooooo = 0;
							// CONFIDENCE_FLAG_NUM = 0;
						}
					}

				}

				CvPoint *fram_dlphay = new CvPoint[2 * CONFIDENCE_FLAG_NUM];
				for (int i = 0; i < CONFIDENCE_FLAG_NUM; i++)
				{
					fram_dlphay[2 * i].x = fram_confidence[i].x - 10;
					fram_dlphay[2 * i].y = fram_confidence[i].y - 10;

					fram_dlphay[2 * i + 1].x = fram_confidence[i].x + 10;
					fram_dlphay[2 * i + 1].y = fram_confidence[i].y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				////刚开始肯定都是等于0的哈
				//if (CONFIDENCE_FLAG_NUM == 0)
				//{
				//	for (int i = 0; i < keypoints_blob.size(); i++)
				//	{
				//		fram_confidence[i] = fram_dlphay[2 * i];
				//		fram_confidence[i+1] = fram_dlphay[2 * i +1];
				//	}
				//	CONFIDENCE_FLAG_NUM = keypoints_blob.size();
				//}

				//for (int i = 0; i < keypoints_blob.size(); i++)
				//{
				//	for (int j = 0; j < CONFIDENCE_FLAG_NUM; j++)
				//	{
				//		if( ( ((fram_confidence[2 * j].x + fram_confidence[2 * j + 1].x )/2 - (fram_dlphay[2 * i].x + fram_dlphay[2 * i + 1].x)/2 ) * ((fram_confidence[2 * j].x + fram_confidence[2 * j + 1].x) / 2) ) )
				//	}

				//}

				////开始搜索
				//for (int i = 0; i < CONFIDENCE_FLAG_NUM; i++)
				//{

				//	cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				//}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++ % 13], SUM_image6_blob);
				SUM_image6_blob = rotateImage(SUM_image6_blob, 180, 1);
				//cvTranspose(SUM_image6_blob, SUM_image6_blob);


				if (QQQQ > 24)  CONFIDENCE_FLAG_NUM = 0;

				if (QQQQ > 55)  DETECTION_FLAG = 0;


				Sleep(10);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}


		}

		// 参数过拟合
		if (5 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;

			if (num_Init_detector_FLAG == 1)
			{
				num_Init_detector = 0;
				num_Init_detector_FLAG = 0;
				CLEAR_VECTOR();
			}

			cap_local.read(img_mat); //shipin

			cv::flip(img_mat, img_mat, 0);
			IplImage SUM_image6_blob_temp = img_mat;
			IplImage *SUM_image6_blob = cvCloneImage(&SUM_image6_blob_temp);

			cv::flip(img_mat, img_mat, 0);
			cv::flip(img_mat, img_mat, 0);
			// 77 49
			img_mat = img_mat(Range(45, 340), Range(69, 503));
			cv::imwrite(path_image[num_Init_detector % 20], img_mat);


			if (num_Init_detector <= 11)
			{
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				GpuMat img_gpu_temp(img_mat);
				img_gpu[num_Init_detector] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[num_Init_detector], Descriptors[num_Init_detector]);
				num_Init_detector++;

				Sleep(100);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}

			if (num_Init_detector == 12)
			{
				num_Init_detector++;

				for (int aa = 0; aa < 4; aa++)
				{
					matcher.match(Descriptors[aa], Descriptors[aa + 8], matches, GpuMat());

					// 计算距离
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}

					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[aa][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[aa + 8][good_matches[i].trainIdx].pt);
					}

					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[aa], xformed, H_p, img_gpu[aa].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[aa + 8], xformed, save, GpuMat(), 0);


					save.download(save_cpu);

					cv::imwrite(descriptors_image[aa % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[aa] = cvCloneImage(&ipl_save_d);
					ipl_img[aa] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}


				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 60;
					keypoints_blob[i].pt.y += 10;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++], SUM_image6_blob);
			}
			if (num_Init_detector == 13)
			{
				num_Init_detector++;
				continue;

			}

			if (num_Init_detector == 14)
			{
				//Mat yyyy;
				cv::imwrite(path_image[19], img_mat);
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				// 开始清除数据！
				for (int i = 0; i < 10; i++)
				{
					img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
					KeyPoints[i] = KeyPoints[i + 1];
					Descriptors[i] = Descriptors[i + 1];
				}
				KeyPoints[11].clear();

				// di 11 每次将关键点和描述信息重新更新
				GpuMat img_gpu_temp(img_mat);
				img_gpu[11] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);

				for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
				{
					ipl_save[i] = ipl_save[i + 1];
				}

				{  // 重新计算
					matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
					// 计算距离
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}
					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
					}
					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

					save.download(save_cpu);

					cv::imwrite(descriptors_image[3 % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[3] = cvCloneImage(&ipl_save_d);
					ipl_img[3] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}

				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 69;
					keypoints_blob[i].pt.y += 45;
				}

				//if (CONFIDENCE_FLAG_NUM == 0 && keypoints_blob.size() == 4)
				if (CONFIDENCE_FLAG_NUM == 0 && keypoints_blob.size() >= 2)
				{
					for (int i = 0; i < keypoints_blob.size(); i++)
					{
						fram_confidence[i].x = keypoints_blob[i].pt.x;
						fram_confidence[i].y = keypoints_blob[i].pt.y;

					}
					CONFIDENCE_FLAG_NUM = keypoints_blob.size();
				}

				int ooooo = 0;
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					for (int j = 0; j < CONFIDENCE_FLAG_NUM; j++)
					{
						// 进行距离的约束！！！
						if ((fram_confidence[j].x - keypoints_blob[i].pt.x) * (fram_confidence[j].x - keypoints_blob[i].pt.x)
							+ (fram_confidence[j].y - keypoints_blob[i].pt.y) * (fram_confidence[j].y - keypoints_blob[i].pt.y) < 200)
						{
							//如果是在范围内就进行更新赋值
							fram_confidence[j].x = keypoints_blob[i].pt.x;
							fram_confidence[j].y = keypoints_blob[i].pt.y;
						}

						//if ((fram_confidence[j].x - keypoints_blob[i].pt.x) * (fram_confidence[j].x - keypoints_blob[i].pt.x)
						//	+ (fram_confidence[j].y - keypoints_blob[i].pt.y) * (fram_confidence[j].y - keypoints_blob[i].pt.y) > 200)
						//{
						//	ooooo++;
						//}

						//如果都没有，证明这个是一个新点！
						if (ooooo == CONFIDENCE_FLAG_NUM)
						{
							//如果不在范围内，就进行
							fram_confidence[++CONFIDENCE_FLAG_NUM].x = keypoints_blob[i].pt.x;
							fram_confidence[++CONFIDENCE_FLAG_NUM].y = keypoints_blob[i].pt.y;
							ooooo = 0;
							// CONFIDENCE_FLAG_NUM = 0;
						}
					}

				}

				CvPoint *fram_dlphay = new CvPoint[2 * CONFIDENCE_FLAG_NUM];
				for (int i = 0; i < CONFIDENCE_FLAG_NUM; i++)
				{
					fram_dlphay[2 * i].x = fram_confidence[i].x - 10;
					fram_dlphay[2 * i].y = fram_confidence[i].y - 10;

					fram_dlphay[2 * i + 1].x = fram_confidence[i].x + 10;
					fram_dlphay[2 * i + 1].y = fram_confidence[i].y + 10;
					if (fram_dlphay[2 * i + 1].x > 390 )
					{
						cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
					}

				}

				////刚开始肯定都是等于0的哈
				//if (CONFIDENCE_FLAG_NUM == 0)
				//{
				//	for (int i = 0; i < keypoints_blob.size(); i++)
				//	{
				//		fram_confidence[i] = fram_dlphay[2 * i];
				//		fram_confidence[i+1] = fram_dlphay[2 * i +1];
				//	}
				//	CONFIDENCE_FLAG_NUM = keypoints_blob.size();
				//}

				//for (int i = 0; i < keypoints_blob.size(); i++)
				//{
				//	for (int j = 0; j < CONFIDENCE_FLAG_NUM; j++)
				//	{
				//		if( ( ((fram_confidence[2 * j].x + fram_confidence[2 * j + 1].x )/2 - (fram_dlphay[2 * i].x + fram_dlphay[2 * i + 1].x)/2 ) * ((fram_confidence[2 * j].x + fram_confidence[2 * j + 1].x) / 2) ) )
				//	}

				//}

				////开始搜索
				//for (int i = 0; i < CONFIDENCE_FLAG_NUM; i++)
				//{

				//	cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				//}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++ % 13], SUM_image6_blob);
				SUM_image6_blob = rotateImage(SUM_image6_blob, 180, 1);
				//cvTranspose(SUM_image6_blob, SUM_image6_blob);


				if (QQQQ > 22)  CONFIDENCE_FLAG_NUM = 0;

				if (QQQQ > 55)  DETECTION_FLAG = 0;


				Sleep(10);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}

		}

		// 连通域阈值
		if (6 == DETECTION_FLAG)
		{
			is_UpdatedisplayFlag = 1;


			if (num_Init_detector_FLAG == 1)
			{
				num_Init_detector = 0;
				num_Init_detector_FLAG = 0;
				CLEAR_VECTOR();
			}

			camera_img = cvQueryFrame(cap_camera);//捕获相机的视频帧，并进行解码操作
			if (camera_img->origin == IPL_ORIGIN_TL)//如果图片原点在左上角，将其沿X轴翻转，使左下角
			{
				cvFlip(camera_img, camera_img);
			}
			IplImage *SUM_image6_blob = camera_img;
			Mat SUM_image6_blob_mat;
			img_mat = cvarrToMat(camera_img);
			SUM_image6_blob_mat = cvarrToMat(camera_img);
			//img_mat = img_mat(Range(45, 340), Range(69, 503));
			cv::imwrite(path_image[num_Init_detector % 20], img_mat);

			if (num_Init_detector <= 11)
			{
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				GpuMat img_gpu_temp(img_mat);
				img_gpu[num_Init_detector] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[num_Init_detector], Descriptors[num_Init_detector]);
				num_Init_detector++;

				//Sleep(100);

				//更新显示
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, _Ilocal, IMAGE_NUM);

			}
			if (num_Init_detector == 12)
			{
				num_Init_detector++;

				for (int aa = 0; aa < 4; aa++)
				{
					matcher.match(Descriptors[aa], Descriptors[aa + 8], matches, GpuMat());

					// 计算距离
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[aa].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}

					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[aa][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[aa + 8][good_matches[i].trainIdx].pt);
					}

					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[aa], xformed, H_p, img_gpu[aa].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[aa + 8], xformed, save, GpuMat(), 0);


					save.download(save_cpu);

					cv::imwrite(descriptors_image[aa % 100], save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[aa] = cvCloneImage(&ipl_save_d);
					ipl_img[aa] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}


				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}

				cvSaveImage(savee_image[0], SUM_image);   // 累计的原始图像

														  //去噪
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				SUM_image2_quzao = SUM_image1;
				cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);
				cvSaveImage(savee_image[1], SUM_image1);
				SUM_image3_smooth = SUM_image1;

				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}

				cvSaveImage(savee_image[2], SUM_image3_smooth);

				SUM_image4_candy = SUM_image3_smooth;
				cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);
				cvSaveImage(savee_image[3], SUM_image4_candy);


				// blob
				params.filterByArea = true;
				params.minArea = 2;
				params.maxArea = 100;
				SimpleBlobDetector blobDetector(params);
				blobDetector.create("SimpleBlob");
				blobDetector.detect(SUM_image4_candy, keypoints_blob);

				//cvSaveImage(savee_image[4], SUM_image4_candy);

				for (unsigned int i = 0; i < keypoints_blob.size(); i++)
				{
					keypoints_blob[i].pt.x += 60;
					keypoints_blob[i].pt.y += 10;
				}

				CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
				for (int i = 0; i < keypoints_blob.size(); i++)
				{
					fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
					fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

					fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
					fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

					cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
				}

				//cvSaveImage(savee_image[5], SUM_image4_candy);
				//drawKeypoints(img_mat, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				// drawKeypoints(pRGBmat_flip, keypoints_blob, SUM_image5_blob, Scalar(0, 0, 255));
				//drawKeypoints(pRGBmat_flip, keypoints_blob, pRGBmat_flip, Scalar(0, 0, 255));
				//cv::imwrite(savee_image[4], SUM_image5_blob);

				cvSaveImage(savee_image[QQQQ++], SUM_image6_blob);
			}
			if (num_Init_detector == 13)
			{
				num_Init_detector++;
				continue;

			}

			// 优化的
			if (num_Init_detector == 14)
			{
				// 开始计时
				int a;
				clock_t start, finish;
				double totaltime;
				start = clock();
				/*************************************************************************************************************************************/
				cvtColor(img_mat, img_mat, CV_BGR2GRAY);
				// 开始清除数据！
				for (int i = 0; i < 10; i++)
				{
					img_gpu[i] = img_gpu[i + 1];  //将后一个压入前一个
					KeyPoints[i] = KeyPoints[i + 1];
					Descriptors[i] = Descriptors[i + 1];
				}
				KeyPoints[11].clear();

				// di 11 每次将关键点和描述信息重新更新
				GpuMat img_gpu_temp(img_mat);
				img_gpu[11] = img_gpu_temp;
				orb_gpu(img_gpu_temp, GpuMat(), KeyPoints[11], Descriptors[11]);

				for (int i = 0; i < 3; i++)  // 前3帧可以不动，只更新最后一帧。
				{
					ipl_save[i] = ipl_save[i + 1];
				}

				{  // 重新计算
					matcher.match(Descriptors[3], Descriptors[11], matches, GpuMat());
					// 计算距离
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						double dist = matches[i].distance;
						if (i == 0)
						{
							min_dist = dist;
							max_dist = dist;
						}
						if (dist < min_dist) min_dist = dist;
						if (dist > max_dist) max_dist = dist;
					}

					// 获得好的匹配点
					for (int i = 0; i < Descriptors[3].rows; i++)
					{
						if (matches[i].distance < COEFF_DISTANCE * max_dist)
						{
							good_matches.push_back(matches[i]);
						}
					}
					for (size_t i = 0; i < good_matches.size(); ++i)
					{
						// get the keypoints from the good matches
						obj.push_back(KeyPoints[3][good_matches[i].queryIdx].pt);
						scene.push_back(KeyPoints[11][good_matches[i].trainIdx].pt);
					}
					const Mat H_p = findHomography(obj, scene, CV_RANSAC);
					gpu::warpPerspective(img_gpu[3], xformed, H_p, img_gpu[3].size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(0), Stream::Null());
					Mat save_cpu;
					Mat img_cpu;
					xformed.download(save_cpu);
					gpu::threshold(xformed, img_A, 0, 255, 0);
					gpu::subtract(img_gpu[11], xformed, save, GpuMat(), 0);

					save.download(save_cpu);

					// 优化部分
					finish = clock();
					totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
					//cv::imwrite("E:\\地面站\\DllUseDemo(debug_x64)(优化速度)\\test.jpg", save_cpu);

					img_A.download(img_cpu);

					IplImage ipl_save_d = save_cpu;
					IplImage ipl_img_d = img_cpu;
					ipl_save[3] = cvCloneImage(&ipl_save_d);
					//ipl_img[3] = cvCloneImage(&ipl_img_d);

					// 清除向量！！！
					CLEAR_VECTOR();
				}

				SUM_image = cvCreateImage(cvGetSize(ipl_save[0]), IPL_DEPTH_8U, 1);
				for (y = 0; y<SUM_image->height; y++)
					for (x = 0; x<SUM_image->width; x++)
					{
						c = ((uchar*)(ipl_save[0]->imageData + ipl_save[0]->widthStep*y))[x] + ((uchar*)(ipl_save[1]->imageData + ipl_save[1]->widthStep*y))[x]
							+ ((uchar*)(ipl_save[2]->imageData + ipl_save[2]->widthStep*y))[x] + ((uchar*)(ipl_save[3]->imageData + ipl_save[3]->widthStep*y))[x];

						if (c<255)
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						else
						{
							c = 255;
							((uchar*)(SUM_image->imageData + SUM_image->widthStep*y))[x] = (uchar)c;
						}
					}
				SUM_TEMP = SUM_image;
				SUM_image1 = SUM_image;
				cvMorphologyEx(SUM_image, SUM_image1, SUM_TEMP, NULL, //default 3*3  
					CV_MOP_OPEN, //CV_MOP_CLOSE,
					1);
				//SUM_image2_quzao = SUM_image1;
				//cvSmooth(SUM_image1, SUM_image2_quzao, CV_GAUSSIAN, 3, 3, 0, 0);



				SUM_image3_smooth = SUM_image1;
				//去除
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = (SUM_image3_smooth->width - 20); x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(20); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = (SUM_image3_smooth->height - 10); y<(SUM_image3_smooth->height); y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}
				for (y = 0; y<20; y++)
					for (x = 0; x<(SUM_image3_smooth->width); x++)
					{
						c = ((uchar*)(SUM_image3_smooth->imageData + y * SUM_image3_smooth->widthStep))[x];
						if (c > 0)
						{
							SUM_image3_smooth->imageData[y*SUM_image3_smooth->widthStep + x] = 0;
						}
					}





			    cvThreshold(SUM_image3_smooth, SUM_image4_candy, 145, 255, THRESH_BINARY);//通过阈值操作把灰度图变成二值图  

				Mat findcontoursImg = cvarrToMat(SUM_image4_candy);
				vector<vector<cv::Point>> contours;
				cv::findContours(findcontoursImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);


				// 寻找最大连通域  
				double maxArea = 0;
				vector<cv::Point> maxContour;
				for (size_t i = 0; i < contours.size(); i++)
				{
					double area = cv::contourArea(contours[i]);
					if (area > maxArea)
					{
						maxArea = area;
						maxContour = contours[i];
					}
				}
				// 将轮廓转为矩形框  
				cv::Rect maxRect = cv::boundingRect(maxContour);

				for (size_t i = 0; i < contours.size(); i++)
				{
					cv::Rect r = cv::boundingRect(contours[i]);
					cv::rectangle(SUM_image6_blob_mat, r, cv::Scalar(0,0,255),2);
				}
				//SUM_image4_candy = SUM_image3_smooth;
				//cvCanny(SUM_image3_smooth, SUM_image4_candy, 250, 150, 3);

				SUM_image6_blob = &IplImage(SUM_image6_blob_mat);

				if(0)
				{
					// blob
					blobDetector.detect(SUM_image4_candy, keypoints_blob);
					CvPoint *fram_dlphay = new CvPoint[2 * keypoints_blob.size()];
					for (int i = 0; i < keypoints_blob.size(); i++)
					{
						fram_dlphay[2 * i].x = keypoints_blob[i].pt.x - 10;
						fram_dlphay[2 * i].y = keypoints_blob[i].pt.y - 10;

						fram_dlphay[2 * i + 1].x = keypoints_blob[i].pt.x + 10;
						fram_dlphay[2 * i + 1].y = keypoints_blob[i].pt.y + 10;

						cvRectangle(SUM_image6_blob, fram_dlphay[2 * i], fram_dlphay[2 * i + 1], cvScalar(0, 0, 255, 0), 1, 8, 0);
					}
				}

				//更新显示

				//SUM_image6_blob = SUM_image;
				IMAGE_WIDTH = SUM_image6_blob[0].width;
				IMAGE_HEIGHT = SUM_image6_blob[0].height;
				IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
				//IMAGE_NUM = IMAGE_WIDTH * IMAGE_HEIGHT * 1;
				delete[]pRGB;
				pRGB = new unsigned char[IMAGE_NUM];
				//_Ilocal = (unsigned char *)SUM_image6_blob[0].imageData;
				memcpy(pRGB, (unsigned char *)SUM_image6_blob[0].imageData, IMAGE_NUM);

				/*************************************************************************************************************************************/

				finish = clock();
				totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
				//finish = clock();
				//totaltime = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
				int b;

			}
		}

		//更新对话框
		if (1 == is_UpdatedisplayFlag)
		{
			is_UpdatedisplayFlag = 0;
			CDC* pDC = g_pView->GetDC();
			BITMAPINFOHEADER bmi;
			CRect rec;
			CRect desrec;
			memset(&bmi, 0, sizeof(bmi));
			bmi.biSize = sizeof(BITMAPINFOHEADER);
			bmi.biBitCount = 24; // 彩色图像显示
			//bmi.biBitCount = 8; // 灰度图像显示
			bmi.biWidth = IMAGE_WIDTH;
			bmi.biHeight = IMAGE_HEIGHT;
			bmi.biCompression = BI_RGB;
			bmi.biPlanes = 1;
			bmi.biSizeImage = IMAGE_NUM;
			pDC->SetStretchBltMode(COLORONCOLOR);
			g_pView->GetClientRect(&rec);

			memcpy((void*)&desrec, (void*)&rec, sizeof(CRect));
			desrec.right = rec.right;
			desrec.bottom = rec.bottom;
			desrec.left = rec.left;
			desrec.top = rec.top;

			DrawDibBegin(
				hdd,
				pDC->m_hDC,
				desrec.right,
				desrec.bottom,
				(LPBITMAPINFOHEADER)(&bmi),
				IMAGE_WIDTH,
				IMAGE_HEIGHT,
				0
			);

			DrawDibRealize(hdd, pDC->m_hDC, FALSE);
			DrawDibDraw(
				hdd,
				pDC->m_hDC,
				desrec.left,
				desrec.top,
				desrec.Width(),
				desrec.Height(),
				(LPBITMAPINFOHEADER)(&bmi),
				pRGB,
				0,
				0,
				IMAGE_WIDTH,
				IMAGE_HEIGHT,
				0
			);

			DrawDibEnd(hdd);
			g_pView->ReleaseDC(pDC);
		}
		else
		{
			Sleep(1000);
			continue;
		}
		
	}
	// 清空数据
	delete []pRGB;
	DrawDibClose(hdd);
	_endthread();
}
/**************************************************************************************/
/*******************************添加代码结束*******************************************/
/**************************************************************************************/

/**************************************************************************************/
/***************************此处为添加的代码******************************************/
/**************************************************************************************/
void Thread_DLL(void* param)
{



	_endthread();
}
/**************************************************************************************/
/*******************************添加代码结束*******************************************/
/**************************************************************************************/

void CDllUseDemoView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//获取主框架窗口的指针
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (pFrame->m_bFullScreen == TRUE)
	{
		//调用主窗口类的自定义函数 EndFullScreen ,便可退出全屏显示状态
		pFrame->EndFullScreen();
	}
	else
	{
		pFrame->OnFullScreen();
	}
}


void CDllUseDemoView::Ondehaze()
{
	dehazeflag = 1;
	detectionflag = 0;
}


void CDllUseDemoView::Onhuifu()
{
	dehazeflag = 0;
}


void CDllUseDemoView::Detector_CAMERA()
{
	//dehazeflag = 0;
	//detectionflag = 1;
	DETECTION_FLAG = 1;
	num_Init_detector_FLAG = 1;
}
void CDllUseDemoView::Detector_ORB_CAMERA()
{
	//dehazeflag = 1;
	//detectionflag = 1;
	DETECTION_FLAG = 2;
	num_Init_detector_FLAG = 1;
}
void CDllUseDemoView::Detector_ORB_LOCAL()
{
	//dehazeflag = 1;
	//detectionflag = 1;
	DETECTION_FLAG = 3;
	num_Init_detector_FLAG = 1;
}

void CDllUseDemoView::Detector_ANA_CONFIDENCE()
{
	//dehazeflag = 1;
	//detectionflag = 1;
	DETECTION_FLAG = 4;
	num_Init_detector_FLAG = 1;
}

// 连通域 Detector_CONNECTED_DOMAIN
// hogsvm Detector_HOGSVM
// Detector_HOUGH
void CDllUseDemoView::Detector_CONNECTED_DOMAIN()
{
	DETECTION_FLAG = 6;
	num_Init_detector_FLAG = 1;
}
void CDllUseDemoView::Detector_HOGSVM()
{

}
void CDllUseDemoView::Detector_HOUGH()
{

}
// 跟踪
// Tracker_tld
// Tracker_meanshift
// Tracker_particle
// Tracker_mtilti
void CDllUseDemoView::Tracker_tld()
{

}
void CDllUseDemoView::Tracker_meanshift()
{

}
void CDllUseDemoView::Tracker_particle()
{

}
void CDllUseDemoView::Tracker_mtilti()
{

}
// 识别
// Detector_yolo
// Detector_hog_svm
void CDllUseDemoView::Detector_yolo()
{

}
void CDllUseDemoView::Detector_hog_svm()
{

}


void CDllUseDemoView::Detector_PARA_OVERFIT()
{
	//dehazeflag = 1;
	//detectionflag = 1;
	DETECTION_FLAG = 5;
	num_Init_detector_FLAG = 1;
}

void correct_border(unsigned char* src)
{
	cv::setUseOptimized(true);
	Mat input = Mat::zeros(1920, 1080, CV_8UC3);
	Mat output = Mat::zeros(1920, 1080, CV_8UC3);
	input.data = src;
	Mat image_gray, outimage;
	clock_t gray1, gray2, thres2, merge1, merge2, addt;
	gray1 = clock();
	cvtColor(input, image_gray, COLOR_RGB2GRAY);
	gray2 = clock();
	threshold(image_gray, outimage, 15, 255, 1);
	thres2 = clock();
	vector<Mat> channels;
	merge1 = clock();
	for (int i = 0; i<3; i++)
	{
		channels.push_back(outimage);
	}

	merge(channels, output);
	merge2 = clock();
	addWeighted(input, 1, output, 1, 0, input);
	addt = clock();
	int gray, thres, merget, addtime;
	gray = gray2 - gray1;
	thres = thres2 - gray2;
	merget = merge2 - merge1;
	addtime = addt - merge2;
}
NO_FUNCTURN f_correct_DlphaY(unsigned char* src)
{
	for (int i = 0; i<1920 * 1080 * 3; i += 3)
	{
		if (src[i]<15 && src[i + 1]<15 && src[i + 2]<15)
			src[i] = src[i - 1] = src[i + 1] = 255;
	}
}

NO_FUNCTURN f_dehaze_Init_DlphaY()
{
	if (hinst != NULL)
	{
		pfFuncInDll = (Dehaze)GetProcAddress(hinst, "IM_HazeRemovalBasedOnDarkChannelPrior");
	}
}

NO_FUNCTURN f_dehaze_DlphaY(unsigned char* src)
{

	if (pfFuncInDll != NULL)
	{
		//去雾参数
		//clock_t start,end;
		//start=clock();
		int d = pfFuncInDll((unsigned char*)pRGB, (unsigned char*)pRGB, 640, 480, stride, 10, 60, 240, 0.9F, 0.1F, 0.9F);
		//end=clock();
		//int time=end-start;
	}

}

/**************************************************************************************/
/*******************************    检测    *******************************************/
/**************************************************************************************/
void CLEAR_VECTOR()
{
	//KeyPoints_1.clear();
	//KeyPoints_2.clear();
	matcher.clear();
	matches.clear();
	good_matches.clear();
	obj.clear();
	scene.clear();
}


void f_gpu_init()
{
	int num_devices = gpu::getCudaEnabledDeviceCount();
	//cout << num_devices << endl;
	if (num_devices <= 0)
	{
		std::cerr << "There is no device." << std::endl;
		//return -1;
	}
	int enable_device_id = -1;
	for (int i = 0; i < num_devices; i++)
	{
		cv::gpu::DeviceInfo dev_info(i);
		if (dev_info.isCompatible())
		{
			enable_device_id = i;
		}
	}
	if (enable_device_id < 0)
	{
		std::cerr << "GPU module isn't built for GPU" << std::endl;
		//return -1;
	}
	gpu::setDevice(enable_device_id);
}

NO_FUNCTURN f_conv_UncharPTR_to_Gpumat(unsigned char * Src)
{
	Mat temp_mat;

}