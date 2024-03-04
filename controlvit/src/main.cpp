#include <Arduino.h>
#include <ModbusMaster.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Preferences.h>
//INICIO VARIABLES DE PANTALLA OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

//FIN VARIABLES DE PANTALLA OLED
//INICIO VARIABLES DE MODBUS

#define TX_RS485 17
#define RX_RS485 16
#define BAUD_RATE_RS485 9600
#define DIR_ID_MODBUS 3

//FIN VARIABLES DE PANTALLA OLED




// Define el objeto de comunicación Modbus RTU, DISPLAY
ModbusMaster node;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//VARIABLES GLOBALES Y FUNCIONES
Preferences preferences;
bool conexionWithInv=false;
WiFiManager wm;
String IpAsignada="";
WebServer server(80);
int ADDRESS_MODBUS= 11015;

void printOLED(String marca,String inversor, String sim);
void printOLEDmensaje(String msj);
void resetSettingWifi();
void printOLEDbienvenida();
String sendHtml();
void handle_NotFound();
void handle_OnConnect();
//FIN VARIABLES GLOBALES 


void setup() {
  // Inicializa la comunicación Serie para la depuración
  Serial.begin(9600);
  // Inicializa la comunicación RS485 con la placa XY485
  Serial2.begin(BAUD_RATE_RS485, SERIAL_8N1, TX_RS485, RX_RS485); // Configura los pines  (TX2) y (RX2) para RS485
  // Inicializa el objeto ModbusMaster
  node.begin(DIR_ID_MODBUS, Serial2); // El número 3 es la dirección del dispositivo Modbus

//FIN CONFIGURACION DE LOS PUERTOS SERIAL

//CONFIGURACION DE LA PANTALLA OLED SSD1306  
  
  if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)){
    Serial.println("FALLO EN PANTALLA");
  }
// FIN CONFIGURACION DE LA PANTALLA OLED SSD1306  

printOLEDbienvenida();

//CONFIGURACION DE LA WIFI
  WiFi.mode(WIFI_STA);
  bool res;
  res=wm.autoConnect("CONTROLVIT","123456789");
  if (!res)
  {
    Serial.println("Fallo de conexion");
    
  }
  else{

    preferences.begin("inversor",false);
    ADDRESS_MODBUS=preferences.getInt("ADDRESS_MODBUS",ADDRESS_MODBUS);
    preferences.end();

    Serial.println("conectado");
    printOLEDmensaje("Cargando...");
    IpAsignada=WiFi.localIP().toString();

    //CONFIGURAMOS EL WEBSERVER PARA CONFIGURAR LOS VALORES DE REGISTROS
    server.on("/",handle_OnConnect);
    //server.on("/api", HTTP_GET, handle_api);
    server.onNotFound(handle_NotFound); 
    server.begin();
    Serial.println("Servidor HTTP iniciado");
  }
  

}

void loop() {
  server.handleClient();//INICIAMOS EL WEBSERVER
  uint8_t result;
  uint16_t data;
  static String comRs485 = "";

  // Realiza una lectura de un registro Modbus (por ejemplo, el registro 0x0001)
  result = node.readHoldingRegisters(ADDRESS_MODBUS, 1);
  // Verifica si la lectura fue exitosa
  if (result == node.ku8MBSuccess) {
  
    data = node.getResponseBuffer(0);

    if (data==0)
    {

    Serial.print("Corte de corriente AC,: ");
    Serial.println(data);

    }else{
    //data = node.getResponseBuffer(0);
    Serial.print("Frecuencia de la red: ");
    Serial.println(data);
    comRs485="OK";
    
    }
    
  } else {
    Serial.print("Error al leer el registro. Código de error: ");
    Serial.println(result);
    comRs485="ERROR";
    //comprobaremos los estados para decidir si enviamos mensajes o no 

    
  }

  delay(1000); // Espera un segundo antes de realizar la siguiente lectura
}


void printOLEDmensaje(String msj){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.println(msj);
  display.display();
}


void resetSettingWifi()
{
  wm.resetSettings();  
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
  display.println("1.Conec. wifi CONTROLVIT");
  display.setCursor(0, 40);
  display.println("2.Pass wifi:123456789");
  display.setCursor(0, 50);
  display.println("3.Conectate y Renicie");
 

  display.display();

}


void handle_NotFound() {
  server.send(404, "text/plain", "ERROR 404 La pagina no existe");
}
void handle_OnConnect() {
  
  server.send(200, "text/html", sendHtml()); 
}
void handle_api(){
  server.send(200,"text/plain","hola api");

}

String sendHtml(){
  const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html><html lang="es"><head> <meta charset="UTF-8"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>Monitorizacion</title> <style> body { background-color: #34495e; /* Fondo oscuro */ display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; font-family: Arial, sans-serif; /* Cambio de tipografía */ } .container { background-color: transparent; display: flex; justify-content: center; } .card { background-color: #fff; /* Tarjeta blanca */ border-radius: 20px; /* Bordes redondeados */ display: flex; width: 80%; padding: 20px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.2); } .left { flex: 1; padding: 20px; } h2 { text-align: center; } p { text-align: center; } .checklist { list-style: none; padding: 0; text-align: center; margin-top: 27px; } .checklist li { margin: 10px 0; } .checklist i { color: #27ae60; /* Cambio de color para el ícono de verificación */ margin-right: 10px; } input { width: 80%; /* Ancho reducido */ padding: 10px; margin-top: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; /* Cambio de tamaño de fuente */ } .form{ margin-left: 35px; } button { background-color: #ff7f50; /* Color anaranjado */ color: #fff; /* Color de texto para el botón */ border: none; border-radius: 5px; /* Bordes menos redondeados */ padding: 10px; font-size: 16px; /* Cambio de tamaño de fuente */ cursor: pointer; margin-top: 15px; /* Separación de 15px por arriba */ } button:hover { background-color: #ff6347; /* Cambio de color de fondo en el hover */ } .right { flex: 1; display: flex; justify-content: center; align-items: center; margin-right: 15px; /* Separación de 15px a la derecha de la imagen */ } img { max-width: 100%; height: auto; width: auto; border-radius: 20px; /* Bordes redondeados */ } .apikey{ color:#ccc; cursor: pointer; } .apikey:hover{ color:#34495e; } .apikeyDiv{ margin-top: 10px; margin-left: 30px; } </style></head><body> <div class="container"> <div class="card"> <div class="left"> <h2>Configuración</h2> <p>Proceso de configuración de tu dispositivo</p> <ul class="checklist"> <li><i class="fa fa-check"></i> Fácil de configurar</li> <li><i class="fa fa-check"></i> Acceso 100% local</li> </ul> <form class="form" action="/settings" method='GET'> <input type="text" name="marca" id="marca" placeholder="Marca Inversor" maxlength="21" required> <input type="text" name="direccion" id="direccion" placeholder="Dirección Modbus Hz" required> <input type="tel" name="telefono" id="telefono" placeholder="Teléfono" required> <input type="text" name="apikey" id="apikey" placeholder="API KEY Whatapp Bot" required> <button name='SUBMIT' value='Submit' type="submit">Guardar Cambios</button> </form> <div class="apikeyDiv"> <a href="https://www.callmebot.com/blog/free-api-signal-send-messages/" class="apikey"><small>*Donde consigo el APIKEY</small></a> </div> </div> </div> </div></body></html>
  )rawliteral";
  return index_html;
 }
 
