#include "WGS84ToAngle.h"

const double wgs84_a = 6378137, wgs84_f = 1/298.257223563; /* WGS84 */
struct geod_geodesic g;


/**
 * @brief 通过WGS84两组经纬度坐标求方位角 变为局部坐标
 * @param start_lon：输入的初始纬度
 * @param start_lat：输入的初始经度
 * @param stop_lon： 输入的终止纬度
 * @param stop_lat： 输入的终止经度
 * @param s12：      两点间的距离   单位为m
 * @param azi1：     两点间的方位角
 * @param azi2：     两点间的方位角
 * @return 两点间的方位角
 */
WGS84_axis_t GPStoXY(double start_lat, double start_lon, double stop_lat, double stop_lon)
{
	WGS84_axis_t WGS84_axis;
	double angle, azi1, azi2, s12;
	
	WGS84_axis.origin_lat = start_lat;
	WGS84_axis.origin_lon = start_lon;
	
	geod_inverse(&g, start_lat,start_lon, stop_lat, stop_lon, &s12, &azi1, &azi2);
	
	WGS84_axis.length =  s12;
	angle = (azi1 / 180) * PI;
	WGS84_axis.x = s12 * sin(angle);
	WGS84_axis.y = s12 * cos(angle);
//  printf("%.15f %.15f \n",  WGS84_axis.x, WGS84_axis.y);
	return WGS84_axis;
}

/*根据起点终点求线的方程 k与b*/
Line_straight_param_t line_fun(double start_x,double start_y,double dest_x,double dest_y)
{
	Line_straight_param_t Line_straight_param = {0};
	
	double dy = dest_y - start_y;
	double dx = dest_x - start_x;
	
	if (fabs(dx) < 0.0001) 
	{
		if(dy > 0) 					 Line_straight_param.k = 9999999;
		else if(dy < 0)      Line_straight_param.k = -9999999;
		else                 Line_straight_param.k = 0;
	}
	else                   Line_straight_param.k = dy / dx; 
	
	Line_straight_param.b = start_y - Line_straight_param.k * start_x;
	
	return Line_straight_param;
}

/*根据方程 k与b 和 一个点，求过此点的垂线方程*/
Line_straight_param_t vertical_line_fun(double x,double y,double k,double b)
{
	Line_straight_param_t Line_straight_param = {0};
	
	double k1 = 1.0/k;
	Line_straight_param.k = 0 - k1;
	Line_straight_param.b = y+ k1 * x;
	
	return Line_straight_param;
}

/*求两线的交点*/
Line_inter_point_t Line_inter_point_math(double k1,double b1,double k2,double b2)
{
	Line_inter_point_t Line_inter_point = {0};
	/*计算点至线的距离*/              
	double denominator = k1 - k2;
	double numerator = b2 - b1;
	Line_inter_point.x = numerator / denominator;
	Line_inter_point.y = k1 * Line_inter_point.x + b1;
	
	return Line_inter_point;
}


/*调试过程中使用，判断是否与目标值是否误差很小*/
//static int checkEquals(double x, double y, double d) {
//  if (fabs(x - y) <= d)
//    return 0;
//  printf("checkEquals fails: %.7g != %.7g +/- %.7g\n", x, y, d);
//  return 1;
//}

