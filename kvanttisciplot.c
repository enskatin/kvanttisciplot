#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//mahdollista dynaamista muistinvarausta varten. ymmärtääkseni olisi turvallisempaa vaihtaa dynaamiseen sillä malloc jne varaa muistia heapista
//vs nyt varataan muistia stackista, josta kaikki kääntäjät eivät välttämmättä tykkää ja muisti on rajoitettu

#define WINDOWHEIGHT 500
#define WINDOWIDTH 500
#define MARGINAL 50
#define HEIGHT_WITH_MARGINAL 450
#define WIDTH_WITH_MARGINAL 450

#define PI 3.14

//gcc `pkg-config --cflags gtk4` -g -o kvanttisciplot kvanttisciplot.c `pkg-config --libs gtk4`

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
    int axis_bool;
    int freq_max; // Histogrammin arvojen freqvencin maksimi.
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;

figure_s *figure(double min_x,double max_x, double min_y, double max_y){
    figure_s *figure = g_new0(figure_s,1); //Luodaan figure ja varataan sille tilaa
    figure->plot_type=SCATTERPLOT;
    //value_adjuster(&min_x,&max_x,&min_y,&max_y);
    figure->min_x = min_x;
    figure->max_x = max_x;
    figure->min_y = min_y;
    figure->max_y = max_y;
    figure->x_label_bool = 0;
    figure->y_label_bool = 0;
    figure->axis_bool = 0;
    figure->stored_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH_WITH_MARGINAL, HEIGHT_WITH_MARGINAL); // luodaan surface ja asetetaan se figureen
    figure->cr = cairo_create(figure->stored_surface); // Luodaan cairon konteksti entiteetti.
    cairo_set_source_rgb(figure->cr,1,1,1); 
    cairo_paint(figure->cr);//Asetetaan tausta valkoiseksi
    cairo_set_source_rgb(figure->cr,1,0,0); 
    return figure; 
}
//funktio kuvaajan värin määrittämiseksi 

//@param figure_s*surface pointteri figure_s tyyppiseen structiin, johon piirretään 

//@param char_color[] stringi, joka on: red, green, blue, yellow, lightblue, pink tai black 

void set_color(figure_s *surface, char color[]){ 
    if(strcmp(color, "red") == 0){ 
        cairo_set_source_rgb(surface->cr, 1,0,0); 
    } else if(strcmp(color, "green") == 0){ 
        cairo_set_source_rgb(surface->cr, 0,1,0); 
    } else if(strcmp(color, "blue") == 0){ 
        cairo_set_source_rgb(surface->cr, 0,0,1); 
    } else if(strcmp(color, "yellow") == 0){ 
        cairo_set_source_rgb(surface->cr, 1,1,0); 
    } else if(strcmp(color, "lightblue") == 0){ 
        cairo_set_source_rgb(surface->cr, 0,1,1); 
    } else if(strcmp(color, "pink") == 0){ 
        cairo_set_source_rgb(surface->cr, 1,0,1); 
    } else if(strcmp(color, "black") == 0){ 
        cairo_set_source_rgb(surface->cr, 0,0,0); 
    } else{ 
        cairo_set_source_rgb(surface->cr, 1,0,0); 
    } 
} 

//pisteen piirtäminen, input: pisteen paikka ja sen säde, TODO: väri?
void draw_point(figure_s* surface, double x, double y, double r) {
    cairo_arc(surface->cr, x, y, r, 0, 2*G_PI);
    cairo_fill(surface->cr);
}

void draw_end_point_values(cairo_t *cr, figure_s *surface){
    char min_xy[54];
    char max_x[50];
    char max_y[50];

    sprintf(min_xy,"(%.2lf,%.2lf)",surface->min_x,surface->min_y);
    sprintf(max_x,"%.2lf",surface->max_x);
    sprintf(max_y,"%.2lf",surface->max_y);
    cairo_text_extents_t extents;
    cairo_text_extents(cr,max_x,&extents);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12.0);
    cairo_move_to(cr,25,15);
    cairo_show_text(cr,max_y);
    cairo_move_to(cr,5,WINDOWHEIGHT-10);
    cairo_show_text(cr,min_xy);
    cairo_move_to(cr,WINDOWIDTH-MARGINAL/4-extents.width,WINDOWHEIGHT-10);
    cairo_show_text(cr,max_x);

}

void draw_axis(figure_s *surface){
    surface->axis_bool = 1;
    cairo_set_source_rgb(surface->cr,0,0,0);
    cairo_set_line_width(surface->cr, 2);
    //PIIRRETÄÄN Y AKSELI X ORIGIN LEVEYDELLE
    cairo_move_to(surface->cr,0,0);
    cairo_line_to(surface->cr,0,HEIGHT_WITH_MARGINAL); 

    cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL,0);
    cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL); 

    //PIIRRETÄÄN X AKSELI Y ORIGIN KORKEUDELLE
    cairo_move_to(surface->cr,0,HEIGHT_WITH_MARGINAL);
    cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL);

    cairo_move_to(surface->cr,0,0);
    cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,0); 

    for(int i=0; i<10;i++){
        cairo_move_to(surface->cr,0,HEIGHT_WITH_MARGINAL/10*i);
        cairo_line_to(surface->cr,10,HEIGHT_WITH_MARGINAL/10*i);

        cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL/10*i);
        cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL-10,HEIGHT_WITH_MARGINAL/10*i);

        cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL/10*i,HEIGHT_WITH_MARGINAL);
        cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL/10*i,HEIGHT_WITH_MARGINAL-10);

        cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL/10*i,0);
        cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL/10*i,10);
    }
    cairo_stroke(surface->cr);
    cairo_set_source_rgb(surface->cr,0,1,0);
}

void draw_x_label(cairo_t* cr, figure_s* surface) {
    cairo_text_extents_t text_attr;
    char* text = surface->x_label_text;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 15);
    cairo_text_extents(cr, text, &text_attr);
    cairo_move_to(cr, WINDOWIDTH/2 - text_attr.width/2, HEIGHT_WITH_MARGINAL + 45);
    cairo_show_text(cr, text);  
}

void draw_y_label(cairo_t* cr, figure_s* surface) {
    cairo_text_extents_t text_attr;
    char* text = surface->y_label_text;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 15);
    cairo_text_extents(cr, text, &text_attr);
    cairo_move_to(cr, 5, HEIGHT_WITH_MARGINAL/2 - text_attr.height/2);
    cairo_rotate(cr, G_PI/2);
    cairo_show_text(cr, text);
    cairo_rotate(cr, -G_PI/2);  
}

void draw_title(cairo_t* cr, figure_s* surface) {
    cairo_text_extents_t text_attr;
    char* text = surface->title_text;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 15);
    cairo_text_extents(cr, text, &text_attr);
    cairo_move_to(cr, WINDOWIDTH/2 - text_attr.width/2, 15);
    cairo_show_text(cr, text);
}

void title(figure_s* surface, char* text) {
    surface->title_text = text;
    surface->title_bool = 1;
}

void x_label(figure_s* surface, char* text) {
    surface->x_label_text = text;
    surface->x_label_bool = 1;
}

void y_label(figure_s* surface, char* text) {
    surface->y_label_text = text;
    surface->y_label_bool = 1;
}


void histogram_marginal_draw(cairo_t *cr, figure_s *surface){
    char min_x[50];
    char max_x[50];
    char half_x[50];
    sprintf(min_x,"%.2lf",surface->min_x);
    sprintf(max_x,"%.2lf",surface->max_x);
    sprintf(half_x,"%.2lf",surface->min_x+(surface->max_x-surface->min_x)/2);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12.0);
    cairo_text_extents_t extents,extents1;
    cairo_text_extents(cr,max_x,&extents1);
    for(int j = 0; j<10; j++){
        char frequence[50] = {"\0"};
        sprintf(frequence,"%d",(surface->freq_max-j*((surface->freq_max)/10)));
        cairo_text_extents(cr,frequence,&extents);
        cairo_move_to(cr,MARGINAL/1.3-extents.width,30+(j*(HEIGHT_WITH_MARGINAL/10)));
        cairo_show_text(cr,frequence);
    }
    cairo_move_to(cr,30,WINDOWHEIGHT-10);
    cairo_show_text(cr,min_x);
    cairo_move_to(cr,WINDOWIDTH-extents1.width,WINDOWHEIGHT-10);
    cairo_show_text(cr,max_x);
    cairo_move_to(cr,WINDOWIDTH-extents1.width-(WIDTH_WITH_MARGINAL/2),WINDOWHEIGHT-10);
    cairo_show_text(cr,half_x);
}


//vektorin alkioiden määrä. tällä hetkellä ei dynaaminen. tarkoitus tehdä dynaamisesti?
//Käyttäjän syöttämän vektorin pituus on staattinen mutta tuntematon.

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
//pisteiden yhdistäminen (kuvaajan piirtäminen)
// input: x koords ja y koords ja niiden koot
void draw_graph(figure_s* surface, double x_vector[], double y_vector[], int x_size, int y_size) {
    if (x_size != y_size) 
    {
        printf("Drawing graph failed. Dimensions of the vectors don't match! ");
        return;
    }
    double window_x;
    double window_y;
    double window_next_x;
    double window_next_y;
    double min_y = surface->min_y;
    double max_y = surface->max_y;
    double min_x = surface->min_x;
    double max_x = surface->max_x;
    
    cairo_set_source_rgb(surface->cr,0,0,0);
    cairo_set_line_width(surface->cr, 2);
    for (int i = 0; i < x_size - 1; i++) {
        window_x = (x_vector[i] - min_x)/(max_x-min_x)*(WIDTH_WITH_MARGINAL);
        window_y = (y_vector[i] - min_y)/(max_y-min_y)*(HEIGHT_WITH_MARGINAL);
        window_next_x = (x_vector[i+1] - min_x)/(max_x-min_x)*(WIDTH_WITH_MARGINAL);
        window_next_y = (y_vector[i+1] - min_y)/(max_y-min_y)*(HEIGHT_WITH_MARGINAL);
        cairo_move_to(surface->cr, window_x, HEIGHT_WITH_MARGINAL - window_y);
        cairo_line_to(surface->cr, window_next_x, HEIGHT_WITH_MARGINAL - window_next_y);
    }
    cairo_stroke(surface->cr);
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

//suoran sovitus

double avarage(double vec[], int size) {
    double r = 0;
    for (int i = 0; i < size; i++) {
        r = r + vec[i];
    }
    return (double) r/size;
}

double SRL_slope(double vec_x[], double vec_y[], int size) {
    double d = 0;
    double r = 0;

    for (int i = 0; i < size; i++) {
        r = r + (vec_x[i]-avarage(vec_x, size))*(vec_y[i]-avarage(vec_y, size));
    }
    for (int i = 0; i < size; i++) {
        d = d + (vec_x[i]-avarage(vec_x, size))*(vec_x[i]-avarage(vec_x, size));
    }
    return (double) r/d;
}

double SRL_intercept(double vec_x[], double vec_y[], int size) {
    double k = SRL_slope(vec_x, vec_y, size);
    return avarage(vec_y, size) - k * avarage(vec_x, size);
}

void linear_fit(figure_s* surface, double vec_x[], double vec_y[], int size) {

    double min_y = surface->min_y;
    double max_y = surface->max_y;
    double min_x = surface->min_x;
    double max_x = surface->max_x;

    double vec_x_max = vec_x[0];
    double vec_x_min = vec_x[0];

    for (int i = 0; i < size; i++) {
        if (vec_x[i] > vec_x_max) vec_x_max = vec_x[i];
        if (vec_x[i] < vec_x_min) vec_x_min = vec_x[i];
    }

    double slope = SRL_slope(vec_x, vec_y, size);
    double intercept = SRL_intercept(vec_x, vec_y, size);
    double start_x = vec_x_min;
    double end_x = vec_x_max;
    double start_y = slope*start_x + intercept;
    double end_y = slope*end_x + intercept;

    double scaled_end_x = (end_x - min_x)/(max_x-min_x)*(WIDTH_WITH_MARGINAL);
    double scaled_end_y = (end_y - min_y)/(max_y-min_y)*(HEIGHT_WITH_MARGINAL);
    double scaled_start_x = (start_x - min_x)/(max_x-min_x)*(WIDTH_WITH_MARGINAL);
    double scaled_start_y = (start_y - min_y)/(max_y-min_y)*(HEIGHT_WITH_MARGINAL);

    cairo_set_source_rgb(surface->cr,0.2,0.2,0.2);
    cairo_set_line_width(surface->cr, 4);
    cairo_move_to(surface->cr, scaled_start_x ,HEIGHT_WITH_MARGINAL - scaled_start_y);
    cairo_line_to(surface->cr, scaled_end_x, HEIGHT_WITH_MARGINAL - scaled_end_y);
    cairo_stroke(surface->cr);
}

void save(gpointer user_data) {
    //ei toimi vielä kunnolla. ei talenna koko plottia. pitää vielä antaa mahdollisuus nimetä tallennettu kuva
    figure_s* figure = (figure_s*) user_data;
    cairo_surface_t *tempsurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WINDOWIDTH, WINDOWHEIGHT);
    cairo_t *cr = cairo_create(tempsurface); // Luodaan cairon konteksti entiteetti.
    cairo_set_source_rgb(cr,0.9,0.9,1);
    cairo_paint(cr);
    switch(figure->plot_type){
    case SCATTERPLOT:
    cairo_set_source_surface(cr, figure->stored_surface, MARGINAL/2, MARGINAL/2);
    cairo_paint(cr);
    if (figure->title_bool) {
        draw_title(cr, figure);
    }
    if (figure->x_label_bool) {
        draw_x_label(cr, figure);
    }
    if (figure->y_label_bool) {
        draw_y_label(cr, figure);
    }
    if(figure->axis_bool){
        draw_end_point_values(cr,figure);
    }
    break;
    case HISTOGRAM:
    cairo_set_source_surface(cr, figure->stored_surface, MARGINAL/1.2, MARGINAL/2);
    cairo_paint(cr);
    if (figure->title_bool) {
        draw_title(cr, figure);
    }
    histogram_marginal_draw(cr,figure);
    break;
    }
    cairo_surface_write_to_png(tempsurface, "Plot.png");
    cairo_destroy(cr);
    cairo_surface_destroy(tempsurface);
}



// HISTOGRAMMI
int imax_finder(int *data, int size){
    if(!data){
        return 0;
    }
    unsigned int max = 0;
    for(int i = 0; i<size;i++){
        if(data[i]>max){
        max = data[i];
        }
    }
    return max;
}
double *zero_one_scaler(int *data, int size, int max){
    double *doublevec = malloc(sizeof(double)*size);
    for(int i = 0; i<size; i++){
        doublevec[i] = data[i]/((double)max);
    }
    return doublevec;
}

int introunder(int max){
    int max_start = 10;
    for(int i = 0; i<32 ;i++){
        if(max<(max_start*10)){
            for(int k = 1; k<=10;k++){
                if(max<(max_start*k)){
                    int value = max_start*k;
                    return (value);
                }
            }
        }
        max_start = max_start*10;
    }
    return 1;
}

// funktio jolla lasketaan osavälien pituus

float divider(int box_amount, int size, double *vec){
    // minimin ja maksimi tarkistus
    double vec_max = vec[0];
    double vec_min = vec[0];

    for (int i = 0; i < size; i++) {
        if (vec[i] > vec_max) vec_max = vec[i];
        if (vec[i] < vec_min) vec_min = vec[i];
    }
    //osavälin pituuden selvittäminen
    float box_width = (vec_max-vec_min)/box_amount;
    return box_width;
}

// muodostetaan heights muuttuja jonne tallennetaan kaikkien palkkien korkeudet (käydään kaikki
// alkiot läpi ja lasketaan kuinka monta milläkin osavälillä on)
int *heights(double *vec, int box_amount, int size, float box_width){
    // minimin ja maksimin tarkistus
    double vec_max = vec[0];
    double vec_min = vec[0];

    for (int i = 0; i < size; i++) {
        if (vec[i] > vec_max) vec_max = vec[i];
        if (vec[i] < vec_min) vec_min = vec[i];
    }
    //printf("vektorimaximi on %lf\n",vec_max);
    int *heights=calloc(box_amount+1, sizeof(int));
    // käydään läpi kaikki osavälit ja tarkistetaan moniko vektorin alkioista on osavälillä
    for (int i=0; i<box_amount; i++){
        for(int j=0; j<size;j++){
            if (vec[j]>=(vec_min+(i*box_width))&&vec[j]<(vec_min+(i+1)*box_width)){
                heights[i]++;
            }
        }
    }
    heights[box_amount-1]++;
    return heights;
}

figure_s *histogram(double *data, int size, int interval){
    figure_s *histo = g_new0(figure_s,1);
    histo->stored_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH_WITH_MARGINAL, HEIGHT_WITH_MARGINAL); // luodaan surface ja asetetaan se figureen
    histo->cr = cairo_create(histo->stored_surface); // Luodaan cairon konteksti entiteetti.
    cairo_set_source_rgb(histo->cr,1,1,1); 
    cairo_paint(histo->cr);//Asetetaan tausta valkoiseksi
    histo->plot_type = HISTOGRAM;
    
    double width = divider(interval,size,data);
    int *hights = heights(data,interval,size,width);
    //for(int i = 0; i<interval;i++){
      //  printf("%d\n",hights[i]);
    //}
    int max = introunder(imax_finder(hights,interval));
    //printf("%d",max);
    double *adjusted_heights = zero_one_scaler(hights,interval,max);

    double data_max = data[0];
    double data_min = data[0];

    for (int i = 0; i < size; i++) {
        if (data[i] > data_max) data_max = data[i];
        if (data[i] < data_min) data_min = data[i];
    }

    histo->freq_max = max;
    histo->max_x = data_max;
    histo->min_x = data_min;

    cairo_set_line_width(histo->cr, 2);
    cairo_set_source_rgb(histo->cr,0,1,1); 
    for(int i = 0; i<interval; i++){
        cairo_rectangle(histo->cr,i*(WIDTH_WITH_MARGINAL/((double)interval)),(HEIGHT_WITH_MARGINAL-adjusted_heights[i]*HEIGHT_WITH_MARGINAL),WIDTH_WITH_MARGINAL/((double)interval),adjusted_heights[i]*HEIGHT_WITH_MARGINAL);
    }
    cairo_fill(histo->cr);
    cairo_set_source_rgb(histo->cr,0.2,0.2,0.2);
    for(int i = 0; i<interval; i++){
        cairo_rectangle(histo->cr,i*(WIDTH_WITH_MARGINAL/((double)interval)),(HEIGHT_WITH_MARGINAL-adjusted_heights[i]*HEIGHT_WITH_MARGINAL),WIDTH_WITH_MARGINAL/((double)interval),adjusted_heights[i]*HEIGHT_WITH_MARGINAL);
    } 
    cairo_stroke(histo->cr);
    draw_axis(histo);
    free(hights);
    free(adjusted_heights);
    return histo;
}

guint prev_key_val;
guint key_val;
guint save_as_png = 0;
static void event_key_pressed_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controller) {
    //if (state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_ALT_MASK)) return;
    key_val = keyval;
    if (prev_key_val == 65507 && key_val == 115) {
        save_as_png = 1;
    }
    prev_key_val = keyval;
}

static void event_key_released_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controlle) {
}

gboolean close_request_cb(GtkWindow* self, gpointer user_data){
    if (save_as_png) {  
        save(user_data);
    }
    figure_s *figure = (figure_s*) user_data;
    cairo_destroy(figure->cr);
    cairo_surface_destroy(figure->stored_surface);
    g_free(figure);
    gtk_window_destroy(self);
    return FALSE;
}

static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    figure_s *figure = (figure_s*) user_data;
    //Tarvitsee tarkistuksen, että figuredata on olemassa ja siinä on jonkinlainen stored_surface, jos ei ole niin pitää piirtä jotain muuta.
    //testaamista vart
    cairo_set_source_rgb(cr,0.9,0.9,1);
    cairo_paint(cr);
    switch(figure->plot_type){
        case SCATTERPLOT:
            cairo_set_source_surface(cr, figure->stored_surface, MARGINAL/2, MARGINAL/2);
            cairo_paint(cr);
            if (figure->title_bool) {
                draw_title(cr, figure);
            }
            if (figure->x_label_bool) {
                draw_x_label(cr, figure);
            }
            if (figure->y_label_bool) {
                draw_y_label(cr, figure);
            }
            if(figure->axis_bool){
                draw_end_point_values(cr,figure);
            }
        break;
        case HISTOGRAM:
            cairo_set_source_surface(cr, figure->stored_surface, MARGINAL/1.2, MARGINAL/2);
            cairo_paint(cr);
            if (figure->title_bool) {
                draw_title(cr, figure);
            }
            histogram_marginal_draw(cr,figure);
        break;
        }

    //HEITTÄÄ SEGVAULTIN JOS LAITTAA TÄMÄN JÄLKEEN KOODIA. KAIKKI HAJOAA
}

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window;
    GtkEventController* event_controller;

    window = gtk_application_window_new(app); //Luodaan applikaation muodostama ikkuna
    gtk_window_set_title(GTK_WINDOW (window), "Plot");
    gtk_window_set_default_size(GTK_WINDOW (window), WINDOWIDTH,WINDOWHEIGHT);
    GtkWidget *drawing_space = gtk_drawing_area_new(); //drawing_space on alue, jolle cairo pystyy piirtämään asioita
    gtk_window_set_child(GTK_WINDOW(window),drawing_space); //asetetaan drawing_space windowing lapseksi
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_space),draw_callback,user_data,NULL); //asetetaan drawing arean piirtämiseen käytettäväksi funktioksi draw_callback
    
    //ikkunan sulkeminen
    g_signal_connect(window,"close-request", G_CALLBACK(close_request_cb),user_data);

    //tallennusominaisuus

    event_controller = gtk_event_controller_key_new();
    g_signal_connect(event_controller, "key-pressed", G_CALLBACK(event_key_pressed_cb), user_data);
    g_signal_connect(event_controller, "key-released", G_CALLBACK(event_key_released_cb), user_data);
    gtk_widget_add_controller(GTK_WIDGET(window), event_controller);
    
    //funktiota kutsutaan figuredatalla
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_space), WINDOWIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_space), WINDOWHEIGHT);
    gtk_window_present(GTK_WINDOW(window));

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

int main(int argc, char **argv){
    int r;
    //testaamista. dataa jostain tähtitieteestä
    double x[29] = {3.95828, 4.08022, 4.06622, 4.17817, 4.22316, 3.99228, 3.61663, 4.13720, 3.52392, 3.89735, 4.35112, 3.89136, 3.62760, 4.48109, 4.26014, 3.77444, 4.03025, 4.13120, 4.24015, 4.11020, 3.73646, 4.37411, 4.42010, 4.27914, 4.32613, 3.87237, 4.19317, 4.17718, 3.93529};
    
    double y[29] = {16.31, 17.12, 17.37, 17.82, 17.77, 16.34, 14.54, 17.58, 15.09, 16.11, 18.51, 16.28, 14.65, 19.35, 18.16, 15.24, 16.76, 17.62, 18.11, 17.36, 15.85, 18.46, 19.34, 18.30, 18.41, 16.61, 17.76, 17.69, 16.37};
    s_scatterplot scatterdata;
    scatterdata.x_vector = x;
    scatterdata.y_vector = y;


    double vec1[10] = {
        3.2000, 3.5222, 3.6444, 3.8667, 3.9889,
        4.2111, 4.6333, 4.7556, 4.8778, 4.9000
    };

    // Array with 10 values from 13 to 19
    double vec2[10] = {
        13.0000, 13.6667, 14.3333, 15.0000, 15.6667,
        16.3333, 17.0000, 17.6667, 18.3333, 19.0000
    };

    s_scatterplot scatterdata2;
    scatterdata2.x_vector = vec1;
    scatterdata2.y_vector = vec2;

    double vec3[200] = {
        3.87, 6.45, 4.82, 3.29, 7.91, 5.46, 5.71, 4.25, 3.88, 4.53,
        5.21, 5.37, 3.92, 6.37, 6.15, 4.09, 6.61, 4.48, 4.42, 4.15,
        4.36, 5.52, 3.18, 4.91, 6.75, 2.58, 2.96, 5.01, 6.84, 3.53,
        5.41, 4.05, 5.11, 3.64, 4.66, 5.24, 4.29, 6.07, 3.77, 4.46,
        4.67, 4.60, 4.18, 3.95, 4.97, 4.31, 6.52, 3.59, 6.21, 3.20,
        5.90, 6.67, 5.00, 4.61, 4.22, 3.74, 3.42, 5.61, 4.01, 5.12,
        4.79, 4.40, 5.99, 6.88, 5.43, 5.03, 4.06, 4.54, 3.66, 4.16,
        3.71, 4.80, 4.20, 4.36, 4.74, 6.97, 3.63, 4.63, 4.10, 5.68,
        3.81, 4.72, 4.27, 3.67, 4.07, 5.15, 4.56, 4.38, 5.34, 4.44,
        4.33, 6.48, 4.11, 3.83, 3.45, 3.51, 4.85, 3.90, 4.30, 5.25,
        4.68, 5.81, 4.69, 4.12, 3.87, 4.55, 3.69, 5.06, 4.32, 5.16,
        4.24, 4.34, 4.49, 4.03, 3.33, 5.65, 5.07, 5.91, 5.18, 3.82,
        4.65, 3.93, 4.23, 3.72, 5.39, 3.46, 4.43, 3.68, 3.54, 5.04,
        4.17, 4.08, 5.20, 4.26, 5.58, 3.94, 3.89, 4.86, 5.59, 5.33,
        4.28, 4.13, 4.02, 3.44, 3.52, 4.35, 4.00, 4.95, 5.51, 4.41,
        4.92, 4.39, 3.75, 4.93, 4.73, 4.71, 4.88, 3.78, 5.42, 5.83,
        3.35, 5.66, 5.28, 3.91, 5.48, 3.73, 4.50, 4.47, 4.76, 4.62,
        4.39, 4.21, 4.75, 4.84, 4.59, 4.87, 4.98, 5.30, 4.78, 3.56,
        3.76, 5.17, 4.57, 5.47, 4.14, 3.60, 4.64, 5.05, 4.19, 5.63,
        4.90, 4.94, 4.52, 5.76, 4.45, 4.37, 3.50, 5.26, 4.70, 5.36
    };
    figure_s *figure2 = histogram(vec3,200,30);

     //funktion piirtämistä
    double X[100];
    double Y[100];
    for (int i = 0; i < 100; i++) {
        X[i] = i * (double)20/100;
        Y[i] = X[i]*X[i]*X[i] - 3 * X[i]*X[i] + 2;
    }
    
    //figure_s* figure1 = figure(50 ,WINDOWIDTH, 50, WINDOWHEIGHT);
    figure_s* figure1 = figure(3,5,12,21);
    draw_graph(figure1, X, Y, 100, 100);
    draw_axis(figure1);
    x_label(figure1, "X-akseli");
    y_label(figure1, "Y-akseli");
    title(figure1, "Kuvajaja");
    title(figure2, "Histogrammi");
    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/2, 25);
    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/4, 25);
    //toimii :D
    set_color(figure1, "blue");
    scatterplot_draw(figure1, &scatterdata, 4, 29, 29);
    set_color(figure1, "pink");
    scatterplot_draw(figure1, &scatterdata2, 4, 10, 10);
    linear_fit(figure1, x, y, 29);
    r = run_gtk(argc, argv, figure2);

    return r;
}
