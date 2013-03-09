#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "glm.cpp"
#include <unistd.h>

#define PI 3.141592653589
#define rows 8
#define tiles 8

using namespace std;
double ht[10][10];  // Height of tiles

static float ypoz = 0, zpoz = 0, xpoz = 0;
float ang_la = 0.0 ,ang_ra = 0.0 , ang_ll = 0.0 , ang_rl = 0.0;   // angular measure of each vertex group
float flag_la = 1.0f, flag_ra = -1.0f , flag_rl = 1.0f, flag_ll = -1.0f;  // flag for each vertex group
double x_axis = 0, y_axis = 0 , z_axis = 0 , angle_axis = 0;   // co-ordinates of obj
float jump_flag = 0,delta_y, j_flag =0; // j_flag to bring hand and legs strght during jump 
float height;
int third_p=0,first_p=0,fall_flag=1,block_flag=0;
int place_flag=0;				//place_flag for position on block
int periodic_flag=0,p_flag=0;   //periodic_flag for moving tiles
int tile_r,tile_c;
int cam=0;
int tile_view=0;
GLfloat hx=4.0f,hy=7.0f,hz=-3.0f;		
GLfloat vhx=0.0f,vhy=-1.3f,vhz=-3.0f;  //helicopter cam
float zoom,hel_x,hel_z;			//helicopter cam

float dh1 = 0.05f,dh2 = 0.05f,dh3 = 0.05f; // delta_h for height varying tiles
float var_height1=0.1f,var_height2=0.90f,var_height3=0.90f; // initial heights
double board[rows][tiles];  //array for storing x-coordinates of tiles
double row_range[rows];   
int rows_ct=0,tiles_ct=0;
int tiles_in_row[rows];   //valid tiles in each row

GLMmodel* pmodel1 = NULL;

void jump();

void fall();
void check();

GLuint displayArea; //The OpenGL id of the display list

void init(void) 
{
   glClearColor (0.5, 0.5, 1.0, 0.0);   
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_COLOR_MATERIAL);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_NORMALIZE);
   glEndList();
}



void drawmodel_box(void)
{
	
    if (!pmodel1) 
	{
      	pmodel1 = glmReadOBJ("agent.obj");	
        if (!pmodel1) 
        	exit(0);
        glmUnitize(pmodel1);	
        glmFacetNormals(pmodel1);        
		glmVertexNormals(pmodel1, 90.0);
    }

    glmDraw1(pmodel1, GLM_SMOOTH| GLM_TEXTURE,0,1,x_axis,y_axis,z_axis,j_flag,ang_ll,ang_rl,ang_la,ang_ra,angle_axis);
    //glDisable(GL_LIGHTING);	
}


void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	// ambient light
	GLfloat ambientColor[] = {0.2f,0.2f,0.2f,1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT , ambientColor);

	//positioned light
	GLfloat lightColor0[] = {0.5f,0.5f,0.5f,1.0f};
	GLfloat lightPos0[] = {0.0f,4.0f,-4.0f,1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//directed light
	GLfloat lightColor1[] = {0.5f,0.2f,0.2f,1.0f};
	GLfloat lightPos1[] = {0.0f,1.5f,0.5f,0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	switch(cam)
	{
		case 0:
	 		glTranslatef(0,-2.6f,-8.4f);
			break;
		case 1:
			gluLookAt(x_axis - 3*sin(angle_axis*(PI/180)) , y_axis + 3 , z_axis - 3*cos(angle_axis*(PI/180)) ,
				x_axis + 0.5 * sin(angle_axis*(PI/180)), y_axis + 1,z_axis + 0.5*cos(angle_axis*(PI/180)),
		 			0,1,0);
		 	break;
		case 2:
		 	gluLookAt(x_axis - 0.1*sin(angle_axis*(PI/180)) , y_axis+0.7 , z_axis -0.1*cos(angle_axis*(PI/180)) ,
		 		x_axis + 0.5 * sin(angle_axis * (PI/180)), y_axis + 0.6,z_axis + 0.5 * cos(angle_axis*(PI/180)),
		 			0,1,0);
		 	break;
		case 3:
		 	gluLookAt(board[tile_view/8][tile_view%8],ht[tile_view/8][tile_view%8]-1,row_range[tile_view/8],
		          x_axis-board[tile_view/8][tile_view%8], y_axis+0.6-ht[tile_view/8][tile_view%8],z_axis-row_range[tile_view/8] ,
		          	0,1,0);
		 	break;
		case 4:
		 	gluLookAt(8.0,6.0,0.0,0-8,0-6,-8,0,1,0);
		  	break;
		case 5:
		   	gluLookAt(hx,zoom,hz,hel_x,vhy,hel_z, 0.0, 1.0 ,0.0);
		  	break;
	}
//	glPushMatrix();	
	// I added these to be able to rotate the whole scene so you can see the box and textures
		glRotatef(ypoz,0,1,0);
		glRotatef(zpoz,0,0,1);
		glRotatef(xpoz,1,0,0);		
		glColor3f(0.65f,0.78f,0.23f);
		drawmodel_box();
//		glPopMatrix();
//	glPopMatrix();
	int l;
	for(l=0;l<tiles;l++)
		tiles_in_row[l]=0;
	float i;		
	int k=0;
	for(float j=0.0;j>(-12.0);j-=1.5)
	{
		k--;
		row_range[rows_ct]=j;
		for(i=0.0;i<(12.0);i+=1.5)
		{
			if(k==16 || k==13 || k==33 || k==45)
			{
				k++;
				continue;
			}
			if(k%2==0)
			{
				glColor3f(0.0f,0.0f,0.0f);
			}
			else
			{
				glColor3f(0.9f,0.9f,0.9f);
			}
			if(k==4 || k==25 || k==30)
				height = 1.0;
			else
				height = 0.3;
			if(k==42)
			{
				var_height1 += dh1;
				if(var_height1>0.90f)
					dh1 *= -1.0;
				if(var_height1<0.1)
					dh1 *= -1.0;
				height = var_height1;
			}
			if(k==22)
			{
				var_height2 += dh2;
				if(var_height2>0.90f)
					dh2 *= -1.0;
				if(var_height2<0.1)
					dh2 *= -1.0;
				height = var_height2;
			}
			if(k==47)
			{
				var_height3 += dh3;
				if(var_height3>0.90f)
					dh3 *= -1.0;
				if(var_height3<0.1)
					dh3 *= -1.0;
				height = var_height3;
			}
			glPushMatrix();
			glTranslatef(i-6,-1.3,j);
			draw_Area(height);
			glPopMatrix();
			k++;
			board[rows_ct][tiles_ct]=i-6;
			ht[rows_ct][tiles_ct++]=height;
			tiles_in_row[rows_ct]++;
		}
		rows_ct++;
		tiles_ct=0;
	}
	check();

	// moving tiles -- needs to be generalised	
	if(tile_r==3 && tile_c==2 && periodic_flag==1)
	{
		y_axis = 1.3-var_height1-0.7;
		p_flag=1;
	}
	else if(tile_r==6 && tile_c==5 && periodic_flag==1) 
	{
		y_axis = var_height3-0.3;
		p_flag=1;
	}
	else if(tile_r==6 && tile_c==1 && periodic_flag==1)
	{
		y_axis = 1.3-var_height2-0.7;
		p_flag=1;
	}
	else if( p_flag==1)
	{
		p_flag=0;
		y_axis = 0;
	}
	periodic_flag=0;	
	glutSwapBuffers();
}

void check()
{
	if(fall_flag==1) 
	{
		int m;
		fall_flag = 0;
		for(m=0;m<rows;m++)
		{
			if( (z_axis) <= row_range[m] && (z_axis) > (row_range[m] - 1.5) )
			{
				int j;
				//printf("in if\n");
				for(j=0;j<tiles_in_row[m];j++)
				{
					if((x_axis) >= board[m][j] && (x_axis) < (board[m][j]+1.5))
					{
						tile_r=m;
						tile_c=j;
						fall_flag=1;
						//printf("m = %d and j = %d\n",m,j);
						if(m == 7 && j == 7)
						{
							printf("You won\n");							
							exit(0);
						}
						if((m==3 && j==2)|| (m==6 && j==1) || (m==6 && j==5))
							periodic_flag=1;
						if(ht[m][j]==1.0)
							block_flag=1;
						else
							block_flag=0;
						break;
					}
				}
				if(jump_flag==1)
					fall_flag=1;
				break;	
			}
			else
			continue;
		}
		if(fall_flag == 0)
		{
			printf("<== GAME OVER ==>\n");
			fall();
			sleep(1);
			exit(0);
		}
	}
}
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
			exit(0);
			break;

		case '1':
			zoom += 0.5;
			break;

		case '2':
			zoom -= 0.5;
			break;

		case ' ':
			j_flag=1;
			jump();
			j_flag=0;
			break;

		case 'c': 
			 cam=(cam+1)%6;
			 break;
		case 't': 
			if(cam==3)
			  {
				tile_view++;
				if(tile_view>63)
		      		tile_view=0;	
			  }	
	                	break;
		case 'w' :
			 if(cam == 5)
			   {
				   hx -= 0.1;
				   vhx -=0.1;
			   }
			   break;
		case 'a' :
			 if(cam==5)
			   {
				   hz+=0.1;
				   vhz+=0.1;
			   }
			   break;
		case 'd': 
			if(cam==5)
			  {
				  hz-=0.1;
				  vhz-=0.1;
			  }
			  break;
		case 's':
			 if(cam==5)
			  {
				  hx+=0.1;
				  vhx+=0.1;
			  }
			  break;
		case 'y':         
			ypoz=ypoz+5;
			if (ypoz>360) ypoz=0;
			//glutPostRedisplay();
			break;

		case 'Y':         
			ypoz=ypoz-5;
			if (ypoz<0) ypoz=360;
			//glutPostRedisplay();
			break;
		case 'z':         
			zpoz = zpoz+5;
			if (zpoz>360) zpoz=0;
			glutPostRedisplay();
			break;	

		case 'Z':         
			zpoz=zpoz-5;
			if (zpoz<0) zpoz=360;
			glutPostRedisplay();
			break;
		case 'x':         
			xpoz=xpoz+5;
			if (xpoz>360) xpoz=0;
			glutPostRedisplay();
			break;
		case 'X':         
			xpoz=xpoz-5;
			if (xpoz<0) xpoz=360;
			glutPostRedisplay();
			break;
		

	}
}

void handleKeypress2(int key, int x, int y) 
{
    if (key == GLUT_KEY_LEFT)
		angle_axis += 10.0;
    if (key == GLUT_KEY_RIGHT)
		angle_axis -= 10.0;
    if (key == GLUT_KEY_UP)
    {
		float dx = 0.05*sin( angle_axis * (PI/180));
		float dz = 0.05*cos( angle_axis * (PI/180));
		x_axis += dx;
		z_axis += dz;
		check();
		
		if(block_flag == 1 && place_flag == 0 && jump_flag == 0)
		{
			block_flag =0; 
			x_axis -=dx;
			z_axis -=dz;
		}
		if(place_flag==1)
		{
			if(periodic_flag==0 && block_flag==0)
			{
				place_flag=0;
				if( (y_axis - 1.3) > (-1.0) )
					y_axis-=0.7;
			}
		}
		jump_flag=1;
		if(fabs(ang_ll) >= 30)
				flag_ll *= -1;
			ang_ll += 3*flag_ll;
		if(fabs(ang_rl) >= 30)
			flag_rl *= -1;
		ang_rl += 3*flag_rl;

		if(fabs(ang_la) >= 30)
			flag_la *= -1;
		ang_la += 3*flag_la;
		if(fabs(ang_ra) >= 30)
			flag_ra *= -1;
		ang_ra += 3*flag_ra;
		
    }
    if (key == GLUT_KEY_DOWN)
    {
		float dx = 0.05*sin( angle_axis * (PI/180));
		float dz = 0.05*cos( angle_axis * (PI/180));
		x_axis -= dx;
		z_axis -= dz;
		check();
		
		if(block_flag == 1 && place_flag == 0 && jump_flag == 0)
		{
			block_flag =0; 
			x_axis +=dx;
			z_axis +=dz;
		}
		if(place_flag==1)
		{
			if(periodic_flag==0 && block_flag==0)
			{
				place_flag=0;
				if( (y_axis - 1.3) > (-1.0) )
					y_axis-=0.7;
			}
		}
		jump_flag=1;
		if(fabs(ang_ll) >= 30)
			flag_ll *= -1;
		ang_ll -= 3*flag_ll;
		if(fabs(ang_rl) >= 30)
			flag_rl *= -1;
		ang_rl -= 3*flag_rl;

		if(fabs(ang_la) >= 30)
			flag_la *= -1;
		ang_la -= 3*flag_la;
		if(fabs(ang_ra) >= 30)
			flag_ra *= -1;
		ang_ra -= 3*flag_ra;
    }
}

void handleKeypress3(int key,int x,int y)
{
	if(key == GLUT_KEY_UP)
		jump_flag=0;
}

void drag(int x, int y){
//  y = 300.0 - y;
    x=250-x;
    y=400-y;
    hel_x=1.0*x/40;
    hel_z=1.0*y/40;
//  printf("drag%d %d\n",x,y);
    glutPostRedisplay();
}


void animate()
{
//	ypoz+=0.5;
//	if (ypoz>360) ypoz=0;
	glutPostRedisplay();
}

void jump()
{
	int t;
	if(jump_flag == 0)
	{
	  for(float i=0.1;i<2.6;i+=0.1)
	  {
			float ty = 0.8*i - (0.61*i*i)/2;
			y_axis += ty;
			display();
			y_axis = y_axis - ty;
	  }
	}
	else 
	{
	  for(t=1;t<25;t++)
	  {
	  	delta_y = 0.08*t - (0.0061*t*t)/2;
		y_axis += 0.15*delta_y;
		
		float dx = 0.08*sin(angle_axis*PI/180);
		float dz = 0.08*cos(angle_axis*PI/180);
		x_axis += 0.6*dx;
		z_axis += 0.6*dz;
		check();
		display();	
	  }
	  for(t=1;t<25;t++)
	  {
		delta_y = 0.08*t - (0.0061*t*t)/2;
		y_axis -= 0.15*delta_y;
		
		float dx = 0.08*sin(angle_axis*PI/180);
		float dz = 0.08*cos(angle_axis*PI/180);
		x_axis += 0.6*dx;
		z_axis += 0.6*dz;
		check();
		
		if(place_flag==1)
		{
			if(block_flag==0 && periodic_flag==0)
			{
				place_flag=0;
				if( (y_axis - 3.0) > (-1.0) )
				y_axis-=0.7;
			}
		}

		if(block_flag==1)	
		{
			block_flag =0;
			if((y_axis - 1.1) < ( ht[tile_r][tile_c]-1.3 ))
			{
				if(place_flag == 0)
				{
					x_axis -=dx;
					z_axis -=dz;
				}
				else
					break;
			}
			else
				place_flag=1;
		}
		display();	
	  }
	}
}

void fall()
{
	int i;
	delta_y = 0.1;
	for(i=0;i<10;i++)
	{
		y_axis -= delta_y;
		delta_y += 0.05;
		display();
	}
}


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMotionFunc(drag);
   glutSpecialFunc(handleKeypress2);
   glutSpecialUpFunc(handleKeypress3);	
   // Do a little animation - rotate the object a bit so we can see it better
   glutIdleFunc(animate);
   glutMainLoop();
   return 0;
}