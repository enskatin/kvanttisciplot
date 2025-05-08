#ifndef KVANTTISCIPLOT_H
#define KVANTTISCIPLOT_H
#include <gtk/gtk.h>

#define WINDOWHEIGHT 500
#define WINDOWIDTH 500
#define MARGINAL 50
#define HEIGHT_WITH_MARGINAL 450
#define WIDTH_WITH_MARGINAL 450
#define PI 3.14

enum plot_type {SCATTERPLOT = 0, HISTOGRAM = 1, HEATMAP = 2};

typedef struct{
    int numberofbars;
    double *data;
} s_histogram;

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
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

extern guint prev_key_val;
extern guint key_val;
extern guint save_as_png;

void testi();
figure_s *figure(double o_min_x,double o_max_x, double o_min_y, double o_max_y);
void set_color(figure_s *surface, char color[]);
void draw_point(figure_s* surface, double x, double y, double r);
void draw_end_point_values(cairo_t *cr, figure_s *surface);
void draw_axis(figure_s *surface);
void draw_x_label(cairo_t* cr, figure_s* surface);
void draw_y_label(cairo_t* cr, figure_s* surface);
void draw_title(cairo_t* cr, figure_s* surface);
void title(figure_s* surface, char* text);
void x_label(figure_s* surface, char* text);
void y_label(figure_s* surface, char* text);
int size_of_vector(double* vec);
void scale_to_interval(double* vec, int size, double min, double max, double* scaled_vec);
void scatterplot_draw(figure_s* surface, void *data, double r, int x_size, int y_size);
double avarage(double vec[], int size);
double SRL_slope(double vec_x[], double vec_y[], int size);
double SRL_intercept(double vec_x[], double vec_y[], int size);
void linear_fit(figure_s* surface, double vec_x[], double vec_y[], int size);
void save(gpointer user_data);
float divider(int box_amount, int size, double *vec);
int *heights(double *vec, int box_amount, int size, float box_width);
double *height_adjuster(int *height,int size);
figure_s *histogram(figure_s *figure, double *data, int size, int interval);
void event_key_pressed_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controller);
void event_key_released_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controlle);
void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data);
void activate(GtkApplication* app, gpointer user_data);
int run_gtk(int argc, char **argv, gpointer user_data);

#endif