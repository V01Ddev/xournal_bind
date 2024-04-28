#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

void find_active_events(int *working_events){

    char dp[] = "/dev/input/event";

    int Count = 0;

    while (Count < 100){

        char path[19];

        strcpy(path, dp);

        char strCount[2];

        sprintf(strCount, "%d", Count);
        strcat(path, strCount);

        if (access(path, F_OK) == 0){
            printf("%s exsists!\n", path);
            int fd = open(path, O_RDONLY);

            if (fd < 0){
            }
            else{
                working_events[Count] = Count;
                close(fd);
            }
            Count++;
        }

        else{
            printf("Couldn't access %s\n", path);
            Count++;
        }
    }
}



void emit(int fo, int type, int key, int value){
    struct input_event event;
    gettimeofday(&event.time, NULL);
    event.type = type;
    event.code = key;
    event.value = value;
    write(fo, &event, sizeof(event));
}


int main(){

    struct input_event {
        struct timeval time;
        unsigned short type;
        unsigned short code;
        unsigned int value;
    };

    // Taking care of events and opening them...
    printf("Finding events\n");

    int active_events[99]= {0};
    find_active_events(active_events);

    int fd;

    int events_array_size = (int) sizeof(active_events)/sizeof(int);
    for (int i=0; i<events_array_size; i++){
        printf("%d, ", active_events[i]);
    }


    // Opening all events that have been found
    char dp[] = "/dev/input/event";
    // Code ignores event0, very unlikely it is the keyboard...

    for (int i=0; i <= events_array_size; i++){

        if (active_events[i] != 0){
            char path[19];

            strcpy(path, dp);

            char strNum[2];
            sprintf(strNum, "%d", active_events[i]);
            strcat(path, strNum);

            printf("opening %s\n", path);

            open(path, O_RDONLY);
        }
    }


    // uinput setup
    struct input_event e;

    int of = open ("/dev/uinput", O_WRONLY | O_NONBLOCK);
    struct uinput_setup usetup;

    if (of < 0){
        printf("Unable to open /dev/uinput\n");
        return -1;
    } 
    else if (fd < 0){
        printf("Unable to open /dev/event\n");
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

    // reading all inputs passed to fd
    while (1) {
        read(fd, &e, sizeof(e));

        printf("----------\n");
        printf("value --> %d\ntype --> %d\ncode --> %d\n", e.value, e.type, e.code);
        printf("----------\n");
        sleep(1);
    }

    read(fd, &e, sizeof(e));

    close(fd);
    return 0;
}
