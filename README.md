# PoolBallRecognition
Use OpenCV to recognize the position of pool balls in a game.

#######################
### Image Stitching ###
#######################
The image will have 2 options.
 * Import a full image of the table
 * The second being that the user can import 2 images to be used stitched for stages to follow
The focus of the project currently is to import 2 images which are to be stitched together. 

#############
### Usage ###
#############
The program should be run as follows:
./PoolTable <LeftImage> <RightImage>

For example, The program will take the two following images:

![LeftImage](https://github.com/Kentokamoto/PoolTableRecognition/blob/master/src/Photos/TableHalf/8/IMG_1.jpg)
![RightImage](https://github.com/Kentokamoto/PoolTableRecognition/blob/master/src/Photos/TableHalf/8/IMG_2.jpg)

The program will recognize the pockets of the image as follows:
![LeftCorner](https://github.com/Kentokamoto/PoolTableRecognition/blob/master/src/Photos/TableHalf/8/Result/LeftCorners.png)
![RightCorner](https://github.com/Kentokamoto/PoolTableRecognition/blob/master/src/Photos/TableHalf/8/Result/RightCorners.png)


If four corners were detected in both the images, the following stitched images will be returned:
![Stitch](https://github.com/Kentokamoto/PoolTableRecognition/blob/master/src/Photos/TableHalf/8/Result/Stitched.png)


