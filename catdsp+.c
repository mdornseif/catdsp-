/* 
 *  
 *  CATDSP
 *  
 *  Opens the the sound card (ie. /dev/dsp) sets the sample rate and 
 *  sends audio out to STDOUT.
 *  
 *  Copyright (C) 2000 Timothy Pozar pozar@lns.com
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  Patched up to catdsp+.c by drt@un.bewaff.net
 *  
 *  $Id: catdsp+.c,v 1.1 2001/07/10 20:47:33 robotnik Exp $
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
/*#include <machine/soundcard.h>*/
#include <sys/soundcard.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

#define RATE 32000

void banner();

char *device=NULL;

int main(int argc, char **argv)
{
	int stereo = TRUE;
	int rate = RATE;
	long seconds = -1;
	int gotmask, len, test;
	int devfh;
	int i=0;
	char buf[BUFSIZ];
	char c;

	while ((c=getopt(argc,argv,"d:r:msh")) != EOF)
	{
		switch(c)
		{
			case 'D':
			case 'd':
				device = optarg;
				break;
			case 'R':
			case 'r':
				rate = atoi(optarg);
				break;
			case 'M':
			case 'm':
				stereo = 0;
				break;
			case 'S':
			case 's':
            	seconds = atoi(optarg);
				break;
			case 'H':
			case 'h':
				banner();
				return 0;
		}
	}

	if (!device)
		device = "/dev/dsp";

	if ((devfh = open(device, O_RDONLY)) == -1)
	{
		fprintf(stderr,"opening device %s: %s",device,strerror(errno));
      	return 1;
	}

	/* What formats does this device support? */
	if(ioctl(devfh, SNDCTL_DSP_GETFMTS, &gotmask) == -1)
	{
		perror("get dsp mask");
		return -1;
	}

   /* Set the number or channels (ie mono vs. stereo)...
      Always set stereo/mono first before sample rate. 
      See http://www.4front-tech.com/pguide/audio.html for details. */
   test = stereo;
	if(ioctl(devfh, SNDCTL_DSP_STEREO, &stereo) == -1)
	{
		perror("Tried to set dsp to mono or stereo");
		exit(1);
	}

	if (stereo != test)
	{
		if(stereo)
			fprintf(stderr,"Tried to set dsp to mono but it only supports stereo.\n");
		else
			fprintf(stderr,"Tried to set dsp to stereo but it only supports mono.\n");
		exit(1);
	}

   /* Set the sample rate... */
   test = rate;
   if(ioctl( devfh, SNDCTL_DSP_SPEED, &test) == -1){
      perror("set sample rate");
      exit(1);
   }
   if(rate != test){
      printf("Could not set the sample rate to: \"%i\". \"%i\" was returned\n",
         rate, test);
   }

   /* read the device and copy to stdout (1) ... */
	while((len = read(devfh, &buf, BUFSIZ))>0)
	{ 
      /* this is much to hackish - we should use SIGALRM */
		i += len / ((1 + stereo) * 2);
		if(i >= rate)
		{
			i -= rate;
			seconds--;
		}

		if(seconds == 0)
			return 0;

		if(write(1, &buf, len) == -1)
		{
			perror("Trouble writing to STDOUT.");
			return -1;
		}
	}

	return 0;
}

void banner()
{
   printf("catdsp: Set \"%s\" sample rate and stereo/mono mode,\n",device);
   printf("        and then read the device and send it to STDOUT.\n");
   printf("   -d device   Sets device name.  Default is \"%s\".\n",device);
   printf("   -m          Sets device to run in mono.  Default is stereo.\n");
   printf("   -r rate     Sets device sample rate.  Default is \"%d\"",RATE);
   printf("   -s S        Reads just S seconds of audio, then quit\n");
   return;
}

