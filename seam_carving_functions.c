
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "image.h"
#include "cost_data.h"

pixel BORDER_PIXEL = {.r = 0, .g = 0, .b = 0, .a = 0};

int min(int a, int b){
    return a < b ? a : b;
}

cost_data compute_single_cost(int r, int c, pixel **pixels, int h, int min_c, int max_c){
        pixel pix1, pix2, pix3;
        int p_r, p_g, p_b, rdiff, gdiff, bdiff;
        cost_data res;
        
        //handle borders
        if(c >= max_c) 
            pix1 = BORDER_PIXEL;
        else
            pix1 = pixels[r][c+1];
        if(c <= min_c)
            pix2 = BORDER_PIXEL;
        else
            pix2 = pixels[r][c-1];
        if(r <= 0)
            pix3 = BORDER_PIXEL;
        else
            pix3 = pixels[r-1][c];
            
        //compute partials
        p_r = abs(pix1.r - pix2.r);
        p_g = abs(pix1.g - pix2.g);
        p_b = abs(pix1.b - pix2.b);
        
        //compute left cost       
        rdiff = p_r + abs(pix3.r - pix2.r);
        gdiff = p_g + abs(pix3.g - pix2.g);
        bdiff = p_b + abs(pix3.b - pix2.b);
        res.left = rdiff + gdiff + bdiff;
        
        //compute up cost
        res.up = p_r + p_g + p_b;
        
        //compute right cost
        rdiff = p_r + abs(pix3.r - pix1.r);
        gdiff = p_g + abs(pix3.g - pix1.g);
        bdiff = p_b + abs(pix3.b - pix1.b);
        res.right = rdiff + gdiff + bdiff;
        
        return res;
}

void compute_all_costs(pixel **pixels, cost_data **costs, int w, int h){
    int min_c = 0;
    int max_c = w-1;
    int r, c;
    for(r = 0; r < h; r++){
        for(c = 0; c < w; c++){
            costs[r][c] = compute_single_cost(r, c, pixels, h, min_c, max_c);
        }
    }       
}

void compute_M(cost_data **costs, int **M, int h, int min_c, int max_c){
    int r, c;
    
    for(c = min_c; c <= max_c; c++){
        M[0][c] = min(costs[0][c].left, min(costs[0][c].up, costs[0][c].right));
    }
    
    for(r = 1; r < h; r++){
        for(c = min_c+1; c < max_c; c++){
            M[r][c] = min(M[r-1][c-1] + costs[r][c].left, min(M[r-1][c] + costs[r][c].up, M[r-1][c+1] + costs[r][c].right));
        }
        M[r][min_c] = min(M[r-1][min_c] + costs[r][min_c].up, M[r-1][min_c+1] + costs[r][min_c].right);
        M[r][max_c] = min(M[r-1][max_c-1] + costs[r][max_c].left, M[r-1][max_c] + costs[r][max_c].up);
    }
}


void find_seam(int **M, int *seam, int h, int min_c, int max_c){    
    int r, c, mid, min_index;
    int acc;
    
    r = h-1;
    min_index = min_c;

    //find min entry in last M row
    for(c = min_c+1; c <= max_c; c++){
        if(M[r][c] < M[r][min_index])
            min_index = c;
    } 
    
    //build seam
    seam[r] = min_index; 
    acc = min_index;
    for(r = h-2; r >= 0; r--){
        mid = min_index;
        if(mid > min_c){
            if(M[r][mid-1] < M[r][min_index])
                min_index = mid - 1;
        }
        if(mid < max_c){
            if(M[r][mid+1] < M[r][min_index])
                min_index = mid + 1;
        }
        seam[r] = min_index;
        acc = acc + min_index;
    }
    seam[h] = (int)(acc/h); //compute mean index (coloumn)
}

void remove_seam(pixel **pixels, cost_data **costs, int *seam, int h, int *min_c, int *max_c){
    int r, c;
    
    //seam[h] contains mean index
    if((*max_c - seam[h]) < (int)(*max_c - *min_c)/2){
        //shift pixels/costs to the left, reduce max_c
        for(r = 0; r < h; r++){          
            for(c = seam[r]; c < *max_c; c++){
                pixels[r][c] = pixels[r][c+1];
            }
        }
                
        for(r = 0; r < h; r++){          
            for(c = seam[r]-2; c < *max_c; c++){
                if(c >= *min_c){
                    if(c <= seam[r]+1)
                        costs[r][c] = compute_single_cost(r, c, pixels, h, *min_c, *max_c-1);
                    else
                        costs[r][c] = costs[r][c+1];
                }
            }
        }
        *max_c = *max_c - 1;
    }
    else{
        //shift pixels/costs to the right, increment min_c
        for(r = 0; r < h; r++){          
            for(c = seam[r]; c > *min_c; c--){
                pixels[r][c] = pixels[r][c-1];
            }
        }
        
        for(r = 0; r < h; r++){          
            for(c = seam[r]+2; c > *min_c; c--){
                if(c <= *max_c){
                    if(c >= seam[r]-1)
                        costs[r][c] = compute_single_cost(r, c, pixels, h, *min_c+1, *max_c);
                    else
                        costs[r][c] = costs[r][c-1];
                }
            }
        }
        *min_c = *min_c + 1;  
    }     
}

