// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    

    This example is essentially just a version of the face_landmark_detection_ex.cpp
    example modified to use OpenCV's VideoCapture object to read from a camera instead 
    of files.


    Finally, note that the face detector is fastest when compiled with at least
    SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
    chip then you should enable at least SSE2 instructions.  If you are using
    cmake to compile this program you can enable them by using one of the
    following commands when you create the build project:
        cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
    This will set the appropriate compiler options for GCC, clang, Visual
    Studio, or the Intel compiler.  If you are using another compiler then you
    need to consult your compiler's manual to determine how to enable these
    instructions.  Note that AVX is the fastest but requires a CPU from at least
    2011.  SSE4 is the next fastest and is supported by most current machines.  
*/

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>

using namespace dlib;
using namespace std;

cv_image<bgr_pixel> cimg;
array2d<rgb_alpha_pixel> origin_img;
array2d<rgb_alpha_pixel> resize_img;
std::vector<rectangle> faces;
int width;
int height;

void stick_mustache(full_object_detection );
void stick_glasses(full_object_detection);
void stick_ear(full_object_detection , char* );
void stick_hat(full_object_detection );

int main(int argc, char** argv)
{
    try
    {

        if (argc != 3)
        {
            cout << "Call this program like this:" << endl;
            cout <<"./webcam_face_pose_ex <type> *.jpg" <<endl;
            cout <<"<type> 0: mustache " << endl;
            cout <<"<type> 1: glasses " << endl;
            cout <<"<type> 2: ear " << endl;
            cout <<"<type> 3: hairband " << endl;
            return 0;
        }

        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        image_window win;

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        // Grab and process frames until the main window is closed by the user.
        while(!win.is_closed())
        {
            // Grab a frame
            cv::Mat temp;
            if (!cap.read(temp))
            {
                break;
            }
            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            //cv_image<bgr_pixel> cimg(temp);
            cimg = temp;

            // Detect faces 
            faces = detector(cimg);

            // Find the pose of each face.
            std::vector<full_object_detection> shapes;
            for (unsigned long i = 0; i < faces.size(); ++i)
                shapes.push_back(pose_model(cimg, faces[i]));

            load_image(origin_img, argv[2]);
            //Resize image using face size
            for(unsigned long i = 0; i < faces.size(); i++)
            {
                full_object_detection shape = pose_model(cimg, faces[i]);

                switch(stoi(argv[1])) {
                    case 0: // Mustache
                        stick_mustache(shape); 
                        break;
                    case 1: // Glasses
                        stick_glasses(shape);
                        break;
                    case 2: // Ear
                        stick_ear(shape, argv[2]);
                        break;
                    case 3: // Hair
                        //stick_hair();
                        break;
                    case 4: // hat
                        stick_hat(shape);
                    default: // Etc
                        break;
                }
            }            
            
            win.clear_overlay();
            win.set_image(cimg);
        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}

void stick_mustache(full_object_detection shape)
{
    //Resize image using face size
    rectangle rect = shape.get_rect();
    long width = rect.right() - rect.left();
    long height = rect.bottom() - rect.top();
    resize_img.set_size(height/2, width/2); // set row, column
    resize_image(origin_img, resize_img, interpolate_bilinear());

    for(int i = 0; i < resize_img.nc(); i++)
    {
        for(int j = 0; j < resize_img.nr(); j++)
        {
            if(resize_img[i][j].alpha == 0)
                continue;
            //Be careful. shape.part(idx)(0) returns row value, shape.part(idx)(1) returns column value
            cimg[i-resize_img.nc()/2 + shape.part(33)(1)][j-resize_img.nr()/2 + shape.part(33)(0)].red = resize_img[i][j].red;
            cimg[i-resize_img.nc()/2 + shape.part(33)(1)][j-resize_img.nr()/2 + shape.part(33)(0)].blue = resize_img[i][j].blue;
            cimg[i-resize_img.nc()/2 + shape.part(33)(1)][j-resize_img.nr()/2 + shape.part(33)(0)].green = resize_img[i][j].green;


            cout << shape.part(33)(0) << ", "<<shape.part(33)(1) << endl;
        }
    }
    for(int i = 0; i < 5; i++ )
    {
        for(int j = 0; j < 5; j++) {
        cimg[i][j].green = 0;
        cimg[i][j].blue = 0;
        }
    }
}

void stick_glasses(full_object_detection shape)
{
    rectangle rect = shape.get_rect();
    long width = rect.right() - rect.left();
    long height = (shape.part(41)(1) - shape.part(19)(1))*3; //left eyebrow to bottom of left eye 
    resize_img.set_size(height, width);
    resize_image(origin_img, resize_img, interpolate_bilinear()); 
    
    long m_y = shape.part(27)(0);
    long m_x = shape.part(27)(1);

    for(int i = 0; i < resize_img.nc(); ++i)
    {
        for(int j = 0; j < resize_img.nr(); ++j)
        {
            if(resize_img[i][j].alpha == 0)
                continue;
            cimg[i - resize_img.nc()/2 + m_x][j - resize_img.nr()/2 + m_y].red = resize_img[i][j].red;
            cimg[i - resize_img.nc()/2 + m_x ][j - resize_img.nr()/2 + m_y].blue = resize_img[i][j].blue;
            cimg[i - resize_img.nc()/2 + m_x ][j - resize_img.nr()/2 + m_y].green = resize_img[i][j].green;
        }
    }
        
}

void stick_ear(full_object_detection shape, char* img_name)
{
    try{
        long h = ((shape.part(21) + shape.part(22)) - shape.part(33))(1);

        dlib::vector<long int, 2l> fore_l = dlib::vector<long int, 2l>((shape.part(19))(0), h);
        dlib::vector<long int, 2l> fore_r = dlib::vector<long int, 2l>((shape.part(24))(0), h);

        int nw = (int)((float)h/origin_img.nr() * origin_img.nc());
        resize_img.set_size(h, nw);
        resize_image(origin_img, resize_img, interpolate_bilinear());
        
        int p = (int)fore_l(0);
        int q = (int)fore_l(1);
        int sw = (int)resize_img.nr()/2;
        int sh = (int)resize_img.nc()/2;

        for(int i = -sw; i < sw; i++) {
            for(int j = -sh; j < sh; j++) {
                if(resize_img[i+sw][j+sh].alpha == 0)
                    continue;
                cimg[i+q][j+p].red = resize_img[i+sw][j+sh].red;
                cimg[i+q][j+p].blue = resize_img[i+sw][j+sh].blue;
                cimg[i+q][j+p].green = resize_img[i+sw][j+sh].green;
            }
        }

        char* new_name;
        strncpy(new_name, img_name, strlen(img_name));
        new_name[strlen(new_name) - 5] = 'r';
        load_image(origin_img, new_name);

        resize_image(origin_img, resize_img, interpolate_bilinear());
        p = (int)fore_r(0);
        q = (int)fore_r(1);

        for(int i = -sw; i < sw; i++) {
            for(int j = -sh; j < sh; j++) {
                if(resize_img[i+sw][j+sh].alpha == 0)
                    continue;
                cimg[i+q][j+p].red = resize_img[i+sw][j+sh].red;
                cimg[i+q][j+p].blue = resize_img[i+sw][j+sh].blue;
                cimg[i+q][j+p].green = resize_img[i+sw][j+sh].green;
            }
        }

        delete(new_name);
    }
    
    catch (exception &e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

void stick_hat(full_object_detection shape)
{
    try{
        long h = ((shape.part(21) + shape.part(22)) - shape.part(33))(1);

        dlib::vector<long int, 2l> fore = dlib::vector<long int, 2l>((shape.part(27))(0), h);

        int w = (int)((float)h/origin_img.nr() * origin_img.nc());
        cout << "(" << w << ", " << h << ")" << endl;
        resize_img.set_size(w, h);
        resize_image(origin_img, resize_img, interpolate_bilinear());
        
        int p = (int)fore(0);
        int q = (int)fore(1);
        int sw = (int)resize_img.nc()/2;
        int sh = (int)resize_img.nr()/2;

        for(int i = -sw; i < sw; i++) {
            for(int j = -sh; j < sh; j++) {
                if(resize_img[i+sw][j+sh].alpha == 0)
                    continue;
                cimg[i+p][j+q].red = resize_img[i+sw][j+sh].red;
                cimg[i+p][j+q].blue = resize_img[i+sw][j+sh].blue;
                cimg[i+p][j+q].green = resize_img[i+sw][j+sh].green;
            }
        }

    }
    
    catch (exception &e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

