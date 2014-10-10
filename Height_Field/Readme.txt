This project was implemented as a part of the first assignment for CSCI 420 -
Computer Graphics in Spring 2014 at the University of Southern California. The
aim of the project was to implement the height field of any given image. The 
project was implemented on Microsoft Visual Studio 2012. Following 
are the features of the program/instructions to run the program:

1) The entrire program is implemented within assign1.cpp. The user needs to open
   Visual Studio 2012, load the project and execute it directly by pressing
   Ctrl+F5 or by pressing F5 for debug mode.

2) The program needs one image for obtaining the height field. The name of the
   image is passed as input argument to the program.

3) The height field can be rendered using the following:
   a) Points         - Press 'W'
   b) Lines          - Press 'E'
   c) Triangles      - Press 'R'
   d) Triangle Strip - Press 'T'   (EXTRA CREDIT)
  
4) The height field has been given with the following rotation modes
   a) Roll          - Press 'A'
   b) Pitch         - Press 'S'
   c) Yaw           - Press 'D'
   d) Stop rotation - Press 'F'

5) The program will start capturing the screenshots by pressing the key 'C'. To
   disable capturing the screenshots press 'C' again. The screenshots will be
   outputed into the Screenshots folder in the project directory. In order to
   change the output to a different directory, the directory name needs to
   specified in the program at line 735 in the assign1.cpp. The program crrently
   doesnt support capturing of individual frames, but can be easily done by
   making minor modifications to the program.

6) The program will accept the gray image as well as colored image.(EXTRA CREDIT)

7) Wireframe is used along side Lines and Triangles for rendering (EXTRA CREDIT)

8) The program colors the vertex with the color from the original image (either
   grayscale or color). This can be easily extended for the case of obtaining color
   from another image to color the vertices of the height field (EXTRA CREDIT).

9) The user can interactively rotate the height field with any arbitrary axis by
   left/middle clicking of the mouse. 

10) The user can interactively translate the height field by holding down the
   Ctrl key and draggin the mouse.

11) The user can interactively scale by holding down the Shift key and dragging
   the mouse to obtain the required level of scaling of the heightfield.

12) The heightfield is rendered using the gluPerspective Function.

13) Lighting the scene is user controlled. In order to light the scene the user
    needs to press the button 'L' in his/her keyboard. Currently the program
    supports point sources, ambient light source and directional light source. 
    To switch off the lighting the user needs to press the 'L' key again. 

14) The height field has also been rendered using the Triangle strip 
    primitive. (EXTRA CREDIT)

15) Smooth Gradient is used for Coloring the image.

16) The program creates a combination of rotation and translation to create
    animation. To start the animation the user needs to press 'M'. To stop the
    animation the user again needs to press 'M'. (EXTRA CREDIT)

17) A video has been included and along with this submission under the name
    "CSCI_420.avi" which can be used for class video.

18) Reset option is currently disabled.

19) To quit the program the user can right click on the screen and left click
    the quit button or simply press 'Q' key from the key board.

20) The height field can be mapped onto Y-Axis (default state) or can be mapped
    onto the Z-Axis by right clicking the screen and choosing the option as
    z-axis in "Height Field Options".
  
21) The program also has been included with a debug feature of rendering a cube.
  
22) The video included in the submission made use of more than 300 frames that
    are provided as a part of the submission.

23) The video was created in Matlab by making use of the screenshots. 
  
REFERENCES:
1) OpenGL RedBook
2) Lecture Notes
