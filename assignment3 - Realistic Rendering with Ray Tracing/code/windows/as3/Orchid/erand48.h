
#pragma once


#ifndef ERAND48
#define ERAND48



	void _dorand48(unsigned short xseed[3]);

	double erand48(unsigned short xseed[3]);

	double drand48(void);

	long lrand48(void);

	long nrand48(unsigned short xseed[3]);

	long mrand48(void);

	long jrand48(unsigned short xseed[3]);

	void srand48(long seed);

	unsigned short *seed48(unsigned short xseed[3]);

	void lcong48(unsigned short p[7]);

#endif //  ERAND48

