difference(){
    cube([128+2,12,12],true);
    linear_extrude(12+2,center=true){
        circle(d=4);
    }
}