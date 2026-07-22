# Firmware para aviônica do foguete Weiss

O firmware do foguete Weiss foi desenvolvido em C++, utilizando o ambiente Arduino IDE (configurado para ESP32), responsável pelo controle e pela operação do sistema de aviônica. Esse ambiente foi escolhido por disponibilizar bibliotecas consolidadas para comunicação com sensores, dispositivos de armazenamento e módulos de comunicação, além de facilitar o desenvolvimento, a depuração e a manutenção do código embarcado.

O funcionamento geral do firmware implementado se baseia em duas fases: setup (onde são inicializados os sensores, definidas as configurações de saída/entrada dos pinos e inicializadas as variáveis para registro de dados) e loop (onde é realizada uma rotina periódica de captação, processamento, transmissão e registro de dados). A transição de um estado para o outro é marcada pelos três beeps do buzzer. Caso não ocorram, houve algum erro de inicialização dos sensores.

<p align="center">
<img width="1916" height="883" alt="image" src="https://github.com/user-attachments/assets/e227e519-3d78-429c-a432-9d62f9dfa3f3" />
</p>

Imagens da eletrônica do Weiss
--- A ser inserido após a manufatura ---

Segue a lista de principais bibliotecas utilizadas no projeto e suas respectivas versões:
- Adafruit BMP280 by Adafruit: 2.6.8
- Adafruit BMP3XX by Adafruit: 2.1.2
- Adafruit MPU6050 by Adafruit: 2.2.4
- TinyGPSPlus by Mikal Hart: 1.0.3

Arquiteturalmente, o software está organizado nos seguintes arquivos:

### Avionica.ino

Arquivo principal do sistema. Define o fluxo de execução do `setup()` e do `loop()`.

Não concentra lógica complexa: atua como **orquestrador**, chamando funções definidas nos demais módulos.

**Funções**
- **debugPacketData**  
  Apresenta todos os dados do sistema registrados no momento da chamada, com foco em debug.

- **setup**  
  Inicializa a operação do sistema, chamando os métodos de setup dos módulos dependentes.

- **loop**  
  Executa continuamente a rotina do ciclo de vida do sistema.

### bmp.h

Módulo responsável pela inicialização e leitura de dados do sensor barométrico **BMP390**.

**Responsabilidades**
- Comunicação I2C com o sensor
- Leitura de pressão, temperatura e altitude
- Definição da altitude inicial de referência
- Atualização do estado do sistema com dados atmosféricos

**Funções**
- **updateBMP**  
  Atualiza a leitura do BMP390.  
  Emite uma mensagem de erro caso nenhum valor válido seja obtido.

- **getInitialAltitude**  
  Define a altitude inicial detectada pelo sensor em uma variável global chamada `initialAltitude`.

- **verifyBMP**  
  Configura uma variável de estado indicando se a comunicação I2C com o BMP390 foi bem-sucedida.

- **setupBMP**  
  Inicializa o BMP390 e define seus parâmetros de operação:
  - Taxa de amostragem de pressão
  - Taxa de amostragem de temperatura
  - Filtro de leitura
  - Taxa de output dos dados

- **readBMP**  
  Lê os valores de:
  - Temperatura  
  - Pressão  
  - Altitude (relativa à altitude inicial)  

  Os dados são salvos nas variáveis de estado do sistema embarcado.

### imu.h

Módulo responsável pela inicialização e leitura de dados do **IMU (MPU6050)**.

O sensor combina **acelerômetro** e **giroscópio**, utilizando o conceito de **Quaternions** para determinar a orientação tridimensional do foguete.

**Responsabilidades**
- Comunicação com o MPU6050
- Calibração do sensor
- Leitura de aceleração e orientação
- Conversão para sistema de ângulos Euler (Roll, Pitch, Yaw)

**Funções**
- **calibrate**  
  Realiza a calibração do sensor, exibindo mensagens de debug durante o processo.

- **verifyMPU**  
  Configura uma variável de estado indicando se a comunicação com o MPU6050 foi realizada com sucesso.

- **setupMPU**  
  Inicializa o sensor IMU e executa a calibração, caso necessário.

- **readMPU**  
  Lê:
  - Valores de aceleração nos eixos **X, Y e Z**
  - Ângulos de orientação no sistema **Euler / RPY** (Roll, Pitch, Yaw)

### gps.h

Módulo responsável por inicializar e ler dados do módulo GPS (**NEO-6M**).

**Responsabilidades**
- Gerenciar a comunicação serial com o GPS
- Interpretar dados de posição e tempo
- Atualizar o estado global do sistema com informações do GPS

**Funções**
- **verifyGPS**  
  Configura uma variável de estado indicando se a comunicação com o GPS foi realizada com sucesso.

- **setupGPS**  
  Inicializa o sensor, criando o canal de comunicação serial com o módulo GPS.

- **getGPSData**  
  Obtém os dados de latitude, longitude, data e hora do GPS.

- **saveGPSData**  
  Salva os dados obtidos do GPS nas variáveis de estado do sistema.

- **updateGPSData**  
  Função orquestradora do módulo GPS: lê os dados atuais e atualiza o estado do sistema.

### messages.h

Módulo responsável por **parsing** e **serialização** dos dados do sistema, gerando mensagens para:
- Registro no cartão SD
- Transmissão via telemetria

> Em sessões posteriores, o formato e significado das mensagens serão detalhados.

**Funções**
- **fixNumberSize**  
  Ajusta o tamanho de um número adicionando zeros à esquerda para garantir `N` caracteres.  
  Exemplo:
  - `5  -> "005"`
  - `15 -> "015"`
  - `100 -> "100"`

- **telemetryMessage**  
  Monta a mensagem de telemetria com **exatos 48 bytes**, organizando os dados relevantes do estado do sistema conforme o formato especificado.

### moduleSD.h

Responsável pela inicialização e escrita de dados no **cartão SD**.

**Funções**
- **verifySD**  
  Define uma variável de estado indicando se a comunicação com o SD foi bem-sucedida.

- **setupSD**  
  Inicializa o módulo SD, criando/abrindo um arquivo `myFile.txt` e escrevendo a linha inicial:

- **writeOnSD**  
Escreve a string passada como parâmetro em uma nova linha do arquivo.

### telemetry.h

Módulo responsável pela **transmissão e recepção de dados via LoRa**.

**Funções**
- **setupTelemetry**  
Inicializa o módulo de telemetria e configura o canal serial.

- **receiveStruct**  
Função de conveniência para leitura de dados brutos (bytes) recebidos via LoRa.

- **receiveString**  
Função de conveniência para leitura de mensagens em formato string.

- **transmitString**  
Envia uma string via LoRa.

- **transmit**  
Função orquestradora para envio de dados por telemetria.

- **receive**  
Função orquestradora para recepção de dados via LoRa.

- **hasSoloMessage**  
Verifica se há mensagens recebidas disponíveis para leitura.

### parachute.h

Módulo que concentra **toda a lógica de acionamento dos paraquedas**.

**Funções**
- **setupSkibPins**  
Configura o pino do SKIB como saída.

- **activateStage1**  
Ativa o estágio 1, incluindo efeitos colaterais:
- Acionamento do buzzer
- Mensagens de debug
- Atualização de estado

- **deactivateStage1**  
Desativa o estágio.

- **altitudeLessThan**  
Compara altitudes considerando a margem de erro permitida entre medições consecutivas.

- **activateParachutes**  
Decide e executa o acionamento dos paraquedas com base no estado atual do sistema.

- **checkIsDropping**  
Determina se o foguete está em queda.

- **checkApogee**  
Orquestra as ações a serem realizadas quando o apogeu é detectado.

- **testActivations**  
Código de teste para simular acionamentos sem variação de altitude.

### serial.h

Arquivo de **funções de conveniência** que abstraem chamadas da porta serial.

**Objetivo**
Simplificar a sintaxe do código, permitindo chamadas como:

```cpp
println("Hello");
```

ao invés de

```cpp
Serial.println("Hello");
```

### debug.h

Concentra funções auxiliares para debug do estado atual do sistema.

### buzzer.h

Módulo responsável pelo controle do buzzer, incluindo funções de acionamento e padrões sonoro

# Explicando um pouco sobre orientação no espaço tridimensional

De maneira resumida, para representar um objeto no espaço precisamos definir sua posição e angulação em relação ao sistema de referência. Um objeto no espaço é representado por um frame, um sistema tridimensional em alguma posição e inclinado (possivelmente) em relação ao sistema global:

<p align="center">
  <img width="587" height="539" alt="image" src="https://github.com/user-attachments/assets/6b70bd4d-49da-4802-b81b-7b0abe8fc54b" />
</p>

No espaço tridimensional, definir a angulação é um desafio. A representação mais simples envolve o uso de três ângulos (representação de Euler/RYP), que medem a inclinação do frame em relação aos eixos ordenados. Esse sistema é ótimo para visualização de objetos no espaço, pois é intuitivo, porém conta com sérias limitações técnicas em termos de representação de objetos matematicamente. Destacadamente, a ocorrência de gimball lock, quando dois eixos se alinham e se torna impossível rotacionar o objeto sem necessariamente mover dois eixos ao mesmo tempo.

<p align="center">
  <img width="1364" height="610" alt="image" src="https://github.com/user-attachments/assets/b59ae16c-d510-47d2-8e75-be17e4b2a13b" />
</p>

A solução mais robusta para esse problema foi desenvolvida por William Rowan Hamilton, em 1843, com a introdução dos quaternions. Um quaternion pode ser interpretado como uma extensão dos números complexos, composto por uma parte real e três componentes imaginárias, sendo usualmente representado na forma q = a + bi + cj + dk, onde i, j e k são unidades imaginárias que obedecem a regras próprias de multiplicação. Quando devidamente normalizados, os quaternions permitem representar orientações e rotações no espaço tridimensional de forma contínua e livre de singularidades. Nesse formalismo, uma rotação de um corpo em torno de um eixo arbitrário por um determinado ângulo pode ser realizada de maneira eficiente por meio da multiplicação de quaternions, garantindo estabilidade numérica e consistência matemática. A principal limitação dessa abordagem reside no fato de que os quaternions não possuem uma representação visual intuitiva direta, o que pode dificultar sua interpretação sem o auxílio de conversões para outras formas de representação, como os ângulos de Euler.


# Ciclo de Vida

Em linhas gerais, o ciclo de vida do sistema é baseado em duas fases:
- Setup: Onde são inicializadas as variáveis de estado e os sensores
- Loop: Onde é executada uma rotina periódica que lê o valor dos sensores e atualiza as variáveis de estado. Nessa também são monitorados esses valores, para agir quando for detectado o apogeu do foguete, acionando os paraquedas.

As variáveis de estado ficam todas encapsuladas em uma variável global chamada allData, cujo formato é PacketData, estrutura que encapsula todos os dados de todos os sensores. Veja, é bem auto-explicativo:

```
struct PacketData
{
  AvionicData data;
  BmpData bmpData;
  ImuData imuData;
  GpsData gpsData;
  int parachute;
};
```

Agora as demais interfaces:

```
struct AvionicData
{
  float time;
  int parachute;
};


struct BmpData
{
  float temperature;
  float pressure;
  float altitude;
};


struct ImuData
{
  float accelX;
  float accelY;
  float accelZ;
  float quaternion_w;
  float quaternion_x;
  float quaternion_y;
  float quaternion_z;
};


struct GpsData
{
  String date;
  String hour;
  double latitude, longitude;
};
```

O sistema embarcado lê os dados dos sensores através das funções read<Sensor>. Dentro delas, os valores de estado global respectivos são atualizados.

No fluxo principal, a seguinte sequência de eventos é realizada:
1. Atualiza-se o estado global através da função “getSensorMeasures()”, orquestrador que chama todos os read<Sensor> dos sensores que foram habilitados.
2. Avalia-se se o foguete está no apogeu. Se sim, é iniciada a rotina de disparo dos paraquedas (detalhes na seção seguinte).
3. São geradas as mensagens de telemetria e SD, salvas em variáveis globais de estado “sd_message” e “telemetry_message”.
4. Em seguida, se habilitados, a mensagem para o sd gerada é salva no cartão e a mensagem para telemetria é transmitida.

# Tempo de delay para Detecção do Apogeu

O acionamento dos paraquedas depende diretamente da estimativa de altitude fornecida pelo sensor barométrico BMP390. Essa variável de estado é atualizada periodicamente de acordo com as configurações definidas na função setupBMP(), bem como pela dinâmica de execução do firmware. Dessa forma, o instante efetivo de detecção do apogeu e, consequentemente, o acionamento do sistema de recuperação estão sujeitos a atrasos inerentes ao sistema.
De maneira geral, três fatores influenciam diretamente a taxa com que a altitude atual é atualizada e interpretada pelo sistema:

1. Tempo de execução do ciclo principal (loop), no qual os estados são atualizados e as decisões de acionamento são tomadas;
2. Taxa de saída configurada no BMP390, definida durante a inicialização do sensor;
3. Ruído intrínseco da medição barométrica, que, conforme o datasheet do sensor, pode atingir valores da ordem de até 5 m na estimativa de altitude.

Como exemplo ilustrativo, considere um cenário em que o tempo de execução do ciclo principal seja de 500 ms, valor significativamente maior que a taxa de atualização do sensor. Nesse caso, no pior cenário, o sistema pode levar até 500 ms após o apogeu para identificar uma variação consistente na altitude. Essa detecção, entretanto, somente ocorre se a diferença entre a altitude atual e a altitude máxima registrada for superior à margem de erro associada ao ruído do sensor.

Para evitar acionamentos indevidos causados por flutuações de medição, o firmware implementa uma margem de segurança definida pela constante SAFE_MARGIN_ALTITUDE_ERROR, presente no módulo parachute.h. Essa constante estabelece uma diferença mínima de altitude necessária para que o sistema considere que o foguete iniciou efetivamente a descida. Por exemplo, se essa margem for configurada para 50 m, o foguete deverá estar ao menos 50 m abaixo do apogeu detectado para que o evento de queda seja confirmado.
Supondo um modelo simplificado de movimento vertical, com aceleração gravitacional constante igual a 9,8 m/s² e desprezando os efeitos da resistência do ar, o tempo necessário para uma queda de 50 m pode ser estimado por:
H = gt2/2 = 4.9t2 t = sqrt(H/4.9) ~ 3.19s 

Na prática, a resistência do ar tende a reduzir levemente esse valor; entretanto, para o perfil de voo considerado, essa diferença não é dominante. Observa-se, portanto, que o atraso introduzido pela margem de segurança é, via de regra, o fator predominante no tempo total até o acionamento do paraquedas, sendo necessariamente superior à incerteza intrínseca do sensor.

A adoção dessa margem de segurança é fundamental para mitigar acionamentos prematuros associados a efeitos transientes de pressão no interior do foguete, especialmente nos instantes iniciais do voo. Devido à inércia do ar confinado na câmara de aviônica, a rápida aceleração do foguete pode provocar um aumento temporário da pressão interna. Como o sensor barométrico interpreta variações de pressão como variações de altitude, esse fenômeno pode ser erroneamente interpretado como uma queda, caso não haja um critério de rejeição adequado. Uma vez que a pressão atmosférica aumenta com a diminuição da altitude, esse pico de pressão inicial pode levar o sistema a inferir uma falsa descida se não for devidamente filtrado.

A taxa de saída do BMP390 também introduz uma latência adicional, porém significativamente menor quando comparada aos demais fatores. Por exemplo, ao configurar o sensor com:
bmp.setOutputDataRate(BMP3_ODR_50_HZ);

obtém-se uma taxa de atualização de 50 Hz, equivalente a uma nova leitura a cada 20 ms. Assim, no pior cenário considerado, o tempo total até o acionamento do paraquedas pode ser estimado como a soma das principais contribuições:
- atraso devido à margem de segurança de altitude (≈ 3,19 s);
- atraso associado ao tempo de execução do loop (0,5 s);
- latência máxima da taxa de saída do sensor (0,02 s).

Resulta, portanto, em um atraso total aproximado de 3,61 s no cenário analisado, valor coerente com os critérios de segurança adotados no firmware para garantir a confiabilidade da detecção do apogeu e do acionamento do sistema de recuperação.

# Algoritmos de Detecção de Apogeu

O algoritmo de detecção do apogeu foi construído de modo a não travar o fluxo de execução principal do sistema, portanto ele é um processo paralelo simulado em um fluxo único, tipo javascript fingindo ter paralelismo enquanto executa uma única thread.

A lógica é fundamentalmente a seguinte:
1. Se o foguete está em queda, aciona o drogue e registra, em uma variável, que o pino está acionado e guarda o instante do acionamento (depois, segue o loop normal do ciclo de vida)
2. Se o foguete está em queda E o drogue já foi acionado e ainda está acionado, verifica se já passaram N segundos. Se sim, desativa o pino do drogue (isso garante que o sinal seja enviado durante um certo tempo para garantir o acionamento e também o barulho do buzzer, para testes)
3. Se o foguete está em queda e o drogue já foi desativado, verifica se já é hora de acionar o paraquedas principal. Se sim, aciona, senão, segue o ciclo de vida normalmente.
4. Após N segundos do acionamento do paraquedas principal, o sinal de acionamento dele é desativado (do mesmo jeito que no primeiro caso)

Portanto a função de acionamento é chamada N vezes durante o ciclo de vida, gerando efeitos diferentes dependendo do momento em que o estado do sistema se encontra.

O acionamento do paraquedas principal tem uma pequena peculiaridade. A princípio, conforme definido junto ao setor de recuperação, ele deveria ser acionado quando o foguete estivesse a 500m acima do solo. Contudo, caso o foguete não chegue a ultrapassar 500m, isso acarreta no não acionamento do paraquedas principal, o que é um problema bem sério. Por isso, há uma condição de segurança: caso a altura máxima alcançada seja menor que 500m, o paraquedas principal irá acionar 8s após o drogue. No código, a variável “enoughHeight” guarda qual caso será usado quando o drogue é acionado (ou seja, quando se inicia a queda).

# Implementação da Telemetria

O módulo LoRa pode transmitir apenas 48 bytes por pacote, portanto, para garantir máxima eficiência da transmissão e evitar que a mensagem fosse truncada em 2 ou mais pacotes (o que introduziria atrasos na transmissão e faria a perda de um pacote ser mais danosa), foi criado um padrão de 48 bytes para transmissão de dados, conforme a string abaixo:

PPPPPAAAAAAZZZZCCCCXXXYYYZZZPLLLLLLGGGGGG

Cada letra representa um byte, portanto, um caractere ASCII. O objetivo é transmitir o máximo de dados com o mínimo de caracteres possível. Para isso:
- Retiramos todos os pontos decimais.
- Retiramos alguns dados menos importantes.
- E, durante a competição LASC 2025, retiramos também os sinais dos valores de latitude e longitude, para aumentar a precisão dos valores de localização

Todos os valores estão normalizados com zeros à esquerda, portanto o pacote 5 terá número “00005” nos 5 primeiros dígitos. O valor transmitido é muitas vezes multiplicado por uma potencia de 10, e apenas o resultado inteiro é transmitido. Dessa forma, temos N campos decimais na string equivalente:
Altitude: 100.62545 (medida pelo sensor).
Multiplicada vezes 1000 -> 100625.45.
Transformada em inteiro -> 100625.
Valor transmitido -> 100625.

Do lado da recepção, é preciso dividir o valor por 1000 para obter a medida real (Note, portanto, que esse padrão é um acordo realizado entre transmissão e recepção; uma mudança feita de um lado precisa ser feita do outro).

Explicando cada letra:
- P -> numeração do pacote (5 dígitos, sem sinal).
- A -> altitude, 6 dígitos, com sinal, e com 3 campos decimais (multiplicada vezes 1000 na transmissão, precisa dividir por 1000 na recepção - não vou mais repetir isso).
- C -> Aceleração em Z, com sinal, 4 dígitos, 2 campos decimais.
- W -> Quaternion W, com sinal, 3 dígitos. 2 campos decimais.
- X -> Quaternion X, com sinal, 3 dígitos, 2 campos decimais.
- Y -> Quaternion Y, com sinal, 3 dígitos, 2 campos decimais.
- Z -> Quaternion Z, com sinal, 3 dígitos, 2 campos decimais.
- P -> booleano que determina se o paraquedas já foi acionado.
- 0 -> nenhum foi acionado.
- 1 -> Drogue acionado.
- 2 -> Drogue e principal acionados.
- L -> latitude, sem sinal (como estamos no hemisfério sul, é sempre negativa. Não ironicamente esse código não vai funcionar se for usado nos US), 4 dígitos decimais, 6 dígitos ao todo.
- G -> longitude, sem sinal (como estamos no ocidente, é sempre negativa. Não ironicamente esse código não vai funcionar se for usado na China). 4 dígitos decimais, 6 dígitos ao todo.
