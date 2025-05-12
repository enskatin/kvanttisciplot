# kvanttisciplot
kvanttisciplot is a C library for all your scientific plotting needs.

IMPORTANT:
The gtk/gtk4 library should be linked and compiled with the .c and .h file to enable it to be used in the creation and presentation of the window. 

functions:
figure_s *figure(double min_x,double max_x, double min_y, double max_y)
    min_x: The minimum horizontal value in the plotting space
    max_x: The maximum horizontal value in the plotting space
    min_y: The minimum vertical value in the plotting space
    max_y: The maximum vertical value in the plotting space

    The maximum should always be larger than the minimum.
    The function initializes and returns a figure_s struct, that will be used in further additions of the plotting space for example adding data as a scatterplot.

void set_color(figure_s *surface, char color[])
    surface: The figure which plot color is being changed.
    color: In text form the color you want to change the color to
    available colors: "red","green","blue","yellow,"light_blue","pink","black"

    The function takes a surface and a color and sets future plots to be this color. Switching color multiple times is supported.

void title(figure_s* surface, char* text);
    surface: The figure which title is being set
    text: a char list which the title of the plot is being set to.
    
    The function takes a surface and a title text and sets the title of the plot to this text.

void x_label(figure_s* surface, char* text)
    surface: The figure which x axis label is being set
    text: a char list which the x axis label of the plot is being set to.

    The function takes a surface and a label text and sets the label of the x axis to this text.

void y_label(figure_s* surface, char* text)
    surface: The figure which y axis label is being set
    text: a char list which the y axis label of the plot is being set to.

    The function takes a surface and a label text and sets the label of the y axis to this text.

void scatterplot_draw(figure_s* surface, void *data, double r, int x_size, int y_size)
    surface: The figure which is used to draw the scatterplot on
    data: a s_scatterplot type struct which contains the x values and the y values
    r: the radius of each point in the scatterplot.
    x_size: the amount of x values
    y_size: the amount of y values

    The function takes a surface which the scatterplot is drawn to, and a struct which contains the values being plotted. The size of the dots in the plotting can be changed by changing the value of r which is the radius of the dots.

void linear_fit(figure_s* surface, double vec_x[], double vec_y[], int size)
    surface: The figure which is used to draw the linear fit on.
    vec_x: vector containing the x values.
    vec_y: vector containing the y values.
    size: the length of the the vectors

    The function takes a surface and two vectors and creates a LSM fit to the datapoints and draws it on the surface.

figure_s *histogram(double *data, int size, int interval)
    data: a vector containing the double values that are used to create the histogram
    size: the length of the vector
    interval: how many bars the data is being split into
    
    The function takes data and a interval and creates and returns a figure which has the data plotted in a histogram.

int run_plot(int argc, char **argv, gpointer user_data)
    argc, argv: values taken from the main function
    user_data: The figure which is shown in the created window.

    The function takes  argc, argv and the figure which is being used to draw the plot that is shown in the created window. The function return int value r that notates the running state of the gtk window.

void draw_graph(figure_s* surface, double x_vector[], double y_vector[], int x_size, int y_size)
    surface: figure on which the graph is being drawn onto
    x_vector: the x values of the graph
    y_vector: the y values of the graph which are usually calculated using the x values
    x_size: the amount of x values
    y_size: the amount of y values

    The function takes x and y values calculated using a f(x) function by the user and plots a smooth graph on the figure.

EXAMPLE MAIN FUNCTION

int main(int argc, char **argv){
    vec_x[3]={0.1,0.5,0.9};
    vec_y[3]={0.2,0.6,0.95};

    s_scatterplot data;
    data->x_vector = vec_x;
    data->y_vector = vec_y;

    figure_s the_figure = figure(0,1,0,1);
    scatterplot_draw(the_figure,&data,4,3,3);

    int r = run_plot(argc,argv,the_figure);
    return r;
}

