#ifndef KVANTTISCIPLOT_H
#define KVANTTISCIPLOT_H
#include <gtk/gtk.h>

#define WINDOWHEIGHT 500
#define WINDOWIDTH 500
#define MARGINAL 50
#define HEIGHT_WITH_MARGINAL 450
#define WIDTH_WITH_MARGINAL 450


enum plot_type {SCATTERPLOT = 0, HISTOGRAM = 1, HEATMAP = 2};

typedef struct{
    double *x_vector;
    double *y_vector;
} s_scatterplot;

typedef struct{
    int plot_type; // Lisätty sitä varten, jos haluaa käyttää figurea erityyppisten plottien hahmottelemisesta. Tällöin ei myöskään haittaa että min_x yms on olemassa.
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    char* x_label_text;
    char* y_label_text;
    char* title_text;
    int x_label_bool;
    int y_label_bool;
    int title_bool;
    int freq_max;
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

figure_s *figure(double min_x,double max_x, double min_y, double max_y);
void set_color(figure_s *surface, char color[]);
void draw_axis(figure_s *surface);
void title(figure_s* surface, char* text);
void x_label(figure_s* surface, char* text);
void y_label(figure_s* surface, char* text);
void scatterplot_draw(figure_s* surface, void *data, double r, int x_size, int y_size);
void linear_fit(figure_s* surface, double vec_x[], double vec_y[], int size);
figure_s *histogram(double *data, int size, int interval);
void draw_graph(figure_s* surface, double x_vector[], double y_vector[], int x_size, int y_size);
int run_gtk(int argc, char **argv, gpointer user_data);
#endif