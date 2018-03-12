1. About qtmm AFSK decoder

Qtmm is a very simple AFSK1200 decoder that uses the soundcard as input device.
Connect your computer to the audio output of your FM receiver, select the
audio device from the list and start decoding packet radio transmissions such
as APRS traffic or telemetry from various cubesats.

Qtmm uses the multimon AFSK1200 engine by Thomas Sailer and the Qt toolkit,
hence the project name qtmm. Qtmm is available for Linux, Mac OS X and Windows.


2. Install

You can install qtmm by compiling the source code yourself or by using one of
the pre-compiled binary packages available for download.

2.1 Compiling the source

You need Qt 5.5 or later. Qt 5.6 or later is strongly recommended.

On Linux you can use the Qt SDK that comes with your distribution. Qtmm needs
the qtcore, qtgui, qtwidgets, qtmultimedia and qtmultimedia-plugins components.

On Mac and Windows, and actually also on Linux, You candownload the latest
Qt SDK from http://qt.io/

Get the source code from https://github.com/csete/qtmm
Open the qtmm.pro file with Qt Creator and compile.

2.2 Pre-compiled binaries

I have made some binary packages available that may or may not work. See the
project downloads. The Windows and Mac binaries contain everything needed and
do not require separate installation of Qt. The Linux packages require the Qt
runtime libraries to be installed (TBC).


3. Future plans

Qtmm was conceived as a small experiment to test integration of multimon source
code into Qt application before doing it in gqrx. I do not plan to do any
active development of the application besides some occasional maiontenance.

You are of course welcome and encouraged to modify the program to suit your
needs and wishes within the terms and conditions of the GNU General Public
License version 2.
