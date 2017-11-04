Sample face image from  
http://www.stardailynews.co.kr/news/photo/201605/97763_119354_614.jpg  
Sample sticker image from  
https://vignette.wikia.nocookie.net/simpsons/images/1/12/Lisa_Simpson-0.png/revision/latest?cb=20161027220133  

### Compile  
```bash  
g++ face_landmark_ex.cpp -o test -std=c++11 -ldlib -lcblas -llapack
```  
### Run  
```bash  
./test shape_predictor_68_face_landmarks.dat face.jpg  
```

### pre-learned data file
```bash  
wget http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
bzip2 -dk shape_predictor_68_face_landmarks.dat.bz2
```
