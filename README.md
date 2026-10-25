Uma vaca que fala no terminal do Windows. É uma versão do clássico cowsay feita em C++ em um único arquivo, sem dependências além da WinAPI.

 ______________
< Olá, mundo! >
 --------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
Recursos
Arquivo único (cowsay.cpp), sem dependências externas
Suporte a UTF-8: acentos e emojis não desalinham o balão
Lê a mensagem dos argumentos ou de um pipe
Quebra de linha automática, com largura configurável
Modo pensamento (--think)
Vacas: default, tux e small
Modos de olhos e língua como no original
Compilando

Precisa de um compilador C++17 para Windows.

MinGW / MSYS2

g++ -O2 -std=c++17 -static cowsay.cpp -o cowsay.exe -lshell32

MSVC

cl /EHsc /utf-8 /O2 cowsay.cpp
Uso
cowsay [opções] [mensagem]

Exemplos:

cowsay Olá, mundo!
echo muuuu | cowsay -f tux
cowsay --think -d "estou pensando..."
cowsay -W 20 uma mensagem bem longa que vai ser quebrada em várias linhas
Opções
Opção	Descrição
-e <olhos>	Define os olhos (2 caracteres)
-T <língua>	Define a língua (2 caracteres)
-f <vaca>	Escolhe a vaca: default, tux, small
-W <n>	Largura do balão (padrão: 40)
-n	Não quebra as linhas
--think	Balão de pensamento
-l	Lista as vacas disponíveis
-h, --help	Mostra a ajuda
Modos de olhos
Opção	Modo	Olhos
-b	borg	==
-d	morta	xx
-g	gananciosa	$$
-p	paranoica	@@
-s	chapada	**
-t	cansada	--
-w	ligada	OO
-y	jovem	..
Dica: deixar o cowsay disponível em qualquer pasta

Copie o cowsay.exe para uma pasta que esteja no seu PATH (por exemplo C:\Tools) e use cowsay direto de qualquer terminal.

Licença

Distribuído sob a licença MIT. Veja o arquivo LICENSE.
