/* User-space application for AD9850 Direct Digital Synthesizer */
/* Steven J. Merrifield */
/* June 2003 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define XTAL_FREQ	20480000
#define DDS_IOC_MAGIC   'k'
#define DDS_IOCTFREQ	_IO(DDS_IOC_MAGIC,3)

int main(void)
{
	int fd;
	unsigned int freq, delta;
	char device[80] = "/dev/dds";
	
	printf("Enter freq: ");
	scanf("%d",&freq);

	delta = (freq * 4294967296) / XTAL_FREQ;
	if ((fd = open(device,O_WRONLY)) < 0) { 
		printf("Failed to open %s\n",device); exit(-1); }

	if (ioctl(fd, DDS_IOCTFREQ, delta) != 0)
		printf("ioctl failed -  %s\n",strerror(errno));
	close(fd);
	return(0);
}

