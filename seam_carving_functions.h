#ifndef SEAM_CARVING
#define SEAM_CARVING

void compute_all_costs(pixel** pixels, cost_data** costs, int w, int h);
void compute_M(cost_data **costs, int **M, int h, int min_c, int max_c);
void find_seam(int **M, int *seam, int h, int min_c, int max_c);
void remove_seam(pixel **pixels, cost_data **costs, int *seam, int h, int *min_c, int *max_c);

#endif
