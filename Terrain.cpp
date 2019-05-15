#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

float cols[6][3] = { {1,0,0}, {0,1,1}, {1,1,0}, {0,1,0}, {0,0,1}, {1,0,1} };

float pos[] = {0,0,0};
float rot[] = {0, 0, 0};
float headRot[] = {0, 0, 0};
float camPos[] = {25, -25, 25};
float angle = 0.0f;
double side;
double overviewSide;

//int maxHeight = 5;
int maximum= 5;
float currentMax,currentMin;
int terrainCircleSizeMax = 15;
int terrainSize;
int numOfIterations, numOfCircles;
float terrain[300][300];
float QuadNorms [300][300][3];
float TriNorms[300][300][6];

int wireframe = 1;
int lighting = 1;
int flatS = 1;
int triMesh = 1;
int alg = 0;
int gouraud = 0;
float v,a,b,d,c;


float nvx,nvy,nvz;

float light_pos[] = {20, 20, 20,1};
float light_pos1[] = {-20, -20, -20, 1};


float amb0[4]  = {0.1, 1, 0.4, 1}; 
float diff0[4] = {1, 0.3, 0, 1};
float spec0[4] = {0, 0, 0.6, 1}; 

/*
float amb0[4]  = {0.2, 0.2, 0.2, 1}; 
float diff0[4] = {0.8, 0.8, 0.8, 1};
float spec0[4] = {0.5, 0.5, 0.5, 1}; 
*/

float m_amb[] = {0.135,0.2225,0.1575, 1.0};
float m_diff[] = {0.54,0.89,0.63, 1.0};
float m_spec[] = {0.316228,0.316228,0.316228, 1.0};
float shiny = 0.1;

float m_amb1[] = {0.05,0.05,0.05,1};
float m_diff1[] = {0.5,0.5,0.5,1};
float m_spec1[] = {0.7,0.7,0.7,1.0};
float shiny1 = 0.078125;

float m_ambS[] = {0.0,0.0,0.0,1};
float m_diffS[] = {0.01,0.01,0.01,1};
float m_specS[] = {0.5,0.5,0.5,1.0};
float shinyS = 0.25;

int window_1, window_2;


void drawAxis()
{
    glPushMatrix();
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(-50,0,0);
    glVertex3f(50,0,0);
    glColor3f(0,1,0);
    glVertex3f(0,-50,0);
    glVertex3f(0,50,0);
    glColor3f(0,0,1);
    glVertex3f(0,0,-50);
    glVertex3f(0,0,50);
    glEnd();
    
    glPopMatrix();
}


void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			camPos[0]-=1;
			break;

		case GLUT_KEY_RIGHT:
			camPos[0]+=1;
			break;

		case GLUT_KEY_UP:
			camPos[2] -=1;
			break;

		case GLUT_KEY_DOWN:
			camPos[2] +=1;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] +=1;
			break;

		case GLUT_KEY_END:
			camPos[1] -=1;
			break;

	}
	glutPostRedisplay();
}

void init(void)
{
	//glClearColor(0.496,0.996,0.828,1); aqua
	glClearColor(0.9375,0.5,0.5,1);
	glColor3f(1, 1, 1);


	glFrontFace(GL_CCW); 
	glCullFace(GL_BACK); 
	glEnable(GL_CULL_FACE); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-2, 2, -2, 2, -2, 2);
	gluPerspective(45, 1, 1, 100);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, amb0); 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff0); 
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec0);

}

void computeNorm(float v1[3], float v2[3]){
	float vx = v1[1]*v2[2]-v1[2]*v2[1];
	float vy = v1[2]*v2[0]-v1[0]*v2[2];
	float vz = v1[0]*v2[1]-v1[1]*v2[0];
	float l = sqrt(vx*vx+vy*vy+vz*vz);

	nvx = vx/l;
	nvy = vy/l;
	nvz = vz/l;
}

void triangleMesh(){
	for (int i=0;i<terrainSize-1;i++){
		for (int j=0;j<terrainSize-1;j++){
			float mapB = 20 - i*side;
			float mapA = -20+ j*side;
			float verts[4][3] = {{mapA,mapB,terrain[i][j]},
								 {mapA+side,mapB,terrain[i][j+1]},
								 {mapA,mapB-side,terrain[i+1][j]},
								 {mapA+side,mapB-side,terrain[i+1][j+1]}};
			currentMax = currentMax - currentMin;
	        //printf("%f\n",currentMax);
    		glBegin(GL_TRIANGLES);
    			glColor3f(verts[0][2]/currentMax,verts[0][2]/currentMax,verts[0][2]/currentMax);
        		glVertex3fv(verts[0]);
        		glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);

        		float v1[3] = {-side,0,terrain[i][j]-terrain[i][j+1]}; //0-1
        		float v2[3] = {-side,-side,terrain[i+1][j]-terrain[i][j+1]}; //2-1
        		computeNorm(v1,v2);
        		float nv[3] = {nvx,nvy,nvz};
        		TriNorms[i][j][0] = nvx;
        		TriNorms[i][j][1] = nvy;
        		TriNorms[i][j][2] = nvz;

        		glNormal3fv(nv);
        		

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);
        		
    		glEnd();
    		
    		//glBegin(GL_LINE_LOOP);
    		glBegin(GL_TRIANGLES);
    			glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);
        		glColor3f(verts[3][2]/currentMax,verts[3][2]/currentMax,verts[3][2]/currentMax);
        		glVertex3fv(verts[3]);

        		float v3[3] = {-side,-side,terrain[i+1][j]-terrain[i][j+1]}; //2-1
        		float v4[3] = {side,-side,terrain[i+1][j+1]-terrain[i][j+1]}; //3-1
        		computeNorm(v3,v4);
        		float nv1[3] = {nvx,nvy,nvz};
        		TriNorms[i][j][3] = nvx;
        		TriNorms[i][j][4] = nvy;
        		TriNorms[i][j][5] = nvz;
        		glNormal3fv(nv1);
				

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);
    		glEnd();
    		

		}
	}
}

void triangleMesh_GShading(){
	for (int i=0;i<terrainSize-1;i++){
		for (int j=0;j<terrainSize-1;j++){
			float mapB = 20 - i*side;
			float mapA = -20+ j*side;
			float verts[4][3] = {{mapA,mapB,terrain[i][j]},
								 {mapA+side,mapB,terrain[i][j+1]},
								 {mapA,mapB-side,terrain[i+1][j]},
								 {mapA+side,mapB-side,terrain[i+1][j+1]}};
			currentMax = currentMax - currentMin;
	        //printf("%f\n",currentMax);
    		glBegin(GL_TRIANGLES);
    			glColor3f(verts[0][2]/currentMax,verts[0][2]/currentMax,verts[0][2]/currentMax);
        		glVertex3fv(verts[0]);
        		glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);

        		nvx = (TriNorms[i-1][j-1][3] + TriNorms[i-1][j][0] + TriNorms[i-1][j][3] + TriNorms[i][j-1][3] + TriNorms[i][j-1][3] + TriNorms[i][j][0])/6;
        		nvy = (TriNorms[i-1][j-1][4] + TriNorms[i-1][j][1] + TriNorms[i-1][j][4] + TriNorms[i][j-1][4] + TriNorms[i][j-1][4] + TriNorms[i][j][1])/6;
        		nvz = (TriNorms[i-1][j-1][5] + TriNorms[i-1][j][2] + TriNorms[i-1][j][5] + TriNorms[i][j-1][5] + TriNorms[i][j-1][5] + TriNorms[i][j][2])/6;




        		float nv[3] = {nvx,nvy,nvz};

        		glNormal3fv(nv);
        		

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);
        		
    		glEnd();
    		
    		//glBegin(GL_LINE_LOOP);
    		glBegin(GL_TRIANGLES);
    			glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);
        		glColor3f(verts[3][2]/currentMax,verts[3][2]/currentMax,verts[3][2]/currentMax);
        		glVertex3fv(verts[3]);

        		float v3[3] = {-side,-side,terrain[i+1][j]-terrain[i][j+1]}; //2-1
        		float v4[3] = {side,-side,terrain[i+1][j+1]-terrain[i][j+1]}; //3-1

        		nvx = (TriNorms[i][j-1][3] + TriNorms[i][j][0] + TriNorms[i][j][3] + TriNorms[i+1][j-1][3] + TriNorms[i+1][j-1][3] + TriNorms[i+1][j][0])/6;
        		nvy = (TriNorms[i][j-1][4] + TriNorms[i][j][1] + TriNorms[i][j][4] + TriNorms[i+1][j-1][4] + TriNorms[i+1][j-1][4] + TriNorms[i+1][j][1])/6;
        		nvz = (TriNorms[i][j-1][5] + TriNorms[i][j][2] + TriNorms[i][j][5] + TriNorms[i+1][j-1][5] + TriNorms[i+1][j-1][5] + TriNorms[i+1][j][2])/6;
        		




        		float nv1[3] = {nvx,nvy,nvz};
        		glNormal3fv(nv1);
				

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);
    		glEnd();
    		

		}
	}
}






void quadMesh_GShading(){
	for (int i=1;i<terrainSize-1;i++){
		for (int j=1;j<terrainSize-1;j++){


			float mapB = 20 - i*side;
			float mapA = -20+ j*side;
			float verts[4][3] = {{mapA,mapB,terrain[i][j]},
								 {mapA+side,mapB,terrain[i][j+1]},
								 {mapA,mapB-side,terrain[i+1][j]},
								 {mapA+side,mapB-side,terrain[i+1][j+1]}};
		currentMax = currentMax - currentMin;
		//printf("%f\n",currentMax);

		glBegin(GL_QUADS);
				glColor3f(verts[0][2]/currentMax,verts[0][2]/currentMax,verts[0][2]/currentMax);
        		glVertex3fv(verts[0]);
        		
        		glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);

        		glColor3f(verts[3][2]/currentMax,verts[3][2]/currentMax,verts[3][2]/currentMax);
        		glVertex3fv(verts[3]);

        		nvx = (QuadNorms[i-1][j-1][0] + QuadNorms[i-1][j][0] + QuadNorms[i][j-1][0] + QuadNorms[i][j][0])/4;
        		nvy = (QuadNorms[i-1][j-1][1] + QuadNorms[i-1][j][1] + QuadNorms[i][j-1][1] + QuadNorms[i][j][1])/4;
        		nvz = (QuadNorms[i-1][j-1][2] + QuadNorms[i-1][j][2] + QuadNorms[i][j-1][2] + QuadNorms[i][j][2])/4;
        		
        		float nv1[3] = {nvx,nvy,nvz};
        		glNormal3fv(nv1);
        		//printf("%f,%f,%f\n",nvx,nvy,nvz);
        		

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);

    		glEnd();
		}
	}
}


void quadMesh(){
	for (int i=0;i<terrainSize-1;i++){
		for (int j=0;j<terrainSize-1;j++){
			float mapB = 20 - i*side;
			float mapA = -20+ j*side;
			float verts[4][3] = {{mapA,mapB,terrain[i][j]},
								 {mapA+side,mapB,terrain[i][j+1]},
								 {mapA,mapB-side,terrain[i+1][j]},
								 {mapA+side,mapB-side,terrain[i+1][j+1]}};
		currentMax = currentMax - currentMin;
		//printf("%f\n",currentMax);

		glBegin(GL_QUADS);
				glColor3f(verts[0][2]/currentMax,verts[0][2]/currentMax,verts[0][2]/currentMax);
        		glVertex3fv(verts[0]);
        		
        		glColor3f(verts[2][2]/currentMax,verts[2][2]/currentMax,verts[2][2]/currentMax);
        		glVertex3fv(verts[2]);

        		glColor3f(verts[3][2]/currentMax,verts[3][2]/currentMax,verts[3][2]/currentMax);
        		glVertex3fv(verts[3]);

        		

        		float v5[3] = {-side,0,terrain[i][j]-terrain[i][j+1]}; //0-1
        		float v6[3] = {side,-side,terrain[i+1][j+1]-terrain[i][j+1]}; //3-1
        		computeNorm(v5,v6);
        		float nv2[3] = {nvx,nvy,nvz};
        		QuadNorms[i][j][0] = nvx;
        		QuadNorms[i][j][1] = nvy;
        		QuadNorms[i][j][2] = nvz;
        		glNormal3fv(nv2);

        		glColor3f(verts[1][2]/currentMax,verts[1][2]/currentMax,verts[1][2]/currentMax);
        		glVertex3fv(verts[1]);

    		glEnd();
		}
	}
}


void drawSphere(){


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambS); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffS); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_specS); 
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shinyS);

	glPushMatrix();
	glTranslatef(0, 0.0, 0.0);
	glutSolidSphere(100.0, 50, 50);
 
	glPopMatrix();
 

}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	float origin[3] = {0,0,0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);

	//printf("light_pos X,Y,Z: %f,%f,%f\n",light_pos[0],light_pos[1],light_pos[2]);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec); 
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
	
	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,0,1);

    glPushMatrix();
    //drawAxis();
    glPopMatrix();

    if (triMesh == 1){
    	if(wireframe == 2){
    		glPushMatrix();
    			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    			if (gouraud == 0){
						triangleMesh();
					}
					else{
						triangleMesh_GShading();
					}
    			
    				glPushMatrix();
    				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    				//glTranslatef(0,0,0.1);
    				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb1); 
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff1); 
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec1); 
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny1);


    				if (gouraud == 0){
						triangleMesh();
					}
					else{
						triangleMesh_GShading();
					}
    			glPopMatrix();
    		glPopMatrix();
    	}
    	else{
    		if (gouraud == 0){
						triangleMesh();
					}
					else{
						triangleMesh_GShading();
					}
    	}
    }
    else{
    	if (wireframe == 2){
    		glPushMatrix();
    			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    			if (gouraud == 0){
						quadMesh();
					}
					else{
						quadMesh_GShading();
					}
    			glPushMatrix();
    				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb1); 
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff1); 
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec1); 
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny1);
					if (gouraud == 0){
						quadMesh();
					}
					else{
						quadMesh_GShading();
					}
    			glPopMatrix();
    		glPopMatrix();
    	}
    	else{
    		if (gouraud == 0){
    			quadMesh();
				}
			else{
				quadMesh_GShading();
    		}
    	}
    }
	glutSwapBuffers();
}

void overview(){

	for(int i=0;i<terrainSize;i++){
		for(int j=0;j<terrainSize;j++){
			float currentX = -1 + j*overviewSide;
			float currentY = 1 - i*overviewSide;
			float verts[4][2] = {{currentX,currentY},
								 {currentX+overviewSide,currentY},
								 {currentX,currentY-overviewSide},
								 {currentX+overviewSide,currentY-overviewSide}};



			glColor3f(terrain[i][j]/currentMax,terrain[i][j]/currentMax,terrain[i][j]/currentMax);
			//glColor3f(0,0,0);
			//printf("%f\n",(terrain[i][j]/currentMax));
		
			glBegin(GL_QUADS);



			
			glVertex2fv(verts[0]);
			glVertex2fv(verts[2]);
			glVertex2fv(verts[3]);
			glVertex2fv(verts[1]);
			glEnd();
		}
	}
	
	glutPostRedisplay();

}

void display_2(void){
    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	glMatrixMode (GL_PROJECTION); 
	glLoadIdentity();  // Clear the matrix
	//glOrtho(-8.0, 8.0, -8.0, 8.0, 0.0, 30.0);  // Setup an Ortho view
	gluLookAt(0, 0,3.42f, 0, 0, 0, 0, 1, 0);
	
	glLoadIdentity();
	overview();
	glFlush();
   
}

void init_2(void){
	glClearColor(0,0,0,1); //aqua
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);

}

void printHeight(){
    for (int i=0; i<terrainSize;i++){
        for (int j=0;j<terrainSize;j++){
            printf("%f,",terrain[i][j]);
        }
        printf("\n");
    }
}

void circles(int x,int y){

	float disp;
    float centerX = -10 + x*side;
    float centerY = 10 - y*side;
    //printf("current center is %i,%i and coord is %f,%f\n",x,y,centerX,centerY);
    disp = rand() % maximum;
    //maximum = disp;
    int terrainCircleSize = rand() % terrainCircleSizeMax + 5;
    for (int i=0; i<terrainSize;i++){
        for (int j=0;j<terrainSize;j++){
            float currentX = -10 + i*side;
            float currentY = 10 - j*side;

            float dist = sqrt(pow(centerX-currentX,2) + pow(centerY-currentY,2));

            float pd = dist*2/terrainCircleSize;
            if (fabs(pd) <= 1.0){
                terrain[i][j] += disp/2 + cos(pd*3.14)*disp/2;
                if (terrain[i][j] > currentMax){
                	currentMax = terrain[i][j];
                }

            }
        }
    }

}

void fault_1(){
	v = rand();
	a = sin(v);
	b = cos(v);
	d = sqrt(2*terrainSize*terrainSize);
	c = (rand()/RAND_MAX) * d - d/2;

	for (int i=0;i<terrainSize;i++){
		for (int j=0;j<terrainSize;j++){
			if (a*i+b*j -c>0){
				terrain[i][j] += 1;
			}
			else{
				terrain[i][j] -= 1;
			}
		}
	}
}

void fault_2(){
	int x1 = rand() % terrainSize;
	int z1 = rand() % terrainSize;
	int x2 = rand() % terrainSize;
	int z2 = rand() % terrainSize;

	//printf("%i,%i,%i,%i\n",x1,z1,x2,z2);

	a = z2-z1;
	b = -x2+x1;
	c = -x1*(z2-z1) + z1*(x2-x1);


	for (int i=0;i<terrainSize;i++){
		for (int j=0;j<terrainSize;j++){
			if (a*i+b*j -c>0){
				terrain[i][j] += 0.5;
				if (terrain[i][j]>currentMax)
					currentMax = terrain[i][j];
			}
			else{
				terrain[i][j] -= 0.5;
					
			}
		}
	}



}

void allTheHeightsFaults(int num){
	for (int i =0;i<num;i++){
		fault_2();
	}
}


void allTheHeightsCircles(int num){
    for (int i=0;i<num;i++){
       
        
		int x = rand() % terrainSize;
    	int y = rand() % terrainSize;
        //printf("current x y r %i,%i\n",x,y);
    	circles(x,y);
    }
}

void initTerrain(){
	for (int i=0; i<terrainSize;i++){
        for (int j=0;j<terrainSize;j++){
        	terrain[i][j]=0.0;
        }
    }

}

void keyboard_2(unsigned char key, int h, int i){
	switch (key)
	{
		case 'q':
	case 27:
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int h, int i)
{

	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
		case 'q':
		case 27:
			exit(0);
			break;
		case 'w':
		case 'W':
			if (wireframe ==0 ){
				wireframe = 1;
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			}
			if (wireframe == 1){
				wireframe = 2;
				break;
			}
			else if (wireframe == 2){
				wireframe = 0;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}
		case 'l':
		case 'L':
			if (lighting==1){
				lighting =0;
				glDisable(GL_LIGHTING);
				break;
			}
			else{
				glEnable(GL_LIGHTING);
				lighting = 1;
				break;
			}
		case 'f':
		case 'F':
			if (flatS == 1){
				flatS = 0;
				glShadeModel(GL_SMOOTH);
				break;
			}
			else{
				flatS = 1;
				glShadeModel(GL_FLAT);
				break;
			}
		case 't':
		case 'T':
			triMesh = 1;
			break;
		case 'y':
		case 'Y':
			triMesh = 0;
			break;
		case 'u':
		case 'U':
				light_pos[2] += 2;
			break;
		case 'j':
		case 'J':
				light_pos[2] -= 2;
			break;
		case 'h':
		case 'H':

			light_pos[1] -= 2;
			break;
		case 'k':
		case 'K':
			light_pos[1] += 2;
			break;
		case 'R':
		case 'r':
			initTerrain();
			if (alg == 1){
    		allTheHeightsCircles(numOfCircles); 
    		}
  			else{
    	 	allTheHeightsFaults(numOfIterations);
    		}
			overview();
			break;
		case 'z':
		case 'Z':
			if (alg == 1){
				alg = 0;
				initTerrain();
				currentMin =0;
				currentMax =0;
				allTheHeightsFaults(numOfIterations);
				break;
			}
			else{
				alg = 1;
				initTerrain();
				currentMin =0;
				currentMax =0;
				allTheHeightsCircles(numOfCircles); 
				break;
			}
		case 'g':
		case 'G':
			if (gouraud == 0){
				gouraud =1;
				break;
			}
			else{
				gouraud =0;
				break;
			}

	}
	glutPostRedisplay();
}

void reshape(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    gluPerspective(45, (float)((w+0.0f)/h), 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

/* main function - program entry point */
int main(int argc, char** argv)
{

	wireframe = 0;
	currentMax = 0;
	currentMin =0;
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(70, 40);
	window_1 = glutCreateWindow(argv[0]);
	glutSetWindowTitle("Terrain");	//creates the window
	glEnable(GL_DEPTH_TEST);
	init();



	printf("\n");
	printf("\n");
	// std::cout << "Please input in the size of the Terrain. (Input a value between 30 - 300)" << std::endl;
	// std::cin >> terrainSize;
	printf("\n");
	printf("\n");

	terrainSize = 100;

	printf("Interactive Terrain Mesh\n");
	printf("Using arrow keys to adjust the point of view\n");
	printf("W/w - Switch from wireframe to filled polygons to mixed views\n");
	printf("L/l - Lights on/ off\n");
	printf("F/f - Smooth/ Flat shading\n");
	printf("T/t - Enable Triangle Mesh\n");
	printf("Y/y - Enable Polyon Mesh\n");
	printf("U/J/H/K - Control the position of one light\n");
	printf("R/r - Reset the whole terrain\n");
	printf("Z/z - Toggle between Circles Algorithm and Fault Algorithm\n");
	printf("G/g - Enable Gouraud shading\n");
	printf("\n");
	printf("\n");




//	terrainSize = 100;
    side = 40.0 / (float)terrainSize;
    srand(time(NULL));
    numOfIterations = 800;
    numOfCircles = 100;
    for (int i=0; i<terrainSize;i++){
        for (int j=0;j<terrainSize;j++){
        	terrain[i][j]=0.0;
        }
    }
    if (alg == 1){
    	allTheHeightsCircles(numOfCircles); 
    }
    else{
    	 allTheHeightsFaults(numOfIterations);
    }

   
    //
   
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);


	glutInitWindowSize(300, 300);
	glutInitWindowPosition(900, 200);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	window_2 = glutCreateWindow (argv[0]);
	glutSetWindowTitle("Height Map");
   	init_2();
   	overviewSide = 2/(float)terrainSize;
   	glutDisplayFunc(display_2);
   	glutReshapeFunc(reshape);
   	glutKeyboardFunc(keyboard_2);

	


	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}
