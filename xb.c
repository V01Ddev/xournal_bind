#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

void emit(int fo, int type, int key, int value){
    struct input_event event;
    gettimeofday(&event.time, NULL);
    event.type = type;
    event.code = key;
    event.value = value;
    write(fo, &event, sizeof(event));
}


int main(){

    struct js_event {
        __u32 time;     /* event timestamp in milliseconds */
        __s16 value;    /* value */
        __u8 type;      /* event type */
        __u8 number;    /* axis/button number */
    };
    
    int fd = open("/dev/input/event3", O_RDONLY);
    struct js_event e;

    int of = open ("/dev/uinput", O_WRONLY | O_NONBLOCK);
    struct uinput_setup usetup;

    if (of < 0){
        printf("Unable to open /dev/uinput/\n");
        return -1;
    } 
    else if (fd < 0){

        printf("Unable to open /dev/\n");
        return -1;
    }

    ioctl(of, UI_SET_EVBIT, EV_KEY);
    ioctl(of, UI_SET_KEYBIT, KEY_LEFTCTRL);
    ioctl(of, UI_SET_KEYBIT, KEY_7);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1;
    usetup.id.product = 0x1;
    strcpy(usetup.name, "Virtual Keyboard");

    ioctl(of, UI_DEV_SETUP, &usetup);
    ioctl(of, UI_DEV_CREATE);

    for (int i=0; i<=3; i++){
        read(fd, &e, sizeof(e));

        printf("----------\n");
        printf("value --> %d\ntype --> %d\nnumber --> %d\n", e.value, e.type, e.number);
        printf("----------\n");

        sleep(1);
    }

    read(fd, &e, sizeof(e));
    return 0;
}
