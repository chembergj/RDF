# RDF
## Radio Direction Finder plugin for Euroscope

Having problems finding the tag for the aircraft, when the pilot calls you on your frequency? At least I have had challenges with this, when covering large sectors on Vatsim, especially FSS positions. Inspired by this video (https://www.youtube.com/watch?v=KpLKP_CxaLo), showing how RDF works at Eurocontrol, I have developed this plugin, with 2 choices for letting Euroscope help you find the calling pilot.
1) Using the Radio Direction Finder view, you will get the same functionality as shown in the video: highlighting the calling aircraft by drawing a circle around the tag. After the pilots transmission has ended, you can still get the tag highlighted by pressing the middle mouse button. If the tag is outside the displayed area, a line will be drawed in the direction of the plane.
Please note, that this only works, if you are using the Standard ES Radar View. In case you are using another plugin defining it's own views, you should probably have a look at choice no 2 below.
TODO: INSERT PICTURE/VIDEO
2) In case you are already using another radar view than Euroscope's standard radar view, I have made a "poorman's solution", where a flashing text will be displayed in the tag of the aircraft of the calling pilot.

TODO: INSERT PICTURE/VIDEO

## Installation of plugin

TODO: Description/pictures

In case you are more into MUAC-like features in Euroscope, I can also recommend this plugin: https://github.com/pierr3/UACPlugin, that on top of other features also includes RDF functionality. In case you are only interested in RDF, and don't want to switch to an entirely new radar view, the use of my plugin would probably be preferable.
