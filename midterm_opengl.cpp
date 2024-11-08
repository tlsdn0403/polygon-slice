#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include<gl/glm/glm/glm.hpp>
#include<gl/glm/glm/ext.hpp>
#include<gl/glm/glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <random>
#include <fstream>
#include <vector>
#include <chrono>


void drawScene();
void Reshape(int w, int h);
void InitBuffer();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
void MouseClick(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Cleanup();
void checkForCuts();
void createFlyingShape();
void flyingRight(int value);
void Timer(int value);
void moving_Stick(int value);
void moving_shape_onTheBucket(int value);
void show_path(int value);
void rotate_Shape(int value);

void make_line(GLfloat x, GLfloat y);
void make_triangle(GLfloat x, GLfloat y);
void make_square(GLfloat x, GLfloat y);
void make_pentagone(GLfloat x, GLfloat y);
void make_stick(GLfloat x, GLfloat y);
void getOpenGLMouseCoords(int mouseX, int mouseY, float& openglX, float& openglY) {
    openglX = (static_cast<float>(mouseX) / glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f; //X
    openglY = -((static_cast<float>(mouseY) / glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f - 1.0f); // Y
}


char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}


class Shape {
public:
    virtual void draw(GLuint shaderProgramID, GLuint vbo[]) = 0;
    virtual ~Shape() {}
    GLfloat position[2];
    GLfloat color[3];
    GLfloat size;
    GLfloat trans[3];
    GLfloat velocity[2] = { 0.0f, 0.0f }; // 이동 속도
    GLfloat rotate = 0;
    int triangle = 0;
    GLfloat dir = 0;
    bool drawed = false;
    bool broken = false;
    int square;
};
class PointShape : public Shape {
public:
    std::chrono::steady_clock::time_point creationTime;

    PointShape() {
        creationTime = std::chrono::steady_clock::now();
    }

    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat vertices[] = {
            position[0], position[1], 0.0f
        };

        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glPointSize(size*50);
        glDrawArrays(GL_POINTS, 0, 1);
    }
};
class LineShape : public Shape {
public:
    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat halfSize = size;
        GLfloat vertices[] = {
            position[0] - halfSize, position[1], 0.0f,
            position[0] , position[1] + halfSize, 0.0f
        };
        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_LINES, 0, 2);
    }
};


class TriangleShape : public Shape {
public:
    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat sizeVal = size;
        GLfloat triVertices_1[] = {
           position[0] - sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + sizeVal, position[1] - sizeVal, 0.0f,
        };
        GLfloat triVertices_2[] = {
           position[0] + sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + sizeVal - (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f
        };
        GLfloat triVertices_3[] = {
           position[0] - sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + sizeVal, position[1] + sizeVal, 0.0f,
        };
        GLfloat triVertices_4[] = {
          position[0] + sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
           position[0] - sizeVal, position[1] - sizeVal, 0.0f,
           position[0] + sizeVal, position[1] - sizeVal, 0.0f,
        };
        GLfloat triVertices_5[] = {
           position[0] , position[1] + sizeVal, 0.0f,
            position[0]  , position[1] - sizeVal, 0.0f,
            position[0] + sizeVal, position[1], 0.0f
        };
        GLfloat triVertices_6[] = {
           position[0] , position[1] + sizeVal, 0.0f,
            position[0] - sizeVal , position[1] , 0.0f,
            position[0]+sizeVal , position[1], 0.0f
        };
        GLfloat triVertices_7[] = {
           position[0]-sizeVal , position[1] + sizeVal, 0.0f,
            position[0] , position[1] , 0.0f,
            position[0] + sizeVal , position[1]+sizeVal, 0.0f
        };
        GLfloat triVertices_8[] = {
           position[0] , position[1] + sizeVal, 0.0f,
            position[0]  , position[1] - sizeVal, 0.0f,
            position[0] - sizeVal, position[1], 0.0f
        };
        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            if (triangle == 1) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_1);
            }
            else if (triangle == 2) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_2);
            }
            else if (triangle == 3) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_3);
            }
            else if (triangle == 4) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_4);
            }
            else if (triangle == 5) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_5);
            }
            else if (triangle == 6) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_6);
            }
            else if (triangle == 7) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_7);
            }
            else if (triangle == 8) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triVertices_1), triVertices_8);
            }
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

class RectangleShape : public Shape {
public:
    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat sizeVal = size;
        GLfloat rectVertices[] = {
            // 첫 삼각형
            position[0] - sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + sizeVal, position[1] - sizeVal, 0.0f,
            // 두번째 삼각형
            position[0] + sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + sizeVal - (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f
        };
        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectVertices), rectVertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};
class PentagonShape : public Shape {
public:
    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat sizeVal = size;
        GLfloat pentaVertices[] = {
            //첫 삼각형 (맨위)
            position[0] , position[1] + 2 * sizeVal - (2 * sizeVal * trans[0]), 0.0f,
            position[0] - (sizeVal) , position[1] - 2 * sizeVal, 0.0f,
            position[0] + (sizeVal) , position[1] - 2 * sizeVal, 0.0f,

            //2번째 삼각형 (왼쪽 )
            position[0] , position[1] + 2 * sizeVal - (2 * sizeVal * trans[0]) , 0.0f,
            position[0] - 2 * sizeVal + (sizeVal * trans[0])  ,position[1],0.0f,
            position[0] - (sizeVal) , position[1] - 2 * sizeVal,0.0f,

            //3번째 삼각형(오른쪽 )
            position[0] , position[1] + 2 * sizeVal - (2 * sizeVal * trans[0]),0.0f,
             position[0] + 2 * sizeVal - (sizeVal * trans[0]) ,position[1], 0.0f,
            position[0] + (sizeVal),position[1] - 2 * sizeVal,0.0f
        };
        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pentaVertices), pentaVertices);
        glDrawArrays(GL_TRIANGLES, 0, 9);
    }
};
class StickShape : public Shape {
public:
    void draw(GLuint shaderProgramID, GLuint vbo[]) override {
        GLfloat sizeVal = size;
        GLfloat rectVertices[] = {
            // 첫 삼각형
            position[0] - 3*sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - 3*sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + 3*sizeVal, position[1] - sizeVal, 0.0f,
            // 두번째 삼각형
            position[0] + 3*sizeVal, position[1] - sizeVal, 0.0f,
            position[0] + 3*sizeVal - (trans[0] * sizeVal), position[1] + sizeVal, 0.0f,
            position[0] - 3*sizeVal + (trans[0] * sizeVal), position[1] + sizeVal, 0.0f
        };
        GLuint colorLoc = glGetUniformLocation(shaderProgramID, "uColor");
        glUniform3fv(colorLoc, 1, color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectVertices), rectVertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};


GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;
GLuint vao, vboArr[1];

bool lineMode = false;
GLfloat speed = 0.01;
std::vector<Shape*> shapes;
std::vector<Shape*> broken_shapes;
std::vector<Shape*> stick_shape;
std::vector<PointShape*> point_shapes; 
std::vector<Shape*> onTheBucket_shape;
const int MAX_SHAPES = 10;

// 현재에 뭐 그리는지
enum class DrawingMode { Point, Line, Triangle, Rectangle };
DrawingMode currentMode = DrawingMode::Point;


// Mouse drag coordinates for cutting
float dragStartX = 0.0f, dragStartY = 0.0f;
float dragEndX = 0.0f, dragEndY = 0.0f;
bool isDragging = false;
bool stop = true;
bool path = false;
bool isWireframe = 0;
bool rotate = false;
int path_mode = 3;
// 랜덤
std::default_random_engine generator(std::random_device{}());

// Main function
int main(int argc, char** argv) {
    //--- Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 50);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("OpenGL Shapes Example");

    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "GLEW Initialization failed: " << glewGetErrorString(glewStatus) << std::endl;
        return EXIT_FAILURE;
    }

    make_shaderProgram();
    InitBuffer();
    glutTimerFunc(50, Timer, 0);
    glutTimerFunc(16, flyingRight, 0);
    glutTimerFunc(16, moving_Stick, 0);
    glutTimerFunc(16, moving_shape_onTheBucket, 0);
    make_stick(0, -0.9);
    //--- Register callback functions
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(MouseClick);
    glutKeyboardFunc(Keyboard);
    atexit(Cleanup); //메모리 누수 방지 (new로 할당된 객체 delete)

    glutMainLoop();
    return 0;
}


void drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Filled mode
    }
    
    glm::mat4 R0 = glm::mat4(1.0f);
    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    unsigned int mvpLocation = glGetUniformLocation(shaderProgramID, "uMVP");

    for (auto shape : shapes) {

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::mat4 Rz = glm::mat4(1.0f);

        Rz = glm::rotate(Rz, glm::radians(shape->rotate), glm::vec3(0.0, 0.0, 1.0)); //기본 적으로 적용된 회전 행렬
        modelMatrix = glm::translate(modelMatrix, glm::vec3(shape->position[0], shape->position[1], 0.0f)); 
        if (shape->dir != 3) {
            modelMatrix = modelMatrix * Rz; 
        }
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-shape->position[0], -shape->position[1], 0.0f)); 
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)); 

        shape->draw(shaderProgramID, vboArr);
    }



    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(R0));
    for (auto shape : onTheBucket_shape) {
        shape->draw(shaderProgramID, vboArr);
    }
    for (auto shape : stick_shape) {
        shape->draw(shaderProgramID, vboArr);
    }
    for (auto shape : point_shapes) {
        shape->draw(shaderProgramID, vboArr);
    }
    glutSwapBuffers();
    glutPostRedisplay();
}



void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

// 오픈지엘 버퍼 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); //VAO 지정하고 할당
    glBindVertexArray(vao);  // VAO 바인드하기
    glGenBuffers(1, vboArr); // 1개의 VBO 할당(유니폼으로 색상을 할당함으로 1개만 필요)


    //posittion
    glBindBuffer(GL_ARRAY_BUFFER, vboArr[0]); //VBO 바인드
    glBufferData(GL_ARRAY_BUFFER, 27 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); //최대 (6개의 정점*3개의 좌표)  (6 * 3 = 18 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

//세이더 프로그램 생성하기
void make_shaderProgram() {
    //버텍스랑 프레그먼트 세이더 만들기
    make_vertexShaders();
    make_fragmentShaders();

    // 세이더 프로그램 만들기
    shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    // 에러체크
    GLint success;
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // 세이더 삭제하기
    glDeleteShader(vertexShader); //-세이더객체를 세이더프로그램에 링크했음으로,세이더객체 자체는 삭제가능
    glDeleteShader(fragmentShader);

    // Shader program 사용
    glUseProgram(shaderProgramID);
}


void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");
    if (!vertexSource) {
        std::cerr << "Failed to load vertex shader." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 버텍스 세이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //  세이더코드를 세이더 객체에 넣기
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);

    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader compilation failed\n" << errorLog << std::endl;
        free(vertexSource); // Free allocated memory
        exit(EXIT_FAILURE);
    }
    free(vertexSource); // Free allocated memory
}


void make_fragmentShaders() {
    //프레그먼트 파일읽기
    fragmentSource = filetobuf("fragment.glsl");
    if (!fragmentSource) {
        std::cerr << "Failed to load fragment shader." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 프레그먼트 객체 만들기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 세이더 코드 세이더 객체에 넣기
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    // 프레그먼티 세이더 컴파일
    glCompileShader(fragmentShader);
    // 컴파일 에러 체크
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader compilation failed\n" << errorLog << std::endl;
        free(fragmentSource); // Free allocated memory
        exit(EXIT_FAILURE);
    }
    free(fragmentSource); // Free allocated memory
}




void Keyboard(unsigned char key, int x, int y) {
    switch (key){
    case's':{
        stop = !stop;
        glutTimerFunc(16, flyingRight, 0);
        glutTimerFunc(16, moving_Stick, 0);
        glutTimerFunc(16, moving_shape_onTheBucket, 0);
        glutTimerFunc(16, show_path, 0);
        break;
    }
    case 'l':  // Switch to line mode
        isWireframe = true;
        break;
    case 'f':  // Switch to fill mode
        isWireframe = false;
        break;
    case'.':
        speed += 0.005;
        if (speed >= 0.1) {
            speed = 0.1;
        }
        break;
    case',': {
        speed -= 0.005;
        if (speed <= 0.005) {
            speed = 0.005;
        }
        break;
    }
    case 'p':  // Switch to line mode
        path = !path;
        glutTimerFunc(30, show_path, 0);
        break;
    case '1': {
        path_mode = 1;
        break;
    }
    case '2': {
        path_mode = 2;
        break;
    }
    case '3': {
        path_mode = 3;
        break;
    }
    case'q': {
        exit(0);
    }
    case'r': {
        rotate = !rotate;
        glutTimerFunc(16, rotate_Shape, 0);
        break;
    }
    default:
        break;
    }
}


void Cleanup() {
    for (auto shape : shapes) {
        delete shape;
    }
    shapes.clear(); //백터에 있는 내용 초기화
}

void make_line(GLfloat x, GLfloat y) {
    Shape* newShape = new LineShape();
    newShape->position[0] = x;  // Left
    newShape->position[1] = y;
    newShape->color[0] = 0.5f;
    newShape->color[1] = 0.1f;
    newShape->color[2] = 0.3f;
    newShape->size = 0.1f;
    newShape->square = 2;
    shapes.push_back(newShape);
}

void make_triangle(GLfloat x, GLfloat y) {
    auto newShape = new TriangleShape();
    newShape->position[0] = x;  // Left
    newShape->position[1] = y;
    newShape->color[0] = 0.7f;
    newShape->color[1] = 0.3f;
    newShape->color[2] = 0.5f;
    newShape->size = 0.1f;
    newShape->square = 3;
    shapes.push_back(newShape);
}

void make_square(GLfloat x, GLfloat y) {
    auto newShape = new RectangleShape();
    newShape->position[0] = x;
    newShape->position[1] = y;
    newShape->color[0] = 0.2f;
    newShape->color[1] = 0.4f;
    newShape->color[2] = 0.8f;
    newShape->size = 0.1f;
    newShape->square = 4;
    shapes.push_back(newShape);
}
void make_pentagone(GLfloat x, GLfloat y) {
    Shape* newShape = new PentagonShape();
    newShape->position[0] = x;  // Left
    newShape->position[1] = y;
    newShape->color[0] = 0.5f;
    newShape->color[1] = 0.1f;
    newShape->color[2] = 0.3f;
    newShape->size = 0.1f;
    newShape->square = 5;
    newShape->trans[0] = 0;
    shapes.push_back(newShape);
}
void make_stick(GLfloat x, GLfloat y) {
    auto newShape = new StickShape();
    newShape->position[0] = x;
    newShape->position[1] = y;
    newShape->color[0] = 0.2f;
    newShape->color[1] = 0.4f;
    newShape->color[2] = 0.8f;
    newShape->size = 0.1f;
    newShape->square = 6;
    newShape->dir = 1;
    stick_shape.push_back(newShape);
}

void Timer(int value) {
    createFlyingShape(); 
    glutPostRedisplay();
    glutTimerFunc(1000, Timer, 0); 
}
void show_path(int value) {
    if (!stop) {
        return;
    }
    if (!path) {
        return;
    }

    // 점들을 생성
    for (auto shape : shapes) {
        if (shape->dir != 3) {
            auto newShape = new PointShape();
            newShape->position[0] = shape->position[0];
            newShape->position[1] = shape->position[1];
            newShape->color[0] = shape->color[0];
            newShape->color[1] = shape->color[1];
            newShape->color[2] = shape->color[2];
            newShape->size = shape->size;
            point_shapes.push_back(newShape);
        }
    }

    // 현재 시간 확인 및 오래된 점 제거
    auto currentTime = std::chrono::steady_clock::now();
    point_shapes.erase(
        std::remove_if(point_shapes.begin(), point_shapes.end(), [currentTime](PointShape* point) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - point->creationTime).count();
            if (duration >= 2) { // 2초 후 제거
                delete point;
                return true;
            }
            return false;
            }),
        point_shapes.end()
    );

    glutPostRedisplay();
    glutTimerFunc(6, show_path, 0);
}

void createFlyingShape() {
    if (!stop) {
        return;
    }
    std::uniform_real_distribution<GLfloat> randomY(0.0f, 0.8f);
    std::uniform_int_distribution<int> shapeType(0, 1);
    std::uniform_int_distribution<int> startSide(0, 1);
    std::uniform_real_distribution<GLfloat> randomColor(0.0f, 1.0f); // Random color

    GLfloat startY = randomY(generator);
    GLfloat startX = startSide(generator);
    if (path_mode == 3) {
        GLfloat velocityX = (startX == -1.0f) ? 0.01f : -0.01f;
    }
    else if (path_mode == 1) {
        startX = 0;
        GLfloat velocityX = 0.01f;
    }
    else if (path_mode == 2) {
        startX = 1;
        GLfloat velocityX = -0.01f;
    }
    GLfloat velocityY = (startY > 0.0f) ? -0.01f : 0.01f;

    Shape* newShape;

    if (shapeType(generator) == 0) {
        newShape = new RectangleShape();
        newShape->size = 0.1f;
    }
    else {
        newShape = new PentagonShape();
        newShape->size = 0.1f;
    }

    
    if (startX == 0) {
        newShape->dir = 1; // 오른
        newShape->position[0] = -1;
    }
    else {
        newShape->dir = 2; // 왼쪽
        newShape->position[0] = startX;
    }
    newShape->position[1] = startY;
    newShape->velocity[0] = 0.1;
    newShape->velocity[1] = 0.1;

    newShape->color[0] = randomColor(generator); // Red 
    newShape->color[1] = randomColor(generator); // Green 
    newShape->color[2] = randomColor(generator); // Blue

    shapes.push_back(newShape);
}

void flyingRight(int value) {
    if (!stop) {
        return;
    }
    for (auto it = shapes.begin(); it != shapes.end();) {
        auto shape = *it;
        auto stick = stick_shape[0];

        if (shape->position[1] <= stick->position[1] + 1.5* stick->size &&
            shape->position[1] >= stick->position[1] + stick->size &&
            shape->position[0] >= stick->position[0] - 3 * stick->size &&
            shape->position[0] <= stick->position[0] + 3 * stick->size) {

            shape->velocity[0] = shape->position[0] - stick->position[0];
            shape->velocity[1] = shape->position[1] - 3*stick->position[1]+shape->size;
            onTheBucket_shape.push_back(shape); // Add to onTheBucket_shape
            it = shapes.erase(it); // Safely remove from shapes vector
        }
        else {
            // Continue with normal movement logic
            if (shape->dir == 1) {
                shape->position[0] += speed;
                shape->position[1] += speed;
                if (shape->position[1] >= 1) {
                    shape->dir = 4;
                }
            }
            else if (shape->dir == 2) {
                shape->position[0] -= speed;
                shape->position[1] += speed;
                if (shape->position[1] >= 1) {
                    shape->dir = 5;
                }
            }
            else if (shape->dir == 3) {
                shape->position[1] -= speed;
            }
            else if (shape->dir == 4) {
                shape->position[0] += speed;
                shape->position[1] -= speed / 2;
            }
            else if (shape->dir == 5) {
                shape->position[0] -= speed;
                shape->position[1] -= speed / 2;
            }
            ++it; // Move to the next shape
        }
        
    }

    glutPostRedisplay();
    glutTimerFunc(16, flyingRight, 0);
}
void rotate_Shape(int value) {
    if (!rotate) {
        return;
    }
    for (auto shape : shapes) {
        shape->rotate += 10;
        if (shape->rotate == 360) {
            shape->rotate = 0;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, rotate_Shape, 0);
}
void moving_Stick(int value) {
    if (!stop) {
        return;
    }
    for (auto shape : stick_shape) {
        if (shape->dir == 1) {
            shape->position[0] += 0.01;
            if (shape->position[0] >= 0.95) {
                shape->dir = 2;
            } 
        }
        else if (shape->dir == 2) {
            shape->position[0] -= 0.01;
            if (shape->position[0] <= -0.95) {
                shape->dir = 1;
            }
        }
        
    }
    glutPostRedisplay();
    glutTimerFunc(16, moving_Stick, 0);
}
void moving_shape_onTheBucket(int value) {
    if (!stop) {
        return;
    }
    for (auto shape : onTheBucket_shape) {
        shape->position[0] = stick_shape[0]->position[0] + shape->velocity[0];
    }
    glutPostRedisplay();
    glutTimerFunc(16, moving_shape_onTheBucket, 0);
}

void MouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            getOpenGLMouseCoords(x, y, dragStartX, dragStartY);

        }
        else if (state == GLUT_UP) {
            getOpenGLMouseCoords(x, y, dragEndX, dragEndY);
            isDragging = false;
            checkForCuts();
        }
    }
}

void MouseMotion(int x, int y) {
    if (isDragging) {
        getOpenGLMouseCoords(x, y, dragEndX, dragEndY);
        glutPostRedisplay();
    }
}

void checkForCuts() {
    float centerX = (dragStartX + dragEndX) / 2.0f;
    float centerY = (dragStartY + dragEndY) / 2.0f;

    for (auto it = shapes.begin(); it != shapes.end(); ) {
        
        Shape* shape = *it;
        if (shape->size <= 0.005f) {
            delete shape;
            it = shapes.erase(it);
            glutPostRedisplay(); // Redraw scene with new shapes
            continue;
        }
        if (shape->position[0] > std::min(dragStartX, dragEndX) &&
            shape->position[0] < std::max(dragStartX, dragEndX) &&
            shape->position[1] > std::min(dragStartY, dragEndY) &&
            shape->position[1] < std::max(dragStartY, dragEndY))
        {
            

            TriangleShape* tri1 = new TriangleShape();

            tri1->position[0] = shape->position[0] - 0.05f; // Left triangle
            tri1->position[1] = shape->position[1] - 0.05f;
            tri1->dir = 3;
            tri1->triangle = 1;
            tri1->color[0] =shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];

            TriangleShape* tri2 = new TriangleShape();
            tri2->position[0] = shape->position[0] + 0.05f; // Right triangle
            tri2->position[1] = shape->position[1] + 0.05f;
            tri2->dir = 3;
            tri2->triangle = 2;
            tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];

            if (shape->broken==true) {
                if (shape->triangle == 1) {
                    tri1->size = shape->size ;
                    tri1->position[0] = shape->position[0] - (0.05f* tri1->size)-tri1->size; // Left triangle
                    tri1->position[1] = shape->position[1] + (0.05f * tri1->size);
                    tri1->dir = 3;
                    tri1->triangle = 5;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;

                    tri2->size = shape->size ;
                    tri2->position[0] = shape->position[0] + (0.05f * tri2->size); // Right triangle
                    tri2->position[1] = shape->position[1] - (0.05f * tri2->size) - tri2->size;
                    tri2->dir = 3;
                    tri2->triangle = 6;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 2) {
                    tri1->size = shape->size;
                    tri1->position[0] = shape->position[0] - (0.05f * tri1->size); // Left triangle
                    tri1->position[1] = shape->position[1] + (0.05f * tri1->size);
                    tri1->dir = 3;
                    tri1->triangle = 7;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;

                    tri2->size = shape->size;
                    tri2->position[0] = shape->position[0] + (0.05f * tri2->size)+ tri2->size; // Right triangle
                    tri2->position[1] = shape->position[1] - (0.05f * tri2->size);
                    tri2->dir = 3;
                    tri2->triangle = 8;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 3) {
                    tri1->size = shape->size;
                    tri1->position[0] = shape->position[0] + (0.05f * tri1->size); // Left triangle
                    tri1->position[1] = shape->position[1] + (0.05f * tri1->size);
                    tri1->dir = 3;
                    tri1->triangle = 7;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;

                    tri2->size = shape->size;
                    tri2->position[0] = shape->position[0] - (0.05f * tri1->size) - tri1->size; 
                    tri2->position[1] = shape->position[1] - (0.05f * tri1->size);
                    tri2->dir = 3;
                    tri2->triangle = 5;
                    tri2->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri2->broken = 1;
                   
                }
                else if (shape->triangle == 4) {
                    tri1->size = shape->size;
                    tri1->position[0] = shape->position[0] - (0.1f * tri1->size) ; // Left triangle
                    tri1->position[1] = shape->position[1] - (0.05f * tri1->size) - tri1->size;
                    tri1->dir = 3;
                    tri1->triangle = 6;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;

                    tri2->size = shape->size;
                    tri2->position[0] = shape->position[0] - (0.05f * tri2->size)+tri2->size; // Right triangle
                    tri2->position[1] = shape->position[1] - (0.05f * tri2->size);
                    tri2->dir = 3;
                    tri2->triangle = 8;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 5) {
                    tri1->size = shape->size/2;
                    tri1->position[0] = shape->position[0] ; // Left triangle
                    tri1->position[1] = shape->position[1] + (0.05f *2* tri1->size) + tri1->size;
                    tri1->dir = 3;
                    tri1->triangle = 1;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;

                    tri2->size = shape->size/2;
                    tri2->position[0] = shape->position[0] + (0.05f * tri2->size); // Right triangle
                    tri2->position[1] = shape->position[1] - (0.05f * tri2->size) - tri2->size;
                    tri2->dir = 3;
                    tri2->triangle = 3;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 6) {

                    tri1->size = shape->size / 2;
                    tri1->position[0] = shape->position[0] - tri1->size; // Left triangle
                    tri1->position[1] = shape->position[1] - (0.05f * 2 * tri1->size);
                    tri1->dir = 3;
                    tri1->triangle = 4;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;



                    tri2->size = shape->size / 2;
                    tri2->position[0] = shape->position[0] + (0.05f * tri2->size) + tri2->size; // Right triangle
                    tri2->position[1] = shape->position[1] - (0.05f * tri2->size) ;
                    tri2->dir = 3;
                    tri2->triangle = 1;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 7) {

                    tri1->size = shape->size / 2;
                    tri1->position[0] = shape->position[0] - (0.05f * 2 * tri1->size) - tri1->size ; // Left triangle
                    tri1->position[1] = shape->position[1] + (0.05f * 2 * tri1->size) + tri1->size;
                    tri1->dir = 3;
                    tri1->triangle = 2;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;



                    tri2->size = shape->size / 2;
                    tri2->position[0] = shape->position[0] + (0.05f * tri2->size) + tri2->size; // Right triangle
                    tri2->position[1] = shape->position[1] + (0.05f * tri2->size) + tri2->size;
                    tri2->dir = 3;
                    tri2->triangle = 3;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                }
                else if (shape->triangle == 8) {

                    tri1->size = shape->size / 2;
                    tri1->position[0] = shape->position[0] - (0.05f * 2 * tri1->size) - tri1->size; // Left triangle
                    tri1->position[1] = shape->position[1] - (0.05f * 2 * tri1->size) - tri1->size;
                    tri1->dir = 3;
                    tri1->triangle = 2;
                    tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                    tri1->broken = 1;



                    tri2->size = shape->size / 2;
                    tri2->position[0] = shape->position[0] - (0.05f * tri2->size)-tri2->size; // Right triangle
                    tri2->position[1] = shape->position[1] + (0.05f * tri2->size) + tri2->size;
                    tri2->dir = 3;
                    tri2->triangle = 4;
                    tri2->color[0] = shape->color[0]; tri2->color[1] = shape->color[1]; tri2->color[2] = shape->color[2];
                    tri2->broken = 1;
                    }
                
                
            }
            else {
                tri1->size = shape->size;
                tri1->position[0] = shape->position[0] - 0.05f; // Left triangle
                tri1->position[1] = shape->position[1] - 0.05f;
                tri1->dir = 3;
                tri1->triangle = 1;
                tri1->color[0] = shape->color[0]; tri1->color[1] = shape->color[1]; tri1->color[2] = shape->color[2];
                tri1->broken = 1;

                tri2->position[0] = shape->position[0] + 0.05f; // Right triangle
                tri2->position[1] = shape->position[1] + 0.05f;
                tri2->dir = 3;
                tri2->triangle = 2;
                tri2->size = shape->size;
                tri2->broken = 1;
            }
            broken_shapes.push_back(tri1);
            broken_shapes.push_back(tri2);
            delete shape;
            it = shapes.erase(it);
        }
        else {
            ++it;
        }
    }
    shapes.insert(shapes.end(), broken_shapes.begin(), broken_shapes.end());
    broken_shapes.clear();

    glutPostRedisplay(); // Redraw scene with new shapes
}