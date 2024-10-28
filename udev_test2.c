/*******************************************
* udev test
*******************************************/
#include <sys/select.h>
#include <sys/time.h>

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libudev.h"

#define BLOCK_SIZE 512

int
main(int argc, char **argv)
{
	int monitor = 0;
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
	struct udev_monitor *mon;
	int fd;

	if ((argc == 2) && (strcmp(argv[1], "-m") == 0)) {
		monitor = 1;
	}

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	if (monitor) {
		/* This section sets up a monitor which will report events when
		   devices attached to the system change.  Events include "add",
		   "remove", "change", "online", and "offline".

		   This section sets up and starts the monitoring. Events are
		   polled for (and delivered) later in the file.

		   It is important that the monitor be set up before the call to
		   udev_enumerate_scan_devices() so that events (and devices) are
		   not missed.  For example, if enumeration happened first, there
		   would be no event generated for a device which was attached after
		   enumeration but before monitoring began.

		   Note that a filter is added so that we only get events for
		   "hidraw" devices. */

		/* Set up a monitor to monitor hidraw devices */
		mon = udev_monitor_new_from_netlink(udev, "udev");
		udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
		udev_monitor_enable_receiving(mon);
		/* Get the file descriptor (fd) for the monitor.
		   This fd will get passed to select() */
		fd = udev_monitor_get_fd(mon);
	}

	/* Create a list of the devices. */
	enumerate = udev_enumerate_new(udev);
//	NOTE: list all devices
//	udev_enumerate_add_match_subsystem(enumerate, "input");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	if (!devices) {
		fprintf(stderr, "Failed to get device list.\n");
		return 1;
	}
	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices)
	{
		const char *path, *tmp;
		unsigned long long disk_size = 0;
		unsigned short int block_size = BLOCK_SIZE;

		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		printf("I: DEVNODE=%s\n", udev_device_get_devnode(dev));
		printf("I: KERNEL=%s\n", udev_device_get_sysname(dev));
		printf("I: DEVPATH=%s\n", udev_device_get_devpath(dev));
//		TODO: udev_device_get_devtype cause SIGSEGV
//		printf("I: DEVTYPE=%s\n", udev_device_get_devtype(dev));

		tmp = udev_device_get_sysattr_value(dev, "size");
		if (tmp)
			disk_size = strtoull(tmp, NULL, 10);

		tmp = udev_device_get_sysattr_value(dev, "queue/logical_block_size");
		if (tmp)
			block_size = atoi(tmp);

		printf("I: DEVSIZE=");
		if (strncmp(udev_device_get_sysname(dev), "sr", 2) != 0)
			printf("%lld GB\n", (disk_size * block_size) / 1000000000);
		else
			printf("n/a\n");

		printf("-----\n");

		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	if (monitor) {
		/* Begin polling for udev events. Events occur when devices
		   attached to the system are added, removed, or change state.
		   udev_monitor_receive_device() will return a device
		   object representing the device which changed and what type of
		   change occured.

		   The select() system call is used to ensure that the call to
		   udev_monitor_receive_device() will not block.

		   The monitor was set up earler in this file, and monitoring is
		   already underway.

		   This section will run continuously, calling usleep() at the end
		   of each pass. This is to demonstrate how to use a udev_monitor
		   in a non-blocking way. */
		while (1) {
			/* Set up the call to select(). In this case, select() will
			   only operate on a single file descriptor, the one
			   associated with our udev_monitor. Note that the timeval
			   object is set to 0, which will cause select() to not
			   block. */
			fd_set fds;
			struct timeval tv;
			int ret;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			tv.tv_sec = 0;
			tv.tv_usec = 0;

			ret = select(fd + 1, &fds, NULL, NULL, &tv);

			/* Check if our file descriptor has received data. */
			if (ret > 0 && FD_ISSET(fd, &fds)) {
				printf("\nselect() says there should be data\n");

				/* Make the call to receive the device.
				   select() ensured that this will not block. */
				dev = udev_monitor_receive_device(mon);
				if (dev) {
					printf("Got Device\n");
					printf("   Node: %s\n",
					    udev_device_get_devnode(dev));
					printf("   Subsystem: %s\n",
					    udev_device_get_subsystem(dev));
					// printf("   Devtype: %s\n",
					// udev_device_get_devtype(dev));

					printf("   Action: %s\n",
					    udev_device_get_action(dev));
					udev_device_unref(dev);
				} else {
					printf("No Device from receive_device(). An "
					       "error occured.\n");
				}
			}
			usleep(250 * 1000);
			printf(".");
			fflush(stdout);
		}
	}

	udev_unref(udev);

	return 0;
}
