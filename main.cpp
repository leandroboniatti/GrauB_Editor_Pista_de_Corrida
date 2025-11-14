/***             GRAU B - Editor de Pista de Corrida              ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***               Editor 2D de Pista com B-Spline                ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/*** Fontes:
                * GRAU A de CGR e GRAU B de Fundamentos de CGR
                * pseudos códigos dos slides de CGR
                * learnopengl.com
***/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>
#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL - Incluir antes de outros que requerem OpenGL (como GLFW)
#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows e gerenciar entrada de teclado/mouse
#include <glm/glm.hpp>	// biblioteca de operações matriciais
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TrackEditor.h"

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca

// Configurações de exportação de arquivos
const string OUTPUT_PATH = "../GrauB_Vizualizador_3D/models/";

// Configurações da janela
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

// Editor de pista
TrackEditor trackEditor;
const int pontosPorSegmento = 100;
const float larguraDaPista = 1.0f;

// Programa de shader e buffers
GLuint shaderProgram = 0;
GLuint pointsVAO = 0, pointsVBO = 0;
GLuint linesVAO = 0, linesVBO = 0;

// Estado do mouse
bool mousePressed = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;

// Funções de Callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Funções auxiliares
void setupBuffers();
void renderEditor();
vec2 screenToWorld(double xpos, double ypos);
GLuint createShaderProgram();


int main() {

    cout << "    Editor de Pista de Corrida - CGR    " << endl;
    cout << endl;

    // inicializa a GLFW (janela, contexto, callbacks, etc)
    if (!glfwInit()) {   // Inicialização da GLFW
        cerr << "Falha ao inicializar GLFW" << endl;
        return EXIT_FAILURE; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // Informa a versão do OpenGL a partir da qual o código funcionará
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // Exemplo para versão 3.3 - adaptar para a versão suportada por sua placa
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // Permite redimensionar a janela

    // Cria janela GLFW
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          "GRAU B - Editor de Pista de Corrida", NULL, NULL);
    if (!window) {
        cerr << "Falha ao criar janela GLFW" << endl;
        glfwTerminate();
        return EXIT_FAILURE; }

    glfwMakeContextCurrent(window); // Define a janela atual como contexto de renderização

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Ajusta a viewport quando a janela é redimensionada
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Captura eventos botão do mouse
    glfwSetKeyCallback(window, key_callback); // Captura eventos de teclado

    // GLAD: Inicializa e carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Falha ao inicializar GLAD" << endl;
        return EXIT_FAILURE;}

    // Definindo as dimensões da viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Configura cor de fundo
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Cria programa de shader
    shaderProgram = createShaderProgram();

    // Configura buffers
    setupBuffers();

    // Instruções
    cout << "  CONTROLES  " << endl;
    cout << "  Clique Esquerdo: Adicionar ponto de controle" << endl;
    cout << "  Clique Direito: Remover ultimo ponto" << endl;
    cout << "  C: Limpar todos os pontos" << endl;
    cout << "  G: Gerar curva B-Spline" << endl;
    cout << "  T: Gerar pista (curvas interna/externa)" << endl;
    cout << "  E: Triangularizar e Exportar para OBJ" << endl;
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
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteBuffers(1, &pointsVBO);
    glDeleteVertexArrays(1, &linesVAO);
    glDeleteBuffers(1, &linesVBO);

    glfwTerminate();
    return 0;
}


// Captura de eventos e callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }


// Captura de clique do mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            vec2 worldPos = screenToWorld(xpos, ypos);
            trackEditor.adicionarPontoDeControle(worldPos); // Adiciona ponto de controle
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            trackEditor.removerUltimoPontoDeControle(); // Remove último ponto de controle
        }
    }
}


// Captura de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true); // Fecha a janela
                break;
                
            case GLFW_KEY_C:    // Limpa todos os pontos de controle
                trackEditor.limparPontosDeControle();   
                break;
                
            case GLFW_KEY_G:    // Gera curva B-Spline
                if (trackEditor.obterPontosDeControle().size() >= 4) {  // Mínimo 4 pontos para B-Spline
                    trackEditor.gerarBSpline(pontosPorSegmento);        // quantidade de pontos por segmento
                } else {
                    cout << "Necessário pelo menos 4 pontos de controle!" << endl;
                }
                break;
                
            case GLFW_KEY_T:    // Calcula curvas interna e externa
                if (trackEditor.obterPontosBSpline().size() > 0) {  // Verifica se a curva B-Spline foi gerada
                    trackEditor.calcularCurvasInternaExterna(larguraDaPista);   
                } else {
                    cout << "Gere a curva B-Spline primeiro (tecla G)!" << endl;
                }
                break;
                
            case GLFW_KEY_E:    // Triangulariza, calcula normais e exporta arquivos
                if (trackEditor.obterCurvaInterna().size() > 0 && trackEditor.obterCurvaExterna().size() > 0) { // Verifica se as curvas foram geradas
                    trackEditor.triangularizarMalha();
                    trackEditor.calcularNormais();      
                    trackEditor.exportarOBJ(OUTPUT_PATH);
                    trackEditor.exportarCurvaAnimacao(OUTPUT_PATH);
                    cout << "Pista exportada com sucesso!" << endl;
                } else {
                    cout << "Gere as curvas da pista primeiro (tecla T)!" << endl;
                }
                break;
        }
    }
}


// Converte coordenadas de tela para a visao do mundo (world coordinates)
vec2 screenToWorld(double xpos, double ypos) {
    // Converte coordenadas de tela para mundo (NDC ajustado para aspect ratio) - NDC (Normalized Device Coordinates)
    float x = (xpos / SCREEN_WIDTH) * 2.0f - 1.0f;
    float y = 1.0f - (ypos / SCREEN_HEIGHT) * 2.0f;
    
    // Ajusta para aspect ratio
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    x *= aspect;
    
    return vec2(x * 5.0f, y * 5.0f);  // Escala o mundo para ±5 unidades
}


// Configura buffers de OpenGL
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


// Renderiza o editor de pista
void renderEditor() {
    glUseProgram(shaderProgram);
    
    // Matriz de projeção ortográfica
    float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    mat4 projection = ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, -1.0f, 1.0f);
    
    // Define uniform da matriz de projeção
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
    
    // Renderiza pontos de controle (vermelho)
    const auto& pontosDeControle = trackEditor.obterPontosDeControle();
    if (!pontosDeControle.empty()) {
        vector<float> pointData;
        for (const auto& p : pontosDeControle) {
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
        glDrawArrays(GL_POINTS, 0, pontosDeControle.size());
    }
    
    // Renderiza curva B-Spline (verde)
    const auto& bSplinePoints = trackEditor.obterPontosBSpline();
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
    const auto& innerCurve = trackEditor.obterCurvaInterna();
    const auto& outerCurve = trackEditor.obterCurvaExterna();
    
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


// Função para criar o programa de shader
GLuint createShaderProgram() {
    // Código fonte do vertex shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec3 aColor;
        
        out vec3 vertexColor;
        
        uniform mat4 projection;
        
        void main()
        {
            gl_Position = projection * vec4(aPos, 0.0, 1.0);
            vertexColor = aColor;
        }
    )";
    
    // Código fonte do fragment shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        
        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
    
    // Compila vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // Verifica erros de compilação do vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "ERRO::VERTEX_SHADER::COMPILACAO_FALHOU\n" << infoLog << endl;
    }
    
    // Compila fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Verifica erros de compilação do fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "ERRO::FRAGMENT_SHADER::COMPILACAO_FALHOU\n" << infoLog << endl;
    }
    
    // Cria programa de shader e linka os shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Verifica erros de linkagem
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        cerr << "ERRO::PROGRAM::LINKAGEM_FALHOU\n" << infoLog << endl;
    }
    
    // Deleta os shaders (já foram linkados ao programa)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}