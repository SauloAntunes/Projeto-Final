# Sistema de Identificação de Acesso  

## Descrição do Projeto  

Este projeto tem como objetivo simular um sistema de segurança para identificação de acesso utilizando o microcontrolador RP2040 com a placa de desenvolvimento BitDogLab. O sistema é ativado por um botão que simula a detecção de entrada de uma pessoa e, em seguida, permite a identificação através de outro botão. Dependendo da resposta do usuário, o sistema exibe mensagens em um display SSD1306 via I2C, utiliza uma matriz de LEDs 5x5 para indicar o status de autorização e aciona um buzzer para alertas sonoros. O projeto também implementa um temporizador para limitar o tempo de identificação e utiliza interrupções com debouncing para garantir a precisão na leitura dos botões, demonstrando a integração eficaz entre hardware e software.  

## Pré-requisitos  

Antes de executar o projeto, certifique-se de ter os seguintes softwares e ferramentas instalados:  

- [Visual Studio Code (VS Code)](https://code.visualstudio.com/download)  
- [Extensão Raspberry Pi Pico para VS Code](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)  
- [Extensão Wokwi Simulator para VS Code](https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode)  
- [Git](https://git-scm.com/downloads)  
- [SDK para Raspberry Pi Pico (Pico SDK)](#instalação-e-configuração-do-ambiente)  

Além disso, será necessário possuir uma conta no [site oficial do Wokwi](https://wokwi.com/).  

## Estrutura do Repositório  

```
├── .vscode/                		    # Configurações específicas do projeto para o VS Code
├── generated/              		    # Biblioteca para configuração do PIO no programa
├── img/                    		    # Imagens utilizadas no README para detalhar o projeto
├── inc/                    		    # Cabeçalhos das fontes (letras e números) e controle do display
├── src/                    		    # Código para configuração e controle do display
├── .gitignore              		    # Arquivos ignorados pelo Git
├── CMakeLists.txt          		    # Configuração do CMake para o projeto
├── README.md               		    # Instruções e detalhes do projeto
├── SE_security.c			            # Código-fonte principal do projeto
├── diagram.json            		    # Arquivo de configuração para o simulador Wokwi
├── pico_sdk_import.cmake   		    # Configuração para importar o Pico SDK
├── wokwi.toml              		    # Arquivo de configuração do Wokwi
└── ws2812.pio              		    # Código de controle para LEDs WS2812 usando PIO
```
`OBS.:` o subdiretório `build/` será adicionado ao diretório principal após a configuração automática do CMake.  

## Instalação e Configuração do Ambiente
1. Clone este repositório para seu ambiente local:  
   ```
   git clone https://github.com/SauloAntunes/Projeto-Final.git  
   ```

2. Com o VS Code aberto, configure o ambiente de desenvolvimento do Pico SDK, seguindo as instruções:  
    - O Pico SDK pode ser configurado de forma automática durante a configuração do projeto através da extensão Raspberry Pi Pico no VS Code.  
    ![Configuração do Pico SDK](img/Config-Pico-SDK.png)  
    - Passo a passo:  
    `1º:` acesse a extensão Raspberry Pi Pico;  
     `2º:` selecione a opção `Import Project`;  
    `3º:` adicione o caminho do projeto no seu dispositivo, selecione a versão 2.1.0 do Pico SDK (é importante selecionar essa versão para evitar possíveis incompatibilidades) , e por fim clique em `Import`.  
    `OBS.:` após isso, a própria ferramenta realizará a configuração do Pico SDK. Durante o processo de configuração, notificações serão exibidas no canto inferior direito da tela, indicando cada etapa.  

3. Compile o projeto:  
  ![Compilar projeto](img/Compile-Project.png)
  - Passo a passo:  
    `1º:` com o projeto aberto no VS Code, acesse a extensão Raspberry Pi Pico;  
    `2º:` clique na opção `Compile Project` e aguarde o processo de compilação.   

4. Com o VS Code aberto, configure o ambiente Wokwi, seguindo as instruções:
    - A configuração do Wokwi para VS Code pode ser realizada seguindo as orientações disponíveis na [documentação oficial](https://docs-wokwi-com.translate.goog/vscode/getting-started?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt&_x_tr_pto=tc&_x_tr_hist=true).

5. Inicie a simulação do projeto:  
    - Para iniciar a simulação do projeto clique no arquivo `diagram.json`, logo em seguida será aberta uma tela com a simulação do projeto, contendo os componentes como a placa Raspberry Pi Pico W, a matriz de LEDs, o display, entre outros. Após a abertura da simulação do projeto, clique no botão verde de começar.  

## Estrutura de Controle    

- Detecção de Entrada com o Botão A  
	- Pressionar o botão A simula a entrada de uma pessoa no local;  
	- Ao ser pressionado, o sistema inicia um temporizador de 3 segundos e ativa o botão B para identificação.  

- Identificação com o Botão B  
	- O botão B é utilizado para identificar a pessoa que entrou;  
	- Dependendo da interação com o botão B, o sistema pode registrar diferentes estados:  
		- Nenhum clique: o sistema interpreta como um intruso;  
			- Mensagens "Intruso Detectado" e "Acionando Seguranca" serão exibidas no display SSD1306;  
			- Um símbolo de "intrusão" (❌), será mostrado na matriz 5x5 em vermelho;  
			- O buzzer emitirá um sinal sonoro de alerta.  
		- 1 clique: o sistema interpreta como uma pessoa não autorizada;  
			- Mensagem "Pessoa Nao Autorizada" será exibida no display SSD1306;  
			- Um símbolo de "atenção" (⚠️), será mostrado na matriz 5x5 em amarelo;  
			- O buzzer emitirá um sinal sonoro de alerta.  
		- 2 cliques: o sistema interpreta como uma pessoa autorizada;  
			- Mensagem "Pessoa Autorizada" será exibida no display SSD1306;  
			- Um símbolo de "autorizado" (✅), será mostrado na matriz 5x5 em verde;  
			- O buzzer emitirá um sinal sonoro de confirmação.  

## Vídeo de Apresentação da Solução

Para mais detalhes sobre a implementação e os resultados, assista ao vídeo da solução: [Link para o vídeo](https://drive.google.com/file/d/1w-UzDMb6R6ArW5Lto_A0-JhxUCMvX-Th/view?usp=sharing).  