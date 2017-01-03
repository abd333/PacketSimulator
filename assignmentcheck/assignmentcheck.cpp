// assignmentcheck.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define TOS 10
#define MAXNOSRC 3
#define MAXQSIZE 50
#define MAXNOE 10
double arrst[MAXNOSRC];
double patq[MAXQSIZE];
double dpst;//departure st
double iat; 
double st;
double simclock;
double delay;
double tdelay;
double avgdelay;
double plr;
double tput;
double smallest;
double tqsize;
double last_check;
double load;
double npd;
double npa;
double tpl;
int sinkStatus;
int ssrc;
int evtype;
int cqsize;
int noe;
int maxqsize;
/*functions prototype*/
double traffic(void);
void init(void);
void updateclock(void);
void scheduler(void);
void arrival(void);
void departure(void);
void result(void);
int main(void);
FILE *out_tdelay; //output file pointer for total delay: total_delay.csv
FILE *out_avgdelay; //output file pointer for average delay: average_delay.csv
FILE *out_plr; //output file pointer for packet loss ratio: packet_loss_ratio.csv
FILE *out_tput;// output file pointer for throughput ; thoroughput.csv
double traffic(void)
/*poisson distribution use to generate traffic*/
{
	double x = rand();
	double iat = 0;
	iat = -log(x / 1.0e+30) / load;
	return iat;
}
void init(void)
{
	int i;
	load += 5;
	arrst[MAXNOSRC] = 0.0;
	patq[MAXQSIZE] = 0.0;
	for (i = 0; i<MAXNOSRC; ++i)
		arrst[i] = rand();
	dpst = 1.0e+30;
	iat = 0.5;
	st = 5.0;
	simclock = 0.0;
	smallest = 1.0e+30;
	avgdelay = 0.0;
	plr = 0.0;
	tput = 0.0;
	tdelay = 0.0;
	npd = 0.0;
	npa = 0.0;
	tpl = 0.0;
	sinkStatus = 0;
	ssrc = 0;
	evtype = 1; //event type (1: arrival; 2: departure)
	cqsize = 0;
	maxqsize += 50;
	tqsize = 0.0;
} //end init
void updateclock(void)
/*Advance the simulation clock*/
{
	simclock = smallest; //assign current smallest time to simclock
}
void scheduler(void)
/*Determine the event type of the next event to occur*/
{
	int i;
	smallest = 1.0e+30;
	for (i = 0; i<MAXNOSRC; ++i)
	{
		if (arrst[i]<smallest)
		{
			smallest = arrst[i];
			ssrc = i;
			evtype = 1; //event type is ARRIVAL
		}
		if (dpst<smallest)
		{
			smallest = dpst;
			evtype = 2; //event type is DEPARTURE
		}
	} //end for
} //end scheduler
void arrival(void)
{
	++npa;
	iat = traffic();
	arrst[ssrc] = simclock + iat;
	if (sinkStatus == 0)
	{
		sinkStatus = 1;
		dpst = simclock + st;
	}
	else if (sinkStatus == 1)
	{
		if (cqsize == MAXQSIZE)
			++tpl;
		else if (cqsize<MAXQSIZE)
		{
			patq[cqsize] = simclock;
			++cqsize;
		} //end else
	} //end elseif
} //end arrival
void departure(void)
{
	++npd;
	if (cqsize == 0)
	{
		sinkStatus = 0;
		dpst = 1.0e+20;
	}
	else if (cqsize != 0)
	{
		delay = simclock - patq[1];
		tdelay += delay;
		for (int i = 0; i<MAXQSIZE; ++i)
			patq[i] = patq[i + 1];
		dpst = simclock + st;
		--cqsize;
	}//end elseif
} //end departure
void result(void)
/*Compute and write estimates of desired measures of performance*/
{
	avgdelay = tdelay / npd;
	plr = tpl / npa;
	tput = npd / npa;
	fprintf(out_tdelay, "%f,%f\n", load, tdelay);
	fprintf(out_avgdelay, "%f,%f\n", load, avgdelay);
	fprintf(out_plr, "%f,%f\n", load, plr);
	fprintf(out_tput, "%f,%f\n", load, tput);
} //end result
int main(void)
{
	out_tdelay = fopen("total_delay.csv", "w");
	out_avgdelay = fopen("average_delay.csv", "w");
	out_plr = fopen("packet_loss_ratio.csv", "w");
	out_tput = fopen("throughput.csv", "w");
	for (noe = 0; noe<MAXNOE; ++noe) //represent the repeatition of experiments
	{
		init();
		//repeatition for one individually experiment
		while (npd<TOS) //TOS based on npd
		{
			scheduler(); //invoke scheduler function
			updateclock(); //invoke updateclock function
			if (evtype == 1)
				arrival(); //invoke arrival function
			else if (evtype == 2)
				departure(); //invoke departure function
		} //end while
		result(); //invoke result function
	} //end for
	fclose(out_tdelay);
	fclose(out_avgdelay);
	fclose(out_plr);
	fclose(out_tput);
	
	return 0;
} //end main
