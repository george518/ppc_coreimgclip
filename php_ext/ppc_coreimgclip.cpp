/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
extern "C"{
	#ifdef HAVE_CONFIG_H
	#include "config.h"
	#endif

	#include "php.h"
	#include "php_ini.h"
	#include "ext/standard/info.h"
	#include "php_ppc_coreimgclip.h"
}

/*
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ppc_coreimgclip.h"
*/

#include <unistd.h>
#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <math.h>
#include <time.h>

using namespace cv;
using namespace std;

/* If you declare any globals in php_ppc_coreimgclip.h uncomment this:*/
ZEND_DECLARE_MODULE_GLOBALS(ppc_coreimgclip)


/* True global resources - no need for thread safety here */
/*static int le_ppc_coreimgclip;*/
long N = 10;
int width,height;//原图宽高
int R0,G0,B0;
int R, G, B;
Point posXY = Point(0,0);//切图位置

//计算两张图像复制位置
Point copyPosion(int x0,int y0,int x1,int y1,int pos){
	int y = y1-y0;
	int x = x1-x0;
	switch (pos)
	{
		case 1:
			//水平居中，垂直居中
			posXY.x = (int)(x/2);
			posXY.y = (int)(y/2);
			break;
		case 2:
			//水平居中 （靠上）
			posXY.x = (int)(x/2);
			posXY.y = 0;
			break;
		case 3:
			//水平居中 （靠下）
			posXY.x = (int)(x/2);
			posXY.y = y;
			break;
		case 4:
			//垂直居中 (靠左)
			posXY.x = 0;
			posXY.y = (int)(y/2);
			break;
		case 5:
			//垂直居中 (靠右)
			posXY.x = x;
			posXY.y = (int)(y/2);
			break;
		case 6:
			//左上角
			posXY.x = 0;
			posXY.y = 0;
			break;
		case 7:
			//右上角
			posXY.x = x;
			posXY.y = 0;
			break;
		case 8:
			//左下角
			posXY.x = 0;
			posXY.y = y;
			break;

		case 9:
			//右下角
			posXY.x = x;
			posXY.y = y;
			break;
		default:
			//默认居中
			posXY.x = (int)(x/2);
			posXY.y = (int)(y/2);
		break;
	}
	return posXY;
}

bool isDiffColor(int R1,int G1,int B1){
	if(abs(R0-R1)>=N || abs(G0-G1)>=N || abs(B0-B1)>=N){
		return true;
	}
	return false;
}

int topPoint(Mat &img){

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			R = (int)img.at<Vec3b>(i,j)[0];
			G = (int)img.at<Vec3b>(i,j)[1];
			B = (int)img.at<Vec3b>(i,j)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return 0;
}

int leftPoint(Mat &img){
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			R = (int)img.at<Vec3b>(j,i)[0];
			G = (int)img.at<Vec3b>(j,i)[1];
			B = (int)img.at<Vec3b>(j,i)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return 0;
}

int rightPoint(Mat &img){

	for (int i = width-1; i >= 0; i--)
	{
		for (int j = 0; j < height; j++)
		{
			R = (int)img.at<Vec3b>(j,i)[0];
			G = (int)img.at<Vec3b>(j,i)[1];
			B = (int)img.at<Vec3b>(j,i)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}

	return width;
}

int bottomPoint(Mat &img){

	for (int i = height-1; i >= 0; i--)
	{
		for (int j = 0; j < width; j++)
		{
			R = (int)img.at<Vec3b>(i,j)[0];
			G = (int)img.at<Vec3b>(i,j)[1];
			B = (int)img.at<Vec3b>(i,j)[2];
			if(isDiffColor(R,G,B)){
				return i;
			}
		}
	}
	return height;
}

//切图代码
PHP_FUNCTION(ppc_coreimgclip)
{
	//用户输入
	char *source_path = NULL;
	char *dest_path = NULL;
	int  source_len, dest_len;
	long dest_height, dest_width;
	long margin;
	long pos;

	//其他参数
	Mat img,coreImg;

	//获取用户输入
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"sslllll",
		&source_path, &source_len,
		&dest_path, &dest_len,
		&dest_width,
		&dest_height,
		&margin,
		&pos,
		&N) == FAILURE) {
		return;
	}

	img = cv::imread( source_path ,-1);
	if( !img.data ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fail to load image from %s", source_path);
		RETURN_FALSE;
	}

	//左上角像素作为底图的颜色
	R0 = (int)img.at<Vec3b>(0,0)[0];
	G0 = (int)img.at<Vec3b>(0,0)[1];
	B0 = (int)img.at<Vec3b>(0,0)[2];

	//原图的宽高
	width  = img.cols;
	height = img.rows;

	//获取核心图坐标数据
	    int top = topPoint(img);
	    int left = leftPoint(img);
	    int right = rightPoint(img);
	    int bottom = bottomPoint(img);

	    //计算核心图片的宽高
	    int coreWidth = right-left;
	    int coreHeight = bottom-top;

	    //计算可用宽高
	    int activeWidth = dest_width-2*margin;
	    int activeHeight = dest_height-2*margin;

	    //缩放后的核心图片宽和高
	    int resizeCoreWidth,resizeCoreHeight;

	    //计算实际图像大小
	    if(coreHeight>coreWidth){
	    	//如果高大于宽
	    	resizeCoreHeight = activeHeight;
	    	resizeCoreWidth = resizeCoreHeight * coreWidth / coreHeight;
	    	//如果比例算出来的宽比可用宽度大,则以宽度为准
	    	if(resizeCoreWidth>activeWidth){
	    		resizeCoreWidth = activeWidth;
	    		resizeCoreHeight = resizeCoreWidth * coreHeight / coreWidth;
	    	}
	    }else{
	    	//同理
	    	resizeCoreWidth = activeWidth;
	    	resizeCoreHeight = resizeCoreWidth * coreHeight / coreWidth;
	    	if(resizeCoreHeight>activeHeight){
	    		resizeCoreHeight = activeHeight;
	    		resizeCoreWidth = resizeCoreHeight * coreWidth / coreHeight;
	    	}
	    }

	    //截取核心图 共用img内存
	    coreImg = img(Rect(left,top,coreWidth,coreHeight));


	    //缩放图片
	    Mat resizeImg(dest_width,dest_height,CV_8UC4,cv::Scalar(R0,G0,B0));
	    resize(coreImg,resizeImg,Size(resizeCoreWidth,resizeCoreHeight),0,0,INTER_LINEAR);

	    //计算合成图片的坐标
	    copyPosion(resizeCoreWidth,resizeCoreHeight,activeWidth,activeHeight,1);
	    //合成图片
	    Mat outImg(dest_height,dest_width,resizeImg.type(),cv::Scalar(R0,G0,B0));
	    resizeImg.copyTo(outImg(cv::Rect((posXY.x+margin),(posXY.y+margin),resizeCoreWidth,resizeCoreHeight)));

		try{
			cv::imwrite(dest_path, outImg);
		}
		catch (exception &e)
		{
			php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
			RETURN_FALSE;
		}

	    //释放资源
		img.release();
		coreImg.release();
		outImg.release();
		resizeImg.release();

		RETURN_TRUE;
}




/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ppc_coreimgclip.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ppc_coreimgclip_globals, ppc_coreimgclip_globals)
    STD_PHP_INI_ENTRY("ppc_coreimgclip.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ppc_coreimgclip_globals, ppc_coreimgclip_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_ppc_coreimgclip_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_ppc_coreimgclip_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "ppc_coreimgclip", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_ppc_coreimgclip_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ppc_coreimgclip_init_globals(zend_ppc_coreimgclip_globals *ppc_coreimgclip_globals)
{
	ppc_coreimgclip_globals->global_value = 0;
	ppc_coreimgclip_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ppc_coreimgclip)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ppc_coreimgclip)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ppc_coreimgclip)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ppc_coreimgclip)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ppc_coreimgclip)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ppc_coreimgclip support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ ppc_coreimgclip_functions[]
 *
 * Every user visible function must have an entry in ppc_coreimgclip_functions[].
 */
const zend_function_entry ppc_coreimgclip_functions[] = {
	PHP_FE(ppc_coreimgclip,NULL)
	PHP_FE(confirm_ppc_coreimgclip_compiled,NULL)
	PHP_FE_END	/* Must be the last line in ppc_coreimgclip_functions[] */
};
/* }}} */

/* {{{ ppc_coreimgclip_module_entry
 */
zend_module_entry ppc_coreimgclip_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ppc_coreimgclip",
	ppc_coreimgclip_functions,
	PHP_MINIT(ppc_coreimgclip),
	PHP_MSHUTDOWN(ppc_coreimgclip),
	PHP_RINIT(ppc_coreimgclip),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ppc_coreimgclip),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ppc_coreimgclip),
	PHP_PPC_COREIMGCLIP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PPC_COREIMGCLIP
ZEND_GET_MODULE(ppc_coreimgclip)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
