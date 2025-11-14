#ifndef TRACKEDITOR_H
#define TRACKEDITOR_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

// Constante para o arquivo MTL
const string TRACK_MTL_FILENAME = "../GrauB_Vizualizador_3D/models/track.mtl";

// Estrutura para armazenar um vértice completo da malha
struct Vertex {
    vec3 position;  // Posição do vértice
    vec3 normal;    // Normal do vértice
    vec2 texCoord;  // Coordenada de textura
};

// Estrutura para armazenar uma face triangular
struct Triangle {
    int v1, v2, v3;  // Índices dos vértices
};

class TrackEditor {
public:
    TrackEditor();
    ~TrackEditor();

    // Adiciona um ponto de controle
    void adicionarPontoDeControle(vec2 point);
    
    // Remove o último ponto de controle
    void removerUltimoPontoDeControle();
    
    // Limpa todos os pontos
    void limparPontosDeControle();
    
    // Gera a curva B-Spline a partir dos pontos de controle
    void gerarBSpline(int resolution);
    
    // Calcula as curvas interna e externa
    void calcularCurvasInternaExterna(float trackWidth);
    
    // Triangulariza a malha da pista
    void triangularizarMalha();
    
    // Calcula as normais das faces
    void calcularNormais();
    
    // Exporta a pista para arquivo OBJ
    bool exportarOBJ(const string& filename);
    
    // Exporta a curva para arquivo de animação
    bool exportarCurvaAnimacao(const string& filename);
    
    // Getters para renderização
    const vector<vec2>& obterPontosDeControle() const { return pontosDeControle; }
    const vector<vec2>& obterPontosBSpline() const { return bSplinePoints; }
    const vector<vec2>& obterCurvaInterna() const { return innerCurve; }
    const vector<vec2>& obterCurvaExterna() const { return outerCurve; }
    const vector<Vertex>& obterVertices() const { return vertices; }
    const vector<Triangle>& obterTriangulos() const { return triangles; }
    
private:
    vector<vec2> pontosDeControle;     // Pontos de controle definidos pelo usuário
    vector<vec2> bSplinePoints;     // Pontos da curva B-Spline
    vector<vec2> innerCurve;        // Pontos da curva interna
    vector<vec2> outerCurve;        // Pontos da curva externa
    vector<Vertex> vertices;        // Vértices da malha triangularizada
    vector<Triangle> triangles;     // Triângulos da malha
    
    // Funções auxiliares
    vec2 avaliarBSpline(float t, int i);
    float baseBSpline(int i, int k, float t);
    vec2 calcularPerpendicular(vec2 p1, vec2 p2);
    float calcularAngulo(vec2 v1, vec2 v2);
};

#endif
