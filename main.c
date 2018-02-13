#include <libinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#define INTERVAL 100 // in ms
#define THRESHOLD 10 

int find (char *src, char *tgt) {
	int pos = 0;
	char tmp;
	int slen = strlen(src);
	int tlen = strlen(tgt);
	for (; pos <= slen-tlen+1; pos++){
		tmp = src[pos+tlen];
		src[pos+tlen] = 0;
		if (strcmp(src+pos, tgt) == 0) {
			src[pos+tlen] = tmp;
			return pos;
		}
		src[pos+tlen] = tmp;
	}
	return -1;
}

int scan_device (int ev) {
	FILE *f = fopen("/proc/bus/input/devices", "r");
	char s[1024];
	int x, pos, y;
	while (fgets(s, 1024, f) != NULL) {
		if (s[0] == 'H') {
			pos = find(s, "event");
			sscanf(s+pos, "event%d", &x);
		} else if (s[0] == 'B') {
			sscanf(s+3, "EV=%x", &y);
			if (y == ev) {
				return x;
			}
		}
	}
	return -1;
}

unsigned long timestamp () {
	struct timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

int open_restricted (const char *path, int flags, void *user_data) {
	int fd = open(path, O_RDONLY | O_NONBLOCK);
	return fd;
}

int close_restricted (int fd, void *user_data) {
	close(fd);
}

void do_left() {
	system("xdotool key alt+ctrl+Right");
}

void do_right() {
	system("xdotool key alt+ctrl+Left");
}

void clean_old_events (struct libinput *li, int direction) {
	dispatch(li);
	while (1) {
		struct libinput_event *ev = libinput_get_event(li);
		if (ev == NULL || libinput_event_get_type(ev) != LIBINPUT_EVENT_POINTER_AXIS || 
				libinput_event_pointer_get_axis_value(ev, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)*direction < THRESHOLD){
			libinput_event_destroy(ev);
			break;
		}
		libinput_event_destroy(ev);
		dispatch(li);
	}
}

inline void handle_pointer_motion (struct libinput_event *);

void handle_pointer_motion (struct libinput_event *ev) {
	enum libinput_pointer_axis_source src = libinput_event_pointer_get_axis_source(ev);
	double axis_value = libinput_event_pointer_get_axis_value(ev, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
	static unsigned long last_time = 0, t = 0;
	t = timestamp();
	if (axis_value > THRESHOLD) {
		if (t > last_time+INTERVAL) {
			do_right();
			last_time = timestamp();
			clean_old_events(libinput_event_get_context(ev), 1);
		}
	} else if (axis_value < -THRESHOLD) {
		if (t > last_time+INTERVAL) {
			do_left();
			last_time = timestamp();
			clean_old_events(libinput_event_get_context(ev), -1);
		}
	}
	libinput_event_destroy(ev);
}

int dispatch (struct libinput *li) {
	struct epoll_event ep[32];
	epoll_wait(libinput_get_fd(li), ep, 32, -1);
	libinput_dispatch(li);
}


int main () {
	int dev = scan_device(0x0b);
	if (dev == -1) {
		exit(-1);
	}
	char dev_str[50];
	sprintf(dev_str, "/dev/input/event%d", dev);

	struct libinput_interface interface = {
		open_restricted,
		close_restricted
	};
	struct libinput *li = libinput_path_create_context(&interface, NULL);
	struct libinput_device *device = libinput_path_add_device(li, dev_str);
	struct libinput_event *ev;
	unsigned long last_left = 0, last_right = 0, t = 0;

	libinput_device_ref(device);
	libinput_ref(li);

	dispatch(li);
	while (1) {
		ev = libinput_get_event(li);
		if (ev == NULL) {
			libinput_event_destroy(ev);
			dispatch(li);
			continue;
		}
		switch (libinput_event_get_type(ev)) {
			case LIBINPUT_EVENT_POINTER_AXIS:
				handle_pointer_motion(ev);
				break;
		}
		dispatch(li);
	}
}
