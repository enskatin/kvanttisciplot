#include <gtk/gtk.h>
#include <stdio.h>

#define WINDOWHEIGHT 200;
#define WINDOWIDTH 200;

//gcc `pkg-config --cflags gtk4` -o kvanttisciplot kvanttisciplot.c `pkg-config --libs gtk4`

enum plot_type {SCATTERPLOT = 0, HISTOGRAM = 1, HEATMAP = 2};

typedef struct{
    int plot_type; //informaatiota siitä, minkä tyyppisen plotin halutaan tehdä datalla esim (0,1,2,3,...)'
    // 0 = scatterplot,  1 = histogrammi, 2 = heatmap,
    double *data; //data pointertyyppinä
} plot_data;

void scatterplot_draw(cairo_t *cr, double *data){

}

void histogram_draw(cairo_t *cr, double *data){

}

static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    plot_data *data = (plot_data*)user_data; //user_data on alunperin gpointer tyyppiä, joten se pitää castaa takaisin plot_dataksi, jotta sitä voidaan käyttää
    int typeof_plot = user_data->plot_type;
    switch(typeof_plot){ //Muodostetaan switch case rakenne, jossa draw_callback tekee eri asioita riippuen datatyypistä
        case 0:
        scatterplot_draw(cr,user_data->data);
        break;
        case 1:
        histogram_draw(cr,user_data->data);
        break;
    }
}

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window;
    window = gtk_application_window_new(app); //Luodaan applikaation muodostama ikkuna
    gtk_window_set_title(GTK_WINDOW (window), "Plot");
    gtk_window_set_default_size(GTK_WINDOW (window), WINDOWIDTH,WINDOWHEIGHT);
    GtkWidget *drawing_space = gtk_drawing_area_new(); //drawing_space on alue, jolle cairo pystyy piirtämään asioita
    gtk_window_set_child(GTK_WINDOW(window),drawing_space); //asetetaan drawing_space windowing lapseksi
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_space),draw_callback,user_data,g_free); //asetetaan drawing arean piirtämiseen käytettäväksi funktioksi draw_callback
    //funktiota kutsutaan käyttäjän haluamalla datalla sekä g_free parametrilla, joka vapauttaa datalle varatun tilan.
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_space), WINDOWIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_space), WINDOWHEIGHT);
    gtk_window_present(GTK_WINDOW(window));
}

void run_gtk(int argc, char **argv, gpointer user_data){ //gtk plotter ottaa argumentikseen käyttäjän plottaaman data struct muodossa.
    //Tärkeää: piirtämisfunktioiden pitää aluksi muuttaa sisältämänsä data strukti gpointer tyyliseksi dataksi. 
    int r = 0;
    GtkApplication *app;
    app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate),user_data);
    r = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return r;
}


int main(int argc, char **argv){
    return r;
}