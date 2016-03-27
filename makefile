#Set compiler commands
#----------------------
CC = g++
COMMONFLAGS = -Wall -std=c++11
OBJS = *.cpp
HDRS = *.h
LDFLAGS =  -L/usr/local/opt/opencv3/lib
CPPFLAGS = $(COMMONFLAGS) -I/usr/local/opt/opencv3/include
LDLIBS:=  -lopencv_core -lopencv_videoio -lopencv_highgui \
	  -lopencv_imgproc -lopencv_video -lopencv_objdetect \
	  `pkg-config opencv3 --libs`
#----------------------
#file1 = CV_Homework_3.cpp findSquare.cpp 

all: file1_exe

file1_exe: $(OBJS) $(HDRS)
	$(CC) $(OBJS) $(CFLAGS) -o cv $(LDFLAGS) $(CPPFLAGS) $(file1) $(LDLIBS)
file2_exe: $(OBJS) $(HDRS)
	$(CC) $(PKG) $(OBJS) $(CPPFlags) -o cv $(LDFLAGS)
clean:
		rm *.out *~ *.txt
