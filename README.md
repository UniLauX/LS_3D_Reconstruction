# Lightstage based 3D Reconstruction

#### 1. About the software
Automatically 3D reconstruction of human body based on synchronized camera array. Jun 2016.<br>
[Yu Liu](https://sites.google.com/site/yuliuunilau/home)' master thesis in omputer Science and Technology, Zhejiang University.<br>

Supervisor: [Guofeng Zhang](http://www.cad.zju.edu.cn/home/gfzhang/), [Hujun Bao](http://www.cad.zju.edu.cn/bao/)


#### 2. Thesis and Demo
[Master thesis](https://drive.google.com/file/d/19U7orKvMit1q33nVchfpTD8frHLep2WU/view) <br>
[Video Demo](https://youtu.be/PzfYyTo8qss) (in low quality)


#### 3. Abstract
Motion Capture is one of the basic research fields in computer vision, which aims
to recover the three-dimensional models and details of texture from moving people,
which are captured by a bunch of synchronized cameras from different views. Motion
Capture has very broad applications ranging from Virtual Reality, Digital Animation to
Movie Making. The emergence of many excellent works based on synchronized
camera array has achieved a series of good results in 3D reconstruction, but they are
always accompanied with some limitations and problems in three aspects: Firstly,
original devices could not satisfy the needs of capturing successive data from different
views with high speed; Secondly, automatical segmentation could not be implemented,
which makes it troublesome for users to cutout massive images manually. Finally, the
results of reconstruction are often limited by the memory thus could not reach to a
relatively high-resolution level.
This paper, based on the Reflection Field of Human Body Capture Platform in
Zhejiang University, with which we can capture synchronized video sequences with
high speed and conduct three-dimensional reconstruction of moving people. Firstly, we
design and build a complete system to recover human body based on synchronized
camera array, which contains modules of data acquisition and calibration, automatical
segmentation, visuallhull construction, depth recovery, pointcloud sampling and 3D
reconstruction. We also propose a novel segmentation method which combines
multiple cues such as color, texture, contrast, edge to cutout foreground automatically:
the first step is to initialize foreground with background subtraction in gray space, then
followed with an initial segmentation using graph cuts, and background cut is
implemented in order to attenuate the background contracts to remove wrong
segmented results. This algorithm combines the merits of three methods effectively.
The experimental results show that, on the one hand, our software is useful and
efficient, which can not only recover depth map with high-resolution as well as get
density controllable pointCloud sampling, but obtain moving people’s 3D models
with texture details and smooth surface. On the other hand, compared with other
segmentation methods, our algorithm could effectively reduce the impact for
segmentation brought by light and color similarity between foreground and
background, and the high quality foreground cutout results lay the foundation for
automatically three-dimensional of human body.

#### 4. Environment
Visual Studio 2010 </br>
C++

#### 5. Run the demo
Please refer to document at "LS_3D_Reconstruction/Document/User Guide for the software.pdf"

#### 6. 3D Reconstruction Results
![image](https://github.com/UniLauX/LS_3D_Reconstruction/blob/master/visual_result0.png)
![image](https://github.com/UniLauX/LS_3D_Reconstruction/blob/master/visualized_result.png)

#### 7. About [Lightstage](https://vgl.ict.usc.edu/LightStages/)
Lightstage in [State Key Lab of CAD&CG](http://www.cad.zju.edu.cn/english.html)
![image](https://github.com/UniLauX/LS_3D_Reconstruction/blob/master/Lightstage%20in%20ZJU.jpg)

Lightstage in [USC ICT Graphics Lab](https://vgl.ict.usc.edu/)
![image](https://github.com/UniLauX/LS_3D_Reconstruction/blob/master/Lightstage6%20in%20USC.jpg)
