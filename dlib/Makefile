CC = g++
RES = test
STD = -std=c++11
LIBS = -ldlib -lcblas -llapack
DAT = shape_predictor_68_face_landmarks.dat.bz2

all:
	$(CC) face_landmark_ex.cpp -o $(RES) $(STD) $(LIBS)

download:
		wget -nc http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
		bzip2 -dk $(DAT)

run:
	./$(RES) $(DAT) face.jpg

clean :
	rm -f $(RES) result.png
