#ifndef TRACKEDITOR_H
#define TRACKEDITOR_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

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
    void addControlPoint(vec2 point);
    
    // Remove o último ponto de controle
    void removeLastControlPoint();
    
    // Limpa todos os pontos
    void clearControlPoints();
    
    // Gera a curva B-Spline a partir dos pontos de controle
    void generateBSpline(int resolution);
    
    // Calcula as curvas interna e externa
    void calculateInnerOuterCurves(float trackWidth);
    
    // Triangulariza a malha da pista
    void triangulateMesh();
    
    // Calcula as normais das faces
    void calculateNormals();
    
    // Exporta a pista para arquivo OBJ
    bool exportToOBJ(const string& filename);
    
    // Exporta a curva para arquivo de animação
    bool exportAnimationCurve(const string& filename);
    
    // Getters para renderização
    const vector<vec2>& getControlPoints() const { return controlPoints; }
    const vector<vec2>& getBSplinePoints() const { return bSplinePoints; }
    const vector<vec2>& getInnerCurve() const { return innerCurve; }
    const vector<vec2>& getOuterCurve() const { return outerCurve; }
    const vector<Vertex>& getVertices() const { return vertices; }
    const vector<Triangle>& getTriangles() const { return triangles; }
    
private:
    vector<vec2> controlPoints;     // Pontos de controle definidos pelo usuário
    vector<vec2> bSplinePoints;     // Pontos da curva B-Spline
    vector<vec2> innerCurve;        // Pontos da curva interna
    vector<vec2> outerCurve;        // Pontos da curva externa
    vector<Vertex> vertices;        // Vértices da malha triangularizada
    vector<Triangle> triangles;     // Triângulos da malha
    
    // Funções auxiliares
    vec2 evaluateBSpline(float t, int i);
    float bSplineBasis(int i, int k, float t);
    vec2 calculatePerpendicular(vec2 p1, vec2 p2);
    float calculateAngle(vec2 v1, vec2 v2);
};

#endif
