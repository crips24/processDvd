#define _POSIX_C_SOURCE 200809L			// pour que vscode arrete de me casser les couilles
#include <time.h>					

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <stdlib.h>



// TODO:
// fix le comportement quand tu spam le f12
// optimiser la mémoire
// handle quand on ferme une fenetre
// ne pas sauter d'un pont (difficile)
// ajouter les args dans la command line / fichier config
// manpage a faire
// multi screen support
// multiple resolutions support


static inline void limit_60hz_abs(void) { // merde temporaire que chatgpt a écrit pour eviter que mon pc se suicide
    static struct timespec next = {0,0};
    const long NS_PER_FRAME = 16666667L;

    if (next.tv_sec==0 && next.tv_nsec==0) {
        clock_gettime(CLOCK_MONOTONIC, &next);
    }

    
    next.tv_nsec += NS_PER_FRAME;
    if (next.tv_nsec >= 1000000000L) { next.tv_sec += 1; next.tv_nsec -= 1000000000L; }

    
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if ((now.tv_sec > next.tv_sec) || (now.tv_sec==next.tv_sec && now.tv_nsec > next.tv_nsec)) {
        next = now; 
        return;
    }

    
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
}


int main(){
	
	Display*d=XOpenDisplay(NULL);
	Window r=DefaultRootWindow(d);
	
	//setup pour recup les infos (touche/fenetres)

	KeyCode k=XKeysymToKeycode(d,XK_F12); 								// je veut ecouter f12
	XGrabKey(d,k,AnyModifier,r,True,GrabModeAsync,GrabModeAsync); 		// j'écoute f12 et je m'enfout de tout (modifier, )
	
	Atom net_active;									
	Window fnet[10];											// pour stocker les fenetres, grand max 10 jusqua que je trouve comment faire mieu
	int speedFnet[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};	// fleme de faire une liste en 2D
	int fnetCount=0;											// index pour mes conneries
	int speed =1;
	int fnetGeometry[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	Atom bordur = XInternAtom(d, "_NET_FRAME_EXTENTS", False);
	


	Atom actual_type;
	int actual_format;
	            unsigned long nitems, bytes_after;
	            unsigned char *prop = NULL;						// bullshit inutile pour recuperer les fenetres

	XWindowAttributes atr;
	XGetWindowAttributes(d,r,&atr);


	int height = atr.height;
	int width = atr.width;


	
    while(True){
		if (fnetCount==10) fnetCount=0;							// oui j'ai ff de mettre des commentaires en plein millieu, me jugez pas :/
        XEvent e;
		while(XPending(d)){
			XNextEvent(d,&e);									// update j'ai FF mon FF donc je remet des commentaires, je ne garentis pas la qualité
        if(e.type==KeyPress) {
			puts("F12!");
			net_active=XInternAtom(d,"_NET_ACTIVE_WINDOW",False);

			XGetWindowProperty(d,r,net_active,0L,(~0L),False,AnyPropertyType,&actual_type,&actual_format,&nitems,&bytes_after,&prop);			//bullshit qui sert juste a récuperer l'id de la fenetre
			
			Window fnetr = (Window)((unsigned long*)prop)[0];
			fnet[fnetCount]=fnetr;
			fnetCount++;
			
			//XMoveWindow(d,(Window)((unsigned long*)prop)[0],100,100);
			XFree(prop);
        }}
		int i=0;


		int xspeed;																					// bullshit pour faire foncitonner ma merde
		int yspeed;																					// y sont clairs mes commentaires non ?
		Window fnetr;
		Window azerty;
		int vrax, vray;
		XWindowAttributes atri;

		long top=0;

		while(i<fnetCount){

			fnetr = fnet[i];
			

			XGetWindowProperty(d,fnetr, bordur, 0, 4, False, XA_CARDINAL,&actual_type,&actual_format,&nitems,&bytes_after,&prop);		// si jamais le wm a des bordures autour des fenetres
			XGetWindowAttributes(d,fnetr,&atri);
			XTranslateCoordinates(d,fnetr,DefaultRootWindow(d),0, 0,&vrax, &vray,&azerty);

			if (prop !=NULL) top = ((long*)prop)[2];
			XFree(prop);

			if(vrax+atri.width>=width || vrax>=width || vrax <= 0) speedFnet[2*i]*=-1;
			if(vray+atri.height>=height || vray>=height || vray <= 0+top) speedFnet[2*i+1]*=-1;				// en cas de bonk on change de traj (*-1 pour inverser gauche/droite/haut/bas)



			xspeed = speedFnet[2*i];																// pour faire plus joli
			yspeed = speedFnet[2*i+1];

			// printf("%s","top= ");
			// printf("%d",top);
			// printf("\n");

			XMoveWindow(d,fnetr,vrax+speed*xspeed,vray+speed*yspeed);								// clusterfuck mais en gros j'applique la vitesse aux coordonées avec la bonne traj
			

			i++;
			
		}   
		XFlush(d);
		limit_60hz_abs();  
    }
	XCloseDisplay(d); 																			// allez dodo (quite le programme)
    	
}
