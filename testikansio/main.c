
#include "kvanttisciplot.h"
#include <stdio.h>

int main(int argc, char **argv){
    figure_s *figure1 = figure(-10,10,0,120);
    double vec_x[201];
    double vec_y[201];
    for(int i = 0; i<201;i++){
        vec_x[i] = -10+i*0.1;
    }
    for(int i = 0; i<201;i++){
        vec_y[i] = vec_x[i]*vec_x[i]+10.0;
    }
    draw_graph(figure1,vec_x,vec_y,201,201);
    draw_axis(figure1);
    title(figure1,"Kuvaajan piirtäminen");
    int r = run_gtk(argc,argv,figure1);
    return r;
}