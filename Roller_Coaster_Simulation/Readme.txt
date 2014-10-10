This project was implemented as a part of the second assignment for CSCI 420 -
Computer Graphics in Spring 2014 at the University of Southern California. The
aim of the project was to implement the simulation of the roller coaster. The 
project was implemented on Microsoft Visual Studio 2012. Following 
are the features of the program/instructions to run the program:

1) The entrire program is implemented within assign2.cpp. The user needs to open
   Visual Studio 2012, load the project and execute it directly by pressing
   Ctrl+F5 or by pressing F5 for debug mode.

2) The program needs eight images for texture mapping. The images need to be
   present in the project directory. The images are provided along with the project.

3) The tracks of the roller coaster is implemented using the splines. The values
  for the splines are loaded from the file mentioned in the track.txt file. The 
  track.txt file contains the rollercoastermine.sp as the file from which the
  spline values need to be loaded from. ( EXTRA CREDIT )
  
4) The environment is rendered as a cube map and skydome implementation. The 
  default setting of the project is to implement a skydome. This can be changed
  by commenting the # define SKY line. The implementation of skydome is for
  EXTRA CREDIT 

5) The program will start capturing the screenshots as soon as the program
   starts executing. In order to disable the capturing of screen shots we need
   to comment out the # define CAPTURE_SCREENSHOT.

6) The crossection of the roller coaster tracks are rendered as cuboids( EXTRA CREDIT )

7) Two roller coaster tracks are rendered (EXTRA CREDIT)

8) Wooden Planks are rendered between the two tracks (EXTRA CREDIT).

9) Lighting is enabled for the scene ( EXTRA CREDIT ). 

10) The material property of the roller coaster tracks are specified (EXTRA
CREDIT).

11) A video has been included and along with this submission under the name
    "CSCI_420.avi" which can be used for class video.

12) To quit the program the user needs to  press 'Q' key from the key board.
  
REFERENCES:
1) OpenGL RedBook
2) Lecture Notes
3) www.flipcode.com
