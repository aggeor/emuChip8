# emuChip8

This is my implementation of a C++ emulator of the Chip8 interpreter. It was made for educational purposes. This project was made in Visual Studio 2019 and it uses the SDL2 library. I chose the SDL2 library because it simplifies graphics, audio and key inputs.

### Installing

To setup the project:

1. Clone this repository in Visual Studio
2. Download the SDL2 library [here](https://www.libsdl.org/download-2.0.php). Make sure to download the one under Development Libraries.

3. Use the instructions [here](https://stackoverflow.com/questions/57732058/how-do-i-link-sdl-with-visual-studio-2019) to setup SDL2 in your project.

Note: If it's still not compiling make sure to set the Character Set in your Configuration Properties properly. To do that, go to Project->Properties->Configuration Properties->Advanced and change Character Set to 'Not Set'


### Controls


The control scheme I chose for input is the following

```
Chip8 Controls            My Controls
  +-+-+-+-+                +-+-+-+-+
  |1|2|3|C|                |1|2|3|4|
  +-+-+-+-+                +-+-+-+-+
  |4|5|6|D|                |Q|W|E|R|
  +-+-+-+-+                +-+-+-+-+
  |7|8|9|E|                |A|S|D|F|
  +-+-+-+-+                +-+-+-+-+
  |A|0|B|F|                |Z|X|C|V|
  +-+-+-+-+                +-+-+-+-+
```

## Acknowledgments

A thank you to all the people who created these amazing resources that are invaluable to beginners who want to learn about emulator development

* http://emulator101.com/
* https://web.archive.org/web/20180201141811/https://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
* https://web.archive.org/web/20180216045558/http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#00E0
* https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
* https://austinmorlan.com/posts/chip8_emulator/
* https://www.reddit.com/r/EmuDev/
