/***             GRAU B - Editor de Pista de Corrida              ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***               Editor 2D de Pista com B-Spline                ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/*** Fontes:
                * GRAU A de CGR
                * pseudos códigos dos slides de CGR
                * learnopengl.com
***/

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "TrackEditor.h"

using namespace std;
using namespace glm;

// Configurações da janela
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

// Editor de pista
TrackEditor trackEditor;

// Shaders e buffers
Shader* editorShader = nullptr;
GLuint pointsVAO = 0, pointsVBO = 0;
GLuint linesVAO = 0, linesVBO = 0;

// Estado do mouse
bool mousePressed = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Funções auxiliares
void setupBuffers();
void renderEditor();
vec2 screenToWorld(double xpos, double ypos);

int main() {
    cout << "=== Editor de Pista de Corrida - CGR ===" << endl;
    cout << endl;

    // Inicializa GLFW
    if (!glfwInit()) {
        cerr << "Falha ao inicializar GLFW" << endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria janela
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          "Editor de Pista de Corrida", NULL, NULL);
    if (!window) {
        cerr << "Falha ao criar janela GLFW" << endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Falha ao inicializar GLAD" << endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Carrega shaders
    editorShader = new Shader("shaders/editor.vs", "shaders/editor.fs");

    // Configura buffers
    setupBuffers();

    cout << "Sistema inicializado com sucesso!" << endl;
    cout << endl;
    cout << "=== CONTROLES ===" << endl;
    cout << "  Clique Esquerdo: Adicionar ponto de controle" << endl;
    cout << "  Clique Direito: Remover ultimo ponto" << endl;
    cout << "  C: Limpar todos os pontos" << endl;
    cout << "  G: Gerar curva B-Spline" << endl;
    cout << "  T: Gerar pista (curvas interna/externa)" << endl;
    cout << "  M: Triangularizar malha" << endl;
    cout << "  E: Exportar para OBJ" << endl;
    cout << "  ESC: Sair" << endl;
    cout << endl;

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        renderEditor();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpeza
    if (editorShader) delete editorShader;
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteBuffers(1, &pointsVBO);
    glDeleteVertexArrays(1, &linesVAO);
    glDeleteBuffers(1, &linesVBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            vec2 worldPos = screenToWorld(xpos, ypos);
            trackEditor.addControlPoint(worldPos);
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            trackEditor.removeLastControlPoint();
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
                
            case GLFW_KEY_C:
                trackEditor.clearControlPoints();
                break;
                
            case GLFW_KEY_G:
                if (trackEditor.getControlPoints().size() >= 4) {
                    trackEditor.generateBSpline(20);  // 20 pontos por segmento
                } else {
                    cout << "Necessario pelo menos 4 pontos de controle!" << endl;
                }
                break;
                
            case GLFW_KEY_T:
                if (trackEditor.getBSplinePoints().size() > 0) {
                    trackEditor.calculateInnerOuterCurves(0.4f);  // Largura da pista - era 0.4f
                } else {
                    cout << "Gere a curva B-Spline primeiro (tecla G)!" << endl;
                }
                break;
                
            case GLFW_KEY_M:
                if (trackEditor.getInnerCurve().size() > 0) {
                    trackEditor.triangulateMesh();
                    trackEditor.calculateNormals();
                } else {
                    cout << "Gere as curvas da pista primeiro (tecla T)!" << endl;
                }
                break;
                
            case GLFW_KEY_E:
                if (trackEditor.getVertices().size() > 0) {
                    trackEditor.exportToOBJ("models/track.obj");
                    trackEditor.exportAnimationCurve("models/track_curve.txt");
                    cout << "Pista exportada com sucesso!" << endl;
                } else {
                    cout << "Gere a malha primeiro (tecla M)!" << endl;
                }
                break;
        }
    }
}

vec2 screenToWorld(double xpos, double ypos) {
    // Converte coordenadas de tela para mundo (NDC ajustado para aspect ratio)
    float x = (xpos / SCREEN_WIDTH) * 2.0f - 1.0f;
    float y = 1.0f - (ypos / SCREEN_HEIGHT) * 2.0f;
    
    // Ajusta para aspect ratio
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    x *= aspect;
    
    return vec2(x * 5.0f, y * 5.0f);  // Escala o mundo para ±5 unidades
}

void setupBuffers() {
    // VAO e VBO para pontos
    glGenVertexArrays(1, &pointsVAO);
    glGenBuffers(1, &pointsVBO);
    
    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    
    // Posição
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // VAO e VBO para linhas
    glGenVertexArrays(1, &linesVAO);
    glGenBuffers(1, &linesVBO);
    
    glBindVertexArray(linesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void renderEditor() {
    editorShader->use();
    
    // Matriz de projeção ortográfica
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    mat4 projection = ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, -1.0f, 1.0f);
    editorShader->setMat4("projection", projection);
    
    // Renderiza pontos de controle (vermelho)
    const auto& controlPoints = trackEditor.getControlPoints();
    if (!controlPoints.empty()) {
        vector<float> pointData;
        for (const auto& p : controlPoints) {
            pointData.push_back(p.x);
            pointData.push_back(p.y);
            pointData.push_back(1.0f);  // Vermelho
            pointData.push_back(0.0f);
            pointData.push_back(0.0f);
        }
        
        glBindVertexArray(pointsVAO);
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(float), 
                     pointData.data(), GL_DYNAMIC_DRAW);
        
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, controlPoints.size());
    }
    
    // Renderiza curva B-Spline (verde)
    const auto& bSplinePoints = trackEditor.getBSplinePoints();
    if (!bSplinePoints.empty()) {
        vector<float> lineData;
        for (const auto& p : bSplinePoints) {
            lineData.push_back(p.x);
            lineData.push_back(p.y);
            lineData.push_back(0.0f);  // Verde
            lineData.push_back(1.0f);
            lineData.push_back(0.0f);
        }
        
        glBindVertexArray(linesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
        glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), 
                     lineData.data(), GL_DYNAMIC_DRAW);
        
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, bSplinePoints.size());
    }
    
    // Renderiza curvas interna e externa (azul e amarelo)
    const auto& innerCurve = trackEditor.getInnerCurve();
    const auto& outerCurve = trackEditor.getOuterCurve();
    
    if (!innerCurve.empty()) {
        vector<float> innerData;
        for (const auto& p : innerCurve) {
            innerData.push_back(p.x);
            innerData.push_back(p.y);
            innerData.push_back(0.0f);  // Azul
            innerData.push_back(0.0f);
            innerData.push_back(1.0f);
        }
        
        glBindVertexArray(linesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
        glBufferData(GL_ARRAY_BUFFER, innerData.size() * sizeof(float), 
                     innerData.data(), GL_DYNAMIC_DRAW);
        
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, innerCurve.size());
    }
    
    if (!outerCurve.empty()) {
        vector<float> outerData;
        for (const auto& p : outerCurve) {
            outerData.push_back(p.x);
            outerData.push_back(p.y);
            outerData.push_back(1.0f);  // Amarelo
            outerData.push_back(1.0f);
            outerData.push_back(0.0f);
        }
        
        glBindVertexArray(linesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
        glBufferData(GL_ARRAY_BUFFER, outerData.size() * sizeof(float), 
                     outerData.data(), GL_DYNAMIC_DRAW);
        
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, outerCurve.size());
    }
    
    glBindVertexArray(0);
}