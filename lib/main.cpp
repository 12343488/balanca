#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <string>

#include "NTPClient.h"
#include "ESPAsyncWebServer.h"
#include "balanca_1.0.cpp"

#define FORMAT_SPIFFS_IF_FAILED true

#define LED_EMB 2

// const char ssid[] = "vena";
// const char password[] = "veninhas";
const char ssid[] = "NetVirtua92_2G";
const char password[] = "14124804";

AsyncWebServer server(80);

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = fs.open(dirname);
	if (!root)
	{
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file)
	{
		if (file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels)
			{
				listDir(fs, file.path(), levels - 1);
			}
		}
		else
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

String Proc(const String& var) 
{
	if(var == "vari")
		return "oi";

	return String();
}

void test()
{
	File T = SPIFFS.open("/a.html", FILE_APPEND);

	std::string a = R"====(
	<html>
		<head>
 			<meta charset='utf-8'/>
		</head>
		<body>
	)====";

	T.write((uint8_t*)a.c_str(), a.size());

	a = R"====(
			<h1>
				oi
			</h1>
	)====";

	T.write((uint8_t *)a.c_str(), a.size());

	a = R"====(
		</body>
	</html>
	)====";

	T.write((uint8_t *)a.c_str(), a.size());
}

void setup()
{
	pinMode(LED_EMB, OUTPUT);

	Serial.begin(115200);
	SPIFFS.begin();
	WiFi.begin(ssid, password);

	Serial.print("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.print('.');
	}
	Serial.println();
	Serial.println(ssid);
	Serial.println(WiFi.localIP());

	// disponibiliza o url "/"
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
	{
    	request->redirect("/page2"); // redireciona para page2 
	});

	// disponibiliza o url "/page2"
	server.on("/page2", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/pagina.html", String(), false, Proc);
	});

	server.on("/page4", HTTP_ANY, [](AsyncWebServerRequest *request)
	{
    	// Listar todos os parâmetros
    	int params = request->params(); // parâmetros
    	int i; // índice
    	for (i = 0; i < params; i++)
		{
      		AsyncWebParameter* p = request->getParam(i); // pega o cabeçalho do índice informado
      		if (p->isFile()) // se parâmetro for um arquivo, ...
      		{
      		  Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      		} 
			else if (p->isPost()) // senão, se parâmetro for do tipo POST, ...
      		{
      			Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      		} 
			else // senão, parâmetro é do tipo GET, ...
      		{
       			Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
     		}
    	}
    	String html = R"====(
    	<html>
  			<head>
    			<meta charset = "utf-8">
    	<script>
     		function imprimeNaSerial()
      		{
        		var xhr = new XMLHttpRequest();
        		xhr.open("POST", "/page4", true); // inicializa uma nova requisição, ou reinicializa uma requisição já existente.
		        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded"); // define o valor do 	cabeçalho de uma requisição HTTP
        		xhr.onreadystatechange = function() {
          		if (this.readyState == 4 && this.status == 200) 
				{
            		// Typical action to be performed when the document is ready:
            		alert("Requisição enviada com sucesso");
          		}
				else if(this.readyState == 4 && this.status != 200)
				{
          			alert("Requisição não foi enviada, pois houve alguma falha");
          		}
        	};
        	xhr.send("parametro1=123&outro-paramentro=xyz"); // envia uma requisição para o servidor.
      		}
    	</script>
  	</head>
  	<body>
    	<h1>Teste com ESP32</h1>
    	<p> Clique no botão abaixo e imprima na Serial dois Parâmetros POST (parametro1 com valor 123 e outro-parametro com valor xyz): </p>
    	<button onclick="imprimeNaSerial()"> Imprimir Parâmetros POST na Serial</button>
  	</body>
	</html>
    )====";
    request->send(200,"text/html",html); 
	});

	server.on("/page5", HTTP_GET, [](AsyncWebServerRequest *request)
	{
    	// Verifique se o parâmetro GET existe ou não
   		if (request->hasParam("nome-qualquer")) {
    	  AsyncWebParameter* p = request->getParam("nome-qualquer");
    	  Serial.printf("O parâmetro GET %s existe e possui o valor %s\n", p->name().c_str(), p->value().c_str());
    	}else{
    	  Serial.print("O parâmetro GET nome-qualquer não existe nesta requisição\n");
    	} 
	});

	// disponibiliza o url "/" por acesso somente via POST
	server.on("/page6", HTTP_POST, [](AsyncWebServerRequest *request)
			  {
    // Verifique se o parâmetro POST existe ou não
    if (request->hasParam("nome-qualquer", true)) 
	{
      AsyncWebParameter* p = request->getParam("nome-qualquer", true);
      Serial.printf("O parâmetro POST %s existe e possui o valor %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.print("O parâmetro POST nome-qualquer não existe nesta requisição\n");
    }
    request->send(200); 
	});

	// disponibiliza o url "/" por acesso somente via GET
	server.on("/page6", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
    String html = R"====(
    <html>
  <head>
    <meta charset = "utf-8">
    <script>
      function imprimeNaSerial() // função, que quando clicar no botão, será enviado uma requisição POST para o server
      {
        var xhr = new XMLHttpRequest();
        xhr.open("POST", '/page6', true); // inicializa uma nova requisição, ou reinicializa uma requisição já existente.
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded"); // define o valor do cabeçalho de uma requisição HTTP
        xhr.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
             // Typical action to be performed when the document is ready:
            alert("Requisição enviada com sucesso");
          }else if(this.readyState == 4 && this.status != 200){
          alert("Requisição não foi enviada, pois houve alguma falha");
          }
        };
        xhr.send(""); // envia uma requisição para o servidor.
      }
    </script>
  </head>
  <body>
    <h1>Teste com ESP32</h1>
    <p> Clique no botão abaixo e imprima na Serial a verificação se um Parâmetro POST (nome-qualquer com valor valor-qualquer) existe ou não: </p>
    <button onclick="imprimeNaSerial()"> Imprimir Parâmetros POST na Serial</button>
  </body>
</html>
    )====";
    request->send(200, "text/html", html); 
	});

	server.on("/page7", HTTP_POST, [](AsyncWebServerRequest *request)
	{
    	// Verifique se o parâmetro POST existe ou não
    	if (request->hasParam("mostrar", true)) 
		{
      		AsyncWebParameter* p = request->getParam("mostrar", true);
      		Serial.printf("O parâmetro POST %s existe e possui o valor %s\n", p->name().c_str(), p->value().c_str());
			test();
			request->send(100);
		}
		else if (request->hasParam("redirecionar", true))
		{
			AsyncWebParameter *p = request->getParam("redirecionar", true);
			Serial.printf("O parâmetro POST %s existe e possui o valor %s\n", p->name().c_str(), p->value().c_str());
			test();
			Serial.println("redirecionando");
			request->redirect("/page8");
			//request->send(200);
		}
		else 
		{
      		Serial.print("O parâmetro POST nome-qualquer não existe nesta requisição\n");
    	} 
	});


	server.on("/page7", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/paginaT.html", String(), false);
	});

	server.on("/page8", HTTP_GET, [](AsyncWebServerRequest *request)
			  { 
		Serial.println("redirecionado");
		request->send(SPIFFS, "/pagina.html", String(), false); });

	server.begin();

	listDir(SPIFFS, "/", 0);
}

void loop()
{
	digitalWrite(LED_EMB, HIGH);
	delay(2);
	digitalWrite(LED_EMB, LOW);
	delay(5000);
}