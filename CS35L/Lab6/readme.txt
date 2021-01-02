The goal here is to reorganize the code in main.c so that it would support
multithreading with multiple threads. We first read through the code, and
plan to multithread it by having it's O(N^4) partitioned into separate
threads. Below are the steps I have taken:

1.) First, take out the code

    if( nthreads != 1 )

    {

      fprintf( stderr, "%s: Multithreading is not supported yet.\n", argv[0] );

      return 1;

    }

This portion of the code at line 170 disables the use of more than 1 thread.

2.) Then, seeing that the N^4 loop is performed for each pixel, I rewrote
the code so that the loop only deals with every x pixels, where x is the
number of threads to be created. Then, I created a separate function for
"pthread_create" to run and moved this portion of code into the body of the
function. The separate threads would be created to start with a different
pixel in the x-axis, so that the loops won't interfere each other's pixel
values.

3.) Then, to complete the above function, I took some of the variables and had
them declared in the global scope, as the values are not modified with each
iteration (so we don't have to worry about race conditions here). The
variables placed into the global scope are:

int nthreads;

scene_t scene;
Vec3 camera_pos;

Vec3 camera_dir;

double camera_fov;

Vec3 bg_color;


double pixel_dx;

double pixel_dy;

double subsample_dx;

double subsample_dy;

The variables are initialized at the beginning of main.c, before the threads
run the function. That way, the function that each thread runs would be able
to access these values.
(Of course, this might be somewhat slow as it would require system calls to
access these global variables. A better alternative would be to create a
struct of already-initialized values to pass to the function so the variables
would be available locally. For the sake of this lab, this works.)

4.) The function running is originally to print out three float values for
each pixel, in order, so the function that we have now must be able to handle
this race condition. In order to do so, I added a global 3D array in order to
hold the float values for each pixel, and have the thread function place the
values in this array.

float final_values[width][height][3];

After running "pthread_join" on each thread, I wrote a loop to output the float
values in the correct order.



Finally, to check the performance of the modified program:
(command and outputs taken from "make clean check")

time ./srt 1-test.ppm >1-test.ppm.tmp

real    0m45.840s
user    0m45.835s
sys     0m0.002s


time ./srt 2-test.ppm >2-test.ppm.tmp

real    0m22.070s
user    0m44.040s
sys     0m0.003s


time ./srt 4-test.ppm >4-test.ppm.tmp

real    0m14.009s
user    0m55.087s
sys     0m0.003s


time ./srt 8-test.ppm >8-test.ppm.tmp

real    0m6.890s
user    0m52.665s
sys     0m0.005s

From what we see, the real time certainly decreases as the number of threads
increases. It's speed has increased to about 6-7 times faster.
The user time somewhat increases, which is from the insignificant overhead of
creating multiple threads and keeping track of each of them.
The system time increases, which is expected due to the increasing number of
system calls for creating the threads.