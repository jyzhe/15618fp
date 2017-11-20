/* heatmap - High performance heatmap creation in C.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Lucas Beyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "lodepng.h"
#include "heatmap.h"

int main()
{
    static const size_t w = 4096, h = 4096, npoints = 10000;
    unsigned char image[4096 * 4096 * 4];
    unsigned i;

    srand(time(NULL));

    unsigned *xs = (unsigned *) malloc(sizeof(unsigned) * npoints);
    unsigned *ys = (unsigned *) malloc(sizeof(unsigned) * npoints);
    /* Add a bunch of random points to the heatmap now. */
    for(i = 0 ; i < npoints ; ++i) {
        /* Fake a normal distribution. */
        unsigned x = rand() % w/3 + rand() % w/3 + rand() % w/3;
        unsigned y = rand() % h/3 + rand() % h/3 + rand() % h/3;
        xs[i] = x;
        ys[i] = y;
    }
    double begin = omp_get_wtime();

    /* Create the heatmap object with the given dimensions (in pixel). */
    heatmap_t* hm = heatmap_new(w, h);

    heatmap_add_points_omp(hm, xs, ys, npoints);

    /*for (i = 0; i < npoints; i++)*/
    /*{*/
        /*heatmap_add_point(hm, xs[i], ys[i]);*/
    /*}*/

    /* This creates an image out of the heatmap.
     * `image` now contains the image data in 32-bit RGBA.
     */
    heatmap_render_default_to(hm, image);

    /* Now that we've got a finished heatmap picture,
     * we don't need the map anymore.
     */
    heatmap_free(hm);

    double end = omp_get_wtime();
    printf("Total time: %f seconds\n", end - begin);

    /* Finally, we use the fantastic lodepng library to save it as an image. */
    {
        unsigned error = lodepng_encode32_file("heatmap.png", image, w, h);
        if(error)
            fprintf(stderr, "Error (%u) creating PNG file: %s\n",
                    error, lodepng_error_text(error));
    }

    return 0;
}
