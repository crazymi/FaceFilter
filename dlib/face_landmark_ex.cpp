// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    


    The face detector we use is made using the classic Histogram of Oriented
    Gradients (HOG) feature combined with a linear classifier, an image pyramid,
    and sliding window detection scheme.  The pose estimator was created by
    using dlib's implementation of the paper:
       One Millisecond Face Alignment with an Ensemble of Regression Trees by
       Vahid Kazemi and Josephine Sullivan, CVPR 2014
    and was trained on the iBUG 300-W face landmark dataset (see
    https://ibug.doc.ic.ac.uk/resources/facial-point-annotations/):  
       C. Sagonas, E. Antonakos, G, Tzimiropoulos, S. Zafeiriou, M. Pantic. 
       300 faces In-the-wild challenge: Database and results. 
       Image and Vision Computing (IMAVIS), Special Issue on Facial Landmark Localisation "In-The-Wild". 2016.
    You can get the trained model file from:
    http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2.
    Note that the license for the iBUG 300-W dataset excludes commercial use.
    So you should contact Imperial College London to find out if it's OK for
    you to use this model file in a commercial product.


    Also, note that you can train your own models using dlib's machine learning
    tools.  See train_shape_predictor_ex.cpp to see an example.

    


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


#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_transforms.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{  
    try
    {
        // This example takes in a shape model file and then a list of images to
        // process.  We will take these filenames in as command line arguments.
        // Dlib comes with example images in the examples/faces folder so give
        // those as arguments to this program.
        if (argc == 1)
        {
            cout << "Call this program like this:" << endl;
            cout << "./face_landmark_detection_ex shape_predictor_68_face_landmarks.dat faces/*.jpg" << endl;
            cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
            cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
            return 0;
        }

        // We need a face detector.  We will use this to get bounding boxes for
        // each face in an image.
        frontal_face_detector detector = get_frontal_face_detector();
        // And we also need a shape_predictor.  This is the tool that will predict face
        // landmark positions given an image and face bounding box.  Here we are just
        // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
        // as a command line argument.
        shape_predictor sp;

        clock_t begin = clock();
        deserialize(argv[1]) >> sp;
        cout << ">>>deserialize: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;

        image_window win, win_faces;

        ofstream outFile("result.txt");
        // Loop over all the images provided on the command line.
        // process only one
        for (int i = 2; i < 3; ++i)
        {
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;
            load_image(img, argv[i]);

            // Make the image larger so we can detect small faces.
            // @@@Modified -> maybe no need, due to type of img is selfie
            // pyramid_up(img);

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            begin = clock();
            std::vector<rectangle> dets = detector(img);
            cout << ">>>frontal face detector: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;
            cout << "Number of faces detected: " << dets.size() << endl;

            // Now we will go ask the shape_predictor to tell us the pose of
            // each face we detected.
            std::vector<full_object_detection> shapes;
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                begin = clock();
                full_object_detection shape = sp(img, dets[j]);
                cout << ">>>shape predictor: " << (double)(clock() - begin) / CLOCKS_PER_SEC << endl;
                cout << "number of parts: "<< shape.num_parts() << endl;               

                
                for(int j=0;j<shape.num_parts();j++){
                    outFile << j << ": " <<  shape.part(j) << endl;
                    if (j>=0 && j<=16){ // ear to ear
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(255,0,0));
                    } else if (j>=27 && j<=30){ // line on top of nose
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(127,0,0));
                    } else if (j>=17 && j<=21){ // left eyebrow
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,255,0));
                    } else if (j>=22 && j<=26){ // right eyebrow
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,127,0));
                    } else if (j>=31 && j<=35){ // bottom part of the nose
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,0,255));
                    } else if (j>=36 && j<=41){ // left eye
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,0,127));
                    } else if (j>=42 && j<=47){ // right eye
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(255,255,0));
                    } else if (j>=48 && j<=59){ // lips outer part
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,255,255));
                    } else if (j>=60 && j<=67){ // lips inside part
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(255,0,255));
                    } else {
                        draw_solid_circle(img, shape.part(j), (double)5, rgb_pixel(0,0,0));
                    }
                }
                
                // You get the idea, you can get all the face part locations if
                // you want them.  Here we just store them in shapes so we can
                // put them on the screen.
                shapes.push_back(shape);

                /*
                dlib::vector<long int, 2l> top_left = dlib::vector<long int, 2l>(shape.get_rect().top(), shape.get_rect().left());
                dlib::vector<long int, 2l> bottom_right = dlib::vector<long int, 2l>(shape.get_rect().bottom(), shape.get_rect().right());
                draw_solid_circle(img, top_left, (double)5, rgb_pixel(255,255,255));
                draw_solid_circle(img, bottom_right, (double)5, rgb_pixel(255,255,255));
                */

                draw_rectangle(img, shape.get_rect(), rgb_pixel(255, 255 ,255));

                cout << "top :" << shape.get_rect().top() << endl;
                cout << "left :" << shape.get_rect().left() << endl;
                cout << "bottom :" << shape.get_rect().bottom() << endl;
                cout << "right :" << shape.get_rect().right() << endl;
                
            }
            

            /*
            full_object_detection shape = sp(img, dets[0]);
            array2d<rgb_pixel> stst;
            load_image(stst, "stst.png");
            
            int p = (int)(shape.part(28))(0);
            int q = (int)(shape.part(28))(1);
            cout << "coord : " << shape.part(28) << endl;

            for(int i=0;i<stst.nc();i++){
                for(int j=0;j<stst.nr();j++){
                    //if((int) stst[i][j].alpha != 0){
                        img[i+q][j+p] = stst[i][j];
                    //}
                }
            }
            */
            

            /*
            // Now let's view our face poses on the screen.
            win.clear_overlay();
            win.set_image(img);
//            win.add_overlay(image_display::overlay_circle(shape.part(0), 5, rgb_pixel(0, 255 ,0)));
            win.add_overlay(render_face_detections(shapes));
            */

            outFile.close();
            save_png(img, "result.png");

            // We can also extract copies of each face that are cropped, rotated upright,
            // and scaled to a standard size as shown here:
            /*
            dlib::array<array2d<rgb_pixel> > face_chips;
            extract_image_chips(img, get_face_chip_details(shapes), face_chips);
            win_faces.set_image(tile_images(face_chips));
            */

            cout << "Hit enter to process the next image..." << endl;
            cin.get();
        }
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------

