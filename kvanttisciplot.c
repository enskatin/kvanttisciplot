#include <gtk/gtk.h>
#include <stdio.h>

#define WINDOWHEIGHT 200
#define WINDOWIDTH 200

//gcc `pkg-config --cflags gtk4` -o kvanttisciplot kvanttisciplot.c `pkg-config --libs gtk4`

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
    int plot_type; //informaatiota siitä, minkä tyyppisen plotin halutaan tehdä datalla esim (0,1,2,3,...)'
    // 0 = scatterplot,  1 = histogrammi, 2 = heatmap,
    void *pdata_form; //esim histogram struct pointer tai scatterplot struct pointer
} plot_data;

void free_plot_data(gpointer data_set){
    plot_data *data_1 = (plot_data*) data_set;
    g_free(data_1->pdata_form);
    g_free(data_1);
}

void scatterplot_draw(cairo_t *cr, void *data){
    s_scatterplot *the_data = (s_scatterplot*) data;
}

void histogram_draw(cairo_t *cr, void *data){
    s_histogram *the_data = (s_histogram*) data;
}

static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    plot_data *data = (plot_data*)user_data; //user_data on alunperin gpointer tyyppiä, joten se pitää castaa takaisin plot_dataksi, jotta sitä voidaan käyttää
    int typeof_plot = user_data->plot_type;
    switch(typeof_plot){ //Muodostetaan switch case rakenne, jossa draw_callback tekee eri asioita riippuen datatyypistä
        case 0:
        scatterplot_draw(cr,data->pdata_form);
        break;
        case 1:
        histogram_draw(cr,data->pdata_form);
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
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_space),draw_callback,user_data,free_plot_data); //asetetaan drawing arean piirtämiseen käytettäväksi funktioksi draw_callback
    //funktiota kutsutaan käyttäjän haluamalla datalla sekä free_plot_data parametrilla, joka vapauttaa datalle varatun tilan.
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


int scatter_plot(int argc, char **argv, double *x_cords, double *y_cords){
    int r = 0; 
    plot_data *pdata = g_new0(plot_data,1); // allokoi tilaa g_mem0 avulla yhden plot_data struktin verran, ja asettaa struktin dataksi käyttäjän datan
    pdata->pdata_form = g_new0(s_scatterplot,1); //allokoi pdatan voidpointterin osoittamaan skatterplot struktiin
    s_scatterplot *p_scatter = pdata->pdata_form; // Selkeyden vuoksi luodaan p_scatter pointteri
    p_scatter->x_vector = x_cords; //asetetaan p_scatterin x ja y arvot osoittamaan x ja y koordinaattilistoihin.
    p_scatter->y_vector = y_cords;
    r = run_gtk(argc,argv,pdata);
    return r;
}

int histogram(int argc, char **argv, double *data, int number_of_bars){

    //Keskeneräinen
    int r = 0; 
    plot_data *pdata = g_new0(plot_data,1); // allokoi tilaa g_mem0 avulla yhden plot_data struktin verran
    pdata->pdata_form = g_new0(s_histogram,1); //allokoi pdatan voidpointterin osoittamaan histogram struktiin
    s_histogram *p_histogram = pdata->pdata_form; // Selkeyden vuoksi luodaan p_histogram pointteri
    p_histogram->data = data; //asetetaan p_histogramin data ja palkkien määrä
    p_histogram->numberofbars = number_of_bars;
    r = run_gtk(argc,argv,pdata);
    return r;
}


int main(int argc, char **argv){
    return r;
}