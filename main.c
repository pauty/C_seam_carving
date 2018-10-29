// System includes
#include <stdio.h>
#include <stdlib.h>

#include "image.h"
#include "cost_data.h"
#include "seam_carving_functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

pixel **build_pixels(const unsigned char *imgv, int w, int h){
    int i, j;
    pixel **pixels = (pixel**)malloc(h*sizeof(pixel*));
    for(i = 0; i < h; i++)
        pixels[i] = (pixel*)malloc(w*sizeof(pixel));

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            pixels[i][j].r = imgv[i*3*w + 3*j];
            pixels[i][j].g = imgv[i*3*w + 3*j + 1];
            pixels[i][j].b = imgv[i*3*w + 3*j + 2];
            //pixels[i*w + j].a = (unsigned char)255;
            //printf("%d %d %d; ",pixels[i*w + j].r,pixels[i*w + j].g,pixels[i*w + j].b);
        }
        //printf("endrow\n");
    }
    //printf("END------\n\n");
    return pixels;
}

unsigned char *flatten_pixels(pixel **pixels, int h, int min_c, int max_c){
    int new_w = (max_c - min_c) + 1;
    unsigned char *flattened = (unsigned char*)malloc(3*new_w*h*sizeof(unsigned char));
    int i, j;
    for(i = 0; i < h; i++){
        for(j = 0; j < new_w; j++){           
            flattened[3*i*new_w + 3*j] = pixels[i][min_c + j].r;
            flattened[3*i*new_w + 3*j + 1] = pixels[i][min_c + j].g;
            flattened[3*i*new_w + 3*j + 2] = pixels[i][min_c + j].b;
            //pixels[i*w + j].a = (unsigned char)255;
            //printf("%d %d %d; ",pixels[i*w + j].r,pixels[i*w + j].g,pixels[i*w + j].b);
        }
    }
    return flattened;
}

int main(int argc, char **argv) {
    int w = 0, h = 0, ncomp = 0;
    unsigned char *imgv = NULL;
    pixel **pixels = NULL;
    cost_data **costs = NULL;
    long **M = NULL;
    int *seam = NULL;
    int i;
     
    imgv = stbi_load("imgs/coast.bmp", &w, &h, &ncomp, 0);
    if(ncomp != 3)
        printf("ERROR -- image does not have 3 components (RGB)\n");
    pixels = build_pixels(imgv, w, h);
    free(imgv);
    
    costs = (cost_data**)malloc(h*sizeof(cost_data*));
    for(i = 0; i < h; i++)
        costs[i] = (cost_data*)malloc(w*sizeof(cost_data));
    M = (long**)malloc(h*sizeof(long*));
    for(i = 0; i < h; i++)
        M[i] = (long*)malloc(w*sizeof(long));
    seam = (int*)malloc((h+1)*sizeof(int));
    
    //initialize costs
    compute_all_costs(pixels, costs, w, h);
    
    int min_c = 0;
    int max_c = w-1;
    int num_iterations = 0;
    
    //printf("size %ld \n",sizeof(long));
    
    //here start the loop
    while(num_iterations < 200){
        
        //call the kernel to calculate all costs 
        
        //call the kernel to compute comulative map
        compute_M(costs, M, h, min_c, max_c);
        
        /*
        for(i = min_c; i <= max_c; i++)
            printf("%ld, ",M[h-1][i]);
        getchar();
        */
        
            
        //kernel to find the seam
        find_seam(M, seam, h, min_c, max_c);
        
        
        for(i = 0; i < h; i++)
            printf("%d \n", seam[i]);
        getchar();
        
        
        
        //remove seam
        remove_seam(pixels, costs, seam, h, &min_c, &max_c);
        
        num_iterations = num_iterations + 1;
    }
    
    /*
    int i;
    for(i = 40; i < w*h; i = i+w){
        if(i % w == 0)
            printf("\n");
        printf("%d ",costs[i]);
    }*/
    
    /*
    pixel pix;
    for(i = 0; i < h; i++){
        pix = pixels[i][min_c+260];
        printf("[ %d %d % d ] \n",pix.r, pix.g, pix.b);
    }*/
    
    /*
    printf("\n\n -------- \n");
    
    for(i = 0; i < h; i++){
        printf("%d ", seam[i]);
    }*/
    
    unsigned char *output = flatten_pixels(pixels, h, min_c, max_c);
    int success = stbi_write_bmp("output.bmp", (max_c-min_c)+1, h, 3, output);
    
    printf("success : %d \n",success);
    
    for(i = 0; i < h; i++)
        free(pixels[i]);
    free(pixels);
    for(i = 0; i < h; i++)
        free(M[i]);
    free(M);
    for(i = 0; i < h; i++)
        free(costs[i]);
    free(costs);
    free(seam);
    free(output);

}
