//
//

#include "stdafx.h"
///******************************************************************************
//*** orb特征点的运动目标检测 ***
//******************************************************************************/
//
//
//
////检测类
//#include "Gpu_orb_DlphaY.h"
//
//#include <iostream> 
//#include "opencv2/core/core.hpp"   
//#include "opencv2/features2d/features2d.hpp"   
//#include "opencv2/highgui/highgui.hpp"   
//#include "opencv2/legacy/legacy.hpp"
//#include <iostream>   
//#include <vector>  
//#include <time.h>
//// GPU
//#include "opencv2/gpu/gpu.hpp"
//#include "opencv2/nonfree/gpu.hpp"
//
//
///******************************************************************************
//*** 命名空间 ***
//******************************************************************************/
//
//using namespace cv;
//using namespace std;
//using namespace cv::gpu;
//
////DLPHAY
////#include "gpu_dlphay.h"
//
///******************************************************************************
//*** orb检测 ***
//******************************************************************************/
//
//#define GRAYSCALE  0
//
////orb匹配点系数
//#define COEFF_DISTANCE 0.63
//
////累计计算的帧数
//#define COUNT_sumNUM  3
//
////要检测几次？
//#define COUNT_jiance_NUM  10
//
////间隔帧数
//#define INTERVAL_IMAGE  9
//
////是否是第一次的检测   是0   不是1
//#define FIRST_IS    0
//#define FIRST_NOT    1
//
///******************************************************************************
//*** 图像本地路径 ***
//******************************************************************************/
//
//char *path_image[] = {
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3702.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3705.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3708.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3711.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3712.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3713.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3714.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3715.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3716.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3717.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3718.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3719.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3720.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3721.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3722.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3723.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3724.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3725.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3726.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3727.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3728.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3729.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3730.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3731.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3732.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3733.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3734.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3735.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3736.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3737.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3738.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3739.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3740.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3741.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3742.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3743.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3744.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3745.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3746.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3747.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3748.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3749.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3750.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3751.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3752.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3753.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3754.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3755.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3756.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3757.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3758.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3759.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3760.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3761.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3762.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3763.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3764.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3765.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3766.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3767.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3768.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3769.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3770.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3771.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3772.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3773.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3774.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3775.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3776.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3777.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3778.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3779.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3780.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3781.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3782.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3783.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3784.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3785.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3786.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3787.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3788.jpg","E:\\asn_test\\data\\capture\\images5\\A005.mpg3789.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\A005.mpg3790.jpg"
//};
//
//char *result_image[] = {
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3702.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3705.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3708.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3711.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3712.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3713.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3714.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3715.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3716.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3717.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3718.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3719.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3720.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3721.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3722.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3723.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3724.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3725.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3726.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3727.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3728.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3729.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3730.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3731.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3732.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3733.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3734.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3735.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3736.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3737.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3738.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3739.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3740.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3741.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3742.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3743.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3744.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3745.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3746.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3747.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3748.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3749.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3750.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3751.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3752.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3753.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3754.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3755.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3756.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3757.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3758.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3759.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3760.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3761.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3762.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3763.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3764.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3765.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3766.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3767.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3768.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3769.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3770.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3771.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3772.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3773.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3774.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3775.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3776.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3777.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3778.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3779.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3780.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3781.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3782.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3783.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3784.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3785.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3786.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3787.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3788.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3789.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\A005.mpg3790.jpg"
//};
//
//char *sum_image[] = {
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3702.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3705.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3708.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3711.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\SUM.mpg3712.jpg"
//};
//
//char *quzao_image[] = {
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3700.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3701.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3702.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3703.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3704.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3705.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3706.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3707.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3708.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3709.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3710.jpg","E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3711.jpg",
//	"E:\\asn_test\\data\\capture\\images5\\orb\\result\\QUZAO.mpg3712.jpg"
//};
//
///******************************************************************************
//*** orb相关变量定义 ***
//******************************************************************************/
//
//ORB_GPU orb_gpu;
//vector<KeyPoint> KeyPoints_1, KeyPoints_2;
//GpuMat Descriptors_1, Descriptors_2;
//BruteForceMatcher_GPU<Hamming> matcher;
//vector<DMatch> matches;
//double max_dist;   //计算距离
//double min_dist;
//vector<DMatch> good_matches;
//std::vector<Point2f> obj;
//std::vector<Point2f> scene;
//
///******************************************************************************
//*** 图像相关变量定义 ***
//******************************************************************************/
//
//int I_count = 0;
//GpuMat xformed;
//GpuMat save;
//GpuMat img;
//IplImage* ipl_save[10];
//IplImage* ipl_img[10];
//IplImage* SUM_image;
//IplImage* SUM_image1;
//IplImage* SUM_TEMP;
//IplImage* SUM_image2_quzao;  //去噪
//IplImage* SUM_image3_smooth;  //平滑
//IplImage* SUM_image4_candy;   //边缘
//cv::Mat SUM_image5_blob;      //blob
//IplImage* ipl_save_TEMP;  //用于检测临时存放
//
//						  /******************************************************************************
//						  *** 去噪尺度计算 ***
//						  ******************************************************************************/
//
//int SCALE_NOISE;
//int STRIDE_SLIDE_WINDOWS;
//IplImage* BGR_image_cpu;
//int COUNT;
//int red_point_x;
//int red_point_y;
//
///******************************************************************************
//*** 检测坐标点计算变量定义 ***
//******************************************************************************/
//int n = 0;
//CvPoint pt1, pt2;
//CvPoint pt3, pt4;
//CvPoint pt5, pt6;
//CvPoint pt7, pt8;
//
///******************************************************************************
//*** 其他变量定义 ***
//******************************************************************************/
//
//int is_first = FIRST_IS;  //第一次
//int x, y, z, t, c;   //循环变量定义
//
//					 //sift 跟踪
//struct feature * fffff;
//
///******************************************************************************
//*** 链表内存释放 ***
//******************************************************************************/
//
//void CLEAR_VECTOR()
//{
//	KeyPoints_1.clear();
//	KeyPoints_2.clear();
//	matcher.clear();
//	matches.clear();
//	good_matches.clear();
//	obj.clear();
//	scene.clear();
//}
//
///******************************************************************************
//*** gpu初始化 ***
//******************************************************************************/
//
//void gpu_init()
//{
//	int num_devices = gpu::getCudaEnabledDeviceCount();
//	//cout << num_devices << endl;
//	if (num_devices <= 0)
//	{
//		std::cerr << "There is no device." << std::endl;
//		//return -1;
//	}
//	int enable_device_id = -1;
//	for (int i = 0; i < num_devices; i++)
//	{
//		cv::gpu::DeviceInfo dev_info(i);
//		if (dev_info.isCompatible())
//		{
//			enable_device_id = i;
//		}
//	}
//	if (enable_device_id < 0)
//	{
//		std::cerr << "GPU module isn't built for GPU" << std::endl;
//		//return -1;
//	}
//	gpu::setDevice(enable_device_id);
//}