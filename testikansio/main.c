
#include "kvanttisciplot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){
    figure_s *figure1 = figure(-5,5,-150,150);
    srand(time(NULL));
    double vec_x[201]={0};
    //double vec_y[201];
    /*
    for(int i = 0; i<201;i++){
        vec_x[i] = -10+i*0.1;
    }
    for(int i = 0; i<201;i++){
        vec_y[i] = vec_x[i]*vec_x[i]*vec_x[i]-5*vec_x[i];
    }
    draw_graph(figure1,vec_x,vec_y,201,201);
    draw_axis(figure1);
    title(figure1,"Kuvaajan piirtäminen");
    */
    
    for(int i = 0;i<12;i++){
        for(int k = 0;k<201;k++){
            vec_x[k]=vec_x[k]+rand()/((double)RAND_MAX);
        }
    }
    for(int k = 0;k<201;k++){
            vec_x[k]=vec_x[k]-6;
        }

    figure_s *histogrammi = histogram(vec_x,201,14);
    title(histogrammi,"Normaalijakauma");
    double hp[50] = {45.000000, 60.000000, 80.000000, 80.000000, 39.000000, 58.000000, 78.000000, 78.000000, 78.000000, 44.000000, 59.000000, 79.000000, 79.000000, 45.000000, 50.000000, 60.000000, 40.000000, 45.000000, 65.000000, 65.000000, 40.000000, 63.000000, 83.000000, 83.000000, 30.000000, 55.000000, 40.000000, 65.000000, 35.000000, 60.000000, 35.000000, 60.000000, 50.000000, 75.000000, 55.000000, 70.000000, 90.000000, 46.000000, 61.000000, 81.000000, 70.000000, 95.000000, 38.000000, 73.000000, 115.000000, 140.000000, 40.000000, 75.000000, 45.000000, 60.000000};
    double attack[50] = {49.000000, 62.000000, 82.000000, 100.000000, 52.000000, 64.000000, 84.000000, 130.000000, 104.000000, 48.000000, 63.000000, 83.000000, 103.000000, 30.000000, 20.000000, 45.000000, 35.000000, 25.000000, 90.000000, 150.000000, 45.000000, 60.000000, 80.000000, 80.000000, 56.000000, 81.000000, 60.000000, 90.000000, 60.000000, 85.000000, 55.000000, 90.000000, 75.000000, 100.000000, 47.000000, 62.000000, 92.000000, 57.000000, 72.000000, 102.000000, 45.000000, 70.000000, 41.000000, 76.000000, 45.000000, 70.000000, 45.000000, 80.000000, 50.000000, 65.000000};
    s_scatterplot scatter;
    scatter.x_vector=hp;
    scatter.y_vector=attack;
    figure_s *figure2 = figure(0,150,0,160);
    set_color(figure2,"blue");
    scatterplot_draw(figure2,&scatter,4,50,50);
    y_label(figure2,"Attack");
    x_label(figure2,"HP");
    title(figure2,"ATTACK/HP");
    linear_fit(figure2,hp,attack,50);
    draw_axis(figure2);
    int r = run_gtk(argc,argv,figure2);
    return r;
}