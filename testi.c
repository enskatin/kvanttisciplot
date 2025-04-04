#include <stdio.h>
#include <gtk/gtk.h>

gcc `pkg-config --cflags gtk4` -o testi testi.c `pkg-config --libs gtk4`

#define WINDOWHEIGHT 200
#define WINDOWIDTH 200

typedef struct{
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

figure_s *figure(double min_x,double max_x, double min_y, double max_y){
    figure_s *figure = g_new0(figure_s,1);
    figure->min_x = min_x;
    figure->max_x = max_x;
    figure->min_y = min_y;
    figure->max_y = max_y;
    figure->stored_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WINDOWIDTH, WINDOWHEIGHT);
    figure->cr = cairo_create(figure->stored_surface);
    cairo_set_source_rgb(figure->cr,1,1,1);
    cairo_paint(figure->cr);
    return figure; 
}
void plot1(figure_s *surface){
    cairo_set_source_rgb(surface->cr,0,1,0);
    cairo_rectangle(surface->cr,WINDOWIDTH/4,WINDOWHEIGHT/4,50,50);
    cairo_fill(surface->cr);
}

void plot2(figure_s *surface){
    cairo_set_source_rgb(surface->cr,1,0,0);
    cairo_rectangle(surface->cr,(WINDOWIDTH*3)/4,(WINDOWHEIGHT*3)/4,50,50);
    cairo_fill(surface->cr);
}



int main(int argc, int **argv) {
    figure_s *figure1 = figure(0,0,0,0);
    plot1(figure1);
    plot2(figure1);
    cairo_surface_write_to_png (figure1->stored_surface,"Testing.png");
    cairo_destroy(figure1->cr);
    cairo_surface_destroy(figure1->stored_surface);
    g_free(figure1);
    return 0;
}
