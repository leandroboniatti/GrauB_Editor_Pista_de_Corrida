# Editor de Pista de Corrida - Grau B CGR

## Descrição
Editor 2D de pista de corrida utilizando curvas B-Spline. O editor permite criar pistas de corrida fechadas através de pontos de controle definidos pelo usuário.

## Estrutura do Projeto

### Arquivos Principais
- **main.cpp**: Loop principal e lógica do editor 2D
- **include/TrackEditor.h**: Cabeçalho da classe do editor de pista
- **src/TrackEditor.cpp**: Implementação do editor (B-Spline, curvas, exportação)
- **include/Shader.h**: Classe auxiliar para gerenciar shaders
- **src/Shader.cpp**: Implementação da classe Shader

### Shaders
- **shaders/editor.vs**: Vertex shader para renderização 2D
- **shaders/editor.fs**: Fragment shader para renderização 2D

### Saídas
- **models/track.obj**: Arquivo OBJ da pista gerada
- **models/track.mtl**: Arquivo de material da pista
- **models/track_curve.txt**: Arquivo com pontos da curva B-Spline para animação

### Dependências (mantidas)
- GLAD: Carregamento de funções OpenGL
- GLFW: Janela e entrada de usuário
- GLM: Matemática vetorial e matricial

## Como Usar o Editor

### 1. Compilar
Execute a task "Build Editor de Pista" no VS Code ou:
```bash
g++ -std=c++17 -g -Iinclude -IDependencies/GLAD/include -IDependencies/GLFW/include -IDependencies/glm main.cpp src/Shader.cpp src/TrackEditor.cpp Dependencies/GLAD/src/glad.c -LDependencies/GLFW/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -o track_editor.exe
```

### 2. Executar
```bash
./track_editor.exe
```

### 3. Controles

#### Criar Pontos de Controle
- **Clique Esquerdo**: Adiciona um ponto de controle na posição do mouse
- **Clique Direito**: Remove o último ponto de controle adicionado
- **C**: Limpa todos os pontos de controle

#### Gerar Pista
Siga esta sequência para criar uma pista:

1. **Adicione pelo menos 4 pontos de controle** clicando na tela
2. **Pressione G**: Gera a curva B-Spline (linha verde)
3. **Pressione T**: Calcula as curvas interna (azul) e externa (amarela) da pista
4. **Pressione M**: Triangulariza a malha da pista
5. **Pressione E**: Exporta a pista para arquivo OBJ

#### Outros Controles
- **ESC**: Sair do editor

### 4. Cores no Editor
- **Vermelho**: Pontos de controle definidos pelo usuário
- **Verde**: Curva B-Spline gerada
- **Azul**: Curva interna da pista
- **Amarelo**: Curva externa da pista

## Algoritmo B-Spline

O editor utiliza curva B-Spline cúbica uniforme para gerar um circuito fechado suave a partir dos pontos de controle.

### Características:
- Curva fechada (loop)
- Grau 3 (cúbica)
- Passa suavemente por todos os segmentos
- Resolução configurável (20 pontos por segmento entre pontos de controle)

## Geração da Pista

### Curvas Interna e Externa
Seguindo o esquema da Figura 2:
1. Para cada ponto da B-Spline, calcula-se o vetor tangente
2. Calcula-se o vetor perpendicular (rotação de 90°)
3. Desloca-se o ponto pela metade da largura da pista em cada direção
4. Largura padrão: 0.4 unidades

### Triangularização
- Conecta pontos das curvas interna e externa formando retângulos
- Cada retângulo é dividido em 2 triângulos
- Cria uma malha fechada e contínua

### Normais
- Calculadas usando produto vetorial entre arestas dos triângulos
- Normalizadas para iluminação correta no visualizador 3D

### Coordenadas de Textura
- Mapeamento fixo: (0,0), (1,0), (0,1), (1,1) para cada retângulo
- Textura repete ao longo da pista
- Textura recomendada: asfalto

## Exportação

### Arquivo OBJ (models/track.obj)
- Vértices convertidos de Y↔Z (editor 2D usa XY, visualizador 3D usa XZ como chão)
- Inclui posições, coordenadas de textura e normais
- Formato compatível com o visualizador 3D existente

### Arquivo de Animação (models/track_curve.txt)
- Contém todos os pontos da curva B-Spline
- Um ponto por linha: X Y Z
- Pode ser usado para movimentar objetos ao longo da pista

### Arquivo MTL (models/track.mtl)
- Define material da pista
- Referencia textura: textures/asfalto.jpg

## Visualização da Pista

Para visualizar a pista gerada em 3D:
1. Gere a pista no editor
2. Adicione uma textura de asfalto em `textures/asfalto.jpg`
3. Use o arquivo `Configurador_Cena_Pista.txt` como referência
4. Compile e execute o visualizador 3D (se disponível)

## Observações Técnicas

- Editor trabalha em 2D (plano XY)
- Visualizador trabalha em 3D (Y é altura, XZ é chão)
- Conversão Y↔Z automática na exportação
- Curva B-Spline precisa de mínimo 4 pontos de controle
- Circuito sempre fechado (primeiro e último ponto conectados)

## Créditos
Alunos: Ian Rossetti Boniatti e Eduardo Tropea  
Curso: Jogos Digitais - Unisinos  
Disciplina: Computação Gráfica em Tempo Real  
Trabalho: Grau B - Editor de Pista de Corrida
