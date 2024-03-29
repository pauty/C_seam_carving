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
        }
    }
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
        }
    }
    return flattened;
}

int main(int argc, char **argv) {
    int w = 0, h = 0, ncomp = 0;
    unsigned char *imgv = NULL;
    pixel **pixels = NULL;
    cost_data **costs = NULL;
    int **M = NULL;
    int *seam = NULL;
    
    
    long seams_to_remove;
    char *check;
    int i;
     
    if(argc < 3){
        printf("usage: %s namefile seams_to_remove\n", argv[0]);
        return 0;
    }
    seams_to_remove = strtol(argv[2], &check, 10);  //10 specifies base-10
    if (check == argv[2]){   //if no characters were converted pointers are equal
        printf("ERROR: can't convert string to number, exiting.\n");
        return 1;
    }
    imgv = stbi_load(argv[1], &w, &h, &ncomp, 0);
    if(imgv == NULL){
        printf("ERROR: can't load image \"%s\" (maybe the file does not exist?), exiting.\n", argv[1]);
        return 1;
    }
    if(ncomp != 3){
        printf("ERROR: image does not have 3 components (RGB), exiting.\n");
        return 1;
    }
    if(seams_to_remove < 0 || seams_to_remove >= w){
        printf("ERROR: number of seams to remove is invalid, exiting.\n");
        return 1;
    }
    
    pixels = build_pixels(imgv, w, h);
    free(imgv);
    
    costs = (cost_data**)malloc(h*sizeof(cost_data*));
    for(i = 0; i < h; i++)
        costs[i] = (cost_data*)malloc(w*sizeof(cost_data));
    M = (int**)malloc(h*sizeof(int*));
    for(i = 0; i < h; i++)
        M[i] = (int*)malloc(w*sizeof(int));
    seam = (int*)malloc((h+1)*sizeof(int));
    
    //initialize costs
    compute_all_costs(pixels, costs, w, h);
    
    int min_c = 0;
    int max_c = w-1;
    int num_iterations = 0;
    
    while(num_iterations < seams_to_remove){
        
        compute_M(costs, M, h, min_c, max_c);
        
        find_seam(M, seam, h, min_c, max_c);
        
        /*
        for(i = 0; i < h; i++)
            printf("%d \n", seam[i]);
        getchar();
        */
        
        remove_seam(pixels, costs, seam, h, &min_c, &max_c);
        
        num_iterations = num_iterations + 1;
    }
    
    unsigned char *output = flatten_pixels(pixels, h, min_c, max_c);
    int success = stbi_write_bmp("img1.bmp", (max_c-min_c)+1, h, 3, output);
    
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
    
    return 0;
}
