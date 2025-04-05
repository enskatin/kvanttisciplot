#include <gtk/gtk.h>
#include <stdio.h>

#define WINDOWHEIGHT 500
#define WINDOWIDTH 500
#define PI 3.14

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

typedef struct{
    int plot_type; // Lisätty sitä varten, jos haluaa käyttää figurea erityyppisten plottien hahmottelemisesta. Tällöin ei myöskään haittaa että min_x yms on olemassa.
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

figure_s *figure(double min_x,double max_x, double min_y, double max_y){
    figure_s *figure = g_new0(figure_s,1); //Luodaan figure ja varataan sille tilaa
    figure->min_x = min_x;
    figure->max_x = max_x;
    figure->min_y = min_y;
    figure->max_y = max_y;
    figure->stored_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WINDOWIDTH, WINDOWHEIGHT); // luodaan surface ja asetetaan se figureen
    figure->cr = cairo_create(figure->stored_surface); // Luodaan cairon konteksti entiteetti.
    cairo_set_source_rgb(figure->cr,1,1,1); 
    cairo_paint(figure->cr);//Asetetaan tausta valkoiseksi
    return figure; 
}
//ESIMERKKEJÄ:
void plot1(figure_s *surface){
    cairo_set_source_rgb(surface->cr,0,1,0); // Etsii surface struktin k.e.:n
    cairo_rectangle(surface->cr,WINDOWIDTH/4,WINDOWHEIGHT/4,50,50);
    cairo_fill(surface->cr);
}

void plot2(figure_s *surface){
    cairo_set_source_rgb(surface->cr,1,0,0); // surfacesta k.e.
    cairo_rectangle(surface->cr,(WINDOWIDTH*3)/4,(WINDOWHEIGHT*3)/4,50,50);
    cairo_fill(surface->cr);
}
//ESIMERKIT LOPPUU.

//pisteen piirtäminen, input: pisteen paikka ja sen säde, TODO: väri?
void draw_point(figure_s* surface, double x, double y, double r) {
    cairo_set_source_rgb(surface->cr, 0, 1, 0);
    cairo_arc(surface->cr, x, y, r, 0, 2*PI);
    cairo_fill(surface->cr);
}

//vektorin alkioiden määrä. tällä hetkellä ei dynaaminen. tarkoitus tehdä dynaamisesti?
//Käyttäjän syöttämän vektorin pituus on staattinen mutta tuntematon.
int size_of_vector(double* vec) {
    int s;
    return s;
} 

//skaalaa vektorin välille min max
double* scale_to_interval(double* vec, double min, double max) {

    return scaled_vec;
}


//scatterplot. varmaan helpompaa piirtää surfacella? vaatii ehkä dynaamisen muistinvarauksen vektoreiden pituuksien määrittämiseksi ellei toista tapaa löydy.
void scatterplot_draw(figure_s* surface, void *data, double r){
    s_scatterplot *the_data = (s_scatterplot*) data;
    double* x_scaled;
    double* y_scaled;
    double min_y = surface->min_y;
    double max_y = surface->max_y;
    double min_x = surface->min_x;
    double max_x = surface->max_y;
    int x_size = size_of_vector(the_data->x_vector);
    int y_size = size_of_vector(the_data->y_vector);
    if (x_size == y_size) {
        x_scaled = scale_to_interval(the_data->x_vector, 0, max_x - min_x);
        y_scaled = scale_to_interval(the_data->y_vector, 0, max_y - min_y);
        for (int i = 0; i < x_size; i++) {
            //otettu huomioon cairon epämukavuudet TARKISTA
            draw_point(surface, x_scaled[i],  max_y - min_y - y_scaled[i], r);
        }
    }
    g_free(x_scaled);
    g_free(y_scaled);
}


static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    figure_s *figure = (figure_s*) user_data;
    //Tarvitsee tarkistuksen, että figuredata on olemassa ja siinä on jonkinlainen stored_surface, jos ei ole niin pitää piirtä jotain muuta.
    //testaamista varten
    cairo_set_source_surface(cr, figure->stored_surface, 0, 0);
    cairo_paint(cr);

}

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window;
    window = gtk_application_window_new(app); //Luodaan applikaation muodostama ikkuna
    gtk_window_set_title(GTK_WINDOW (window), "Plot");
    gtk_window_set_default_size(GTK_WINDOW (window), WINDOWIDTH,WINDOWHEIGHT);
    GtkWidget *drawing_space = gtk_drawing_area_new(); //drawing_space on alue, jolle cairo pystyy piirtämään asioita
    gtk_window_set_child(GTK_WINDOW(window),drawing_space); //asetetaan drawing_space windowing lapseksi
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_space),draw_callback,user_data,NULL); //asetetaan drawing arean piirtämiseen käytettäväksi funktioksi draw_callback
    //funktiota kutsutaan figuredatalla
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_space), WINDOWIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_space), WINDOWHEIGHT);
    gtk_window_present(GTK_WINDOW(window));
    //onko tarpeellinen?
    // free_plot_data(user_data); <--- EI OLE TARPEELLINEN
}

int run_gtk(int argc, char **argv, gpointer user_data){ //gtk plotter ottaa argumentikseen käyttäjän plottaaman data struct muodossa.
    //Tärkeää: piirtämisfunktioiden pitää aluksi muuttaa sisältämänsä data strukti gpointer tyyliseksi dataksi. 
    int r = 0;
    GtkApplication *app;
    app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate),user_data);
    r = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return r;
}

// OHEISET KOMENNOT OVAT VANHENTUNEITA JA OLIVAT AIKASEMMAN INFRASTRUKTUURIN JÄÄNTEITÄ:
// KANNATTAA SUUNNITELLA KOKONAAN UUDET

/*
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

*/


int main(int argc, char **argv){
    int r;
    //testaamista
    figure_s* figure1 = figure(0,0,0,0);

    draw_point(figure1, WINDOWHEIGHT/2, WINDOWIDTH/2, 25);
    draw_point(figure1, WINDOWHEIGHT/4, WINDOWIDTH/4, 25);

    r = run_gtk(argc, argv, figure1);

    return r;
}
