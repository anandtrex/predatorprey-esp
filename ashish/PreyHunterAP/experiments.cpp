/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "experiments.h"
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>

//Variables for display
int SHOW = 0;
int MAP_LENGTH = 100;
int MAP_HEIGHT = 100;

//--------------------------------------------------------------------
// OpenGL initialization functions, copied from
// http://www.opengl.org/About/FAQ/technical/gettingstarted.htm#0030
//--------------------------------------------------------------------
static int sAttribList[] = {
   GLX_RGBA,
   GLX_RED_SIZE, 1,
   GLX_GREEN_SIZE, 1,
   GLX_BLUE_SIZE, 1,
   None };


//#define NO_SCREEN_OUT

//Perform evolution on Predprey for gens generations

Population *predprey_test(int gens) {


    // OpenGL variables
    Display *dpy;
    XVisualInfo *vi;
    XSetWindowAttributes swa;
    Window win;
    GLXContext cx;

  //Switching off display for condor runs - ADITYA
  { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "Error" <<endl; } //ADI


    dpy = XOpenDisplay(0);

  { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "Error" <<endl; }//ADI
    if ((vi=glXChooseVisual(dpy,DefaultScreen(dpy),sAttribList))==NULL) {
      fprintf(stderr, "ERROR: Can't find suitable visual!\n");
      return 0;
    }
    swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
                                   vi->visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
  //  win = XCreateSimpleWindow(dpy, RootWindow(dpy,DefaultScreen(dpy)),
  //                      0, 0, 1000, 1000, 1,
  //                      BlackPixel(dpy, DefaultScreen(dpy)),
  //                      WhitePixel(dpy, DefaultScreen(dpy)));
     win = XCreateWindow(dpy,RootWindow(dpy, vi->screen),
           30, 30, 500, 500, 0, vi->depth, CopyFromParent,
           vi->visual,CWBackPixel | CWBorderPixel | CWColormap, &swa);
     XMapWindow(dpy, win);
     cx = glXCreateContext(dpy, vi, 0, GL_TRUE);
     glXMakeCurrent(dpy, win, cx);


    // clear window
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
    glPointSize (10);
    glFlush();


    Population *pop=0;
    Genome *start_genome;
    char curword[20];
    int id;

    ostringstream *fnamebuf;
    int gen;

    int expcount;
    int status;
    int runs[NEAT::num_runs];
    int totalevals;
    int samples;  //For averaging

    memset (runs, 0, NEAT::num_runs * sizeof(int));

    ifstream iFile("singlepreyhunter_startgenes",ios::in);

    cout<<"START PREDPREY EVOLUTION"<<endl;

    cout<<"Reading in the start genome"<<endl;
    //Read in the start Genome
    iFile>>curword;
    iFile>>id;
    cout<<"Reading in Genome id "<<id<<endl;
    start_genome=new Genome(id,iFile);
    iFile.close();
  
    //Run multiple experiments
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {

      cout<<"EXPERIMENT #"<<expcount<<endl;

      cout<<"Start Genome: "<<start_genome<<endl;
      
      //Spawn the Population
      cout<<"Spawning Population off Genome"<<endl;
      
      //cout<<NEAT::pop_size<<endl;

      pop=new Population(start_genome,NEAT::pop_size);
      
      cout<<"Verifying Spawned Pop"<<endl;
      pop->verify();

//      cout<<gens;

      for (gen=1;gen<=gens;gen++) {
    	  if (gen == 1){
    		  SHOW = 1;
    	  }
    	  if (gen > 501) {
    		  SHOW = 0;
    	  }
	cout<<"Generation "<<gen<<endl;
	
	fnamebuf=new ostringstream();
	(*fnamebuf)<<"gen_"<<gen<<ends;  //needs end marker

#ifndef NO_SCREEN_OUT
	cout<<"name of fname: "<<fnamebuf->str()<<endl;
#endif	

	char temp[50];
        sprintf (temp, "gen_%d", gen);

	status=predprey_epoch(pop,gen,temp);
	//status=(pole1_epoch(pop,gen,fnamebuf->str()));
	
        if (status) {
	  runs[expcount]=status;
	  gen=gens+1;
	}
	
	fnamebuf->clear();
	delete fnamebuf;
	
      }

      if (expcount<NEAT::num_runs-1) delete pop;
    }

    totalevals=0;
    samples=0;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      cout<<runs[expcount]<<endl;
      if (runs[expcount]>0)
      {
        totalevals+=runs[expcount];
        samples++;
      }
    }

    cout<<"Failures: "<<(NEAT::num_runs-samples)<<" out of "<<NEAT::num_runs<<" runs"<<endl;
    cout<<"Average evals: "<<(samples>0 ? (double)totalevals/samples : 0)<<endl;

    return pop;

}

int predprey_epoch(Population *pop,int generation,char *filename) {
  cout << "--------------PREDPREY_EPOCH BEGINS------------------";
  cout << "#GENERATION: " << generation << endl;
  vector<Organism*>::iterator curorg1, curorg2, curorg3; //To hold the three predators evaluated during a trial
  vector<Species*>::iterator curspecies;
  //char cfilename[100];
  //strncpy( cfilename, filename.c_str(), 100 );

  //ofstream cfilename(filename.c_str()); 

  //Initializing the fitnesses of all the organisms to zero
  for(curorg1 = (pop->organisms).begin(); curorg1 != (pop->organisms).end(); ++curorg1) {
      (*curorg1)->fitness = 0;
      (*curorg1)->num_trials = 0;
  }

  bool win=false;
  int winnernum;
  int i, j; //Counters for loops

  int popsize = (int)(pop->organisms).size(); // The size of the current population
  int pred1, pred2, pred3; //The three predators that are randomly chosen from the population for each trial

  /* //Evaluate each organism on a test
  for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
    if (predprey_evaluate(*curorg)) win=true;
  }*/

  for (int t = 0; t < popsize; t++) { // Evaluating each predator in the population (single predator case)

	  pred1 = t;
	  //Evaluate three organisms (picked randomly each time) simultaneously a thousand times
	  for(j = 0; j < 5; j ++) {
		  /*
		  cout<<"Num_trials = "<<(pop->organisms)[pred1]->num_trials;
		  ((pop->organisms)[pred1]->num_trials)++; //Increment the number of trials for pred1 so that its final fitness can be found by averaging
		  cout<<" "<<(pop->organisms)[pred1]->num_trials<<endl; */


	   /* do {
			  pred2 = (lrand48() % popsize);
		  } while (pred2 == pred1); //Don't get the same predator twice */

		  pred2 = (lrand48() % popsize);
		  //((pop->organisms)[pred1]->num_trials)++;

		  pred3 = (lrand48() % popsize);
		  //((pop->organisms)[pred1]->num_trials)++;


		  //Find the iterator corresponding to each of these three randomly picked predators
		  for(curorg1 = (pop->organisms).begin(), i = 0; i < pred1; ++curorg1, i++);
		  for(curorg2 = (pop->organisms).begin(), i = 0; i < pred2; ++curorg2, i++);
		  for(curorg3 = (pop->organisms).begin(), i = 0; i < pred3; ++curorg3, i++);


		  if (predprey_evaluate(*curorg1)) win=true;
	  }
  }

  for(curorg1 = (pop->organisms).begin(); curorg1 != (pop->organisms).end(); ++curorg1) {
      if((*curorg1)->num_trials != 0) {
          (*curorg1)->fitness = (*curorg1)->fitness / (double)((*curorg1)->num_trials);
      }
  }

  double gen_avg_fitness = 0;

  for(curorg1 = (pop->organisms).begin(); curorg1 != (pop->organisms).end(); ++curorg1) {
      gen_avg_fitness += (*curorg1)->fitness;

  }

  gen_avg_fitness /= (double)popsize;

  cout<<"Average fitness: "<<gen_avg_fitness<<endl;

  //Print generation avg fitness to file
  ofstream outfile("results_new_weights", ios::app);

  outfile<<generation<<"\t"<<gen_avg_fitness<<"\t";

  outfile.close();
  
  //Average and max their fitnesses for dumping to file and snapshot
  for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {

    //This experiment control routine issues commands to collect ave
    //and max fitness, as opposed to having the snapshot do it, 
    //because this allows flexibility in terms of what time
    //to observe fitnesses at

    (*curspecies)->compute_average_fitness();
    (*curspecies)->compute_max_fitness();
  }

  //Take a snapshot of the population, so that it can be
  //visualized later on
  //if ((generation%1)==0)
  //  pop->snapshot();

  //Only print to file every print_every generations
    //Create the next generation
  pop->epoch(generation);

  cout << "--------------PREDPREY_EPOCH ENDS------------------";
  if (win) return ((generation-1)*NEAT::pop_size+winnernum);
  else return 0; 

}

bool predprey_evaluate(Organism *org1) {
  Network *net1, *net2, *net3;

  int numnodes;  /* Used to figure out how many nodes
		    should be visited during activation */
  int thresh;  /* How many visits will be allowed before giving up 
		  (for loop detection) */

  //  int MAX_STEPS=120000;
 int MAX_STEPS=100000;
  
  //net1 = org1->net;
 // numnodes=((org->gnome)->nodes).size();
  //thresh=numnodes*2;  //Max number of visits allowed per activation
  
  vector <double> fitnesses; //To hold the fitnesses of the three predators

  //Evaluate in the domain
  fitnesses = predprey_domain(org1, MAX_STEPS, thresh);

  org1->fitness = org1->fitness + fitnesses[0];

  org1->num_trials = org1->num_trials + 1;

  return false;

}

vector<double> predprey_domain(Organism *org1,int max_steps,int thresh) {
	 cout << "----------- PREDPREY_DOMAIN BEGINS -------------" << endl;
	 cout << "GENERATION: " << org1->generation << endl;
	 cout << "GENOME ID : " << org1->gnome->genome_id << endl;
	 Network *net1 = org1->net; // get the network from the organism
	 // OpenGL
	 if (SHOW) {

	  // clear display window
	  glClearColor (1.0, 1.0, 1.0, 0.0);
	  { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "2. Error" <<endl; } //ADI

	  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	  { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "3. Error" <<endl; } //ADI

	  glFlush();
	  { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "4. Error" <<endl; } //ADI

	     for(int i = 0; i < MAP_LENGTH; i++) {
	             for(int j = 0; j < MAP_LENGTH; j++) {
	                     glBegin (GL_POINTS);
	                     glColor3f(1.0f, 1.0f, 1.0f);
	                     glVertex3f (i, j, 0);
	                     void glEnd(void);
	             }
	     }

	 }

	 cout << "Initializing agents ..." << endl;
    //Vectors to hold the positions of the three predators and the four prey
    double pred_x[3];
    double pred_y[3];
    double prey_x[4];
    double prey_y[4];
    const unsigned int noHunters = 1;
    double hunter_x[noHunters];
    double hunter_y[noHunters];

    bool prey_caught[4] = {false, false, false, false}; // To see if a prey has already been caught or not
    bool pred_caught[3] = {false, false, false};		// See if hunter caught the predator

    int num_of_prey_caught = 0; //To count the number of prey caught by the team
    int num_of_prey_caught_individually[3] = {0, 0, 0}; //To count the number of prey caught by each predator

    int i, j, k; //Counters for loops 

    //Arrange the predators and prey randomly on the 100x100 grid
    for (i = 0; i < 3; i++) {
        pred_x[i] = (int) (drand48() * 100); 
        pred_y[i] = (int) (drand48() * 100);
    }
    for (i = 0; i < 4; i++) {
        prey_x[i] = (int) (drand48() * 100); 
        prey_y[i] = (int) (drand48() * 100);
    }
    for (i = 0; i < noHunters; i++) {
    	hunter_x[i] = (int) (drand48() * 100); // should we check if the prey and the hunter are on the same block by mistake
    	hunter_y[i] = (int) (drand48() * 100); // should we check if the prey and the hunter are on the same block by mistake
    }
  
    vector <double> fitnesses; //To hold the fitnesses of the three predators
    fitnesses.clear(); // empty it just to make sure

    const int maxSteps = 500; //Maximum number of timeteps [TODO: shouldn't this vary with the size of the grid]

    //Prey moves at 0.9 the speed of the predator
    double prey_move_probability = 0.9;

    double in[5];  //Input loading array

    double out1; // TODO:?????
    double out2; // TODO:?????

    vector<NNode*>::iterator out_iter; // for iterating over the output nodes of the network

    int steps = 0; //To count up to the maximum number of timesteps

   /*--- Iterate through the action-learn loop. ---*/
   cout << "Beginning the game ..." << endl;
   while (steps++ < maxSteps) {
       cout << "#step: " << steps << endl;
       cout << "prey movement begins ..." << endl;
       for(j = 0; j < 1; j++) {

    		int old_prey_x = prey_x[j]; // old prey positions are needed for erasing previous position
    		int old_prey_y = prey_y[j]; // old prey positions are needed for erasing previous position


           if (drand48() < prey_move_probability) {   
           
               //To hold the distances of the prey from its nearest predator
               int nearestDist = MAP_LENGTH * 2 + 1; //Big number: to compare distances
           
               int nearestPred = 0; //To hold the nearest predator to the prey
               
               int dist; // TODO: To hold the distance ?????
           
               //To hold the x- and y-distances from different predators
               int x_dist, y_dist, temp;
           
               int i; //Counter for loops
           
               int preyAction;
                              
               if (prey_caught[j]) { // if prey already caught move to the next prey
                   preyAction=4;
                   continue;         // next iteration of for loop!!!!!!
               }           

               for (i = 0; i < 1; i++) { // calculate the distance between the 1 prey and the 1 predator
                   dist = calc_dist (pred_x[i], pred_y[i], prey_x[j], prey_y[j], MAP_LENGTH, MAP_HEIGHT);
                   if (dist < nearestDist) { // this part is not needed
                       nearestDist = dist;   // get the actual distance between the predator and prey
                       nearestPred = i;      // nearest predator is the closest one.
                   }
               }

               x_dist = pred_x[nearestPred] - prey_x[j]; // NOTE: Predator - Prey
               
               if ((abs(x_dist)) > (MAP_LENGTH/2)) { // account for the toroid
                   temp = x_dist;
                   x_dist = MAP_LENGTH  - abs(x_dist);
                   if (temp > 0)
                       x_dist = 0 - x_dist;         // account for who is behind who
               }
               
               // do the same thing for y
               y_dist = pred_y[nearestPred] - prey_y[j]; // NOTE: Predator - Prey
               
               if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
                   temp = y_dist;
                   y_dist = MAP_HEIGHT  - abs(y_dist);
                   if (temp > 0)
                       y_dist = 0 - y_dist;
               }

               if (y_dist<0 && (abs(y_dist)>=abs(x_dist))) {		
                   preyAction = 0;
               }
               
               else if (x_dist<0 && (abs(x_dist)>=abs(y_dist))) {
                   preyAction = 1;
               }
               
               else if (y_dist>0 && (abs(y_dist)>=abs(x_dist))) {
                   preyAction = 2;
               }
               
               else if (x_dist>0 && (abs(x_dist)>=abs(y_dist))) {
                   preyAction = 3;
               }
               
               else {
                   preyAction = 4;
               }

               if (preyAction==0) {
                   prey_y[j]++; 
               }
	
               else if (preyAction==1) {
                   prey_x[j]++; }
               
               else if (preyAction==2) {
                   prey_y[j]--; }
               
               else if (preyAction==3) {
                   prey_x[j]--; }
               
               else if (preyAction==4) {}
               
               // account for the toroid
               if (prey_x[j] > MAP_LENGTH) 
                   prey_x[j] -= MAP_LENGTH;
       
               if (prey_y[j] > MAP_HEIGHT) 
                   prey_y[j] -= MAP_HEIGHT;
       
               if (prey_x[j] < 0) 
                   prey_x[j] += MAP_LENGTH;
       
               if (prey_y[j] < 0) 
                   prey_y[j] += MAP_HEIGHT;

            } //End of if condition to see if prey moves or not

       	if (SHOW) {

       		// erase old prey point
       		glBegin (GL_POINTS);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "15. Error" <<endl; } //ADI

       		glColor3f (1.0f, 1.0f, 1.0f);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "14. Error" <<endl; } //ADI

       		glVertex3f (old_prey_x, old_prey_y, 0);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "16. Error" <<endl; } //ADI

       		void glEnd(void);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "17. Error" <<endl; } //ADI

       		if(prey_caught[j] == false) { //Added this if-else condition so that caught prey are drawn in white ******PADMINI

       			// draw new prey point
       			glBegin (GL_POINTS);
       			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "19. Error" <<endl; } //ADI

                   // YELLOW for the prey
                   glColor3f (1.0f, 1.0f, 0.0f); //trying to change the colors of different prey ******PADMINI


       			glVertex3f (prey_x[j], prey_y[j], 0);
       			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "20. Error" <<endl; } //ADI

       			glEnd ();
       			//{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
       		}

       		else {
       			// draw new prey point
       			glBegin (GL_POINTS);
       			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "19. Error" <<endl; } //ADI

       			glColor3f (1.0f, 1.0f, 1.0f); //white color for the prey that've already been caught, i.e. they vanish ******PADMINI
       			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "18. Error" <<endl; } //ADI

       			glVertex3f (prey_x[j], prey_y[j], 0);
       			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "20. Error" <<endl; } //ADI

       			glEnd ();
       		//{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
       		}

       		glFlush();
       		//{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "22. Error" <<endl; } //ADI
       	}

       } //End of the prey action loop

       //Predator Action
       cout << "predator movement begins ..." << endl;
       for(i = 0; i < 1; i++) {

    	   //Saving old predator locations for display
    	   int old_pred_x = pred_x[i];
    	   int old_pred_y = pred_y[i];


           //To store the x- and y-distances from various prey
           int x_dist;
           int y_dist;

           int temp;

           for(j = 0, k = 0; j < 1; j++, k+=2) {
               
               if (!prey_caught[j]) {
                   x_dist = abs(pred_x[i] - prey_x[j]); //The distance in x-direction between predator and prey
                   y_dist = abs(pred_y[i] - prey_y[j]); //The distance in y-direction between predator and prey
               }

               else {
                   x_dist = 0; 
                   y_dist = 0;
               }

               if ((abs(x_dist)) > (MAP_LENGTH/2)) {
                   temp = x_dist;
                   x_dist = MAP_LENGTH  - abs(x_dist);
                   
                   if (temp > 0)
                       x_dist = 0 - x_dist;
               }
               
               if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
                   temp = y_dist;
                   y_dist = MAP_HEIGHT  - abs(y_dist);
                   
                   if (temp > 0)
                       y_dist = 0 - y_dist; 
               }

               in[1] = ( (double) x_dist ) / MAP_LENGTH; // dist(prey, predator); earlier in[k]
               in[2] = ( (double) y_dist ) / MAP_LENGTH; // dist(prey, predator); earlier in[k+1]
               in[3] = 0.0; // 0 => PREY
               //cout << "SENSOR INPUT: " << in[k] << "\t" << in[k+1] << endl;

           } //End of prey distance loop

           /*for(j = 0; j < 8; j++) {
               cout<<in[j]<<" ";
           }

           cout<<endl;
           cin>>j; */

		   // predator, hunter distance
           // using the same x_dist, y_dist
           x_dist = abs(pred_x[i] - hunter_x[0]); //The distance in x-direction between predator and prey
           y_dist = abs(pred_y[i] - hunter_y[0]); //The distance in y-direction between predator and prey
           if ((abs(x_dist)) > (MAP_LENGTH/2)) {
               temp = x_dist;
               x_dist = MAP_LENGTH  - abs(x_dist);

               if (temp > 0)
                   x_dist = 0 - x_dist;
           }

           if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
               temp = y_dist;
               y_dist = MAP_HEIGHT  - abs(y_dist);

               if (temp > 0)
                   y_dist = 0 - y_dist;
           }
           in[4] = ( (double) x_dist ) / MAP_LENGTH; // dist(predator, hunter); earlier in[k]
           in[5] = ( (double) y_dist ) / MAP_LENGTH; // dist(predator, hunter); earlier in[k+1]
           in[6] = 1.0; // designates => HUNTER
           in[0] = 1.0; // BIAS

           if(i == 0) { //Predator 1

        	   cout << "feeding inputs: ";
        	   for ( int oogabooga = 0; oogabooga < 5; oogabooga++ )
        		   cout << in[oogabooga] << "; ";
        	   cout << endl;

               net1->load_sensors(in);

               //Activate the net
               //If it loops, exit returning only fitness of 1 step
               if (!(net1->activate())) {
            	   cout<<"Net not activating!!"<<endl;
            		print_Genome_tofile((org1)->gnome,"Defective_genome");
               	   return fitnesses;
               }

               //Decide which way to move by seeing which output unit is greatest
               out_iter=net1->outputs.begin();

               double greatest = (*out_iter)->activation;
               k = 0;

               for(j = 0; j < 5; j++) { // 5 because there are 5 outputs. But will it query the right parts of the network
                   if (SHOW) {
                	   //cout << (*out_iter)->activation<<endl;
                   }
                   if((*out_iter)->activation > greatest) { //TODO: Is this okay? Should be but check!!!!
                   
                       greatest = (*out_iter)->activation;
                       k = j;  // store the highest activation or chosen direction.
                   }

                   ++out_iter;
               }               

           } //End of if condition for predator 1

          /* else if (i == 1) { //Predator 2

               net2->load_sensors(in);

               //Activate the net
               //If it loops, exit returning only fitness of 1 step
               if (!(net2->activate())) return fitnesses;

               //Decide which way to move by seeing which output unit is greatest
               out_iter=net2->outputs.begin();

               double greatest = (*out_iter)->activation;
               k = 0;

               for(j = 0; j < 5; j++) {
               
                   if((*out_iter)->activation > greatest) {
                   
                       greatest = (*out_iter)->activation;
                       k = j;
                   }

                   ++out_iter;
               }               

           } //End of if condition for predator 2

           else { //Predator 3

               net3->load_sensors(in);

               //Activate the net
               //If it loops, exit returning only fitness of 1 step
               if (!(net3->activate())) return fitnesses;

               //Decide which way to move by seeing which output unit is greatest
               out_iter=net3->outputs.begin();

               double greatest = (*out_iter)->activation;
               k = 0;

               for(j = 0; j < 5; j++) {
               
                   if((*out_iter)->activation > greatest) {
                   
                       greatest = (*out_iter)->activation;
                       k = j;
                   }

                   ++out_iter;
               }               

           } //End of if condition for predator 3 */


           int predAction = k;

           if(predAction == 0) pred_y[i]++;
           
           else if(predAction == 1) pred_x[i]++;

           else if(predAction == 2) pred_y[i]--;

           else if(predAction == 3) pred_x[i]--;

           else {} //idle

           if (pred_x[i] > MAP_LENGTH) 
               pred_x[i] -= MAP_LENGTH;
           
           if (pred_y[i] > MAP_HEIGHT) 
               pred_y[i] -= MAP_HEIGHT;
           
           if (pred_x[i] < 0) 
               pred_x[i] += MAP_LENGTH;
           
           if (pred_y[i] < 0) 
               pred_y[i] += MAP_HEIGHT;

       	//To display predators
       	double color1 = 0;
       	double color2 = 0;
       	double color3 = 0;
       	double colorFlicker = 0;
		// Implies the predator is black in color

       	if (SHOW) {

       		// erase old pred point
       		glBegin (GL_POINTS);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "6. Error" <<endl; } //ADI

       		glColor3f(1.0f, 1.0f, 1.0f);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "5. Error" <<endl; } //ADI

       		glVertex3f (old_pred_x, old_pred_y, 0);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "7. Error" <<endl; } //ADI

       		void glEnd(void);
       		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "8. Error" <<endl; } //ADI

       		// draw new pred point

       		        if (i==0) {
       		        	color1 = 1; // COLOR is RED
                               }
       		        else if (i==1) {
       		        	color2 = 1;
                               }
       		        else if (i==2) {
       		        	color3 = 1;
                               }

       		        glBegin (GL_POINTS);
       		        { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "10. Error" <<endl; } //ADI

       		        glColor3f (color1, color2, color3);
       		        { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "9. Error" <<endl; } //ADI

       		        glVertex3f (pred_x[i], pred_y[i], 0);
       		        { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "11. Error" <<endl; } //ADI

       		        void glEnd(void);
       		        { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "12. Error" <<endl; } //ADI

       		        glFlush();
       		        { GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "13. Error" <<endl; } //ADI

         	} //End display predators

           //Check if the predator caught a prey
           for(j = 0; j < 1; j++) {
               if(prey_caught[j] == false && pred_x[i] == prey_x[j] && pred_y[i] == prey_y[j]) {
                   prey_caught[j] = true;
                   //Prey reappears
                   //prey_x[j] = (int) (drand48() * 100);
                   //prey_y[j] = (int) (drand48() * 100);

                   //Fitness sharing
                   num_of_prey_caught++;  //Adding exception for zebra. The whole team gets fitness on catching a zebra
                   
                   num_of_prey_caught_individually[i]++; 
               }
           }

           //Check if the hunter caught the predator
           for(int jHunter = 0; jHunter < 1; jHunter++) { // iterate over all hunters
               if(pred_caught[i] == false && hunter_x[jHunter] == pred_x[i] && hunter_y[jHunter] == pred_y[i]) {
                   pred_caught[i] = true;
               }
           }


       } //End of predator action loop

       // HUNTER action
       cout << "hunter movement begins ..." << endl;
       int jHunter = 0; // Only one hunter
       int hunterAction = (int) (drand48() * 4.0);
       int old_hunter_x, old_hunter_y;
       old_hunter_x = hunter_x[jHunter];
       old_hunter_y = hunter_y[jHunter];

       if (hunterAction==0) {
           hunter_y[jHunter]++;
       }
       else if (hunterAction==1) {
           hunter_x[jHunter]++; }

       else if (hunterAction==2) {
           hunter_y[jHunter]--; }

       else if (hunterAction==3) {
           hunter_x[jHunter]--; }

       else if (hunterAction==4) {}

       // account for the toroid
       if (hunter_x[jHunter] > MAP_LENGTH)
           hunter_x[jHunter] -= MAP_LENGTH;

       if (hunter_y[jHunter] > MAP_HEIGHT)
           hunter_y[jHunter] -= MAP_HEIGHT;

       if (hunter_x[jHunter] < 0)
           hunter_x[jHunter] += MAP_LENGTH;

       if (hunter_y[jHunter] < 0)
           hunter_y[jHunter] += MAP_HEIGHT;

       // draw Hunter
      	if (SHOW) {

      		// erase old hunter point
      		glBegin (GL_POINTS);
      		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "15. Error" <<endl; } //ADI

      		glColor3f (1.0f, 1.0f, 1.0f);
      		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "14. Error" <<endl; } //ADI

      		glVertex3f (old_hunter_x, old_hunter_y, 0);
      		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "16. Error" <<endl; } //ADI

      		void glEnd(void);
      		{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "17. Error" <<endl; } //ADI

			// draw new hunter point
			glBegin (GL_POINTS);
			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "19. Error" <<endl; } //ADI

      // COLOR is GREY
			glColor3f (0.0f, 0.5f, 0.5f); //trying to change the colors of different hunters ******ASHU


			glVertex3f (hunter_x[jHunter], hunter_y[jHunter], 0);
			{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "20. Error" <<endl; } //ADI

			glEnd ();
			//{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void

      		glFlush();
      		//{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "22. Error" <<endl; } //ADI
      	}

       cout << "evaluate how many prey are caught ..." << endl;
       //While break condition when all prey are caught 
       int count_prey_caught = 0;
       for (i = 0; i < 1; i++) {
           if (prey_caught[i] == true) {
               count_prey_caught++;
           }
       }

       // Remove if you want the prey to reappear. I am keeping this @ASHU
       if(count_prey_caught >= 1) {
    	   cout << "prey caught ... breaking from big while loop" << endl;
           break;
       }

       // Break from the big while loop
       cout << "evaluate how many predators are caught by hunters ..." << endl;
       int count_pred_caught = 0;
       for (int iPredator = 0; iPredator < 1; iPredator++) {
    	   cout << "iPredator: " << iPredator << endl;
    	   if (pred_caught[iPredator] == true) {
    		   count_pred_caught++;
    	   }
       }

       // Break from the big while loop
       if (count_pred_caught == 1) {
    	   cout << "OOPs predator caught ... breaking from big while loop" << endl;
    	   break;
       }

       //Display stuff
       if (SHOW) {
    	   usleep(100000);
       }

   } //End of BIG while loop steps++ < maxSteps

   //Display stuff
   if (SHOW) { //Added another clear screen here ******PADMINI

       for(i = 0; i < MAP_LENGTH; i++) {
               for(j = 0; j < MAP_LENGTH; j++) {
                       glBegin (GL_POINTS);
                       glColor3f(1.0f, 1.0f, 1.0f);
                       glVertex3f (i, j, 0);
                       void glEnd(void);
               }
       }

   }

   //Calculate fitnesses
   for(i = 0; i < 1; i++) {
	   if (pred_caught[i] == false)
		   fitnesses.push_back(num_of_prey_caught*(maxSteps-steps)); // Reward for how quickly it caught the prey
	   else
		   fitnesses.push_back(-1*10*maxSteps);  // REALLY BAD FITNESS VALUE
   }
   if (num_of_prey_caught > 1) {
	   cout << " CAUGHT more than 1 prey  "<< num_of_prey_caught<<endl;
	   cin >> i;

   }
   for (i = 0; i < 1; i++) {
	   cout << "FITNESS: " << fitnesses[0] << endl;
   }
   //cout<<fitnesses[0]<<" "<<fitnesses[1]<<" "<<fitnesses[2]<<endl;

   cout << "----------- PREDPREY_DOMAIN ENDS -------------" << endl;
   return fitnesses;

}

// Calculates distance (actual distance, using pythag. thm)
// Takes shortest distance, ie. will wrap around edges
int calc_dist (int x1, int y1, int x2, int y2, int MAP_LENGTH, int MAP_HEIGHT) {

  int x_dist = 0;
  int y_dist = 0;

  x_dist = abs (x1 - x2);
  if (x_dist > (MAP_LENGTH/2))
    x_dist = MAP_LENGTH - x_dist;

  y_dist = abs (y1 - y2);
  if (y_dist > (MAP_HEIGHT/2))
    y_dist = MAP_HEIGHT - y_dist;

  return (x_dist + y_dist);

}


//Perform evolution on XOR, for gens generations
Population *xor_test(int gens) {
    Population *pop=0;
    Genome *start_genome;
    char curword[20];
    int id;

    ostringstream *fnamebuf;
    int gen;
 
    int evals[NEAT::num_runs];  //Hold records for each run
    int genes[NEAT::num_runs];
    int nodes[NEAT::num_runs];
    int winnernum;
    int winnergenes;
    int winnernodes;
    //For averaging
    int totalevals=0;
    int totalgenes=0;
    int totalnodes=0;
    int expcount;
    int samples;  //For averaging

    memset (evals, 0, NEAT::num_runs * sizeof(int));
    memset (genes, 0, NEAT::num_runs * sizeof(int));
    memset (nodes, 0, NEAT::num_runs * sizeof(int));

    ifstream iFile("xorstartgenes",ios::in);

    cout<<"START XOR TEST"<<endl;

    cout<<"Reading in the start genome"<<endl;
    //Read in the start Genome
    iFile>>curword;
    iFile>>id;
    cout<<"Reading in Genome id "<<id<<endl;
    start_genome=new Genome(id,iFile);
    iFile.close();

    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      //Spawn the Population
      cout<<"Spawning Population off Genome2"<<endl;

      pop=new Population(start_genome,NEAT::pop_size);
      
      cout<<"Verifying Spawned Pop"<<endl;
      pop->verify();
      
      for (gen=1;gen<=gens;gen++) {
	cout<<"Epoch "<<gen<<endl;	

	//This is how to make a custom filename
	fnamebuf=new ostringstream();
	(*fnamebuf)<<"gen_"<<gen<<ends;  //needs end marker

	#ifndef NO_SCREEN_OUT
	cout<<"name of fname: "<<fnamebuf->str()<<endl;
	#endif

	char temp[50];
	sprintf (temp, "gen_%d", gen);

	//Check for success
	if (xor_epoch(pop,gen,temp,winnernum,winnergenes,winnernodes)) {
	  //	if (xor_epoch(pop,gen,fnamebuf->str(),winnernum,winnergenes,winnernodes)) {
	  //Collect Stats on end of experiment
	  evals[expcount]=NEAT::pop_size*(gen-1)+winnernum;
	  genes[expcount]=winnergenes;
	  nodes[expcount]=winnernodes;
	  gen=gens;

	}
	
	//Clear output filename
	fnamebuf->clear();
	delete fnamebuf;
	
      }

      if (expcount<NEAT::num_runs-1) delete pop;
      
    }

    //Average and print stats
    cout<<"Nodes: "<<endl;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      cout<<nodes[expcount]<<endl;
      totalnodes+=nodes[expcount];
    }
    
    cout<<"Genes: "<<endl;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      cout<<genes[expcount]<<endl;
      totalgenes+=genes[expcount];
    }
    
    cout<<"Evals "<<endl;
    samples=0;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      cout<<evals[expcount]<<endl;
      if (evals[expcount]>0)
      {
	totalevals+=evals[expcount];
	samples++;
      }
    }

    cout<<"Failures: "<<(NEAT::num_runs-samples)<<" out of "<<NEAT::num_runs<<" runs"<<endl;
    cout<<"Average Nodes: "<<(samples>0 ? (double)totalnodes/samples : 0)<<endl;
    cout<<"Average Genes: "<<(samples>0 ? (double)totalgenes/samples : 0)<<endl;
    cout<<"Average Evals: "<<(samples>0 ? (double)totalevals/samples : 0)<<endl;

    return pop;

}

bool xor_evaluate(Organism *org) {
  Network *net;
  double out[4]; //The four outputs
  double this_out; //The current output
  int count;
  double errorsum;

  bool success;  //Check for successful activation
  int numnodes;  /* Used to figure out how many nodes
		    should be visited during activation */

  int net_depth; //The max depth of the network to be activated
  int relax; //Activates until relaxation

  //The four possible input combinations to xor
  //The first number is for biasing
  double in[4][3]={{1.0,0.0,0.0},
		   {1.0,0.0,1.0},
		   {1.0,1.0,0.0},
		   {1.0,1.0,1.0}};
  
  net=org->net;
  numnodes=((org->gnome)->nodes).size();

  net_depth=net->max_depth();

  //TEST CODE: REMOVE
  //cout<<"ACTIVATING: "<<org->gnome<<endl;
  //cout<<"DEPTH: "<<net_depth<<endl;

  //Load and activate the network on each input
  for(count=0;count<=3;count++) {
    net->load_sensors(in[count]);

    //Relax net and get output
    success=net->activate();

    //use depth to ensure relaxation
    for (relax=0;relax<=net_depth;relax++) {
      success=net->activate();
      this_out=(*(net->outputs.begin()))->activation;
    }

    out[count]=(*(net->outputs.begin()))->activation;

    net->flush();

  }
  
  if (success) {
    errorsum=(fabs(out[0])+fabs(1.0-out[1])+fabs(1.0-out[2])+fabs(out[3]));
    org->fitness=pow((4.0-errorsum),2);
    org->error=errorsum;
  }
  else {
    //The network is flawed (shouldnt happen)
    errorsum=999.0;
    org->fitness=0.001;
  }

  #ifndef NO_SCREEN_OUT
  cout<<"Org "<<(org->gnome)->genome_id<<"  error: "<<errorsum<<"  ["<<out[0]<<" "<<out[1]<<" "<<out[2]<<" "<<out[3]<<"]"<<endl;
  cout<<"Org "<<(org->gnome)->genome_id<<"  fitness: "<<org->fitness<<endl;
  #endif

  //  if (errorsum<0.05) { 
  //if (errorsum<0.2) {
  if ((out[0]<0.5)&&(out[1]>=0.5)&&(out[2]>=0.5)&&(out[3]<0.5)) {
    org->winner=true;
    return true;
  }
  else {
    org->winner=false;
    return false;
  }

}

int xor_epoch(Population *pop,int generation,char *filename,int &winnernum,int &winnergenes,int &winnernodes) {
  vector<Organism*>::iterator curorg;
  vector<Species*>::iterator curspecies;
  //char cfilename[100];
  //strncpy( cfilename, filename.c_str(), 100 );

  //ofstream cfilename(filename.c_str());

  bool win=false;


  //Evaluate each organism on a test
  for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
    if (xor_evaluate(*curorg)) {
      win=true;
      winnernum=(*curorg)->gnome->genome_id;
      winnergenes=(*curorg)->gnome->extrons();
      winnernodes=((*curorg)->gnome->nodes).size();
      if (winnernodes==5) {
	//You could dump out optimal genomes here if desired
	//(*curorg)->gnome->print_to_filename("xor_optimal");
	//cout<<"DUMPED OPTIMAL"<<endl;
      }
    }
  }
  
  //Average and max their fitnesses for dumping to file and snapshot
  for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {

    //This experiment control routine issues commands to collect ave
    //and max fitness, as opposed to having the snapshot do it, 
    //because this allows flexibility in terms of what time
    //to observe fitnesses at

    (*curspecies)->compute_average_fitness();
    (*curspecies)->compute_max_fitness();
  }

  //Take a snapshot of the population, so that it can be
  //visualized later on
  //if ((generation%1)==0)
  //  pop->snapshot();

  //Only print to file every print_every generations
  if  (win||
       ((generation%(NEAT::print_every))==0))
    pop->print_to_file_by_species(filename);


  if (win) {
    for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
      if ((*curorg)->winner) {
	cout<<"WINNER IS #"<<((*curorg)->gnome)->genome_id<<endl;
	//Prints the winner to file
	//IMPORTANT: This causes generational file output!
	print_Genome_tofile((*curorg)->gnome,"xor_winner");
      }
    }
    
  }

  pop->epoch(generation);

  if (win) return 1;
  else return 0;

}

//Perform evolution on single pole balacing, for gens generations
Population *pole1_test(int gens) {
    Population *pop=0;
    Genome *start_genome;
    char curword[20];
    int id;

    ostringstream *fnamebuf;
    int gen;

    int expcount;
    int status;
    int runs[NEAT::num_runs];
    int totalevals;
    int samples;  //For averaging

    memset (runs, 0, NEAT::num_runs * sizeof(int));

    ifstream iFile("pole1startgenes",ios::in);

    cout<<"START SINGLE POLE BALANCING EVOLUTION"<<endl;

    cout<<"Reading in the start genome"<<endl;
    //Read in the start Genome
    iFile>>curword;
    iFile>>id;
    cout<<"Reading in Genome id "<<id<<endl;
    start_genome=new Genome(id,iFile);
    iFile.close();
  
    //Run multiple experiments
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {

      cout<<"EXPERIMENT #"<<expcount<<endl;

      cout<<"Start Genome: "<<start_genome<<endl;
      
      //Spawn the Population
      cout<<"Spawning Population off Genome"<<endl;
      
      pop=new Population(start_genome,NEAT::pop_size);
      
      cout<<"Verifying Spawned Pop"<<endl;
      pop->verify();

      for (gen=1;gen<=gens;gen++) {
	cout<<"Generation "<<gen<<endl;
	
	fnamebuf=new ostringstream();
	(*fnamebuf)<<"gen_"<<gen<<ends;  //needs end marker

#ifndef NO_SCREEN_OUT
	cout<<"name of fname: "<<fnamebuf->str()<<endl;
#endif	

	char temp[50];
        sprintf (temp, "gen_%d", gen);

	status=pole1_epoch(pop,gen,temp);
	//status=(pole1_epoch(pop,gen,fnamebuf->str()));
	
	if (status) {
	  runs[expcount]=status;
	  gen=gens+1;
	}
	
	fnamebuf->clear();
	delete fnamebuf;
	
      }

      if (expcount<NEAT::num_runs-1) delete pop;
    }

    totalevals=0;
    samples=0;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      cout<<runs[expcount]<<endl;
      if (runs[expcount]>0)
      {
        totalevals+=runs[expcount];
        samples++;
      }
    }

    cout<<"Failures: "<<(NEAT::num_runs-samples)<<" out of "<<NEAT::num_runs<<" runs"<<endl;
    cout<<"Average evals: "<<(samples>0 ? (double)totalevals/samples : 0)<<endl;

    return pop;

}

int pole1_epoch(Population *pop,int generation,char *filename) {
  vector<Organism*>::iterator curorg;
  vector<Species*>::iterator curspecies;
  //char cfilename[100];
  //strncpy( cfilename, filename.c_str(), 100 );

  //ofstream cfilename(filename.c_str());

  bool win=false;
  int winnernum;

  //Evaluate each organism on a test
  for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
    if (pole1_evaluate(*curorg)) win=true;
  }
  
  //Average and max their fitnesses for dumping to file and snapshot
  for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {

    //This experiment control routine issues commands to collect ave
    //and max fitness, as opposed to having the snapshot do it, 
    //because this allows flexibility in terms of what time
    //to observe fitnesses at

    (*curspecies)->compute_average_fitness();
    (*curspecies)->compute_max_fitness();
  }

  //Take a snapshot of the population, so that it can be
  //visualized later on
  //if ((generation%1)==0)
  //  pop->snapshot();

  //Only print to file every print_every generations
  if  (win||
       ((generation%(NEAT::print_every))==0))
    pop->print_to_file_by_species(filename);

  if (win) {
    for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
      if ((*curorg)->winner) {
	winnernum=((*curorg)->gnome)->genome_id;
	cout<<"WINNER IS #"<<((*curorg)->gnome)->genome_id<<endl;
      }
    }    
  }

  //Create the next generation
  pop->epoch(generation);

  if (win) return ((generation-1)*NEAT::pop_size+winnernum);
  else return 0;

}

bool pole1_evaluate(Organism *org) {
  Network *net;

  int numnodes;  /* Used to figure out how many nodes
		    should be visited during activation */
  int thresh;  /* How many visits will be allowed before giving up 
		  (for loop detection) */

  //  int MAX_STEPS=120000;
 int MAX_STEPS=100000;
  
  net=org->net;
  numnodes=((org->gnome)->nodes).size();
  thresh=numnodes*2;  //Max number of visits allowed per activation
  
  //Try to balance a pole now
  org->fitness = go_cart(net,MAX_STEPS,thresh);

#ifndef NO_SCREEN_OUT
  cout<<"Org "<<(org->gnome)->genome_id<<" fitness: "<<org->fitness<<endl;
#endif

  //Decide if its a winner
  if (org->fitness>=MAX_STEPS) { 
    org->winner=true;
    return true;
  }
  else {
    org->winner=false;
    return false;
  }

}

//     cart_and_pole() was take directly from the pole simulator written
//     by Richard Sutton and Charles Anderson.
int go_cart(Network *net,int max_steps,int thresh)
{
   float x,			/* cart position, meters */
         x_dot,			/* cart velocity */
         theta,			/* pole angle, radians */
         theta_dot;		/* pole angular velocity */
   int steps=0,y;

   int random_start=1;

   double in[5];  //Input loading array

   double out1;
   double out2;

//     double one_degree= 0.0174532;	/* 2pi/360 */
//     double six_degrees=0.1047192;
   double twelve_degrees=0.2094384;
//     double thirty_six_degrees= 0.628329;
//     double fifty_degrees=0.87266;

   vector<NNode*>::iterator out_iter;

   if (random_start) {
     /*set up random start state*/
     x = (lrand48()%4800)/1000.0 - 2.4;
     x_dot = (lrand48()%2000)/1000.0 - 1;
     theta = (lrand48()%400)/1000.0 - .2;
     theta_dot = (lrand48()%3000)/1000.0 - 1.5;
    }
   else 
     x = x_dot = theta = theta_dot = 0.0;
   
   /*--- Iterate through the action-learn loop. ---*/
   while (steps++ < max_steps)
     {
       
       /*-- setup the input layer based on the four iputs --*/
       //setup_input(net,x,x_dot,theta,theta_dot);
       in[0]=1.0;  //Bias
       in[1]=(x + 2.4) / 4.8;;
       in[2]=(x_dot + .75) / 1.5;
       in[3]=(theta + twelve_degrees) / .41;
       in[4]=(theta_dot + 1.0) / 2.0;
       net->load_sensors(in);

       //activate_net(net);   /*-- activate the network based on the input --*/
       //Activate the net
       //If it loops, exit returning only fitness of 1 step
       if (!(net->activate())) return 1;

      /*-- decide which way to push via which output unit is greater --*/
       out_iter=net->outputs.begin();
       out1=(*out_iter)->activation;
       ++out_iter;
       out2=(*out_iter)->activation;
       if (out1 > out2)
	 y = 0;
       else
	 y = 1;
       
       /*--- Apply action to the simulated cart-pole ---*/
       cart_pole(y, &x, &x_dot, &theta, &theta_dot);
       
       /*--- Check for failure.  If so, return steps ---*/
       if (x < -2.4 || x > 2.4  || theta < -twelve_degrees ||
	   theta > twelve_degrees) 
         return steps;             
     }
   
   return steps;
} 


//     cart_and_pole() was take directly from the pole simulator written
//     by Richard Sutton and Charles Anderson.
//     This simulator uses normalized, continous inputs instead of 
//    discretizing the input space.
/*----------------------------------------------------------------------
   cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
----------------------------------------------------------------------*/
void cart_pole(int action, float *x,float *x_dot, float *theta, float *theta_dot) {
  float xacc,thetaacc,force,costheta,sintheta,temp;
  
  const float GRAVITY=9.8;
  const float MASSCART=1.0;
  const float MASSPOLE=0.1;
  const float TOTAL_MASS=(MASSPOLE + MASSCART);
  const float LENGTH=0.5;	  /* actually half the pole's length */
  const float POLEMASS_LENGTH=(MASSPOLE * LENGTH);
  const float FORCE_MAG=10.0;
  const float TAU=0.02;	  /* seconds between state updates */
  const float FOURTHIRDS=1.3333333333333;

  force = (action>0)? FORCE_MAG : -FORCE_MAG;
  costheta = cos(*theta);
  sintheta = sin(*theta);
  
  temp = (force + POLEMASS_LENGTH * *theta_dot * *theta_dot * sintheta)
    / TOTAL_MASS;
  
  thetaacc = (GRAVITY * sintheta - costheta* temp)
    / (LENGTH * (FOURTHIRDS - MASSPOLE * costheta * costheta
		 / TOTAL_MASS));
  
  xacc  = temp - POLEMASS_LENGTH * thetaacc* costheta / TOTAL_MASS;
  
  /*** Update the four state variables, using Euler's method. ***/
  
  *x  += TAU * *x_dot;
  *x_dot += TAU * xacc;
  *theta += TAU * *theta_dot;
  *theta_dot += TAU * thetaacc;
}

/* ------------------------------------------------------------------ */
/* Double pole balacing                                               */
/* ------------------------------------------------------------------ */

//Perform evolution on double pole balacing, for gens generations
//If velocity is false, then velocity information will be withheld from the 
//network population (non-Markov)
Population *pole2_test(int gens,int velocity) {
    Population *pop=0;
    Genome *start_genome;
    char curword[20];
    int id;

    ostringstream *fnamebuf;
    int gen;
    CartPole *thecart;

    //Stat collection variables
    int highscore;
    int record[NEAT::num_runs][1000];
    double recordave[1000];
    int genesrec[NEAT::num_runs][1000];
    double genesave[1000];
    int nodesrec[NEAT::num_runs][1000];
    double nodesave[1000];
    int winnergens[NEAT::num_runs];
    int initcount;
    int champg, champn, winnernum;  //Record number of genes and nodes in champ
    int run;
    int curtotal; //For averaging
    int samples;  //For averaging

    ofstream oFile("statout",ios::out);

    champg=0;
    champn=0;

    //Initialize the stat recording arrays
    for (initcount=0;initcount<gens;initcount++) {
      recordave[initcount]=0;
      genesave[initcount]=0;
      nodesave[initcount]=0;
      for (run=0;run<NEAT::num_runs;++run) {
	record[run][initcount]=0;
	genesrec[run][initcount]=0;
	nodesrec[run][initcount]=0;
      }
    }
    memset (winnergens, 0, NEAT::num_runs * sizeof(int));

    char *non_markov_starter="pole2startgenes2";
    char *markov_starter="pole2startgenes1";
    char *startstring;

    if (velocity==0) startstring=non_markov_starter;
    else if (velocity==1) startstring=markov_starter;
    ifstream iFile(startstring,ios::in);
    //ifstream iFile("pole2startgenes",ios::in);

    cout<<"START DOUBLE POLE BALANCING EVOLUTION"<<endl;
    if (!velocity)
      cout<<"NO VELOCITY INPUT"<<endl;

    cout<<"Reading in the start genome"<<endl;
    //Read in the start Genome
    iFile>>curword;
    iFile>>id;
    cout<<"Reading in Genome id "<<id<<endl;
    start_genome=new Genome(id,iFile);
    iFile.close();

    cout<<"Start Genome: "<<start_genome<<endl;

    for (run=0;run<NEAT::num_runs;run++) {
      
      cout<<"RUN #"<<run<<endl;

      //Spawn the Population from starter gene
      cout<<"Spawning Population off Genome"<<endl;
      pop=new Population(start_genome,NEAT::pop_size);
      
      //Alternative way to start off of randomly connected genomes
      //pop=new Population(pop_size,7,1,10,false,0.3);

      cout<<"Verifying Spawned Pop"<<endl;
      pop->verify();
      
      //Create the Cart
      thecart=new CartPole(true,velocity);
      
      for (gen=1;gen<=gens;gen++) {
	cout<<"Epoch "<<gen<<endl;
	
	fnamebuf=new ostringstream();
	(*fnamebuf)<<"gen_"<<gen<<ends;  //needs end marker
#ifndef NO_SCREEN_OUT
	cout<<"name of fname: "<<fnamebuf->str()<<endl;
#endif

	char temp[50];
        sprintf (temp, "gen_%d", gen);

	highscore=pole2_epoch(pop,gen,temp,velocity, thecart,champg,champn,winnernum,oFile);
	//highscore=pole2_epoch(pop,gen,fnamebuf->str(),velocity, thecart,champg,champn,winnernum,oFile);  
	
	//cout<<"GOT HIGHSCORE FOR GEN "<<gen<<": "<<highscore-1<<endl;
	
	record[run][gen-1]=highscore-1;
	genesrec[run][gen-1]=champg;
	nodesrec[run][gen-1]=champn;
	
	fnamebuf->clear();
	delete fnamebuf;
	
	//Stop right at the winnergen
	if (((pop->winnergen)!=0)&&(gen==(pop->winnergen))) {
	  winnergens[run]=NEAT::pop_size*(gen-1)+winnernum;
	  gen=gens+1;
	}
	
	//In non-MARKOV, stop right at winning (could go beyond if desired)
	if ((!(thecart->MARKOV))&&((pop->winnergen)!=0))
	  gen=gens+1;

#ifndef NO_SCREEN_OUT
      cout<<"gen = "<<gen<<" gens = "<<gens<<endl;
#endif

      if (gen==(gens-1)) oFile<<"FAIL: Last gen on run "<<run<<endl;
      

      }

      if (run<NEAT::num_runs-1) delete pop;
      delete thecart;

    }

    cout<<"Generation highs: "<<endl;
    oFile<<"Generation highs: "<<endl;
    for(gen=0;gen<=gens-1;gen++) {
      curtotal=0;
      for (run=0;run<NEAT::num_runs;++run) {
	if (record[run][gen]>0) {
	  cout<<setw(8)<<record[run][gen]<<" ";
	  oFile<<setw(8)<<record[run][gen]<<" ";
	  curtotal+=record[run][gen];
	}
	else {
	  cout<<"         ";
	  oFile<<"         ";
	  curtotal+=100000;
	}
	recordave[gen]=(double) curtotal/NEAT::num_runs;
	
      }
      cout<<endl;
      oFile<<endl;
    }

    cout<<"Generation genes in champ: "<<endl;
    for(gen=0;gen<=gens-1;gen++) {
      curtotal=0;
      samples=0;
      for (run=0;run<NEAT::num_runs;++run) {
	if (genesrec[run][gen]>0) {
	  cout<<setw(4)<<genesrec[run][gen]<<" ";
	  oFile<<setw(4)<<genesrec[run][gen]<<" ";
	  curtotal+=genesrec[run][gen];
	  samples++;
	}
	else {
	  cout<<setw(4)<<"     ";
	  oFile<<setw(4)<<"     ";
	}
      }
      genesave[gen]=(double) curtotal/samples;

      cout<<endl;
      oFile<<endl;
    }

    cout<<"Generation nodes in champ: "<<endl;
    oFile<<"Generation nodes in champ: "<<endl;
    for(gen=0;gen<=gens-1;gen++) {
      curtotal=0;
      samples=0;
      for (run=0;run<NEAT::num_runs;++run) {
	if (nodesrec[run][gen]>0) {
	  cout<<setw(4)<<nodesrec[run][gen]<<" ";
	  oFile<<setw(4)<<nodesrec[run][gen]<<" ";
	  curtotal+=nodesrec[run][gen];
	  samples++;
	}
	else {
	  cout<<setw(4)<<"     ";
	  oFile<<setw(4)<<"     ";
	}
      }
      nodesave[gen]=(double) curtotal/samples;

      cout<<endl;
      oFile<<endl;
    }

    cout<<"Generational record fitness averages: "<<endl;
    oFile<<"Generational record fitness averages: "<<endl;
    for(gen=0;gen<gens-1;gen++) {
      cout<<recordave[gen]<<endl;
      oFile<<recordave[gen]<<endl;
    }

    cout<<"Generational number of genes in champ averages: "<<endl;
    oFile<<"Generational number of genes in champ averages: "<<endl;
    for(gen=0;gen<gens-1;gen++) {
      cout<<genesave[gen]<<endl;
      oFile<<genesave[gen]<<endl;
    }

    cout<<"Generational number of nodes in champ averages: "<<endl;
    oFile<<"Generational number of nodes in champ averages: "<<endl;
    for(gen=0;gen<gens-1;gen++) {
      cout<<nodesave[gen]<<endl;
      oFile<<nodesave[gen]<<endl;
    }

    cout<<"Winner evals: "<<endl;
    oFile<<"Winner evals: "<<endl;
    curtotal=0;
    samples=0;
    for (run=0;run<NEAT::num_runs;++run) {
      cout<<winnergens[run]<<endl;
      oFile<<winnergens[run]<<endl;
      if (winnergens[run]>0)
      {
        curtotal+=winnergens[run];
        samples++;
      }
    }
    cout<<"Failures: "<<(NEAT::num_runs-samples)<<" out of "<<NEAT::num_runs<<" runs"<<endl;
    oFile<<"Failures: "<<(NEAT::num_runs-samples)<<" out of "<<NEAT::num_runs<<" runs"<<endl;

    cout<<"Average # evals: "<<(samples>0 ? (double) curtotal/samples : 0)<<endl;
    oFile<<"Average # evals: "<<(samples>0 ? (double) curtotal/samples : 0)<<endl;

    oFile.close();

    return pop;

}

//This is used for list sorting of Species by fitness of best organism
//highest fitness first
//Used to choose which organism to test
//bool order_new_species(Species *x, Species *y) {
//
//  return (x->compute_max_fitness() > 
//	  y->compute_max_fitness());
//}

int pole2_epoch(Population *pop,int generation,char *filename,bool velocity,
		CartPole *thecart,int &champgenes,int &champnodes,
		int &winnernum, ofstream &oFile) {
  //char cfilename[100];
  //strncpy( cfilename, filename.c_str(), 100 );

  //ofstream cfilename(filename.c_str());

  vector<Organism*>::iterator curorg;
  vector<Species*>::iterator curspecies;

  vector<Species*> sorted_species;  //Species sorted by max fit org in Species

  int pause;
  bool win=false;

  double champ_fitness;
  Organism *champ;

  //double statevals[5]={-0.9,-0.5,0.0,0.5,0.9};
  double statevals[5]={0.05, 0.25, 0.5, 0.75, 0.95};

  int s0c,s1c,s2c,s3c;

  int score;

  thecart->nmarkov_long=false;
  thecart->generalization_test=false;

  //Evaluate each organism on a test
  for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {

    //shouldn't happen
    if (((*curorg)->gnome)==0) {
      cout<<"ERROR EMPTY GEMOME!"<<endl;
      cin>>pause;
    }

    if (pole2_evaluate((*curorg),velocity,thecart)) win=true;

  }

  //Average and max their fitnesses for dumping to file and snapshot
  for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {

    //This experiment control routine issues commands to collect ave
    //and max fitness, as opposed to having the snapshot do it, 
    //because this allows flexibility in terms of what time
    //to observe fitnesses at

    (*curspecies)->compute_average_fitness();
    (*curspecies)->compute_max_fitness();
  }

  //Take a snapshot of the population, so that it can be
  //visualized later on
  //if ((generation%1)==0)
  //  pop->snapshot();

  //Find the champion in the markov case simply for stat collection purposes
  if (thecart->MARKOV) {
    champ_fitness=0.0;
    for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
      if (((*curorg)->fitness)>champ_fitness) {
	champ=(*curorg);
	champ_fitness=champ->fitness;
	champgenes=champ->gnome->genes.size();
	champnodes=champ->gnome->nodes.size();
	winnernum=champ->gnome->genome_id;
      }
    }
  }

  //Check for winner in Non-Markov case
  if (!(thecart->MARKOV)) {
    
    cout<<"Non-markov case"<<endl;

    //Sort the species
    for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {
      sorted_species.push_back(*curspecies);
    }

    //sorted_species.sort(order_new_species);
    std::sort(sorted_species.begin(), sorted_species.end(), NEAT::order_new_species);

    //std::sort(sorted_species.begin(), sorted_species.end(), order_species);


    cout<<"Number of species sorted: "<<sorted_species.size()<<endl;

    //First update what is checked and unchecked
    for(curspecies=sorted_species.begin();curspecies!=sorted_species.end();++curspecies) {
      if (((*curspecies)->compute_max_fitness())>((*curspecies)->max_fitness_ever))
	(*curspecies)->checked=false;

    }

    //Now find a species that is unchecked
    curspecies=sorted_species.begin();
    cout<<"Is the first species checked? "<<(*curspecies)->checked<<endl;
    while((curspecies!=(sorted_species.end()))&&
	  ((*curspecies)->checked))
    {
      cout<<"Species #"<<(*curspecies)->id<<" is checked"<<endl;
      ++curspecies;
    }

    if (curspecies==(sorted_species.end())) curspecies=sorted_species.begin();

    //Remember it was checked
    (*curspecies)->checked=true;
    cout<<"Is the species now checked? "<<(*curspecies)->checked<<endl;

    //Extract the champ
    cout<<"Champ chosen from Species "<<(*curspecies)->id<<endl;
    champ=(*curspecies)->get_champ();
    champ_fitness=champ->fitness;
    cout<<"Champ is organism #"<<champ->gnome->genome_id<<endl;
    cout<<"Champ fitness: "<<champ_fitness<<endl;
    winnernum=champ->gnome->genome_id;

    cout<<champ->gnome<<endl;

    //Now check to make sure the champ can do 100,000
    thecart->nmarkov_long=true;
    thecart->generalization_test=false;

    //The champ needs tp be flushed here because it may have
    //leftover activation from its last test run that could affect
    //its recurrent memory
    (champ->net)->flush();


    //champ->gnome->print_to_filename("tested");
    
    if (pole2_evaluate(champ,velocity,thecart)) {
      cout<<"The champ passed the 100,000 test!"<<endl;

      thecart->nmarkov_long=false;

      //Given that the champ passed, now run it on generalization tests
      score=0;
      for (s0c=0;s0c<=4;++s0c)
	for (s1c=0;s1c<=4;++s1c)
	  for (s2c=0;s2c<=4;++s2c)
	    for (s3c=0;s3c<=4;++s3c) {
	      thecart->state[0] = statevals[s0c] * 4.32 - 2.16;
	      thecart->state[1] = statevals[s1c] * 2.70 - 1.35;
	      thecart->state[2] = statevals[s2c] * 0.12566304 - 0.06283152;
	      /* 0.06283152 =  3.6 degrees */
	      thecart->state[3] = statevals[s3c] * 0.30019504 - 0.15009752;
	      /* 00.15009752 =  8.6 degrees */
	      thecart->state[4]=0.0;
	      thecart->state[5]=0.0;
	      
	      cout<<"On combo "<<thecart->state[0]<<" "<<thecart->state[1]<<" "<<thecart->state[2]<<" "<<thecart->state[3]<<endl;
	      thecart->generalization_test=true;
	      
	      (champ->net)->flush();  //Reset the champ for each eval

	      if (pole2_evaluate(champ,velocity,thecart)) {
		cout<<"----------------------------The champ passed its "<<score<<"th test"<<endl;
		score++;
	      }
	      
	    }

      if (score>=200) {
	cout<<"The champ wins!!! (generalization = "<<score<<" )"<<endl;
	oFile<<"(generalization = "<<score<<" )"<<endl;
	oFile<<"generation= "<<generation<<endl;
        (champ->gnome)->print_to_file(oFile);
	champ_fitness=champ->fitness;
	champgenes=champ->gnome->genes.size();
	champnodes=champ->gnome->nodes.size();
	winnernum=champ->gnome->genome_id;
	win=true;
      }
      else {
	cout<<"The champ couldn't generalize"<<endl;
	champ->fitness=champ_fitness; //Restore the champ's fitness
      }
    }
    else {
      cout<<"The champ failed the 100,000 test :("<<endl;
      cout<<"made score "<<champ->fitness<<endl;
      champ->fitness=champ_fitness; //Restore the champ's fitness
    }
  }
  
  //Only print to file every print_every generations
  if  (win||
       ((generation%(NEAT::print_every))==0)) {
    cout<<"printing file: "<<filename<<endl;
    pop->print_to_file_by_species(filename);
  }

  if ((win)&&((pop->winnergen)==0)) pop->winnergen=generation;

  //Prints a champion out on each generation
  //IMPORTANT: This causes generational file output!
  print_Genome_tofile(champ->gnome,"champ");

  //Create the next generation
  pop->epoch(generation);

  return (int) champ_fitness;
}

bool pole2_evaluate(Organism *org,bool velocity, CartPole *thecart) {
  Network *net;

  int thresh;  /* How many visits will be allowed before giving up 
		  (for loop detection)  NOW OBSOLETE */

  int pause;

  net=org->net;

  thresh=100;  //this is obsolete

  //DEBUG :  Check flushedness of org
  //org->net->flush_check();

  //Try to balance a pole now
  org->fitness = thecart->evalNet(net,thresh);

#ifndef NO_SCREEN_OUT
  if (org->pop_champ_child)
    cout<<" <<DUPLICATE OF CHAMPION>> ";

  //Output to screen
  cout<<"Org "<<(org->gnome)->genome_id<<" fitness: "<<org->fitness;
  cout<<" ("<<(org->gnome)->genes.size();
  cout<<" / "<<(org->gnome)->nodes.size()<<")";
  cout<<"   ";
  if (org->mut_struct_baby) cout<<" [struct]";
  if (org->mate_baby) cout<<" [mate]";
  cout<<endl;
#endif

  if ((!(thecart->generalization_test))&&(!(thecart->nmarkov_long)))
  if (org->pop_champ_child) {
    cout<<org->gnome<<endl;
    //DEBUG CHECK
    if (org->high_fit>org->fitness) {
      cout<<"ALERT: ORGANISM DAMAGED"<<endl;
      print_Genome_tofile(org->gnome,"failure_champ_genome");
      cin>>pause;
    }
  }

  //Decide if its a winner, in Markov Case
  if (thecart->MARKOV) {
    if (org->fitness>=(thecart->maxFitness-1)) { 
      org->winner=true;
      return true;
    }
    else {
      org->winner=false;
      return false;
    }
  }
  //if doing the long test non-markov 
  else if (thecart->nmarkov_long) {
    if (org->fitness>=99999) { 
      //if (org->fitness>=9000) { 
      org->winner=true;
      return true;
    }
    else {
      org->winner=false;
      return false;
    }
  }
  else if (thecart->generalization_test) {
    if (org->fitness>=999) {
      org->winner=true;
      return true;
    }
    else {
      org->winner=false;
      return false;
    }
  }
  else {
    org->winner=false;
    return false;  //Winners not decided here in non-Markov
  }
}

CartPole::CartPole(bool randomize,bool velocity)
{
  maxFitness = 100000;

  MARKOV=velocity;

  MIN_INC = 0.001;
  POLE_INC = 0.05;
  MASS_INC = 0.01;

  LENGTH_2 = 0.05;
  MASSPOLE_2 = 0.01;

  // CartPole::reset() which is called here
}

//Faustino Gomez wrote this physics code using the differential equations from 
//Alexis Weiland's paper and added the Runge-Kutta himself.
double CartPole::evalNet(Network *net,int thresh)
{
  int steps=0;
  double input[NUM_INPUTS];
  double output;

  int nmarkovmax;  

  double nmarkov_fitness;

  double jiggletotal; //total jiggle in last 100
  int count;  //step counter

  //init(randomize);		// restart at some point
  
  if (nmarkov_long) nmarkovmax=100000;
  else if (generalization_test) nmarkovmax=1000;
  else nmarkovmax=1000;


  init(0);

  if (MARKOV) {
    while (steps++ < maxFitness) {
      
         
      input[0] = state[0] / 4.8;
      input[1] = state[1] /2;
      input[2] = state[2]  / 0.52;
      input[3] = state[3] /2;
      input[4] = state[4] / 0.52;
      input[5] = state[5] /2;
      input[6] = .5;
      
      net->load_sensors(input);
      
      //Activate the net
      //If it loops, exit returning only fitness of 1 step
      if (!(net->activate())) return 1.0;
      
      output=(*(net->outputs.begin()))->activation;
      
      performAction(output,steps);
      
      if (outsideBounds())	// if failure
	break;			// stop it now
    }
    return (double) steps;
  }
  else {  //NON MARKOV CASE

    while (steps++ < nmarkovmax) {
      

     //Do special parameter summing on last hundred
     //if ((steps==900)&&(!nmarkov_long)) last_hundred=true;

     /*
     input[0] = state[0] / 4.8;
     input[1] = 0.0;
     input[2] = state[2]  / 0.52;
     input[3] = 0.0;
     input[4] = state[4] / 0.52;
     input[5] = 0.0;
     input[6] = .5;
     */

      //cout<<"nmarkov_long: "<<nmarkov_long<<endl;

      //if (nmarkov_long)
      //cout<<"step: "<<steps<<endl;

     input[0] = state[0] / 4.8;
     input[1] = state[2]  / 0.52;
     input[2] = state[4] / 0.52;
     input[3] = .5;
      
      net->load_sensors(input);

      //cout<<"inputs: "<<input[0]<<" "<<input[1]<<" "<<input[2]<<" "<<input[3]<<endl;

      //Activate the net
      //If it loops, exit returning only fitness of 1 step
      if (!(net->activate())) return 0.0001;
      
      output=(*(net->outputs.begin()))->activation;

      //cout<<"output: "<<output<<endl;

      performAction(output,steps);

      if (outsideBounds())	// if failure
	break;			// stop it now

      if (nmarkov_long&&(outsideBounds()))	// if failure
	break;			// stop it now
    }

   //If we are generalizing we just need to balance it a while
   if (generalization_test)
     return (double) balanced_sum;
 
   //Sum last 100
   if ((steps>100)&&(!nmarkov_long)) {

     jiggletotal=0;
     cout<<"step "<<steps-99-2<<" to step "<<steps-2<<endl;
     //Adjust for array bounds and count
     for (count=steps-99-2;count<=steps-2;count++)
       jiggletotal+=jigglestep[count];
   }

   if (!nmarkov_long) {
     if (balanced_sum>100) 
       nmarkov_fitness=((0.1*(((double) balanced_sum)/1000.0))+
			(0.9*(0.75/(jiggletotal))));
     else nmarkov_fitness=(0.1*(((double) balanced_sum)/1000.0));

#ifndef NO_SCREEN_OUTR
     cout<<"Balanced:  "<<balanced_sum<<" jiggle: "<<jiggletotal<<" ***"<<endl;
#endif

     return nmarkov_fitness;
   }
   else return (double) steps;

  }

}

void CartPole::init(bool randomize)
{
  static int first_time = 1;

  if (!MARKOV) {
    //Clear all fitness records
    cartpos_sum=0.0;
    cartv_sum=0.0;
    polepos_sum=0.0;
    polev_sum=0.0;
  }

  balanced_sum=0; //Always count # balanced

  last_hundred=false;

  /*if (randomize) {
    state[0] = (lrand48()%4800)/1000.0 - 2.4;
    state[1] = (lrand48()%2000)/1000.0 - 1;
    state[2] = (lrand48()%400)/1000.0 - 0.2;
    state[3] = (lrand48()%400)/1000.0 - 0.2;
    state[4] = (lrand48()%3000)/1000.0 - 1.5;
    state[5] = (lrand48()%3000)/1000.0 - 1.5;
  }
  else {*/


  if (!generalization_test) {
    state[0] = state[1] = state[3] = state[4] = state[5] = 0;
    state[2] = 0.07; // one_degree;
  }
  else {
    state[4] = state[5] = 0;
  }

    //}
  if(first_time){
    cout<<"Initial Long pole angle = %f\n"<<state[2]<<endl;;
    cout<<"Initial Short pole length = %f\n"<<LENGTH_2<<endl;
    first_time = 0;
  }
}

void CartPole::performAction(double output, int stepnum)
{ 
  
  int i;
  double  dydx[6];

  const bool RK4=true; //Set to Runge-Kutta 4th order integration method
  const double EULER_TAU= TAU/4;
 
  /*random start state for long pole*/
  /*state[2]= drand48();   */
     
  /*--- Apply action to the simulated cart-pole ---*/

  if(RK4){
    for(i=0;i<2;++i){
      dydx[0] = state[1];
      dydx[2] = state[3];
      dydx[4] = state[5];
      step(output,state,dydx);
      rk4(output,state,dydx,state);
    }
  }
  else{
    for(i=0;i<8;++i){
      step(output,state,dydx);
      state[0] += EULER_TAU * dydx[0];
      state[1] += EULER_TAU * dydx[1];
      state[2] += EULER_TAU * dydx[2];
      state[3] += EULER_TAU * dydx[3];
      state[4] += EULER_TAU * dydx[4];
      state[5] += EULER_TAU * dydx[5];
    }
  }

  //Record this state
  cartpos_sum+=fabs(state[0]);
  cartv_sum+=fabs(state[1]);
  polepos_sum+=fabs(state[2]);
  polev_sum+=fabs(state[3]);
  if (stepnum<=1000)
    jigglestep[stepnum-1]=fabs(state[0])+fabs(state[1])+fabs(state[2])+fabs(state[3]);

  if (false) {
    //cout<<"[ x: "<<state[0]<<" xv: "<<state[1]<<" t1: "<<state[2]<<" t1v: "<<state[3]<<" t2:"<<state[4]<<" t2v: "<<state[5]<<" ] "<<
    //cartpos_sum+cartv_sum+polepos_sum+polepos_sum+polev_sum<<endl;
    if (!(outsideBounds())) {
      if (balanced_sum<1000) {
	cout<<".";
	++balanced_sum;
      }
    }
    else {
      if (balanced_sum==1000)
	balanced_sum=1000;
      else balanced_sum=0;
    }
  }
  else if (!(outsideBounds()))
    ++balanced_sum;

}

void CartPole::step(double action, double *st, double *derivs)
{
    double force,costheta_1,costheta_2,sintheta_1,sintheta_2,
          gsintheta_1,gsintheta_2,temp_1,temp_2,ml_1,ml_2,fi_1,fi_2,mi_1,mi_2;

    force =  (action - 0.5) * FORCE_MAG * 2;
    costheta_1 = cos(st[2]);
    sintheta_1 = sin(st[2]);
    gsintheta_1 = GRAVITY * sintheta_1;
    costheta_2 = cos(st[4]);
    sintheta_2 = sin(st[4]);
    gsintheta_2 = GRAVITY * sintheta_2;
    
    ml_1 = LENGTH_1 * MASSPOLE_1;
    ml_2 = LENGTH_2 * MASSPOLE_2;
    temp_1 = MUP * st[3] / ml_1;
    temp_2 = MUP * st[5] / ml_2;
    fi_1 = (ml_1 * st[3] * st[3] * sintheta_1) +
           (0.75 * MASSPOLE_1 * costheta_1 * (temp_1 + gsintheta_1));
    fi_2 = (ml_2 * st[5] * st[5] * sintheta_2) +
           (0.75 * MASSPOLE_2 * costheta_2 * (temp_2 + gsintheta_2));
    mi_1 = MASSPOLE_1 * (1 - (0.75 * costheta_1 * costheta_1));
    mi_2 = MASSPOLE_2 * (1 - (0.75 * costheta_2 * costheta_2));
    
    derivs[1] = (force + fi_1 + fi_2)
                 / (mi_1 + mi_2 + MASSCART);
    
    derivs[3] = -0.75 * (derivs[1] * costheta_1 + gsintheta_1 + temp_1)
                 / LENGTH_1;
    derivs[5] = -0.75 * (derivs[1] * costheta_2 + gsintheta_2 + temp_2)
                  / LENGTH_2;

}

void CartPole::rk4(double f, double y[], double dydx[], double yout[])
{

	int i;

	double hh,h6,dym[6],dyt[6],yt[6];


	hh=TAU*0.5;
	h6=TAU/6.0;
	for (i=0;i<=5;i++) yt[i]=y[i]+hh*dydx[i];
	step(f,yt,dyt);
	dyt[0] = yt[1];
	dyt[2] = yt[3];
	dyt[4] = yt[5];
	for (i=0;i<=5;i++) yt[i]=y[i]+hh*dyt[i];
	step(f,yt,dym);
	dym[0] = yt[1];
	dym[2] = yt[3];
	dym[4] = yt[5];
	for (i=0;i<=5;i++) {
		yt[i]=y[i]+TAU*dym[i];
		dym[i] += dyt[i];
	}
	step(f,yt,dyt);
	dyt[0] = yt[1];
	dyt[2] = yt[3];
	dyt[4] = yt[5];
	for (i=0;i<=5;i++)
		yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
}

bool CartPole::outsideBounds()
{
  const double failureAngle = thirty_six_degrees; 

  return 
    state[0] < -2.4              || 
    state[0] > 2.4               || 
    state[2] < -failureAngle     ||
    state[2] > failureAngle      ||
    state[4] < -failureAngle     ||
    state[4] > failureAngle;  
}

void CartPole::nextTask()
{

   LENGTH_2 += POLE_INC;   /* LENGTH_2 * INCREASE;   */
   MASSPOLE_2 += MASS_INC; /* MASSPOLE_2 * INCREASE; */
   //  ++new_task;
   cout<<"#Pole Length %2.4f\n"<<LENGTH_2<<endl;
}

void CartPole::simplifyTask()
{
  if(POLE_INC > MIN_INC) {
    POLE_INC = POLE_INC/2;
    MASS_INC = MASS_INC/2;
    LENGTH_2 -= POLE_INC;
    MASSPOLE_2 -= MASS_INC;
    cout<<"#SIMPLIFY\n"<<endl;
    cout<<"#Pole Length %2.4f\n"<<LENGTH_2;
  }
  else
    {
      cout<<"#NO TASK CHANGE\n"<<endl;
    }
}
