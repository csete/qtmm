1. About qtmm

Qtmm is a very simple AFSK1200 decoder that uses the soundcard as input device.
Connect your computer to the audio output of tyour FM receiver, select the
input device from the list and start decoding packet radio transmissions such
as APRS traffic or telemetry from various cubesats.

Qtmm uses the multimon AFSK1200 engine by Thomas Sailer and the Qt toolkit from
Nokia. Qmm is available for Linux, Mac OS X and Windows.


2. Install

You can install qtmm compiling the source code yourself or by using one of the
pre-compile binary packages available for download.

2.1 Compiling the source

You need to have Qt 4.7 or later.
For Mac or windows get the latest Qt SDK from http://qt.nokia.com/
On Linux you can just use the Qt SDK that comes with your distribution. On
Ubuntu (and maybe other distros) you also need to install libqtmultimediakit
and qtmobility-dev packages.

Get the source code from https://github.com/csete/qtmm - you can either git-clone
it or download an approrpriate tarball.
Open the qtmm.pro file with Qt Creator and compile.

2.2 Pre-compiled binaries

I have made some binary packages available that may or may not work.
The Windows and Mac binary packages contain everything needed and do not require
separate installation of Qt.
The Linux packages require the Qt runtime libraries to be installed.


3. Future plans

Qtmm was conceived as a small experiment to test integration of multimon source
code into Qt application before doing it on a larger scale. I do not plan to
do any active development of the application besides bringing it to a usable
and stable form.

You are of course welcome and encouraged to modify the program to suit your
needs and wishes within the terms and conditions of the license.
