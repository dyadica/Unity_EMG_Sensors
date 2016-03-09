# Unity_EMG_Sensors

Code for using the Gove &amp; MyoWare EMG sensors within Unity

This repository contains EMG Sensor code for use with both the Grove & MyoWare EMG
Sensors with Arduino and the Unity3D game engine.

A tolerance value for a muscle trigger event can be set via sending "T,Value" over serial from Unity to the Arduino example. Code is also provided to demonstrate use of both simple average smoothing and very basic low-pass filtering.
Please note it'll take about serval seconds to detect a session static analog value. During this time you should hold your muscle static.

Please see http://www.dyadica.co.uk/emg-muscle-sensors-and-arduino for more information

THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

www.dyadica.co.uk

https://www.facebook.com/ADropInTheDigitalOcean
