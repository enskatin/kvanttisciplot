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
    cairo_surface_t *stored_surface;
    cairo_t *cr;
} figure_s;
//Value adjuster funktion tarkoituksena on ottaa molemmat minimit ja maksimit ja muottaa maksimin ja minimin
//arvot hyvämuotoiseksi siten, että akselinpiirtofunktio pystyy jakamaan y ja x akselin tasaisiin väleihin.
// value adjuster muuttaa rajoja siten, että jos esim x max on 20, ja x min on -1, se muuttaa x min arvoksi -2
// jotta välin -2,20 voi jakaa yhteentoista osaan.
void value_adjuster(double *min_x, double *max_x, double *min_y, double *max_y){
    int running = 1;
    int i = 1;
    if(*max_x>0 && (*min_x<0)){
            //Tärkein tilanne, jossa x max on suurempi kuin nolla ja x min on pienempi kuin nolla.
            if(-(*min_x)>*max_x){ // katsotaan, kumman itseisarvo on suurempi, minimin vai maksimin.
                while(running){
                    if(*max_x<=(-i)*(*min_x/10)){ // Katsotaan, kuinka monta kertaa pitää lisätä |minimin| kymmenesosa, että saadaan enemmän kuin maksimi
                        *max_x=(-i)*(*min_x/10); // Asetetaan uudeksi maksimiksi tämä
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            } else{ // Tilanne, jossa maksimi on suurempi kuin minimin itseisarvo 
                while(running){
                    if(*min_x>=(-i)*(*max_x/10)){ // Katsotaan, kuinka monta kertaa pitää poistaa maksimin kymmenesosa, että vähemmän kuin minimi
                        *min_x=(-i)*(*max_x/10); // Aseteteaan uudeksi minimiksi tämä
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            }
        }
    if(*max_y>0&&(*min_y<0)){
            //Tärkein tilanne, jossa y max on suurempi kuin nolla ja y min on pienempi kuin nolla.
            if(-(*min_y)>*max_y){ // Tilanne jossa minimin itseisarvo on suurempi kuin maksimi
                while(running){
                    if(*max_y<=(-i)*(*min_y/10)){ //katsotaan, kuinka monta kertaa pitää lisätä |minimin| moninkerta
                        *max_y=(-i)*(*min_y/10); // asetetaan maksimiksi tämä
                        running = 0;
                    }
                    i++;
                }
                i=0;
                running = 1;
            } else{
                while(running){
                    if(*min_y>=(-i)*(*max_y/10)){ // katsotaan, kuinka monta kertaa poistaa maksimin kymmenesosa.
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

figure_s *figure(double o_min_x,double o_max_x, double o_min_y, double o_max_y){
    figure_s *figure = g_new0(figure_s,1); //Luodaan figure ja varataan sille tilaa
    double max_x = o_max_x;
    double max_y = o_max_y;
    double min_x = o_min_x;
    double min_y = o_min_y;
    //value_adjuster(&min_x,&max_x,&min_y,&max_y);
    figure->min_x = min_x;
    figure->max_x = max_x;
    figure->min_y = min_y;
    figure->max_y = max_y;
    figure->x_label_bool = 0;
    figure->y_label_bool = 0;
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
//funktio kuvaajan värin määrittämiseksi 

//@param figure_s*surface pointteri figure_s tyyppiseen structiin, johon piirretään 

//@param char_color[] stringi, joka on: red, green, blue, yellow, lightblue, pink tai black 

int set_color(figure_s *surface, char color[]){ 
    if(color=="red"){ 
        cairo_set_source_rgb(surface->cr, 1,0,0); 
    } else if(color=="green"){ 
        cairo_set_source_rgb(surface->cr, 0,1,0); 
    } else if(color=="blue"){ 
        cairo_set_source_rgb(surface->cr, 0,0,1); 
    } else if(color=="yellow"){ 
        cairo_set_source_rgb(surface->cr, 1,1,0); 
    } else if(color=="lightblue"){ 
        cairo_set_source_rgb(surface->cr, 0,1,1); 
    } else if(color=="pink"){ 
        cairo_set_source_rgb(surface->cr, 1,0,1); 
    } else if(color=="black"){ 
        cairo_set_source_rgb(surface->cr, 0,0,0); 
    } else{ 
        cairo_set_source_rgb(surface->cr, 1,0,0); 
    } 
} 

//pisteen piirtäminen, input: pisteen paikka ja sen säde, TODO: väri?
void draw_point(figure_s* surface, double x, double y, double r) {
    set_color(surface, "pink");
    cairo_arc(surface->cr, x, y, r, 0, 2*G_PI);
    cairo_fill(surface->cr);
}

void draw_end_point_values(cairo_t *cr, figure_s *surface){
    char min_xy[10];
    char max_x[10];
    char max_y[10];

    sprintf(min_xy,"(%.2lf,%.2lf)",surface->min_x,surface->min_y);
    sprintf(max_x,"%.2lf",surface->max_x);
    sprintf(max_y,"%.2lf",surface->max_y);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12.0);
    cairo_move_to(cr,25,15);
    cairo_show_text(cr,max_y);
    cairo_move_to(cr,5,WINDOWHEIGHT-10);
    cairo_show_text(cr,min_xy);
    cairo_move_to(cr,WINDOWIDTH-40,WINDOWHEIGHT-10);
    cairo_show_text(cr,max_x);

}

void draw_axis(figure_s *surface){

    //ASETETAAN KOHDAT WINDOWISSA, MIHIN HALUTAAN PISTÄÄ ORIGO.
    double x_origin = -(surface->min_x)/((surface->max_x)-(surface->min_x))*WIDTH_WITH_MARGINAL;
    double y_origin = -(surface->min_y)/((surface->max_y)-(surface->min_y))*HEIGHT_WITH_MARGINAL;
    // min multit ovat tärkeitä akseleiden piirtämistä varten, koska ne nollaavat minimin jos alla olevat ehdot toteutuu.
    int x_min_multi = 1;
    int y_min_multi = 1;
    if(surface->min_x>0){//Jos x minimi on suurempaa kuin suurempaa kuin nolla, laitetaan akseli vasempaan reunaan. 
        x_origin=0;
        x_min_multi=0; // asetetaan a:n arvo nollaksi myöhempää varten
    }
    if(surface->max_x<0){
        x_origin=WIDTH_WITH_MARGINAL; //Jos x maksimi on vähempää kuin nolla, laitetaan akseli oikeaan reunaan
    }
    // sama kuin x min ja x max
    if(surface->min_y>0){
        y_origin=0;
        y_min_multi=0;
    }
    if(surface->max_y<0){
        y_origin=HEIGHT_WITH_MARGINAL;
    }

    cairo_set_source_rgb(surface->cr,0.7,0.7,0.7);
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
    //SEURAAVA IF HELVETTI PIIRTÄÄ X JA Y AKSELEIHIN VIIVAT TIETYLLÄ INTERVALLILLA AKSELEILLE
    //INTERVALLI MÄÄRITELLÄÄN SITEN, ETTÄ OTETAAN PIDEMPI PÄTKÄ AKSELISTA (- tai +), JAETAAN SE KYMMENELLÄ,
    //JONKA JÄLKEEN PÄTKITÄÄN KOKO AKSELI NÄILLÄ INTERVALLEILLA.
    /*
    int drawing = 1;
    int i = 0;
    if(surface->max_y>0 && ((-surface->min_y)<surface->max_y)){ //kun max y positiivinen ja suurempi kuin |min_y|
        while(drawing){
            if(surface->max_y/10*i>(surface->max_y-(y_min_multi)*surface->min_y)){ //Piirretään viivoja kunnes y akselin pituus täyttyy
                break;
            }
        cairo_move_to(surface->cr,x_origin+10,(surface->max_y/10/(surface->max_y-(y_min_multi)*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
        cairo_line_to(surface->cr,x_origin-10,(surface->max_y/10/(surface->max_y-(y_min_multi)*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
        i++;
    }
    i=0;
    } else if(surface->max_y>0 && ((-surface->min_y)>surface->max_y)){ // kun max y on pienempi kuin |min_y|
        while(drawing){
            if(-surface->min_y/10*i>(surface->max_y-(y_min_multi)*surface->min_y)){ // Piirretään, kunnes y akseli täyttyy
                break;
            }
            cairo_move_to(surface->cr,x_origin+10,((-surface->min_y)/10/(surface->max_y-(y_min_multi)*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
            cairo_line_to(surface->cr,x_origin-10,((-surface->min_y)/10/(surface->max_y-(y_min_multi)*surface->min_y))*i*(HEIGHT_WITH_MARGINAL));
            i++;
        }
        i=0;
    } else {
        for(int k = 1; k<11;k++){ //kun max_y on negatiivinen, jaetaan akseli vain kymmeneen osaan. 
            cairo_move_to(surface->cr,x_origin+10,(HEIGHT_WITH_MARGINAL/10)*k);
            cairo_line_to(surface->cr,x_origin-10,(HEIGHT_WITH_MARGINAL/10)*k);
        }
    }
    if(surface->max_x>0 && ((-surface->min_x)<surface->max_x)){// Samma på svenska. 
    i=0;
    while(drawing){
        if(surface->max_x/10*i>(surface->max_x- (x_min_multi)* surface->min_x)){
            break;
        }
        cairo_move_to(surface->cr,(surface->max_x/10/(surface->max_x-(x_min_multi)*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin+10);
        cairo_line_to(surface->cr,(surface->max_x/10/(surface->max_x-(x_min_multi)*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin-10);
        i++;
    }
    } else if(surface->max_x>0 && ((-surface->min_x)>surface->max_x)){
        while(drawing){
            if((-surface->min_x)/10*i>(surface->max_x- (x_min_multi)* surface->min_x)){
                break;
            }
            cairo_move_to(surface->cr,(-(surface->min_x)/10/(surface->max_x-(x_min_multi)*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin+10);
            cairo_line_to(surface->cr,(-(surface->min_x)/10/(surface->max_x-(x_min_multi)*surface->min_x))*i*(WIDTH_WITH_MARGINAL),HEIGHT_WITH_MARGINAL-y_origin-10);
            i++;
        }
    }  else{
        for(int k = 1; k<11;k++){
            cairo_move_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*k,HEIGHT_WITH_MARGINAL-y_origin+10);
            cairo_line_to(surface->cr,(WIDTH_WITH_MARGINAL/10)*k,HEIGHT_WITH_MARGINAL-y_origin-10);
        }
    }
    
    
    */
    cairo_stroke(surface->cr);
    //PIIRRETÄÄN AKSELEIDEN PÄIHIN 40 MITTAISET PÄÄT
    //cairo_set_line_width(surface->cr, 2);

    //cairo_move_to(surface->cr,0,0);
    //cairo_line_to(surface->cr,20,0);

    //cairo_move_to(surface->cr,x_origin+20,HEIGHT_WITH_MARGINAL);
    //cairo_line_to(surface->cr,x_origin-20,HEIGHT_WITH_MARGINAL);

    //cairo_move_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL);
    //cairo_line_to(surface->cr,WIDTH_WITH_MARGINAL,HEIGHT_WITH_MARGINAL-20);

    //cairo_move_to(surface->cr,0,HEIGHT_WITH_MARGINAL-y_origin+20);
    //cairo_line_to(surface->cr,0,HEIGHT_WITH_MARGINAL-y_origin-20);


    //cairo_stroke(surface->cr);
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


guint key_val;

static gboolean event_key_pressed_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controller) {
    
    //if (state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_ALT_MASK)) return FALSE;

    key_val = keyval;

    g_print("Key pressed!\n");
    g_print("%d %d \n", key_val, keycode);
    return TRUE;
}

static gboolean event_key_released_cb(GtkWidget* drawing_area, guint keyval, guint keycode, GdkModifierType state, GtkEventControllerKey* event_controlle) {
    g_print("\nKey Released!\n");
    return FALSE;
}


static void draw_callback(GtkDrawingArea *drawing_space, cairo_t *cr, int width, int height, gpointer user_data){
    figure_s *figure = (figure_s*) user_data;
    //Tarvitsee tarkistuksen, että figuredata on olemassa ja siinä on jonkinlainen stored_surface, jos ei ole niin pitää piirtä jotain muuta.
    //testaamista vart
    cairo_set_source_rgb(cr,0.9,0.9,1);
    cairo_paint(cr);
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
    draw_end_point_values(cr,figure);
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
    
    //tallennusominaisuus

    event_controller = gtk_event_controller_key_new();
    g_signal_connect(event_controller, "key-pressed", G_CALLBACK(event_key_pressed_cb), user_data);
    g_signal_connect(event_controller, "key-released", G_CALLBACK(event_key_released_cb), user_data);
    gtk_widget_add_controller(GTK_WIDGET(window), event_controller);

    
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
    //testaamista. dataa jostain tähtitieteestä
    double x[29] = {3.95828, 4.08022, 4.06622, 4.17817, 4.22316, 3.99228, 3.61663, 4.13720, 3.52392, 3.89735, 4.35112, 3.89136, 3.62760, 4.48109, 4.26014, 3.77444, 4.03025, 4.13120, 4.24015, 4.11020, 3.73646, 4.37411, 4.42010, 4.27914, 4.32613, 3.87237, 4.19317, 4.17718, 3.93529};
    
    double y[29] = {16.31, 17.12, 17.37, 17.82, 17.77, 16.34, 14.54, 17.58, 15.09, 16.11, 18.51, 16.28, 14.65, 19.35, 18.16, 15.24, 16.76, 17.62, 18.11, 17.36, 15.85, 18.46, 19.34, 18.30, 18.41, 16.61, 17.76, 17.69, 16.37};
    s_scatterplot scatterdata;
    scatterdata.x_vector = x;
    scatterdata.y_vector = y;

    //figure_s* figure1 = figure(50 ,WINDOWIDTH, 50, WINDOWHEIGHT);
    figure_s* figure1 = figure(3,5,13, 20);
    draw_axis(figure1);
    x_label(figure1, "X-akseli");
    y_label(figure1, "Y-akseli");
    title(figure1, "Kuvajaja");

    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/2, 25);
    //draw_point(figure1, WIDTH_WITH_MARGINAL/4, HEIGHT_WITH_MARGINAL/4, 25);
    //toimii :D
    scatterplot_draw(figure1, &scatterdata, 4, 29, 29);
    linear_fit(figure1, x, y, 29);
    r = run_gtk(argc, argv, figure1);

    return r;
}
