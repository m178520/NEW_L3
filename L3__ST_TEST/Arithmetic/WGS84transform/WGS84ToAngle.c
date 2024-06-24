#include "WGS84ToAngle.h"

const double wgs84_a = 6378137, wgs84_f = 1/298.257223563; /* WGS84 */
struct geod_geodesic g;


/**
 * @brief ͨ��WGS84���龭γ��������λ�� ��Ϊ�ֲ�����
 * @param start_lon������ĳ�ʼγ��
 * @param start_lat������ĳ�ʼ����
 * @param stop_lon�� �������ֹγ��
 * @param stop_lat�� �������ֹ����
 * @param s12��      �����ľ���   ��λΪm
 * @param azi1��     �����ķ�λ��
 * @param azi2��     �����ķ�λ��
 * @return �����ķ�λ��
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

/*��������յ����ߵķ��� k��b*/
Line_straight_param_t line_fun(double start_x,double start_y,double dest_x,double dest_y)
{
	Line_straight_param_t Line_straight_param = {0};
	
	double dy = dest_y - start_y;
	double dx = dest_x - start_x;
	
	if (fabs(dx) < 0.0001) Line_straight_param.k = 99999;
	else                   Line_straight_param.k = dy / dx; 
	
	Line_straight_param.b = start_y - Line_straight_param.k * start_x;
	
	return Line_straight_param;
}

/*���ݷ��� k��b �� һ���㣬����˵�Ĵ��߷���*/
Line_straight_param_t vertical_line_fun(double x,double y,double k,double b)
{
	Line_straight_param_t Line_straight_param = {0};
	
	double k1 = 1.0/k;
	Line_straight_param.k = 0 - k1;
	Line_straight_param.b = y+ k1 * x;
	
	return Line_straight_param;
}

/*�����ߵĽ���*/
Line_inter_point_t Line_inter_point_math(double k1,double b1,double k2,double b2)
{
	Line_inter_point_t Line_inter_point = {0};
	/*��������ߵľ���*/              
	double denominator = k1 - k2;
	double numerator = b2 - b1;
	Line_inter_point.x = numerator / denominator;
	Line_inter_point.y = k1 * Line_inter_point.x + b1;
	
	return Line_inter_point;
}


/*���Թ�����ʹ�ã��ж��Ƿ���Ŀ��ֵ�Ƿ�����С*/
static int checkEquals(double x, double y, double d) {
  if (fabs(x - y) <= d)
    return 0;
  printf("checkEquals fails: %.7g != %.7g +/- %.7g\n", x, y, d);
  return 1;
}

