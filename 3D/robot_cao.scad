//Echelle 
CM=10; //1cm = 10 mm
MM=CM/10;
Max=CM*12; //Taille max 12 CM
Max2=999;
Dessin_complet=true;

// Raspberry
Rasp_Long=8.7*CM; // longueur 
Rasp_Larg=5.9*CM; // largeur 
Rasp_Diam_Trou=2.75*MM; // Diamètre d'un trou 
Rasp_Long1=5.8*CM; // Distance entre les trous sur la longueur 
Rasp_Larg1=4.9*CM; // Distance entre les trous sur la largeur
Rasp_Esp_Trou=3.5*MM;// Distance entre le trou et le bord de la plaque
Rasp_Diam_Tige=6.2*MM;// Diamètre de la tige de support
Rasp_Lng_Tige=0.75*CM; // Longeur des petites entretoises 
Rasp_Haut=3*CM; // Volume Raspberry

// Puissance
Puiss_Long=9*CM;
Puiss_Larg=8*CM;

// Servo moteur
Smot_Long=40.6*MM;
Smot_Larg=2*CM;
Smot_Prof=36.6*MM;
Smot_Prof2=26.8*MM; // partie sous la plaque
Smot_Esp_Larg=0.5*CM;
Smot_Esp_Long=0.4*CM;
Smot_Long1=53.4*MM;
Smot_Long2=48.4*MM;

// Chassis
Ch_Eps=CM/4; // Epaisseur du chassi 2,5 mm
Ch_Haut_Tige=4*CM; // Hauteur tige chassi haut
Ch_Haut_Tige2=5.2*CM; // Hauteur tige chassi bas
Esp_etage1=Ch_Haut_Tige*2+Ch_Haut_Tige2;
Ch_entretoise=0.2*CM; // Longeur des toutes petites entretoises 

// Moteur
Ch_Diam_Roue=6.5*CM;
Ch_Larg_Roue=2.6*CM;
Ch_Eps_Roue=1*CM;
Ch_Diam_Axe_Roue=1.7*CM;
Ch_Long_Axe_Roue=Ch_Larg_Roue+6*MM;

// Plaque essai
Plaque_Haut=8*MM;

// Composants

Cmp_epaisseur=1.5*MM; // épaisseur des plaques électroniques

echo("Hauteur totale=",Ch_Eps*4+Ch_Haut_Tige+Ch_Haut_Tige2*2);


Motor_Long=42*MM;
Motor_Larg=22.7*MM;
Motor_Prof=13.8*MM;
Motor_Long2=45.1*MM;
Motor_Fix_Esp=1.7*CM; // Espacement fixation
Motor_Fix_Haut=1.4*MM; // Hauteur fixation
Motor_Fix_Diam=4*MM; // Dimamètre fixation
Motor_Vis_Diam=2.6*MM;
Motor_Vis_Esp=1.15*MM; // Espace entre le bord et les vis
Motor_Vis_Long=2.5*CM;
Motor_Axe_Esp=1*CM;
Motor_Axe_Diam=3.6*MM;
Motor_Axe_Long=36.7*MM;
Motor_Axe_Roue=8.5*MM; // partie de l'axe qui supporte la roue

module support1 () // Support Df robot capteur lumière
{
    difference()
    {
        color("blue")
        cube ([22*MM,30*MM,Cmp_epaisseur],true);
        entretoise_support ();
    }
}

module entretoise_support () // Entretoise des capteurs
{
    translate([22*MM/2-Rasp_Esp_Trou,-30*MM/2+Rasp_Esp_Trou,0])
    cylinder(h=Ch_entretoise,r=Rasp_Diam_Tige/2, center=true);
    translate([-22*MM/2+Rasp_Esp_Trou,-30*MM/2+Rasp_Esp_Trou,0])
    cylinder(h=Ch_entretoise,r=Rasp_Diam_Tige/2, center=true);
}

module support2 () // Support Df robot capteur de son
{
    difference ()
    {
        color("pink")
        cube ([22*MM,30*MM,2*MM],true);
        entretoise_support ();
    }
}

module motor()
{
    rotate(a=[90,0,0])
    color("yellow")
    cube([Motor_Long,Motor_Larg,Motor_Prof],true); // différentiel
    translate([-(Motor_Long/2-(Motor_Vis_Esp+Motor_Vis_Diam/2)),0,
        (Motor_Larg/2)-(Motor_Vis_Esp+Motor_Vis_Diam/2)])
    rotate(a=[90,0,0])
    color("lightgrey")
    cylinder(h=Motor_Vis_Long, r=Motor_Vis_Diam/2, center=true); // vis
    translate([-(Motor_Long/2-(Motor_Vis_Esp+Motor_Vis_Diam/2)),0,
        -((Motor_Larg/2)-(Motor_Vis_Esp+Motor_Vis_Diam/2))])
    rotate(a=[90,0,0])
    color("lightgrey")
    cylinder(h=Motor_Vis_Long, r=Motor_Vis_Diam/2, center=true); // vis
        
    translate([Motor_Prof-Motor_Larg/2,(Motor_Long2-Motor_Prof)/2,0])
    color("lightgrey")
    intersection(){
        translate([Motor_Larg/2,0,0])
        rotate(a=[90,0,0])
        cylinder(h=Motor_Long2, r=Motor_Larg/2, center=true); // moteur 
        translate([Motor_Larg/2,0,0])
        rotate(a=[90,0,90])
        cube([Motor_Long2,Motor_Larg,Motor_Prof],true);   
    }
    translate([Motor_Long/2-Motor_Fix_Esp,-Motor_Prof/2-Motor_Fix_Haut/2,0])
    rotate(a=[90,0,0])
    cylinder(h=Motor_Fix_Haut, r=Motor_Fix_Diam/2, center=true); // point de fixation
    color("lightgrey")
    translate([-(Motor_Long/2-Motor_Axe_Esp),(Motor_Axe_Long-Motor_Prof)/2-Motor_Axe_Roue,0])
    rotate(a=[90,0,0])
    cylinder(h=Motor_Axe_Long, r=Motor_Axe_Diam/2, center=true); // axe moteur
}

module raspberry()
{
    color("lightgreen")
    rotate([0,0,180])
    translate([-Rasp_Long/2,-Rasp_Larg/2-1,Ch_Eps+1])
    import("/Users/Vincent/Things/raspberry.stl");
}

module puissance()
{
    color("red")
    translate([0,0,-Ch_Haut_Tige+Rasp_Haut/2])
    cube([Puiss_Long,Puiss_Larg,20],true); 
}

module servo(X,Y,Z)
{
    color("grey")
    translate([X,Y,Z])
    cube([Smot_Long,Smot_Larg,Smot_Prof],true);
    difference()
    {
    color("grey")
    translate([X+Smot_Long/2+5*MM,Y,Z+Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cube([1*CM,Smot_Larg,5*MM],true); 
    servo_trou(X,Y,Z); 
    }  
    difference()
    {
    color("grey")
    translate([X-Smot_Long/2-5*MM,Y,Z+Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cube([1*CM,Smot_Larg,5*MM],true); 
    servo_trou(X,Y,Z);
    }
}

module servo_trou(X,Y,Z)
{
    translate([X+Smot_Long/2+(Smot_Long1-Smot_Long2),Y+0.5*CM,Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true);   
    translate([X+Smot_Long/2+(Smot_Long1-Smot_Long2),Y-0.5*CM,Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true); 
    translate([X-Smot_Long/2-(Smot_Long1-Smot_Long2),Y+0.5*CM,Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true);   
    translate([X-Smot_Long/2-(Smot_Long1-Smot_Long2),Y-0.5*CM,Smot_Prof/2-(Smot_Prof-Smot_Prof2)+3*MM])
    cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true);
}

module Roue()
{
    
    difference(){
    translate([-(Motor_Long/2-Motor_Axe_Esp),(Motor_Axe_Long-Motor_Prof)/2-Motor_Axe_Roue-Ch_Larg_Roue,0])
    rotate(a=[90,0,0])
    cylinder(h=Ch_Larg_Roue, r=Ch_Diam_Roue/2, center=true);
    translate([-(Motor_Long/2-Motor_Axe_Esp),(Motor_Axe_Long-Motor_Prof)/2-Motor_Axe_Roue-Ch_Larg_Roue,0])
    rotate(a=[90,0,0])
    cylinder(h=Ch_Larg_Roue, r=Ch_Diam_Roue/2-Ch_Eps_Roue, center=true); 
    }
    translate([-(Motor_Long/2-Motor_Axe_Esp),(Motor_Axe_Long-Motor_Prof)/2-Motor_Axe_Roue-Ch_Larg_Roue,0])
    rotate(a=[90,0,0])
    cylinder(h=Ch_Larg_Roue, r=Ch_Diam_Axe_Roue/2, center=true); // axe roue
    
}

module entretoise(nb,P)
{
    for ( i= [0:nb-1])   
    {
        color("yellow")
        translate(P[i]) 
        difference()
            {    
                cylinder(h=Rasp_Lng_Tige, r=Rasp_Diam_Tige/2, center=true);
                cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true);
            }
    }
}

module tige(nb,P,H)
{
    for ( i= [0:nb-1])   
    {
        color("yellow")
        translate(P[i]) 
        difference()
        {    
            cylinder(h=H, r=Rasp_Diam_Tige/2, center=true);
            cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true);
        }
    }
}

module plaque(nb,P)
{
    difference()
    {    
        cylinder(h=Ch_Eps, r=Max/2, center=true); // base
        for ( i= [0:nb-1])   
        {
            translate(P[i])
            cylinder(h=Max2, r=Rasp_Diam_Trou/2, center=true); // trou 
        }
    }
 }


// ********* Tracé du chassi étage Raspberry *************

if (Dessin_complet) raspberry();

X1=(Rasp_Long-Rasp_Long1)/2-Rasp_Esp_Trou;
Y1=Rasp_Larg1/2;
Z1=Ch_Eps;

P1=[X1+Rasp_Long1/2,Y1,Z1];
P2=[X1-Rasp_Long1/2,Y1,Z1];
P3=[X1+Rasp_Long1/2,-Y1,Z1];
P4=[X1-Rasp_Long1/2,-Y1,Z1];

entretoise(4,[P1,P2,P3,P4]);

X2=Rasp_Long/2+Rasp_Diam_Tige/2;
Y2=Rasp_Larg/2+Rasp_Diam_Tige/2;
Z2=Ch_Haut_Tige2/2+Ch_Eps/2;

P5=[X2,Y2,Z2];
P6=[-X2,Y2,Z2];
P7=[X2,-Y2,Z2];
P8=[-X2,-Y2,Z2];

tige(4,[P5,P6,P7,P8],Ch_Haut_Tige2);

plaque(8,[P1,P2,P3,P4,P5,P6,P7,P8]);

// ******** tracé étage puissance *********

if (Dessin_complet) puissance();

Z3=-Ch_Haut_Tige+Ch_Eps;

P9=[-(Puiss_Long/2-0.5*CM),Puiss_Larg/2-0.5*CM,Z3];
P10=[Puiss_Long/2-0.5*CM,Puiss_Larg/2-1.75*CM,Z3];
P11=[-(Puiss_Long/2-0.5*CM),-(Puiss_Larg/2-1*CM),Z3];
P12=[Puiss_Long/2-1.5*CM,-(Puiss_Larg/2-1*CM),Z3];

entretoise(4,[P9,P10,P11,P12]);

X4=Max/2-3.5*CM;
Y4=Max/2-1*CM;
Z4=-Ch_Haut_Tige/2+Ch_Eps/2;

P13=[X4,Y4,Z4];
P14=[-X4,Y4,Z4];
P15=[X4,-Y4,Z4];
P16=[-X4,-Y4,Z4];

tige(4,[P13,P14,P15,P16],Ch_Haut_Tige);

translate([0,0,-Ch_Haut_Tige])
    plaque(8,[P9,P10,P11,P12,P13,P14,P15,P16]);

// ******** tracé étage camera *********
X6=Smot_Long/2;
Y6=0;
Z6=Ch_Haut_Tige2+Ch_Eps/2-(Smot_Prof/2-(Smot_Prof-Smot_Prof2));
if (Dessin_complet) 
{ servo(X6,Y6,Z6); 
    translate([X6+Smot_Long/2,Y6,Z6+3.5*CM])
    rotate(a=[90,0,90])
    servo(0,0,0);
    }

Z5=Ch_Haut_Tige2/2+Ch_Eps/2;

P17=[X2,Y2,Z5];
P18=[-X2,Y2,Z5];
P19=[X2,-Y2,Z5];
P20=[-X2,-Y2,Z5];

difference(){
translate([0,0,Ch_Haut_Tige2])
    plaque(4,[P17,P18,P19,P20]);
    servo(X6,Y6,Z6);
    servo_trou(X6,Y6,Z6);  
}



translate([0,-3.9*CM,Ch_Haut_Tige2+Ch_entretoise+Cmp_epaisseur])
rotate([0,0,-30])
{
    if (Dessin_complet) support1(); // capteur de lumière droit
    entretoise_support();
}

translate([-2.5*CM,-3.2*CM,Ch_Haut_Tige2+Ch_entretoise+Cmp_epaisseur])
rotate([0,0,-30])
{
    if (Dessin_complet) support2(); // capteur de son droit
    entretoise_support();
}

mirror([0,1,0])
{
    translate([0,-3.9*CM,Ch_Haut_Tige2+Ch_entretoise+Cmp_epaisseur])
    rotate([0,0,-30])
    {
        if (Dessin_complet) support1();  // capteur de lumière gauche
        entretoise_support();
    }

    translate([-2.5*CM,-3.2*CM,Ch_Haut_Tige2+Ch_entretoise+Cmp_epaisseur])
    rotate([180,0,-30])
    {
        if (Dessin_complet) support2(); // capteur de son gauche
        entretoise_support();
    }
}

translate([-4.2*CM,0,Ch_Haut_Tige2+Ch_entretoise+Cmp_epaisseur])
rotate([180,0,90])
{
    if (Dessin_complet) support1(); // capteur télécommande
    entretoise_support();
}

// ******** tracé étage moteur *********

Pile_Long=5*CM;

module batterie()
{
    if (Dessin_complet) 
    rotate([0,0,90])
    color("lime")
    for (i=[1:6])
       translate([-(1.5*CM*3+1.5*CM/2)+1.5*CM*i,0,0])
            cylinder(h=Pile_Long, r=1.5*CM/2, center=true); 
    
    difference(){
    color("cornflowerblue")
    translate([0,0,-Pile_Long/4])
    cube([1.5*CM+0.5*CM,1.5*CM*6,Pile_Long/2],true);
    translate([0,-2.5*MM,-Pile_Long/4])
    cube([1.5*CM+0.5*CM-4*MM,1.5*CM*6,Pile_Long/2],true);  
    }
}

module pile_9V()
{
    color("lightblue")
    import("/Users/Vincent/Things/9V_bat.STL");
}

module plaque_essai()
{
    color("chocolate")
    cube([8.5*CM,5.5*CM,Plaque_Haut],true);
}

module Motor_Sur_Plaque()
{
    if (Dessin_complet) 
        translate([Motor_Long/2,-4*CM,-(Esp_etage1+3.5*Ch_Eps)+Motor_Long2/2])
            rotate([0,180,0])
                {
                motor();
                    color("lightgrey")
                    Roue();
                }
            
    difference(){
        translate([Motor_Long/2,-4*CM-Motor_Prof/2-2.5*MM/2,-(Esp_etage1+2.4*Ch_Eps)+Motor_Long2/2])
            rotate([90,0,0])
                color("red")
                    cube([Motor_Long2,Motor_Larg+10,2.5*MM],true); // support moteur
        translate([Motor_Long/2,-4*CM,-(Esp_etage1+3.5*Ch_Eps)+Motor_Long2/2])
            rotate([0,180,0])
                motor();
        translate([-(Motor_Long/2-Motor_Axe_Esp),(Motor_Axe_Long-Motor_Prof)/2-Motor_Axe_Roue-Ch_Larg_Roue,0])
        rotate(a=[90,0,0])
        cylinder(h=Ch_Larg_Roue, r=Ch_Diam_Axe_Roue/2, center=true); // axe roue
   }
}

difference(){
translate([0,0,-(Esp_etage1)])
    plaque(1,[P17,P18,P19,P20]);
    translate([Motor_Long/2-1*CM,-4*CM-Motor_Prof/2-2.8*CM/2,-(Esp_etage1+3*Ch_Eps)+Motor_Long2/2])
            rotate([90,0,0])
                    cube([Motor_Long2,Motor_Larg+10,2*CM],true);
    mirror([0,1,0])
    translate([Motor_Long/2-1*CM,-4*CM-Motor_Prof/2-2.8*CM/2,-(Esp_etage1+3*Ch_Eps)+Motor_Long2/2])
            rotate([90,0,0])
                    cube([Motor_Long2,Motor_Larg+10,2*CM],true);
}

Motor_Sur_Plaque();
mirror([0,1,0])
    Motor_Sur_Plaque();
translate([-1.7*CM,0,-Esp_etage1+Pile_Long/2])
batterie();

if (Dessin_complet)
{translate([-4.1*CM,0,-Esp_etage1+Pile_Long/2-0.8*CM])
rotate([180,0,0])
pile_9V();
}

// Etage alimentation

translate([0,0,-Ch_Haut_Tige*2])
    plaque(0,[P9]); 

if (Dessin_complet)
translate([-1*CM,0,-Ch_Haut_Tige*2+Plaque_Haut/2])
plaque_essai();

color("Linen")
translate([0,-4.6*CM,-Ch_Haut_Tige*2+Plaque_Haut/2-0.5*CM])
import("/Users/Vincent/Things/ultra_sensor_holder.STL");

mirror([0,1,0])
color("Linen")
translate([0,-4.6*CM,-Ch_Haut_Tige*2+Plaque_Haut/2-0.5*CM])
import("/Users/Vincent/Things/ultra_sensor_holder.STL");


color("Linen")
translate([4.6*CM,0,-Ch_Haut_Tige*2+Plaque_Haut/2-0.5*CM])
rotate([0,0,90])
import("/Users/Vincent/Things/ultra_sensor_holder.STL");