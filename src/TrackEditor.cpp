#include "TrackEditor.h"
#include <fstream>
#include <cmath>
#include <iostream>

//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif

// construtor
TrackEditor::TrackEditor() { }

// destrutor
TrackEditor::~TrackEditor() { }


void TrackEditor::adicionarPontoDeControle(vec2 point) {
    pontosDeControle.push_back(point);
    cout << "Ponto de controle adicionado: (" << point.x << ", " << point.y << ")" << endl;
}

void TrackEditor::removerUltimoPontoDeControle() {
    if (!pontosDeControle.empty()) {
        pontosDeControle.pop_back();
        cout << "Ultimo ponto de controle removido" << endl;
    }
}

void TrackEditor::limparPontosDeControle() {
    pontosDeControle.clear();
    bSplinePoints.clear();
    innerCurve.clear();
    outerCurve.clear();
    vertices.clear();
    triangles.clear();
    cout << "Todos os pontos limpos" << endl;
}

// Função de base B-Spline (base de Cox-de Boor)
float TrackEditor::baseBSpline(int i, int k, float t) {
    // Para B-Spline uniforme de grau 3 (cúbica)
    // Implementação simplificada para curva fechada
    
    if (k == 0) {
        return (t >= i && t < i + 1) ? 1.0f : 0.0f;
    }
    
    float c1 = 0.0f, c2 = 0.0f;
    
    float denom1 = k;
    if (denom1 != 0.0f) {
        c1 = (t - i) / denom1;
    }
    
    float denom2 = k;
    if (denom2 != 0.0f) {
        c2 = (i + k + 1 - t) / denom2;
    }
    
    return c1 * baseBSpline(i, k - 1, t) + c2 * baseBSpline(i + 1, k - 1, t);
}

// Avalia a curva B-Spline no parâmetro t
vec2 TrackEditor::avaliarBSpline(float t, int i) {
    int n = pontosDeControle.size();
    if (n < 4) return vec2(0, 0);  // Precisa de pelo menos 4 pontos
    
    // B-Spline cúbica uniforme
    float u = t - floor(t);
    
    // Matriz de base B-Spline cúbica
    float b0 = (1 - u) * (1 - u) * (1 - u) / 6.0f;
    float b1 = (3 * u * u * u - 6 * u * u + 4) / 6.0f;
    float b2 = (-3 * u * u * u + 3 * u * u + 3 * u + 1) / 6.0f;
    float b3 = u * u * u / 6.0f;
    
    // Índices com wraparound para curva fechada
    int i0 = i % n;
    int i1 = (i + 1) % n;
    int i2 = (i + 2) % n;
    int i3 = (i + 3) % n;
    
    vec2 point = b0 * pontosDeControle[i0] +
                 b1 * pontosDeControle[i1] +
                 b2 * pontosDeControle[i2] +
                 b3 * pontosDeControle[i3];
    
    return point;
}

void TrackEditor::gerarBSpline(int resolution) {
    bSplinePoints.clear();
    
    int n = pontosDeControle.size();
    if (n < 4) {
        cout << "Necessario pelo menos 4 pontos de controle para gerar B-Spline" << endl;
        return;
    }
    
    // Gera pontos ao longo da curva B-Spline fechada
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < resolution; j++) {
            float t = float(j) / float(resolution);
            vec2 point = avaliarBSpline(i + t, i);
            bSplinePoints.push_back(point);
        }
    }
    
    cout << "B-Spline gerada com " << bSplinePoints.size() << " pontos" << endl;
}

// Calcula o vetor perpendicular normalizado
vec2 TrackEditor::calcularPerpendicular(vec2 p1, vec2 p2) {
    vec2 tangent = p2 - p1;
    
    // Normaliza o vetor tangente
    float length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
    if (length > 0.0001f) {
        tangent /= length;
    }
    
    // Perpendicular (rotação 90° no sentido anti-horário)
    vec2 perpendicular(-tangent.y, tangent.x);
    
    return perpendicular;
}

// Calcula o ângulo entre dois vetores
float TrackEditor::calcularAngulo(vec2 v1, vec2 v2) {
    // Normaliza os vetores
    float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
    float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);
    
    if (len1 < 0.0001f || len2 < 0.0001f) return 0.0f;
    
    v1 /= len1;
    v2 /= len2;
    
    // Produto escalar para o cosseno do ângulo
    float dot = v1.x * v2.x + v1.y * v2.y;
    
    // Limita o valor para evitar erros numéricos
    dot = glm::clamp(dot, -1.0f, 1.0f);
    
    return acos(dot);
}

// Calcula as curvas interna e externa da pista
void TrackEditor::calcularCurvasInternaExterna(float trackWidth) {
    innerCurve.clear();
    outerCurve.clear();
    
    int numPoints = bSplinePoints.size();
    if (numPoints < 2) {
        cout << "Necessario gerar B-Spline antes de calcular curvas interna/externa" << endl;
        return;
    }
    
    // Para cada ponto da B-Spline
    for (int i = 0; i < numPoints; i++) {
        vec2 current = bSplinePoints[i];
        vec2 next = bSplinePoints[(i + 1) % numPoints];
        
        // Calcula vetor perpendicular
        vec2 perpendicular = calcularPerpendicular(current, next);
        
        // Calcula a largura da pista (metade para cada lado)
        float halfWidth = trackWidth / 2.0f;
        
        // Calcula pontos interno e externo
        vec2 innerPoint = current - perpendicular * halfWidth;
        vec2 outerPoint = current + perpendicular * halfWidth;
        
        innerCurve.push_back(innerPoint);
        outerCurve.push_back(outerPoint);
    }
    
    cout << "Curvas interna e externa calculadas com " << innerCurve.size() << " pontos cada" << endl;
}

// Triangulariza a malha da pista
void TrackEditor::triangularizarMalha() {
    vertices.clear();
    triangles.clear();
    
    int numPoints = innerCurve.size();
    if (numPoints < 2 || outerCurve.size() != numPoints) {
        cout << "Necessario calcular curvas interna/externa antes de triangularizar" << endl;
        return;
    }
    
    // Cria vértices da malha
    for (int i = 0; i < numPoints; i++) {
        Vertex vInner, vOuter;
        
        // Posição (Z será 0 para o editor 2D)
        vInner.position = vec3(innerCurve[i].x, innerCurve[i].y, 0.0f);
        vOuter.position = vec3(outerCurve[i].x, outerCurve[i].y, 0.0f);
        
        // Coordenadas de textura (fixas nos cantos para cada retângulo)
        // A textura se repete ao longo da pista
        float v = float(i) / float(numPoints);
        
        vInner.texCoord = vec2(0.0f, v);
        vOuter.texCoord = vec2(1.0f, v);
        
        // Normal será calculada depois
        vInner.normal = vec3(0, 0, 1);
        vOuter.normal = vec3(0, 0, 1);
        
        vertices.push_back(vInner);
        vertices.push_back(vOuter);
    }
    
    // Cria triângulos conectando os vértices
    for (int i = 0; i < numPoints; i++) {
        int current = i * 2;
        int next = ((i + 1) % numPoints) * 2;
        
        // Primeiro triângulo do retângulo
        Triangle t1;
        t1.v1 = current;      // Inner atual
        t1.v2 = current + 1;  // Outer atual
        t1.v3 = next;         // Inner próximo
        triangles.push_back(t1);
        
        // Segundo triângulo do retângulo
        Triangle t2;
        t2.v1 = current + 1;  // Outer atual
        t2.v2 = next + 1;     // Outer próximo
        t2.v3 = next;         // Inner próximo
        triangles.push_back(t2);
    }
    
    cout << "Malha triangularizada com " << vertices.size() << " vertices e " 
         << triangles.size() << " triangulos" << endl;
}

// Calcula as normais das faces
void TrackEditor::calcularNormais() {
    if (vertices.empty() || triangles.empty()) {
        cout << "Necessario triangularizar antes de calcular normais" << endl;
        return;
    }
    
    // Inicializa todas as normais como zero
    for (auto& v : vertices) {
        v.normal = vec3(0, 0, 0);
    }
    
    // Para cada face, calcula a normal e acumula nos vértices
    for (const auto& tri : triangles) {
        vec3 v0 = vertices[tri.v1].position;
        vec3 v1 = vertices[tri.v2].position;
        vec3 v2 = vertices[tri.v3].position;
        
        // Vetores das arestas
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        
        // Normal da face (produto vetorial)
        vec3 faceNormal = cross(edge1, edge2);
        
        // Acumula a normal nos três vértices
        vertices[tri.v1].normal += faceNormal;
        vertices[tri.v2].normal += faceNormal;
        vertices[tri.v3].normal += faceNormal;
    }
    
    // Normaliza todas as normais
    for (auto& v : vertices) {
        float length = sqrt(v.normal.x * v.normal.x + 
                          v.normal.y * v.normal.y + 
                          v.normal.z * v.normal.z);
        if (length > 0.0001f) {
            v.normal /= length;
        }
    }
    
    cout << "Normais calculadas" << endl;
}

// Exporta a pista para arquivo OBJ
bool TrackEditor::exportarOBJ(const string& outputPath) {
    if (vertices.empty() || triangles.empty()) {
        cout << "Nenhuma malha para exportar" << endl;
        return false; }

    string filenameObj = outputPath + "track.obj";
    string filenameMtl = outputPath + "track.mtl";    
    
    ofstream file(filenameObj); // stream para o arquivo OBJ

    if (!file.is_open()) {
        cout << "Erro ao abrir arquivo: " << filenameObj << endl;
        return false; }
    
    file << "# Editor de Pista de Corrida" << endl;
    file << "# Quantidade de Vertices: " << vertices.size() << endl;
    file << "# Quantidade de Faces/Triangulos: " << triangles.size() << endl;
    file << endl;
    
    file << "mtllib " << filenameMtl << endl;
    file << "usemtl track_material" << endl;
    file << endl;
    
    // Exporta vértices (converte Y -> Z para o visualizador 3D)
    for (const auto& v : vertices) {
        file << "v " << v.position.x << " " << v.position.z << " " << v.position.y << endl;
    }

    file << endl;
    
    // Exporta coordenadas de textura
    for (const auto& v : vertices) {
        file << "vt " << v.texCoord.x << " " << v.texCoord.y << endl;
    }
    file << endl;
    
    // Exporta normais (converte Y -> Z para o visualizador 3D)
    for (const auto& v : vertices) {
        file << "vn " << v.normal.x << " " << v.normal.z << " " << v.normal.y << endl;
    }
    file << endl;
    
    // Exporta faces (OBJ usa índices começando em 1)
    for (const auto& tri : triangles) {
        file << "f " 
             << (tri.v1 + 1) << "/" << (tri.v1 + 1) << "/" << (tri.v1 + 1) << " "
             << (tri.v2 + 1) << "/" << (tri.v2 + 1) << "/" << (tri.v2 + 1) << " "
             << (tri.v3 + 1) << "/" << (tri.v3 + 1) << "/" << (tri.v3 + 1) << endl;
    }
    
    file.close();
    cout << "Pista exportada para: " << filenameObj << endl;
    
    // Cria arquivo MTL
    ofstream mtlFile(filenameMtl);
    if (mtlFile.is_open()) {
        mtlFile << "# Track Material" << endl;
        mtlFile << "newmtl track_material" << endl;
        mtlFile << "Ka 1.0 1.0 1.0" << endl;
        mtlFile << "Kd 0.8 0.8 0.8" << endl;
        mtlFile << "Ks 0.3 0.3 0.3" << endl;
        mtlFile << "Ns 32" << endl;
        mtlFile << "map_Kd textures/asfalto.jpg" << endl;
        mtlFile.close();
        cout << "Material exportado para: " << filenameMtl << endl;
    }
    
    return true;
}

// Exporta a curva b-spline para arquivo de animação
bool TrackEditor::exportarCurvaAnimacao(const string& outputPath) {

    if (bSplinePoints.empty()) {
        cout << "Nenhuma curva para exportar" << endl;
        return false; }
    
    string filenameTxt = outputPath + "track_curve.txt";    

    ofstream file(filenameTxt); // stream para o arquivo de curva

    if (!file.is_open()) {
        cout << "Erro ao abrir arquivo: " << filenameTxt << endl;
        return false; }
    
    file << "# Curva de Animacao b-Spline" << endl;
    file << "# Total de pontos: " << bSplinePoints.size() << endl;
    file << endl;
    
    // Exporta pontos da curva B-Spline (Y -> Z para o visualizador)
    for (const auto& point : bSplinePoints) { file << point.x << " " << 0.0f << " " << point.y << endl; }
    
    file.close();

    cout << "Curva de animacao exportada para: " << filenameTxt << endl;
    
    return true;
}
