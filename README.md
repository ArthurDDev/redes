# Trabalho de redes 1

## ROADMAP

### Transferir alguma coisa

- [ ] Abrir Raw socket básico
- [ ] Criar mensagem
    - [ ] CRC
    - [ ] Tipos
    - [ ] Seq
- [ ] Enviar/receber mensagem
- [ ] Stop and Wait
- [ ] Timeout
- [ ] Separar dados em varias mensagens
- [ ] Lista de escuta

### Jogo

- [ ] Desenhar tabuleiro
- [ ] Armazenar dados do jogo
- [ ] Calcular os movimentos do jogador
- [ ] Calcular os movimentos do fantasma
- [ ] Fog of war

### Arquivos

- [ ] Como abrir arquivos .txt
- [ ] Como abrir arquivos .jpeg
- [ ] Como abrir arquivos .mp4

### Server (lado root)

- [ ] Controlar toda a lógica do jogo
- [ ] Armazenar o labirinto (matriz 40x40)
- [ ] Calcular movimentos:
    - [ ] PacMan
    - [ ] Fantasmas
- [ ] Aplicar regras:
    - [ ] Colisões
    - [ ] Coleta de pastilhas
    - [ ] Fim de jogo
    - [ ] Controlar fog of war
- [ ] Gerenciar arquivos (.txt, .jpg, .mp4)
- [ ] Enviar estado atualizado do jogo para o cliente
- [ ] Implementar protocolo (CRC, sequência, timeout, etc.)

### Client (lado usuário)

- [ ] Capturar entrada do jogador (movimentos)
- [ ] Enviar comandos para o servidor
- [ ] Receber dados do servidor
- [ ] Renderizar o jogo (mostrar tabuleiro no terminal?)
- [ ] Exibir arquivos recebidos (.txt, .jpg, .mp4)

