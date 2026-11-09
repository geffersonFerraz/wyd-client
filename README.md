# Objetivos
- Fazer com que este projeto rode utilizando a base de arquivos do Client WYD Global/KR de Versão 7.69+

# O que foi realizado
- Alterado as Estruturas para ler os arquivos de forma correta
- Alterado a forma em que o cliente le o arquivo sn.bin, sendo nescessário utilizar o bloco de notas para editar o mesmo.
- Foi adicionado uma tool de conversão da itemlist
- Foi adicionado o cliente base para o funcionamento do mesmo
- Foi adicionado a leitura do Lang.txt
- Foi adicionado inicialização dos componentes da arena real @SkyDrive
- Foi adicionado os slots novos 16 e 17, deve ser alterado no servidor o MAX_EQUIP para 18
- Foi adicionado algumas descompilações na pasta Descompilations
- Deve se utilizar o visual studio 2022




# TMProject

Este projeto é baseado em uma descompilação do cliente do jogo With Your Destiny. 
Nós não reindividicamos nenhum direito autoral do projeto. Este projeto foi criado apenas para fins de estudo sendo todos os direitos reservados exclusivamente e inteiramente a Hanbitsoft.

Este código é uma descompilação fiel ao original, isto é, o código que aqui consta não remete ao que nós, desenvolvedores, faríamos. Como o projeto como um todo já foi completamente finalizado, tendo todas as funções necessárias para que o jogo seja jogável, o mesmo pode ser alterado como quiser.

## Compilação
No Windows, o projeto é compilado utilizando Visual Studio (o projeto está preparado para a versão 2019, mas o mesmo não há limitação quanto a versão mínima ou máxima). 

O cliente é somente x64. Use `build.ps1`: o `WYD.exe`, o launcher `JogarWYD.exe` (precisa do Go) e as DLLs do D3DX são gerados em `Build\x64\<Config>\`. Com `-Deploy`, eles são copiados para a pasta do jogo, `..\wyd-others\Release Cliente` por padrão ou a indicada em `-ReleaseDir`. Os jogadores abrem o `JogarWYD.exe`. Código novo não deve usar tamanhos de struct numéricos (`dwSize = 36`): use sempre `sizeof`, pois structs com ponteiros mudam de tamanho em x64.

### Linux (Ubuntu 24.04 ou mais novo)

O `WYD.exe` x64 é compilado com mingw-w64 e executado com Wine. A pasta do jogo padrão é `../wyd-others/Release LInux Cliente`; para usar outra, defina `WYD_RELEASE_DIR`.

```sh
./wyd-linux.sh deps    # instala as dependências (usa sudo)
./wyd-linux.sh build   # gera o WYD.exe e copia para a pasta do jogo
./wyd-linux.sh run     # abre o jogo
./wyd-linux.sh bundle  # põe o Wine embutido e o lançador na pasta do jogo
```

Depois do `bundle`, a pasta do jogo funciona sozinha: basta abrir `WYD-Linux`. Ele valida o sistema, oferece instalar o que faltar, prepara o Wine na primeira execução, mostra a janela de configurações e grava o log em `~/.local/share/wyd-linux/wyd.log`. `./WYD-Linux --check` só mostra o resultado da validação, e `./WYD-Linux --jogar` abre o jogo sem a janela.

O código é um só para os dois sistemas: o Linux compila com mingw-w64 e roda com Wine, e o Windows compila com Visual Studio. Um `.cpp` novo precisa entrar no `TMProject.vcxproj`; o CMake recusa o build se faltar, salvo os listados em `WYD_LINUX_ONLY`. Distribua a pasta em `.tar.xz`, pois `.zip` costuma perder a permissão de execução.

## Dependências

### Visual Studio
Para compilar o projeto você precisará ter o Visual Studio 2022 com os seguintes componentes instalados:
- Desenvolvimento para desktop com C++
- ATL do C++ para a versão da toolchain que está sendo utilizada

A única dependência externa do projeto é o DirectX, incluído no repositório. O D3DX 43 (`Dependencies\D3DX`) vem do pacote oficial `Microsoft.DXSDK.D3DX`, versão 9.29.952.8.

## Bugs
Este projeto é uma descompilação e o mesmo pode e contém problemas. 

Sinta-se a vontade para reportá-los e abrir um pull request para o projeto (esperamos que você faça isso). 

### Pull-request
Para manter o código mais organizado, o padrão de código que consta atualmente deverá ser seguido, como por exemplo:
- Variáveis com estilo hungarian, e.g: pPointer, nInt, bBoolean, sz para sized string e etc.
- Nome de classes em PascalCase
- entre outros...

Como não temos um documento que dite todas as regras, caso surja uma dúvida, procure no código algum exemplo do que você está tentando fazer.

## Contribuidores
Este projeto foi desenvolvido por:
- Eric Santos (SKEWED)
- Wed Souza (FREEDOM)
- Kevin Kouketsu (shepher)

# General Public License:

The code is under the [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html), so you can be feel free to study and contribute.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
