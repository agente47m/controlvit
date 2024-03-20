#include <Arduino.h>
#include <ModbusMaster.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <WebServer.h>
#include <WiFi.h>


//INICIO VARIABLES DE MODBUS

#define TX_RS485 17
#define RX_RS485 16
#define BAUD_RATE_RS485 9600
#define DIR_ID_MODBUS 3

int ADDRESS_MODBUS= 8448;
ModbusMaster node;

//INICIO VARIABLES DE PANTALLA OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void printMenu();
void printHelp();
String getModeloName(int idName);
void calculateCo2();
String getEstadoName(int estado);
void getDataModBus(int Dirmodbus);// primer parte de datos para obtener el modelo y estado del equipo
void getDataModbusDos(int Dirmodbus);// segun 
void printVariables();
String sendHtml();
void handle_NotFound();
void handle_OnConnect();
void getData(); 
void resetSettingWifi();
void printOLEDbienvenida();


//variables globales

float frecuencia;
int estadoVariador;
int modelo;
int tensionBus;
int tensionSalida;
float intensidadsalida;
float velocidadRPM;
float senalA1;
float senalA2;
float senalA3;
bool error485=true;
int numeroErrores=0;
//WIFI 
WiFiManager wm;
String IpAsignada="";
WebServer server(80);

void setup() {
 Serial.begin(9600);
  // Inicializa la comunicación RS485 con la placa XY485
  Serial2.begin(BAUD_RATE_RS485, SERIAL_8N1, TX_RS485, RX_RS485); // Configura los pines  (TX2) y (RX2) para RS485
  // Inicializa el objeto ModbusMaster
  node.begin(DIR_ID_MODBUS, Serial2); // El número 3 es la dirección del dispositivo Modbus
//configuramos pantalla
   if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)){
    Serial.println("FALLO EN PANTALLA");
  }



//CONFIGURACION WIFI
WiFi.mode(WIFI_STA);
  bool res;
  res=wm.autoConnect("CONTROLVIT","123456789");
  if (!res)
  {
    Serial.println("Fallo de conexion");
    printOLEDbienvenida();
    
  }else{

    
    IpAsignada=WiFi.localIP().toString();
    //CONFIGURAMOS EL WEBSERVER PARA CONFIGURAR LOS VALORES DE REGISTROS
    server.on("/",handle_OnConnect);
    server.on("/data", HTTP_GET, getData);
    server.onNotFound(handle_NotFound); 
    server.begin();
    Serial.println("Servidor HTTP iniciado");
  }


}

void loop() {

  server.handleClient();//INICIAMOS EL WEBSERVER

  getDataModBus(8448);
  delay(500);
  getDataModbusDos(12288);

  printVariables();

  if(error485){
    numeroErrores++;
    if(numeroErrores==3)
    {
      printHelp();
    }
    
    
  }else
  {
    printMenu();
    numeroErrores=0;
  }
  
  delay(1000); // Espera un segundo antes de realizar la siguiente lectura

}



void handle_OnConnect(){
  server.send(200, "text/html", sendHtml()); 

}
void getData(){

  String dataJson="{\"modelo\":"+String(modelo)+
  ",\"hz\":"+String(frecuencia)+
  ",\"estado\":"+String(estadoVariador)+
  ",\"Tbus\":"+String(tensionBus)+
  ",\"Tsalida\":"+String(tensionSalida)+
  ",\"Isalida\":"+String(intensidadsalida)+
  ",\"rpm\":"+String(frecuencia*60)+
  ",\"A1\":"+String(senalA1)+
  ",\"A2\":"+String(senalA2)+
  ",\"A3\":"+String(senalA3)  +"}";

  server.send(200,"text/plain",dataJson);

}
void handle_NotFound(){
  server.send(404, "text/plain", "ERROR 404 La pagina no existe");
}


String sendHtml(){
  const char index_html[] PROGMEM = R"rawliteral(
 <!DOCTYPE html><html lang="es"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>CONTROLVIT</title><style>body{background-color:#f0f0f0;justify-content:center;align-items:center;height:100vh;margin:0;font-family:Arial,sans-serif;display:block}.header-container{display:flex;align-items:center;justify-content:center;color:#fff;text-anchor:middle;font-family:sans-serif;font-size:20px;font-weight:700;background-color:#cb302d;border-bottom-left-radius:25px;border-bottom-right-radius:25px;padding-top:10px}.container{display:flex;align-items:center;justify-content:space-around}.container-card{margin-top:60px;display:flex;justify-content:space-around}.card{background-color:#fff;border-radius:20px;display:flex;padding:20px;box-shadow:0 0 10px rgba(0,0,0,.2);width:215px;height:215px;margin-left:20px}.card-text{background-color:#fff;border-radius:20px;padding:20px;box-shadow:0 0 10px rgba(0,0,0,.2);width:215px;height:215px;margin-left:20px}.card-titulo{display:flex;align-items:center;justify-content:center;color:#818181;text-anchor:middle;font-family:sans-serif;font-size:20px;font-weight:700}.card-value{display:flex;align-items:center;justify-content:center;color:#0c0c0c;text-anchor:middle;font-family:sans-serif;font-size:40px;font-weight:700}.card-senal{font-size:20px;font-family:sans-serif;align-items:center}.card-item-senal{display:flex;align-items:center;justify-content:space-around}.gauge{width:205px;height:205px}@media screen and (max-width:430px){.container-card{margin-top:60px;display:block;justify-content:space-between;align-items:center}.card{width:115px;height:115px;margin-top:10px}.card-text{margin-top:20px;width:115px;height:115px}.gauge{width:105px;height:105px}.card-titulo{font-size:10px}.card-value{font-size:20px}.card-senal{font-size:10px}}</style></head><body><div class="header-container"><p>CONTROLVIT<samp id="modelo">n/a</samp></p></div><div class="container"><div class="container-card"><div class="card-text"><div class="card-titulo"><p>Frecuencia</p></div><div class="card-value"><p id="hz">-</p></div></div><div class="card-text"><div class="card-titulo"><p>RPM</p></div><div class="card-value"><p id="RPM">-</p></div></div></div><div class="container-card"><div class="card-text"><div class="card-titulo"><p>Estado</p></div><div class="card-value"><p id="estado">OK</p></div></div><div class="card-text"><div class="card-titulo"><p>CO2</p></div><div class="card-value"><p id="co2">2,33 kg</p></div></div></div></div><div class="container"><div class="container-card"><div class="card-text"><div class="card-titulo"><p>Tension de salida</p></div><div class="card-value"><p id="Tsalida">230v</p></div></div><div class="card-text"><div class="card-titulo"><p>Intensidad de salida</p></div><div class="card-value"><p id="Isalida">10A</p></div></div></div><div class="container-card"><div class="card-text"><div class="card-titulo"><p>Tension bus dc</p></div><div class="card-value"><p id="Tbus">411V</p></div></div><div class="card-text"><div class="card-titulo"><p>Estado señales</p></div><div class="card-senal"><div class="card-item-senal"><p>A1:</p><p id="A1">2.3V</p></div><div class="card-item-senal"><p>A2:</p><p id="A2">1.8V</p></div><div class="card-item-senal"><p>A3:</p><p id="A3">0.0V</p></div></div></div></div></div><script>// definimos programa
const a1=document.getElementById("A1");
const a2=document.getElementById("A2");
const a3=document.getElementById("A3");
const Tbus=document.getElementById("Tbus");
const Isalida=document.getElementById("Isalida");
const Tsalida=document.getElementById("Tsalida");
const co2=document.getElementById("co2");
const estado=document.getElementById("estado");
const modelo=document.getElementById("modelo");
const frecuencia=document.getElementById("hz");
const rpm=document.getElementById("RPM");


function consultaGet(){
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
           if (xmlhttp.status == 200) {
                let data= JSON.parse(xmlhttp.responseText);
                //estado.innerText=data.estado;
                a1.innerText=data.A1 + "V";
                a2.innerText=data.A2+ "V";
                a3.innerText=data.A3+ "V";
                Tbus.innerText=data.Tbus+ "V";
                Isalida.innerText=data.Isalida+ "A";
                Tsalida.innerText=data.Tsalida+ "V";
                frecuencia.innerText=data.hz;
                rpm.innerText=data.rpm;

                co2.innerText="n/a";
                switch(data.modelo){
                    case 269:
                    modelo.innerText=" CV-10";
                        break;
                    case 262:
                    modelo.innerText=" CV-30";
                        break;
                    case 263:
                    modelo.innerText=" CV-50";
                        break;
                    default:
                    modelo.innerText=" na";
                        break;
                }

                switch(data.estado){
                    case 1:
                    estado.innerText="Adelante";
                    break;
                    case 2:
                    estado.innerText="Inverso";
                    break;
                    case 3:
                    estado.innerText="Paro";
                    break;
                    case 4:
                    estado.innerText="Fallo";
                    break;
                    case 5:
                    estado.innerText="POFF";
                    break;
                    case 6:
                    estado.innerText="Preexcitacion";
                    break;
                    default:
                    estado.innerText="N/A";

                    break;
                }

              //estado.innerText=data.estado;

           }
           else {
              console.log('error', xmlhttp);
           }
        }
    };
    xmlhttp.open("GET", "data", true);//data TIENE QUE SER UN ENDPOINT DEFINIDO EN NUESTRA ESP32
    xmlhttp.send();

    }

    (function scheduleAjax() {
    consultaGet();                       //  funcion EJECUTAR LA FUNCION CADA SEGUNDO
    setTimeout(scheduleAjax, 1000);
    })();</script></body>
        )rawliteral";
      return index_html;

}


void printOLEDbienvenida(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Bienvenido");
  //-----------------
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(" Wifi no configurada ");
  //---------------------
  display.setCursor(0, 30);
  display.println("1.Conec. wifi Controlvit");
  display.setCursor(0, 40);
  display.println("2.Pass wifi:123456789");
  display.setCursor(0, 50);
  display.println("3.Conectate y Renicie");
 

  display.display();

}

void resetSettingWifi()
{
  wm.resetSettings();  
}


void getDataModBus(int Dirmodbus){

  uint8_t result;
  uint16_t data;

  // Realiza una lectura de un registro Modbus (por ejemplo, el registro 0x0001)
  result = node.readHoldingRegisters(Dirmodbus, 4);

  // Verifica si la lectura fue exitosa
  if (result == node.ku8MBSuccess) {
  
    error485=false;
    
    estadoVariador=node.getResponseBuffer(0);
    modelo=node.getResponseBuffer(3);



    Serial.println(estadoVariador);
    Serial.println(modelo);
    
  } else {
   Serial.print("Error al leer el registro. Código de error: ");
   // Serial.println(result);
    error485=true;
  }
}

void getDataModbusDos(int Dirmodbus)
{
  uint8_t result;
  uint16_t data;

  // Realiza una lectura de un registro Modbus (por ejemplo, el registro 0x0001)
  result = node.readHoldingRegisters(Dirmodbus,16);

  // Verifica si la lectura fue exitosa
  if (result == node.ku8MBSuccess) {
  
    error485=false;
    Serial.println(node.getResponseBuffer(0));
    frecuencia=float(node.getResponseBuffer(0))/100;
    


    tensionBus=float(node.getResponseBuffer(2))/10;
    tensionSalida=(node.getResponseBuffer(3));
    intensidadsalida=float(node.getResponseBuffer(4))/10;
    velocidadRPM=(node.getResponseBuffer(5));
    senalA1=float(node.getResponseBuffer(12))/100;
    senalA2=float(node.getResponseBuffer(13))/100;
    senalA3=float(node.getResponseBuffer(14))/100;

  

    
    
  } else {
   Serial.print("Error al leer el registro. Código de error: ");
   // Serial.println(result);
    error485=true;
  }

}


void printMenu(){

  display.clearDisplay();
  display.setCursor(0, 1);
  display.drawRoundRect(0,1,SCREEN_WIDTH,SCREEN_HEIGHT-1,5,WHITE);//RECTANGULO EXPERIOR CON 5PX DE REDONDEO 
  display.drawLine(59,1,59,(SCREEN_HEIGHT-12),WHITE);//LINEA QUE SEPARA LA PANTALLA POR LA MITAD
  display.drawLine(0,SCREEN_HEIGHT-12,SCREEN_WIDTH,(SCREEN_HEIGHT-12),WHITE); //LINEA ENCIMA IP
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, SCREEN_HEIGHT-10);
  display.println(" IP:"+ String(IpAsignada));
  display.setCursor(2, 6);
  display.println("Velocidad");
  display.setCursor(5, 19);
  display.setTextSize(2);
  
  display.println(int(frecuencia));
  display.setCursor(30, 26);
  display.setTextSize(1);
  display.println("Hz");
  display.setCursor(5, 40);
  
  display.println(int(velocidadRPM));
  display.setCursor(30, 40);

  display.println("rpm");

  display.setCursor(62, 6);
  display.setTextSize(1);
  display.println(" Potencia");
  display.setCursor(69, 20);
  display.setTextSize(2);
  display.println(intensidadsalida);
  display.setCursor(117, 26);
  display.setTextSize(1);
  display.println("A");
  display.setCursor(69, 40);
  display.println(int(tensionSalida));
  display.setCursor(89, 40);
  display.println("v");
  display.display();

}

 void printHelp(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(2, 5);
  display.println(" Error Comunicacion");
  display.setTextSize(1);
  display.setCursor(33, 16);
  display.println("P14.00 = 3"); // configurar direccion modbus 3 al variador 
  display.setCursor(33, 27);
  display.println("P14.01 = 3");// configurar variador con 9600 bps
  display.setCursor(33, 39);
  display.println("P14.02 = 0");//
  display.setCursor(24, 51);
  display.println("Jumper 4 = ON");//
  display.drawRoundRect(0,1,SCREEN_WIDTH,SCREEN_HEIGHT-1,5,WHITE);//RECTANGULO EXPERIOR CON 5PX DE REDONDEO 
  display.display();

 }
 
void calculateCo2(){
//1kwh --> 0,138 kg de c02

}
String getEstadoName(int estado){

String state;

  switch (estado)
  {
  case 1:
    state="Adelante";
    break;
    case 2:
    state="Inverso";
    break;
    case 3:
    state="Paro";
    break;
    case 4:
    state="Fallo";
    break;
    case 5:
    state="POFF";
    break;
    case 6:
    state="Preexcitacion";
    break;
  
  default:
      state="N/A";

    break;
  }
  return state;
}

void printVariables(){
  Serial.print("frecuencia:");
  Serial.println(frecuencia);
    Serial.print("Estado:");

  Serial.println(estadoVariador);
    Serial.print("Modelo:");

  Serial.println(modelo);
    Serial.print("T. BUS:");

  Serial.println(tensionBus);
    Serial.print("T. SALIDA:");

  Serial.println(tensionSalida);
    Serial.print("RPM:");

     Serial.print("I. SALIDA:");
     Serial.println(intensidadsalida);
    

  Serial.println(velocidadRPM);
    Serial.print("A1:");

  Serial.println(senalA1);
      Serial.print("A2:");

  Serial.println(senalA2);
      Serial.print("A3:");

  Serial.println(senalA3);

}