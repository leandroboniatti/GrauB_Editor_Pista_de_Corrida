# TESTES RECOMENDADOS PARA O EDITOR

## 🧪 Teste 1: Pista Simples (4 pontos)
**Objetivo**: Verificar funcionamento básico

1. Execute o editor: `./track_editor.exe`
2. Clique em 4 pontos formando um quadrado aproximado:
   - Canto superior esquerdo
   - Canto superior direito
   - Canto inferior direito
   - Canto inferior esquerdo
3. Pressione G → Ver curva verde suave
4. Pressione T → Ver bordas azul e amarela
5. Pressione M → Triangularizar
6. Pressione E → Verificar arquivos em /models/

**Resultado esperado**: 
- Pista retangular suave com bordas visíveis
- Arquivos track.obj e track_curve.txt criados

---

## 🧪 Teste 2: Pista Circular (8 pontos)
**Objetivo**: Testar curva mais complexa

1. Execute o editor
2. Clique em 8 pontos formando um círculo
3. Siga a sequência: G → T → M → E

**Resultado esperado**:
- Pista circular suave
- Curvas interna e externa concêntricas

---

## 🧪 Teste 3: Pista em Forma de "8" (10+ pontos)
**Objetivo**: Testar curvas complexas

1. Execute o editor
2. Crie pontos formando um "8" deitado
3. Siga a sequência: G → T → M → E

**Resultado esperado**:
- Pista em formato de oito
- Transições suaves entre as curvas

---

## 🧪 Teste 4: Adicionar e Remover Pontos
**Objetivo**: Testar controles de edição

1. Execute o editor
2. Adicione 5 pontos aleatórios (clique esquerdo)
3. Clique direito 2 vezes para remover os últimos 2 pontos
4. Adicione 3 novos pontos
5. Pressione C para limpar tudo
6. Adicione 4 novos pontos
7. Continue com G → T → M → E

**Resultado esperado**:
- Clique direito remove pontos corretamente
- C limpa todos os pontos
- Editor funciona normalmente após operações

---

## 🧪 Teste 5: Verificação de Arquivos OBJ
**Objetivo**: Validar exportação

1. Crie uma pista qualquer e exporte (E)
2. Abra `models/track.obj` em um editor de texto
3. Verifique:
   - Linhas começam com v (vértices)
   - Linhas começam com vt (coordenadas de textura)
   - Linhas começam com vn (normais)
   - Linhas começam com f (faces)
4. Abra `models/track.mtl`
   - Verifique referência à textura

**Resultado esperado**:
- Formato OBJ válido
- Número correto de vértices e faces

---

## 🧪 Teste 6: Verificação de Conversão Y↔Z
**Objetivo**: Validar conversão de coordenadas

1. Crie pista simples no editor (plano XY)
2. Exporte (E)
3. Abra `models/track.obj`
4. Verifique que:
   - Primeiro número = X (igual ao editor)
   - Segundo número = Z (era Y no editor, deve ser 0)
   - Terceiro número = Y (era Z=0 no editor, agora tem valor de Y original)

**Resultado esperado**:
- Coordenadas corretas para visualizador 3D
- Y representa altura (0 ou próximo)
- XZ representa o plano horizontal

---

## 🧪 Teste 7: Arquivo de Animação
**Objetivo**: Verificar curva de animação

1. Crie pista e exporte
2. Abra `models/track_curve.txt`
3. Verifique formato: X Y Z (um ponto por linha)
4. Conte o número de linhas
   - Deve ser: num_pontos_controle × 20

**Resultado esperado**:
- Arquivo texto com coordenadas
- Número correto de pontos

---

## 🧪 Teste 8: Teste de Stress (Muitos Pontos)
**Objetivo**: Testar limites do editor

1. Execute o editor
2. Adicione 20-30 pontos de controle
3. Gere B-Spline (G)
4. Observe performance
5. Continue: T → M → E

**Resultado esperado**:
- Editor deve funcionar (pode ser mais lento)
- Curva deve ser gerada corretamente
- Exportação deve funcionar

---

## 📋 CHECKLIST DE VALIDAÇÃO

Após os testes, verifique:

- [ ] Compilação sem erros
- [ ] Janela do editor abre corretamente
- [ ] Clique esquerdo adiciona pontos (vermelhos)
- [ ] Clique direito remove último ponto
- [ ] Tecla G gera curva verde
- [ ] Tecla T gera curvas azul e amarela
- [ ] Tecla M não causa erros (processo invisível)
- [ ] Tecla E cria arquivos em /models/
- [ ] Tecla C limpa todos os pontos
- [ ] ESC fecha o editor
- [ ] Console mostra mensagens de progresso
- [ ] Arquivos OBJ são válidos
- [ ] Arquivo de curva está correto

---

## 🐛 PROBLEMAS COMUNS E SOLUÇÕES

### Problema: "Necessário pelo menos 4 pontos"
**Solução**: Adicione mais pontos de controle antes de pressionar G

### Problema: "Gere a curva B-Spline primeiro"
**Solução**: Pressione G antes de pressionar T

### Problema: Curva não aparece
**Solução**: Certifique-se de ter 4+ pontos e pressione G

### Problema: Arquivos não são criados
**Solução**: 
1. Verifique que a pasta /models/ existe
2. Complete a sequência: G → T → M → E
3. Verifique o console para mensagens de erro

### Problema: Compilação falha
**Solução**: 
1. Verifique que todas as dependências estão no lugar
2. Use a task "Build Editor de Pista"
3. Verifique o caminho do g++

---

## 📊 RESULTADOS ESPERADOS

### Console deve mostrar:
```
=== Editor de Pista de Corrida - CGR ===
Sistema inicializado com sucesso!
=== CONTROLES ===
...
Ponto de controle adicionado: (x, y)
B-Spline gerada com N pontos
Curvas interna e externa calculadas com N pontos cada
Malha triangularizada com N vertices e N triangulos
Normais calculadas
Pista exportada para: models/track.obj
Material exportado para: models/track.mtl
Curva de animacao exportada para: models/track_curve.txt
```

### Arquivos gerados:
- `models/track.obj` (vários KB)
- `models/track.mtl` (menos de 1 KB)
- `models/track_curve.txt` (vários KB)

---

**Teste completo**: Execute todos os 8 testes e marque o checklist!
