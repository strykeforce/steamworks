#ifndef CODEUTILS_H
#define CODEUTILS_H
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdint>

typedef unsigned char byte;

#define ssleep(__SECONDS__) msleep(__SECONDS__ * 1000);

#define msleep(__MILLISECONDS__) usleep(__MILLISECONDS__ * 1000);

double StepChange(double val, double tol, double goal);
double Random();
double Hypot(double x, double y);
double ToDegrees(double rad);
double ToRadians(double deg);
double Sign(double val);
struct Vector2d {
  double x;
  double y;
};
Vector2d Normalize(double x, double y);
bool ApplyJoystickDeadZone(double* y, double margin);
bool ApplyJoystickDeadZone(double* x, double* y, double margin);
enum OutputFilter { Everything, ImportantVarible, Warning, Fatal, Nothing };
double BasicPositionControl(double Error, double Start_Slope,
                            double Close_Enough, double MinSpeed,
                            double MaxSpeed, int* counter = NULL);
byte* ReadWholeFile(char* pathname);
void RemoveComments(char* orginal);
template <typename T>
T Limit(T a, T b, T c);
#endif
