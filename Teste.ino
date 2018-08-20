#include <WiFi.h> // Importa a Biblioteca ESP8266WiFi.H ->  No ESP32 trocado por apenas WiFi.h
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient

//defines:
//defines de id mqtt e tópicos para publicaçãoo e subscribe
#define TOPICO_SUBSCRIBE "MQTTtestFarmEnvia"     //Tópico MQTT de escuta
#define TOPICO_PUBLISH   "MQTTtestFarmRecebe"    //Tópico MQTT de envio de informaÃ§ões para Broker
                                                   //IMPORTANTE: recomendamos fortemente alterar os nomes
                                                   //            desses tópicos. Caso contrário, há¡ grandes
                                                   //            chances de você controlar e monitorar o NodeMCU
                                                   //            de outra pessoa.
#define ID_MQTT  "Teste"     //id mqtt (para identificação de sessão)
                               //IMPORTANTE: este deve ser Único no broker (ou seja, 
                               //            se um client MQTT tentar entrar com o mesmo 
                               //            id de outro já¡ conectado ao broker, o broker 
                               //            irá fechar a conexão de um deles).


//defines - mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
//#define D3    0
#define LED_BUILTIN    4 //D4 led
#define D5    14
#define D6    12
#define D7    13

//#define DHT22_PIN    16 //Sensor de Umidade e Temperatura do ar
#define D9    3
#define D10   1
// WIFI
const char* SSID = "Nome-wifi"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "Senha"; // Senha da rede WI-FI que deseja se conectar
 
// MQTT
const char* BROKER_MQTT = "iot.eclipse.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT


//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída


void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);

/* 
 *  Implementação das funções
 */
void setup() 
{
    //inicializações:
    //inicializaSensorUmidadeTemperaturaDoAr();
    //configuracoesIniciaisSensorUmidadeSolo();
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
}
 
//Funçãoo: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
}

//Funçãoo: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
  pinMode(LED_BUILTIN,OUTPUT);
    delay(1000);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconectWiFi();
}
 
//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
//Funçãoo: funçãoo de callback 
//        esta funçãoo é chamada toda vez que uma informaçãoo de 
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
  
    //toma açãoo dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensâo na saída D2:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("D"))
    {
        digitalWrite(LED_BUILTIN, LOW);
        EstadoSaida = '1';
    }

    //verifica se deve colocar nivel alto de tensâo na saída D2:
    if (msg.equals("L"))
    {
        digitalWrite(LED_BUILTIN, HIGH);
        EstadoSaida = '0';
    }
}
 
//Funçãoo: reconecta-se ao broker MQTT (caso ainda nâo esteja conectado ou em caso de a conexâo cair)
//        em caso de sucesso na conexâo ou reconexâo, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else 
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
 
//Funçãoo: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, sâo efetuadas tentativas de conexâo
    if (WiFi.status() == WL_CONNECTED)
        return;
        
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

//Funçãoo: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexâo (qualquer uma das duas), a conexâo
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se no há conexâo com o Broker, a conexâo é refeita
    
     reconectWiFi(); //se nâo há conexâo com o WiFI, a conexâo é refeita
}

//Funçãoo: envia ao Broker o estado atual do output 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '1')
      MQTT.publish(TOPICO_PUBLISH, "D");

    if (EstadoSaida == '0')
      MQTT.publish(TOPICO_PUBLISH, "L");
      
  Serial.println("enviado estado output");
}

     

//Funçãoo: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);          
}


//programa principal
void loop() 
{ 
    
    
  //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();

    //envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoOutputMQTT();
    //Serial.println(read_humidity());

    //keep-alive da comunicaçãoo com broker MQTT
    MQTT.loop();
}


// --- Desenvolvimento das Funções do Sensor de umidade do solo ---
/*float read_humidity(void)
{
    //lê valor do sinal analógico e salva em sensorUmidadeSolo
    int ValorADC;
    float UmidadePercentual;
 
     ValorADC = analogRead(0);   //978 -> 3,3V
     Serial.print("[Leitura ADC] ");
     Serial.println(ValorADC);
 
     //Quanto maior o numero lido do ADC, menor a umidade.
     //Sendo assim, calcula-se a porcentagem de umidade por:
     //      
     //   Valor lido                 Umidade percentual
     //      _    0                           _ 100
     //      |                                |   
     //      |                                |   
     //      -   ValorADC                     - UmidadePercentual 
     //      |                                |   
     //      |                                |   
     //     _|_  978                         _|_ 0
     //
     //   (UmidadePercentual-0) / (100-0)  =  (ValorADC - 978) / (-978)
     //      Logo:
     //      UmidadePercentual = 100 * ((418-ValorADC) / 978)  
      
     UmidadePercentual = 100 * ((978-(float)ValorADC) / 978);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");
 
     return UmidadePercentual;
} //end read_humidity
void inicializaSensorUmidadeTemperaturaDoAr(){
      Serial.begin(115200);
    Serial.println("dht22_test.ino");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT_LIB_VERSION);
    Serial.println();
    Serial.println("Type,\tstatus,Umid(%),Temp (C)\tTime (us)");
  }
