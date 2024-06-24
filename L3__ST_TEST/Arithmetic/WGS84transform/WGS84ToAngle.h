#ifndef __WGS84TOANGLE_H__
#define __WGS84TOANGLE_H__

#include "geodesic.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265

typedef struct {
	double length;
	double origin_lat;
	double origin_lon;
	double x;
	double y;
}WGS84_axis_t;	

typedef struct
{
	double k;
	double b;
}Line_straight_param_t;

typedef struct
{
	double x;
	double y;
}Line_inter_point_t;

extern const double wgs84_a, wgs84_f; /* WGS84 */
extern struct geod_geodesic g;

WGS84_axis_t GPStoXY(double start_lon, double start_lat, double stop_lon, double stop_lat);
Line_straight_param_t line_fun(double start_x,double start_y,double dest_x,double dest_y);
Line_straight_param_t vertical_line_fun(double x,double y,double k,double b);
Line_inter_point_t Line_inter_point_math(double k1,double b1,double k2,double b2);

#endif

