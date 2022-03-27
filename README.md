# Video Encoder
## About
These programs encode common video file to tjpeg and traw.
- video2traw: encodes common video file to uncompressed original video type(traw).
- video2jpeg: encodes common video file to compressed original video type(tjpeg).
These video file types help Raspberry Pi Zero W to play videos with 1x
speed.
**We recommend to run these program on host machine.**
## Requirements(for Build)
- libjpeg9-dev
- FFmpeg
## Requirements(for Run)
- libjpeg9
## Build(ffmpeg)
```
$ git clone git@github.com/FFmpeg/FFmpeg.git ffmpeg
$ cd ffmpeg
# Please change /path/to/ffmpeg/5.0 (for host machine)
$ ./configure --prefix=/path/to/ffmpeg/5.0 --extra-libs="-ldl"
# Please change /path/to/ffmpeg/5.0 (for raspberry pi)
$ ./configure --enable-cross-compile --cross-prefix=arm-linux-gnueabi-
--arch=armel --target-os=linux --extra-libs="-ldl" --prefix=/path/to/ffmpeg/5.0
$ make
$ make install
```

## Build(libjpeg)
```
$ curl -o jpegsrc.v9e.tgz http://www.ijg.org/files/jpegsrc.v9e.tar.gz
$ tar xvzf jpegsrc.v9e.tgz
$ cd jpeg-9e
# Please change /path/to/jpeg/9e (for host machine)
$ ./configure --prefix=/path/to/jpeg/9e
# Please change /path/to/jpeg/9e (for raspberry pi)
$ ./configure --host=arm-linux-gnueabi --prefix=/path/to/jpeg/9e
$ make
$ make install
```

## Build
```
$ export PKG_CONFIG_PATH=/path/to/ffmpeg/5.0/lib/pkgconfig:/path/to/libjpeg/9e/lib/pkgconfig
$ make (for host machine)
$ make PREFIX=arm-linux-gnueabi- (for raspberry pi)
```
# License
## libjpeg
This software is based in part on the work of the Independent JPEG Group.
## FFmpeg
FFmpeg is licensed under the GNU Lesser General Public License (LGPL) version 2.1 or later.
(See [here](https://www.ffmpeg.org/legal.html)).
