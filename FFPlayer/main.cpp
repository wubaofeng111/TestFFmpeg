#include <stdio.h>
#include <string.h>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#include <malloc/_malloc.h>
#include <math.h>
#include <GLKit/GLKit.h>

enum AttribEnum
{
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
    ATTRIB_COLOR,
};


#define true 1
#define false 0

//YUV数据枚举
enum TextureType
{
    TEXY = 0,
    TEXU,
    TEXV,
    TEXC
};

typedef struct Size
{
    GLuint width;
    GLuint height;
}Size;


Size windowSize;
GLuint                  _textureYUV[3];
GLuint                  _renderBuffer;

#pragma pack(push, 1)

typedef struct H264FrameDef
{
    unsigned int    length;
    unsigned char*  dataBuffer;
    
}H264Frame;

typedef struct  H264YUVDef
{
    unsigned int    width;
    unsigned int    height;
    H264Frame       luma;
    H264Frame       chromaB;
    H264Frame       chromaR;
    
}H264YUV_Frame;


#pragma pack(pop)



GLuint _framebuffer;
GLuint _program;

GLuint CompileShader(char *shaderString,GLenum shaderType)
{
    if (shaderString == NULL) {
        return 0;
    }else{
        
    }
    
    GLuint shaderHandle = glCreateShader(shaderType);
    size_t shaderString_Length = strlen(shaderString);
    glShaderSource(shaderHandle, 1, (const GLchar *const *)&shaderString, (const GLint *)&shaderString_Length);
//    glShaderSource(shaderHandle, 1, &shaderString, &shaderString_Length);
    
    glCompileShader(shaderHandle);
    
    GLint compileSuccess = GL_FALSE;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        printf("%s\n",messages);
//        exit(0);
    }
    
    return shaderHandle;
}

void loadShader()
{
    
    
    
    
    FILE *filefs = fopen("/Users/friday/Documents/LearnExample/TestFFmpeg/FFPlayer/shader.fsh", "ab+");
    fseek(filefs, 0, SEEK_END);
    size_t filesize = ftell(filefs);
    
    
    char *fsStr = (char*)malloc(filesize);
    fseek(filefs, 0, SEEK_SET);
    fread(fsStr, filesize, 1, filefs);
    
    printf("%s",fsStr);
    GL_GEOMETRY_SHADER;
    GLuint vertexShader = CompileShader(fsStr, GL_VERTEX_SHADER);
    
    free(fsStr);
    
    fsStr = NULL;
    
    FILE *filevs = fopen("/Users/friday/Documents/LearnExample/TestFFmpeg/FFPlayer/shader.vsh", "ab+");
    fseek(filevs, 0, SEEK_END);
    filesize = ftell(filevs);
    fsStr = (char*)malloc(filesize);
    fseek(filevs, 0, SEEK_SET);
    fread(fsStr, filesize, 1, filevs);
    
    printf("%s",fsStr);
    
    
    GLuint fragmentShader = CompileShader(fsStr,GL_FRAGMENT_SHADER);
    _program = glCreateProgram();
    
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    
    glBindAttribLocation(_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(_program, ATTRIB_TEXTURE, "TexCoordIn");
    glLinkProgram(_program);
    
    GLint linkSuccess;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
    
    if(linkSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog(_program, sizeof(messages), 0, &messages[0]);
        printf("link error:%s\n",messages);
    }
    
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }
}




/**
 渲染
 */
void render()
{
    
    
    //把数据显示在这个视窗上
    glViewport(1, 1, windowSize.width * 1 -2, windowSize.height * 1 -2);
    
    //
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    
    
    static const GLfloat coordVertices[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
    
    //更新属性值
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    //开启定点属性数组
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    
    
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);
    
    //绘制
    
    //当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
    //绘制方式,从数组的哪一个点开始绘制(一般为0),顶点个数
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    //将该渲染缓冲区对象绑定到管线上
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    
    //把缓冲区（render buffer和color buffer）的颜色呈现到UIView上
//    [_glContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
}



void displayYUV420pData(H264YUV_Frame * frame)
{
    int w = frame->width;
    int h = frame->height;
    windowSize.width = w;
    windowSize.height = h;
    
    if (w != 600 || h != 600) {
        glutReshapeWindow(w, h);
    }
    //绑定
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)w, (GLsizei)h, GL_RED, GL_UNSIGNED_BYTE, frame->luma.dataBuffer);
    
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)w/2, (GLsizei)h/2, GL_RED, GL_UNSIGNED_BYTE, frame->chromaB.dataBuffer);
    
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)w/2, (GLsizei)h/2, GL_RED, GL_UNSIGNED_BYTE, frame->chromaR.dataBuffer);
    //渲染
    render();
}




bool createFrameAndRenderBuffer()
{
    glGenFramebuffers(1, &_framebuffer);
    glGenRenderbuffers(1, &_renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, windowSize.width, windowSize.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    
    //检查当前帧缓存的关联图像和帧缓存参数
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("创建缓冲区错误 0x%x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return false;
    }
    
    return true;
}



const GLfloat pi = 3.1415926326;

void drawWJX()
{
    GLfloat a = 1 / (2-2*cos(72*pi/180));
    GLfloat bx = a* cos(18*pi/180);
    GLfloat by = a * sin(18*pi/180);
    GLfloat cy = -a * cos(18*pi/180);
    
    GLfloat pointa[2] = {0,a},
    pointb[2] = {bx,by},
    pointc[2] = {0.5,cy},
    pointd[2] = {-0.5,cy},
    pointe[2] = {-bx,by};
    
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINE_LOOP);
    {
        glVertex2fv(pointa);
        glVertex2fv(pointc);
        glVertex2fv(pointe);
        glVertex2fv(pointb);
        glVertex2fv(pointd);
    }
    glEnd();
    
    glFlush();
}
const float factor = 0.1f;

void DrawWZHS()
{
    GLfloat x;
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    {
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        
        glVertex2f(0.0f, 1.0f);
        glVertex2f(0.0f, -1.0f);
    }
    glEnd();
    
    glBegin(GL_LINES);
    {
        for(x = -1.0f/factor; x < 1.0f/factor;x+=0.01f)
        {
            glVertex2f(x*factor, sin(x)*factor);
        }
    }
    glEnd();
    
    glFlush();
}

void drawQiuti()
{
    GLfloat x;
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    {
        glutSolidSphere(0.4, 1, 1);
    }
    glEnd();
    
   
    
    glFlush();
}

void draw()
{
    drawQiuti();
    
}



int main(int argc, char* argv[])
{
    
    glutInitWindowSize(600, 600);
    
    glutInit(&argc, (char**)argv);
    //创建一个窗口并制定窗口名
    glutCreateWindow("HelloWorld");
    
    loadShader();
    
    const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
    const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
    const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
    const GLubyte* gluVersion= gluGetString(GLU_VERSION); //返回当前GLU工具库版本
    printf("OpenGL实现厂商的名字：%s\n", name);
    printf("渲染器标识符：%s\n", biaoshifu);
    printf("OpenGL实现的版本号：%s\n",OpenGLVersion );
    printf("OGLU工具库版本：%s\n", gluVersion);
    
    //注册一个绘图函数，操作系统在必要时刻就会对窗体进行重新绘制操作
    glutDisplayFunc(&draw);
    glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
    glGenFramebuffers(1, &_framebuffer);
    
    glCompileShader(_framebuffer);
    
    
    glutMainLoop();
    
    return 0;
}
