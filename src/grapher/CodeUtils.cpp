#include "CodeUtils.h"

double StepChange(double val, double tol, double goal)
{
	double newval = goal;
	double Diff = goal - val;
	if(fabs(Diff) > tol)
	{
		newval = val + Sign(Diff) * tol;
	}
	return newval;
}

double Random()
{
	return (double) random() / RAND_MAX;
}

double Hypot(double x, double y)
{
	return sqrt(pow(x, 2) + pow(y, 2));
}

double ToDegrees(double rad)
{
	return rad / M_PI * 180.0;
}

double ToRadians(double rad)
{
	return rad * M_PI / 180.0;
}

double Sign(double val)
{
	if(val < 0) return -1;
	if(val > 0) return 1;
	return 0;
}

Vector2d Normalize(double x, double y)
{
	double h = Hypot(x, y);
	Vector2d returned;
	returned.x = x / h;
	returned.y = y / h;
	return returned;
}

bool ApplyJoystickDeadZone(double* y, double margin)
{
	if (fabs(*y) < margin)
	{
		*y = 0;
		return true;
	}
	else
	{
		int sign = Sign(*y);
		*y = fabs(*y);
		*y = sign * (*y - margin) / (1 - margin);
		return false;
	}
}

bool ApplyJoystickDeadZone(double* x, double* y, double margin)
{
	double X = *x;
	double Y = *y;
	double h = Hypot(X, Y);
	if(h < margin)
	{
		*x = 0;
		*y = 0;
		return true;
	}
	else
	{
		double normal_x = X / h;
		double normal_y = Y / h;
		*x = X - normal_x * margin;
		*y = Y - normal_y * margin;
		return false;
	}
}
double BasicPositionControl(double Error, double Start_Slope, double Close_Enough, double MinSpeed, double MaxSpeed, int* counter)
{
	double AAE = fabs(Error);
	double Speed = 0;
	if(AAE < Start_Slope)
	{
		if(counter != NULL)
		{
			if(AAE < Close_Enough)
			{
				*counter = *counter + 1;
			}
			else
			{
				*counter = 0;	
			}
		}
		if(AAE < Close_Enough)
		{
			Speed = 0;
		}
		else
		{
			Speed = MinSpeed + ((AAE - Close_Enough) / (Start_Slope - Close_Enough)) * (MaxSpeed - MinSpeed);						
		}
	}
	else
	{
		Speed = MaxSpeed;
	}
	return Speed * Sign(Error);
}

byte* ReadWholeFile(char* pathname)
{
	int fd = open(pathname, O_RDONLY);
	if(fd < 0)
	{
		printf("ReadFile: Could not open pathname \"%s\", (errno == %i)\n", pathname, errno);
		return NULL;
	}
	struct stat s;
	stat(pathname, &s);
	off_t filesize = s.st_size;
	byte* file_buffer = (byte*) malloc(filesize);
	read(fd, (char*) file_buffer, filesize);
	close(fd);
	return file_buffer;
}
void RemoveComments(char* root)
{
	int offset = 0;
	for(int i = 0;i<strlen(root) && offset<strlen(root);i++)
	{
		switch(root[i])
		{
			case '/':
			{
				if(root[i + 1] == '/')
				{
					while(root[i] != '\n')
					{
						i++;
					}
				}
				else if(root[i + 1] == '*')
				{
					while(!(root[i-2] == '*' && root[i-1] == '/'))
					{
						i++;
					}
				}
				break;
			}
		}
		root[offset] = root[i];
		offset++;
	}
	root[offset] = '\0';
}
template <typename T>
T Limit(T min, T val, T max) 
{
	if(min > val) return min;
	if(max < val) return max;
	return val;
} 

template int Limit<int>(int a, int b, int c);
template double Limit<double>(double a, double b, double c);


















