#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_DEVICES 128 // Maximum number of input devices to scan

int is_keyboard(int fd) {
    char name[256];
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
        perror("Unable to get device name");
        return 0;
    }

    printf("Device Name: %s\n", name);
    // Check if the device name contains the string "keyboard" or "kbd"
    if (strstr(name, "keyboard") != NULL || strstr(name, "kbd") != NULL || strstr(name, "KeyBoard") != NULL) {
        return 1; // It's a keyboard
    }

    return 0; // Not a keyboard
}

int find_keyboard_device(char *device_path) {
    DIR *dir = opendir("/dev/input");
    if (dir == NULL) {
        perror("Unable to open /dev/input directory");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Look for event devices with names like "eventX"
        if (strncmp(entry->d_name, "event", 5) == 0) {
            snprintf(device_path, 256, "/dev/input/%s", entry->d_name);
            
            int fd = open(device_path, O_RDONLY);
            if (fd == -1) {
                continue; // Unable to open, skip to next device
            }

            // Check if this device is a keyboard
            if (is_keyboard(fd)) {
                close(fd);
                return 1; // Keyboard found
            }

            close(fd);
        }
    }

    closedir(dir);
    return 0; // No keyboard found
}

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0); // Returns 1 if file exists, 0 if not
}

void run_paplay_in_background(const char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: run paplay command
        system(command);
        exit(0); // Exit the child process after running the command
    } else if (pid > 0) {
        // Parent process: continue execution without waiting for the child
        return;
    } else {
        // Fork failed
        perror("Failed to fork a child process");
    }
}

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Usage: %s <sound-file>\n", argv[0]);
        return -1;
    }

    // Check if the file exists
    if (!file_exists(argv[1])) {
        printf("Error: File does not exist: %s\n", argv[1]);
        return -1;
    }

    char device_path[256];
    char command[512];
    snprintf(command, sizeof(command), "sh -c 'paplay %s' > /dev/null &", argv[1]);

    // Automatically find a keyboard device
    if (!find_keyboard_device(device_path)) {
        printf("No keyboard device found.\n");
        return -1;
    }

    printf("Found keyboard device: %s\n", device_path);

    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Unable to open keyboard device");
        return -1;
    }

    struct input_event ev;
    while (1) {
        ssize_t bytesRead = read(fd, &ev, sizeof(struct input_event));
        
        if (bytesRead == -1) {
            perror("Error reading from event device");
            break;
        }

        if (bytesRead == sizeof(struct input_event)) {
            // Check if the event type is EV_KEY (key press/release)
            if (ev.type == EV_KEY) {
                if (ev.value == 1) {
                    //printf("Key Pressed: %d\n", ev.code); // Key press
                    run_paplay_in_background(command);
                }
            }
        } else {
            printf("Unexpected bytes read: %zd\n", bytesRead);
        }
    }

    close(fd);
    return 0;
}
