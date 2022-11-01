#include <iostream>
#include <cmath>

#include "glut.h"

#include "GL_movement.h"   // здесть удобный класс 3д точки и действия с камерой
#include "readBMP.h"

// меня замучали предупреждения о преобразованиях double во float так что будем игнорировать эти предупреждения

#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  

// ключевое слово extern информирует компиллятор,
// что это внешние переменные и их значение доступно другим файлам
// используется внутри GL_movement

extern GLfloat alX=0, alY=0; // угол поворота

// начальные данные о положении и совершенном перемещении мыши
extern int lx0=0,lx1=0,ly0=0,ly1=0; // левая клавиша
extern int rx0=0,rx1=0,ry0=0,ry1=0; // правая клавиша
extern int dveX0=0,dveY0=0,dveX1=0,dveY1=0; //две клавиши нажатые вместе

/*
 предопредленый угол зрения (FOV -Field of View) в перспективном режиме
 или величина приближения(в абсолютных единицах) в ортогональном режиме
 для приближения камеры(увеличения pzoom) удобно вызывать zoom(+3);
 для отдаления камеры(уменьшения pzoom) удобно вызывать zoom(-3);
 */
extern int pzoom=60; 
 
// тип используемой камеры по умолчанию
// 1 - ортогональная 2-перспективная                        
extern int view=1;

extern bool m1=false; // нажата ли левая клавиша мыши
extern bool m2=false; // нажата ли правая клавиша мыши

// ининциализация глобавльных переменных
extern GLdouble w=900,h=900,px=0,py=0;

extern MyPoint e(10,20,10);
extern MyPoint c(0,0,0);
extern MyPoint u(0,0,1);

MyPoint Light_pos(10,0,5);
bool plosk_smooth=true; // сглаживать изображение плоскости?



// указатель на текстуру
GLubyte *resImage = 0;
// номер текстуры
GLuint texture1;

// переменная для списка отображения
GLuint Item1;

int resImageWidth, resImageHeight;

int mode=0;

static void Resize(int width, int height);
static void Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

// функция задания свойст источников освещения
void InitLight(void)
 {
 
  GLfloat amb[] = {0.1,0.1,0.1,1.};
  GLfloat dif[] = {1.,1.,1.,1.};
  GLfloat spec[] = {1.,1.,1.,1.};
  GLfloat pos[] = {Light_pos.x,Light_pos.y,Light_pos.z,1.};

// параметры источника света
// позиция источника
  glLightfv(GL_LIGHT0, GL_POSITION, pos);



// характеристики излучаемого света
// фоновое освещение (рассеянный свет)
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
// диффузная составляющая света
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
// зеркально отражаемая составляющая света
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
  
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glPointSize(10);
  glColor3ub(255,255,0);
  glBegin(GL_POINTS);
   glVertex3fv(Light_pos);
  glEnd();
  glEnable(GL_LIGHTING);

}


// функции обработки событий

//extern GLfloat alX=0, alY=0; // угол поворота
int mx0,mx1,my0,my1;

void view_select(void)
{
    view++;
    if (view>2)
        view = 1;

    pzoom = 80;
    c.Set(0, 0, 0);
    e.Set(10, 20, 10);
    zoom(0);
}

void Light_in_Camera(void)
{
    if (view == 2) // если в перспективе
    {
        Light_pos = e;
    }
}

void plosk_select(void)
{
    plosk_smooth = !plosk_smooth;
}

void loadImage()
{
    int i,j;
    const char bmpFileName[] = "List.bmp";
    int *srcImage = loadBMP(bmpFileName, resImageWidth, resImageHeight);
    if (!srcImage)
    {
        std::cout << "Could not load an image from the file " << bmpFileName
                  << "\ncheck if the file is present in the working directory.\n";
        return;
    }

    //Выделяем память под наше изображение
    delete[] resImage;
    resImage=new unsigned char[resImageWidth * resImageHeight * 4];

    //Процессим изображение (циклы по линиям и по столбцам)
    for (i=0;i<resImageHeight;i++)
    {
    	for(j=0;j<resImageWidth;j++)
    	{
    		//Переприсваиваем в нашем изображении цветовые значения исходного
    		
            int pixelValue = srcImage[i * resImageWidth + j];
            unsigned char red = pixelValue % 256;
            pixelValue >>= 8;
            unsigned char green = pixelValue % 256;
            unsigned char blue = pixelValue >> 8;

            resImage[i*resImageWidth*4+j*4]=red;
    		resImage[i*resImageWidth*4+j*4+1]=green;
    		resImage[i*resImageWidth*4+j*4+2]=blue;
    		
    		// если цвет их всех равен 0, то сделать альфа компонент =0;
            resImage[i*resImageWidth * 4 + j * 4 + 3] =
                (red || green || blue) ? 255 : 0;
    	}
    }
    delete srcImage;
}

void Textura_use(void)
{
    loadImage();

    // устанавливаем формат хранения пикселей
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //  Команды glGenTextures() и glBindTexture() именуют и создают текстурный объект для изображения текстуры.
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // устанавливаем параметры повторения на краях
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // параметры отображения текстур если происходит уменьшение или увеличение текстуры
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    // даем команду на пересылку текстуры в память видеокарты
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth, resImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage);


    // способ наложения текстуры GL_MODULATE, GL_DECAL, and GL_BLEND.
    // GL_MODULATE - умножение на цвет от освещения
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // создадим два новых списка отображения
    Item1 = glGenLists(2);

    glNewList(Item1, GL_COMPILE);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(0, 0);
    glVertex3f(-30, -30, -3);


    glTexCoord2f(1, 0);
    glVertex3f(30, -30, -3);

    glTexCoord2f(1, 1);
    glVertex3f(30, 30, -3);

    glTexCoord2f(0, 1);
    glVertex3f(-30, 30, -3);

    glEnd();
    glEndList();

    MyPoint A(-10, -10, 0), B(-10, 10, 0), C(10, -10, 0), D(10, 10, 0), S(0, 0, 20), N1, N2, N3, N4, T1, T2;

    T1 = B - A;
    T2 = S - A;
    // опереатором *= я переопределил векторное умнежение векторов
    N1 = T2 *= T1;
    N1.Normalize_Self();

    T1 = (D - B);
    T2 = (S - B);
    N2 = T2 *= T1;
    N2.Normalize_Self();

    T1 = (A - C);
    T2 = (S - C);
    N3 = T2 *= T1;
    N3.Normalize_Self();

    T1 = (C - D);
    T2 = (S - D);
    N4 = T2 *= T1;
    N4.Normalize_Self();

    glNewList(Item1 + 1, GL_COMPILE);

    //Рисуем пирамидку
    glBegin(GL_TRIANGLES);

    glNormal3fv(N1);
    glTexCoord2f(1, 1); glVertex3fv(B);
    glTexCoord2f(0, 1); glVertex3fv(A);
    glTexCoord2f(0.5, 0); glVertex3fv(S);

    glNormal3fv(N2);
    glTexCoord2f(1, 1); glVertex3fv(D);
    glTexCoord2f(0, 1); glVertex3fv(B);
    glTexCoord2f(0.5, 0); glVertex3fv(S);

    glNormal3fv(N3);
    glTexCoord2f(1, 1); glVertex3fv(C);
    glTexCoord2f(0, 1); glVertex3fv(D);
    glTexCoord2f(0.5, 0); glVertex3fv(S);

    glNormal3fv(N4);
    glTexCoord2f(1, 1); glVertex3fv(A);
    glTexCoord2f(0, 1); glVertex3fv(C);
    glTexCoord2f(0.5, 0); glVertex3fv(S);

    glEnd();

    glEndList();
    mode = 1;

}

void texture_mode(void)
{
    static bool t = 0;
    if (t)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    t = !t;
}

void Key_DOWN(void)
{
  if(view==1)
    jamp(-3,0);
  else
    strate(-3,0);
}
void Key_UP(void)
{
  if(view==1)
    jamp(3,0);
  else
    strate(3,0);
}
void Key_LEFT(void)
{
  jamp(0,-3);
}
void Key_RIGHT(void)
{
   jamp(0,3);
}

// Общий обработчик нажатия клавиш.
void keyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ' ': view_select(); break;
    case 'w':
    case 'W': Key_UP(); break;
    case 's':
    case 'S': Key_DOWN(); break;
    case 'a':
    case 'A': Key_LEFT(); break;
    case 'd':
    case 'D': Key_RIGHT(); break;
    case 'L':
    case 'l':
    case '1': Light_in_Camera(); break;
    case 'P':
    case 'p':
    case '2': plosk_select(); break;
    case 'T':
    case 't':
    case '3': Textura_use(); break;
    case '4': texture_mode(); break;
    }
}
void specialKeyPressed(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: Key_DOWN(); break;
    case GLUT_KEY_LEFT: Key_LEFT(); break;
    case GLUT_KEY_RIGHT: Key_RIGHT(); break;
    case GLUT_KEY_UP: Key_UP(); break;
    }
}

// действие по нажатию или отпусканию кнопки мыши.
void mouseEvent(int button, int state, int x, int y)
{

    // действие по нажатию левой кнопки мыши
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        m1 = true;          // дать пометку, что левая кнопка была нажата
        lx0 = x;            // сохранить х координату мышки
        ly0 = y;            // сохранить y координату мышки

        if ((m1) && (m2))       // если обе(и правая и левая) кнопки мыши были нажаты
        {                       // то сохранить х и y координаты мыши в отдельные переменные,
            dveX0 = x;          //  отвечающие за действие приближения (в орто проекции)  
            dveY0 = y;          //  или  изменения угла зрения в перспективной проекции  

        }

    }
    // действие по событию отпускания левой кнопки мыши
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        m1 = false;
    }
    // действие по событию нажатия правой кнопки
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        m2 = 1;               // дать пометку, что правая кнопка была нажата
        rx0 = x;              // сохранить х координату мышки
        ry0 = y;              // сохранить y координату мышки

        if ((m1) && (m2))
        {                     // если обе(и правая и левая) кнопки мыши были нажаты
            dveX0 = x;        // то сохранить х и y координаты мыши в отдельные переменные,
            dveY0 = y;        //  отвечающие за действие приближения (в орто проекции)  
        }                     //  или  изменения угла зрения в перспективной проекции 
    }
    // действие по событию отпускания правой кнопки мыши
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        m2 = 0;
    }

}

// общий обработчик вызывающийся при движении мыши
void mouse_move(int x, int y)
{

 if((m1)&&(m2))                                  // а не было ли жвижения мышки с нажатыми обоими кнопками?
    {dveX1=x-dveX0;
     dveY1 = y-dveY0;
     
     zoom((int)(dveX1+dveY1));                  // если да то применить приближение/изменения угла зрения

     dveX0 = x;
     dveY0 = y;  
     return;
    }
 else
    {
        if (m1)                                        // может была нажата левая кнопка мыши?
            {  lx1 = x-lx0;
               ly1 = y-ly0;
                    
                    jamp(-ly1,lx1);                  // тогда сдвинуться вверх и влево(по перемещению) 
                    
               lx0 = x;
               ly0 = y;
            }
        else if(m2)                                  // или может была нажата правая кнопка мыши?
        {
            
            rx1 = x-rx0;
            ry1 = y-ry0;
                
                look_around(-rx1/10.0,ry1/30.0);              // тогда повернуть камеру
                
            rx0 = x;
            ry0 = y;
        }
    } 	
   
}

void TimerCallback(int fictive)
{
  glutPostRedisplay();
}

int main(int argc, char *argv[])
{



// регистрация функций обработки событий от мыши и от клавиатуры
    std::cout
      << "Active keys:\n"
      << "[Space bar] - toggle camera mode (parallel or perspective),\n"
      << "[Left arrow] or [A] or [a] - move to the left,\n"
      << "[Right arrow] or [D] or [d] - move to the right,\n"
      << "[Up arrow] or [W] or [w] - move forward (in the perspective mode)\n"
      << "                           or move up in the parallel projection mode,\n"
      << "[Down arrow] or [S] or [s] - move backward (in the perspective mode)\n"
      << "                           or move down in the parallel projection mode.\n"

      << "[1] or [l] or [L] - place the light source in the eye position\n"
      << "                    (works only in the perspective projection mode),\n"
      << "[2] or [p] or [P] - toggle the brightness model,\n"
      << "[3] or [t] or [T] - the firtree with textures,\n"
      << "[4] bilinear filtration on/off.\n";
    
    
    
    glutInit(&argc, argv);

    // размер окна OpenGL
    windowW = 600;
    windowH = 600;

    // расположение окна OpenGL на экране
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowW, windowH);

    // инициализация окна OpenGL с заголовком Title
    glutCreateWindow("Laba 2");

    glutMouseFunc(mouseEvent);

	// для события смещения - общий обработчик и при ненажатых кнопках, и при нажатых.
	// один для нажатия правой, левой и одновременно обоих нажатых кнопок
    glutMotionFunc(mouse_move);
    glutPassiveMotionFunc(mouse_move);

    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialKeyPressed);


// установка основных параметров работы OpenGL
// цветовой режим RGB | включение Z-буфера для сортировки по глубине

	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );

// Инициализация источников света
// если вне видового преобразования, то источник ориентирован относительно 
// наблюдателя, т.е. он будет фиксирован в системе координат, 
// XOY которой связана с экраном. 
// В нашем примере источник с pos[] = {0.,0.,1.,0.}; светит перпендикулярно экрану
   InitLight();

// регистрация функции, которая вызывается при изменении размеров окна
//  Resize() - функция пользователя
	glutReshapeFunc(Resize);

// регистрация функции, которая вызывается при перерисовке 
// и запуск цикла обработки событий
// Draw() - функция пользователя

    glutDisplayFunc(Draw);
    glutMainLoop();
  return 0;
}



static void Resize(int width, int height) // создается пользователем
{ 
//1. получение текущих координат окна 


	w = width;
	h = height;
// сохраняем размеры окна вывода w,h  
    if(h == 0)
    	h = 1;
// установка новых размеров окна вывода
glViewport(0,0,w,h);


// в ней мы установим тип проекции (ортогональная или перспективная)
    zoom(0);

}



static void Draw(void) // создается пользователем
{
// 1. очистка буферов (цвета, глубины и возможно др.)

  // установка цвета фона
	  glClearColor(0.75f,0.75f,0.75f,1.0f) ;

  // очистка всех буферов
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//2.установка режимов рисования
  
  // нормализация нормалей : их длины будет равна 1
  glEnable(GL_NORMALIZE);

  // включение режима сортировки по глубине
  glEnable(GL_DEPTH_TEST) ;


  
  glEnable(GL_LINE_SMOOTH); // устранение ступенчатости для линий
  glEnable(GL_POINT_SMOOTH);
  
  // для качественного устранения ступенчатости 
  // нам надо включить режим альфа-наложения(альфа смешения BLEND - смешивать)
  glEnable(GL_BLEND);
  
  // Настройка альфа сглаживания:
  glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);

  // тип рисования полигонов
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // первый параметр: рисовать все грани, или только внешние или внутренние
  // GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
  // второй: как рисовать 
  // GL_POINT (точки) GL_LINE(линии на границе), GL_FILL(заполнять)
  

  //
  // тип закраски полигонов
    glShadeModel(GL_FLAT) ; 
  // GL_FLAT все пикселы полигона имеюют одинаковый цвет ( за цвет принимается 
  // цвет первой обрабатываемой вершины, если включены источники, 
  // то этот цвет модифицируется с учетом источников и нормали в вершине)
  // GL_SMOOTH цвет каждого пиксела рассчитывается интерполяцией цветов вершин 
  // с учетом источников света. если они включены
  
  //  включить режим учета освещения
     glEnable(GL_LIGHTING); 

  //   задать параметры освещения
  //  параметр GL_LIGHT_MODEL_TWO_SIDE - 
  //                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
  //                1 - лицевые и изнаночные обрабатываются разными режимами       
  //                соответственно лицевым и изнаночным свойствам материалов.    
  //  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
  //                не зависящее от сточников
  // по умолчанию (0.2, 0.2, 0.2, 1.0)
      
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0 ); 

  
  //  включить нужное количество источников 
   glEnable(GL_LIGHT0);

  // new!  автоматического задания свойств материала 
  // функцией glColor() не используем!!!
  // glEnable(GL_COLOR_MATERIAL);
  // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
 
// 3.установка видового преобразования

// выбор видовой матрицы в качестве текущей

  glMatrixMode( GL_MODELVIEW );

// Сохранение текущего значения матрицы в стеке
  //glPushMatrix();

// загрузка единичной матрицы
  glLoadIdentity();

// установка точки наблюдения

 gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

 InitLight();
// если здесь инициализировать источник света
// то он будет зафиксирован в мировых координатах, т.е.
// при poition(0.,0.,1.,0.) он  будет светить вдоль
//  мировой оси Z ( в нашем примере синяя ось)

// преобразования объектов
// (угол поворота, меняется машью или клавиатурой)
// glRotatef(alX,1.0f,0.0f, 0.0f);
// glRotatef(alY,0.0f,1.0f, 0.0f);

//new!  InitLight();
// если здесь инициализировать источник, то он будет вращается 
//вместе с объектом. В нашем случае при позиции источника 
// pos[] = {0.,0.,1.,0.} при любых вращениях ярко освещена будет 
//только синяя грань

// 4. вызов модели
  DrawModel() ;

// 5. завершение видового преобразования
  // Restore the view matrix
//  glPopMatrix();

//6. Завершить все вызванные на данный момент операции OpenGL
  glFinish();
  glutSwapBuffers() ;
  int delay = 0;
  if((!m1)&&(!m2))
    delay = 100;
  glutTimerFunc(delay, TimerCallback, 0);
}

void DrawModel (void)
{
// геометрическая модель включает шар и
// 3 отрезка вдоль координатных осей


  glColor3f( 0.5f,0.5f,0.5f); 

  GLfloat amb[] = {0.2,0.2,0.1,1.}; 
  GLfloat dif[] = {0.4,0.65,0.5,1.};
  GLfloat spec[] = {0.9,0.8,0.3,1.};
  GLfloat sh= 0.1f*256; 

  
  glMaterialfv(GL_FRONT,GL_AMBIENT,amb);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,dif);
  glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
  glMaterialf(GL_FRONT,GL_SHININESS,sh);
  
glDisable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glutSolidSphere(1.0, 100, 100);
  glShadeModel(GL_FLAT) ; 
  glutSolidTorus (0.5,1.5,100,100);


if(plosk_smooth)
 glShadeModel(GL_SMOOTH);
else
 glShadeModel(GL_FLAT) ; 
  //Ground
glNormal3f(0,0,1);
glBegin(GL_QUADS);
int i,j;
for(i=-10; i<20;i++)
 for(j=-10;j<20;j++)
 {
  glVertex3f(i,j,0);
  glVertex3f(i+1,j,0);
   glVertex3f(i+1,j+1,0);
  glVertex3f(i,j+1,0);

  }
 glEnd();


 if(mode)
 { glEnable(GL_TEXTURE_2D);
   // рисуем прямоугольник
   // формулу вычисления освещения пикселя см в справке
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glCallList(Item1);
 glTranslatef(-15,-15,0);
   // рисуем пирамидку
   // цвет текстуры заменяет освещенность
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glCallList(Item1+1);

   glEnable(GL_BLEND);
// настройти приемника и источника
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 glPushMatrix();
int i;
 glTranslatef(30,0,0);

// цвет текстуры умножается модифицируется интенсивностью освещения
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// елочка рисуется 6 раз вывается пирамидка
   for(i=0;i<=6;i++)
   {
   glScalef(1.1-i*0.1,1.1-i*0.1,0.8);
   glTranslatef(0,0,5*i);
   glCallList(Item1+1);
    }
   glPopMatrix();
   
  }
  
// отключаем режим учета освещенности, чтобы нарисовать оси неизменного цвета
     glDisable(GL_LIGHTING); 

//OX RED
  glBegin(GL_LINES);

  glColor3f( 1.0f,0.0f,0.0f);
  glNormal3f(0., 0., 1.);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(3.0f,0.0f,0.0f); 
  glEnd();

// OY GREEN
  glBegin(GL_LINES);
  glColor3f( 0.0f,1.0f,0.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,3.0f,0.0f); 
  glEnd();

// OZ BLUE
  glBegin(GL_LINES);
  glColor3f( 0.0f,0.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,3.0f); 
  glEnd();

}
