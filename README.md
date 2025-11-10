# 🏁 Editor de Pista de Corrida - Grau B CGR

> Editor 2D interativo para criação de pistas de corrida usando curvas B-Spline

## 📋 Descrição

Projeto desenvolvido para a disciplina de **Computação Gráfica em Tempo Real** do curso de **Jogos Digitais** da Unisinos. O editor permite criar pistas de corrida fechadas através de pontos de controle definidos pelo usuário, gerando automaticamente curvas suaves usando o algoritmo B-Spline cúbica.

## ✨ Características Principais

- ✅ Interface 2D interativa com OpenGL
- ✅ Definição de pontos de controle via mouse
- ✅ Geração automática de curva B-Spline cúbica fechada
- ✅ Cálculo de curvas interna e externa da pista
- ✅ Triangularização automática da malha
- ✅ Cálculo de normais para iluminação 3D
- ✅ Exportação para formato OBJ (compatível com visualizadores 3D)
- ✅ Exportação de curva para animação

## 🚀 Início Rápido

### Compilar
```bash
# No VS Code: Ctrl+Shift+B → "Build Editor de Pista"
# Ou via terminal:
g++ -std=c++17 -g -Iinclude -IDependencies/GLAD/include -IDependencies/GLFW/include -IDependencies/glm main.cpp src/Shader.cpp src/TrackEditor.cpp Dependencies/GLAD/src/glad.c -LDependencies/GLFW/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -o track_editor.exe
```

### Executar
```bash
./track_editor.exe
```

### Criar uma Pista
1. **Clique** em pelo menos 4 pontos na tela (pontos de controle)
2. Pressione **G** para gerar a curva B-Spline
3. Pressione **T** para calcular as bordas da pista
4. Pressione **M** para triangularizar a malha
5. Pressione **E** para exportar para OBJ

## 🎮 Controles

| Tecla | Ação |
|-------|------|
| **Clique Esquerdo** | Adicionar ponto de controle |
| **Clique Direito** | Remover último ponto |
| **C** | Limpar todos os pontos |
| **G** | Gerar curva B-Spline |
| **T** | Gerar pista (curvas interna/externa) |
| **M** | Triangularizar malha |
| **E** | Exportar para OBJ |
| **ESC** | Sair |

## 🎨 Visualização

- 🔴 **Vermelho**: Pontos de controle definidos pelo usuário
- 🟢 **Verde**: Curva B-Spline gerada
- 🔵 **Azul**: Borda interna da pista
- 🟡 **Amarelo**: Borda externa da pista

## 📦 Dependências

- **GLAD** - OpenGL function loader
- **GLFW** - Gerenciamento de janela e entrada
- **GLM** - Biblioteca de matemática vetorial
- **C++17** - Compilador com suporte a C++17

## 📁 Arquivos Gerados

Após pressionar **E**, são criados:
- `models/track.obj` - Malha 3D da pista
- `models/track.mtl` - Material da pista
- `models/track_curve.txt` - Pontos da curva para animação

## 📚 Documentação

- **[INDICE.txt](INDICE.txt)** - Índice de toda a documentação
- **[GUIA_RAPIDO.txt](GUIA_RAPIDO.txt)** - Tutorial rápido de uso
- **[README_EDITOR.md](README_EDITOR.md)** - Documentação técnica completa
- **[PROJETO_COMPLETO.md](PROJETO_COMPLETO.md)** - Visão geral do projeto
- **[TESTES.md](TESTES.md)** - Testes e validação
- **[ESTRUTURA_VISUAL.txt](ESTRUTURA_VISUAL.txt)** - Estrutura de arquivos

## 🔧 Requisitos do Sistema

- Windows 10/11
- MinGW-w64 (g++)
- Visual Studio Code (recomendado)
- OpenGL 3.3+

## 🎓 Créditos

**Alunos**: Ian Rossetti Boniatti e Eduardo Tropea  
**Curso**: Jogos Digitais  
**Universidade**: Unisinos  
**Disciplina**: Computação Gráfica em Tempo Real  
**Professor**: [Nome do Professor]  
**Trabalho**: Grau B - Editor de Pista de Corrida  
**Ano**: 2025

## 📄 Licença

Este projeto é um trabalho acadêmico desenvolvido para fins educacionais.

## 🤝 Contribuições

Este é um projeto acadêmico. Para dúvidas ou sugestões, entre em contato com os desenvolvedores.

---

**Status do Projeto**: ✅ Completo e Funcional  
**Última Atualização**: Novembro 2025
