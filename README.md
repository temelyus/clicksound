# ClickSound

<img style="padding-top: 0;" width="96" align="right" src="https://static.wikia.nocookie.net/minecraft/images/6/63/Note_Block_animate.gif" alt="ICON">

ClickSound is a simple Linux application that detects keyboard events and plays a sound (e.g., `pop.mp3`) every time a key is pressed. It runs in the background and provides an interactive way to add sound feedback to your typing experience.

**Note to Developers:**

I chose C for this project because, after extensive testing with other languages such as Go, Rust, Python, and Ruby, none of their libraries were capable of performing this task reliably. Despite my efforts with multiple approaches, no solution yielded consistent results. This is why I ultimately decided to go with C – it simply works, and that’s what matters.

## Features

- Automatically detects a connected keyboard device.
- Plays a sound file (e.g., `pop.mp3`) every time a key is pressed.
- Runs the sound-playing process in the background, allowing the main program to continue detecting events.
- Supports any `.mp3` or `.wav` sound file.

## Requirements

- Linux-based operating system.
- `paplay` command-line tool (part of the `pulseaudio-utils` package).
- Access to `/dev/input` devices (may require root or group permissions).
  
## Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/the-abra/clicksound.git
   cd clicksound
   ```

2. **Compile the Program**:
   ```bash
   gcc -o clicksound clicksound.c
   ```

3. **Install Dependencies**:
   Make sure you have `paplay` installed on your system. If it’s not installed, you can do so via your package manager:
   ```bash
   sudo apt install pulseaudio-utils
   ```

4. **Grant Permissions** (Optional):
   You may need to run the program as root or add your user to the `input` group to access input devices:
   ```bash
   sudo usermod -aG input $USER
   ```

## Usage

To run the program and play a sound every time a key is pressed, execute the following command:

```bash
./clicksound sounds/pop.mp3
```

Replace `/path/to/sounds/pop.mp3` with the actual path to your desired sound file.

### Example:

```bash
nohup ./clicksound sounds/pop.mp3 &> /dev/null & # Works on background
```
```bash
kill $(ps -aux | grep clicksound | head -n1 | awk '{print $2}') # kill the clicksound
```

### CLI Setup

```bash
sudo ln -s $(pwd)/clicksound-cli.sh /usr/local/bin/clicksound-cli
clicksound-cli help
```

### File Structure

```
clicksound/
├── clicksound.c        # Source code for the ClickSound program
├── clicksound          # Pre-Compiled version of source code
├── sounds/
│   └── pop.mp3         # Sound to be played on key press
└── README.md           # Project documentation
```

### Key Features

- **Keyboard Event Detection**: Automatically detects keyboard devices in the `/dev/input` directory and listens for key events.
- **Sound Playback**: The sound file is played using the `paplay` command in the background whenever a key is pressed.
- **Non-Blocking**: The sound playback does not block the key event detection, allowing real-time keyboard monitoring.

### License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
