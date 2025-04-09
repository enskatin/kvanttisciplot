#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
//mahdollista dynaamista muistinvarausta varten. ymmärtääkseni olisi turvallisempaa vaihtaa dynaamiseen sillä malloc jne varaa muistia heapista
//vs nyt varataan muistia stackista, josta kaikki kääntäjät eivät välttämmättä tykkää ja muisti on rajoitettu

#define WINDOWHEIGHT 500
#define WINDOWIDTH 500
#define MARGINAL 50
#define HEIGHT_WITH_MARGINAL 450
#define WIDTH_WITH_MARGINAL 450

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
    int plot_type; // Lisätty sitä varten, jos haluaa käyttää figurea erityyppisten plottien hahmottelemisesta. Tällöin ei myöskään haittaa että min_x yms on olemassa.
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

void value_adjuster(double *min_x, double *max_x, double *min_y, double *max_y){
    int running = 1;
    int i = 1;
    if(*max_x>0){
        if(*min_x<0){
            if(-(*min_x)>*max_x){
                while(running){
                    if(*max_x<(-i)*(*min_x/10)){
                        *max_x=(-i)*(*min_x/10);
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            } else{
                while(running){
                    if(*min_x>(-i)*(*max_x/10)){
                        *min_x=(-i)*(*max_x/10);
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            }
        }
    }
    if(*max_y>0){
        if(*min_y<0){
            if(-(*min_y)>*max_y){
                while(running){
                    if(*max_y<(-i)*(*min_y/10)){
                        *max_y=(-i)*(*min_y/10);
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            } else{
                while(running){
                    if(*min_y>(-i)*(*max_y/10)){
                        *min_y=(-i)*(*max_y/10);
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            }
        }
    }
}  

figure_s *figure(double o_min_x,double o_max_x, double o_min_y, double o_max_y){
    figure_s *figure = g_new0(figure_s,1); //Luodaan figure ja varataan sille tilaa
    double max_x = o_max_x;
    double max_y = o_max_y;
    double min_x = o_min_x;
    double min_y = o_min_y;
    value_adjuster(&min_x,&max_x,&min_y,&max_y);
    figure->min_x = min_x;
    figure->max_x = max_x;
    figure->min_y = min_y;
    figure->max_y = max_y;
    figure->stored_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH_WITH_MARGINAL, HEIGHT_WITH_MARGINAL); // luodaan surface ja asetetaan se figureen
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
    cairo_arc(surface->cr, x, y, r, 0, 2*G_PI);
    cairo_fill(surface->cr);
}

void draw_axis(figure_s *surface){
    double x_origin = -(surface->min_x)/((surface->max_x)-(surface->min_x))*WIDTH_WITH_MARGINAL;
    double y_origin = -(surface->min_y)/((surface->max_y)-(surface->min_y))*HEIGHT_WITH_MARGINAL;

    int a = 1;
    int c = 1;

    if(surface->min_x>0){
        x_origin=0;
        a=0;
    }
    if(surface->max_x<0){
        x_origin=WIDTH_WITH_MARGINAL;
    }
    if(surface->min_y>0){
        y_origin=0;
        c=0;
    }
    if(surface->max_y<0){
        y_origin=HEIGHT_WITH_MARGINAL;
    }

    cairo_set_source_rgb(surface->cr,0.7,0.7,0.7);
    cairo_set_line_width(surface->cr, 2);
    cairo_move_to(surface->cr,x_origin,0);
    cairo_line_to(surface->cr,x_origin,HEIGHT_WITH_MARGINAL);

    cairo_move_to(surface->cr,0,HEIGHT_WITH_MARGINAL-y_origin);
    cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL-y_origin);
    int drawing = 1;
    int i = 0;
    if(surface->max_y>0 && ((-surface->min_y)<surface->max_y)){
    while(drawing){
        if(surface->max_y/10*i>(surface->max_y-c*surface->min_y)){
            break;
        }
        cairo_move_to(surface->cr,x_origin+10,(surface->max_y/10/(surface->max_y-c*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
        cairo_line_to(surface->cr,x_origin-10,(surface->max_y/10/(surface->max_y-c*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
        i++;
    }
    i=0;
    } else if(surface->max_y>0 && ((-surface->min_y)>surface->max_y)){
        while(drawing){
            if(-surface->min_y/10*i>(surface->max_y-c*surface->min_y)){
                break;
            }
            cairo_move_to(surface->cr,x_origin+10,((-surface->min_y)/10/(surface->max_y-c*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
            cairo_line_to(surface->cr,x_origin-10,((-surface->min_y)/10/(surface->max_y-c*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
            i++;
        }
        i=0;
    } else {
        for(int k = 1; k<11;k++){
            cairo_move_to(surface->cr,x_origin+10,(HEIGHT_WITH_MARGINAL/10)*k);
            cairo_line_to(surface->cr,x_origin-10,(HEIGHT_WITH_MARGINAL/10)*k);
        }
    }
    if(surface->max_x>0 && ((-surface->min_x)<surface->max_x)){
    i=0;
    while(drawing){
        if(surface->max_x/10*i>(surface->max_x- a* surface->min_x)){
            break;
        }
        cairo_move_to(surface->cr,(surface->max_x/10/(surface->max_x-a*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin+10);
        cairo_line_to(surface->cr,(surface->max_x/10/(surface->max_x-a*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin-10);
        i++;
    }
    } else if(surface->max_x>0 && ((-surface->min_x)>surface->max_x)){
        while(drawing){
            if((-surface->min_x)/10*i>(surface->max_x- a* surface->min_x)){
                break;
            }
            cairo_move_to(surface->cr,(-(surface->min_x)/10/(surface->max_x-a*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin+10);
            cairo_line_to(surface->cr,(-(surface->min_x)/10/(surface->max_x-a*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin-10);
            i++;
        }
    }  else{
        for(int k = 1; k<11;k++){
            cairo_move_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*k,HEIGHT_WITH_MARGINAL-y_origin+10);
            cairo_line_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*k,HEIGHT_WITH_MARGINAL-y_origin-10);
        }
    }

    //for(int i = 1; i<11;i++){
       //cairo_move_to(surface->cr,x_origin+10,(HEIGHT_WITH_MARGINAL/10)*i);
      //  cairo_line_to(surface->cr,x_origin-10,(HEIGHT_WITH_MARGINAL/10)*i);
    //}

    //for(int i = 1; i<11;i++){
     //   cairo_move_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*i,HEIGHT_WITH_MARGINAL-y_origin+10);
      //  cairo_line_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*i,HEIGHT_WITH_MARGINAL-y_origin-10);
    //}


    cairo_stroke(surface->cr);
    cairo_set_line_width(surface->cr, 4);

    cairo_move_to(surface->cr,x_origin+20,0);
    cairo_line_to(surface->cr,x_origin-20,0);

    cairo_move_to(surface->cr,x_origin+20,HEIGHT_WITH_MARGINAL);
    cairo_line_to(surface->cr,x_origin-20,HEIGHT_WITH_MARGINAL);

    cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL-y_origin+20);
    cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL-y_origin-20);

    cairo_move_to(surface->cr,0,HEIGHT_WITH_MARGINAL-y_origin+20);
    cairo_line_to(surface->cr,0,HEIGHT_WITH_MARGINAL-y_origin-20);


    cairo_stroke(surface->cr);
}

//vektorin alkioiden määrä. tällä hetkellä ei dynaaminen. tarkoitus tehdä dynaamisesti?
//Käyttäjän syöttämän vektorin pituus on staattinen mutta tuntematon.
int size_of_vector(double* vec) {
} 

//skaalaa vektorin välille min max. skaalaa vektorista vec->scaled_vec. tämäkin ehkä parempi tehdä jossain vaiheessa dynaamisesti
void scale_to_interval(double* vec, int size, double min, double max, double* scaled_vec) {

    if (!scaled_vec) {
        printf("Scaled vec is null.\n");
        return;
    }

    //etsitään vec max ja min arvot. varmaan tehokkaampiakin tapoja etsiä
    double vec_max = vec[0];
    double vec_min = vec[0];

    for (int i = 0; i < size; i++) {
        if (vec[i] > vec_max) vec_max = vec[i];
        if (vec[i] < vec_min) vec_min = vec[i];
    }
    //skaalaus
    for (int i = 0; i < size; i++) {
        scaled_vec[i] = (double) (vec[i]-vec_min)*(max-min)/(vec_max-vec_min) + min;
    }
}


//scatterplot. varmaan helpompaa piirtää surfacella? testaamista varten annettu vektoreiden koot. vaatii ehkä dynaamisen muistinvarauksen vektoreiden pituuksien määrittämiseksi ellei toista tapaa löydy.
void scatterplot_draw(figure_s* surface, void *data, double r, int x_size, int y_size){
    s_scatterplot *the_data = (s_scatterplot*) data;
    double min_y = surface->min_y;
    double max_y = surface->max_y;
    double min_x = surface->min_x;
    double max_x = surface->max_x;

    double* x_vector = the_data->x_vector;
    double* y_vector = the_data->y_vector;

    if (x_size == y_size) {
        for (int i = 0; i < x_size; i++) {

            double window_x = (x_vector[i] - min_x)/(max_x-min_x)*(WIDTH_WITH_MARGINAL);
            double window_y = (y_vector[i] - min_y)/(max_y-min_y)*(HEIGHT_WITH_MARGINAL);

            //otettu huomioon cairon epämukavuudet TARKISTA
            draw_point(surface, window_x,  HEIGHT_WITH_MARGINAL - window_y, r);
        }
    }

}


static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    figure_s *figure = (figure_s*) user_data;
    //Tarvitsee tarkistuksen, että figuredata on olemassa ja siinä on jonkinlainen stored_surface, jos ei ole niin pitää piirtä jotain muuta.
    //testaamista varten
    cairo_set_source_rgb(cr,1,1,1);
    cairo_paint(cr);
    cairo_set_source_surface(cr, figure->stored_surface, MARGINAL/2, MARGINAL/2);
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
    double x[5] = {-50, 100, 150, 160, 180};
    double y[5] = {-50, 100, 150, 160, 180};
    s_scatterplot scatterdata;
    scatterdata.x_vector = x;
    scatterdata.y_vector = y;

    //figure_s* figure1 = figure(50 ,WINDOWIDTH, 50, WINDOWHEIGHT);
    figure_s* figure1 = figure(-100,200,-100,200);
    draw_axis(figure1);

    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/2, 25);
    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/4, 25);
    //toimii :D
    scatterplot_draw(figure1, &scatterdata, 5, 5, 5);
    
    r = run_gtk(argc, argv, figure1);

    return r;
}
