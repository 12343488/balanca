#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <string>
#include <stdio.h>

#include "NTPClient.h"
#include "ESPAsyncWebServer.h"
#include "LiquidCrystal_I2C.h"
#include "balanca_1.0.cpp"
#include "esp_wpa2.h"

#define FORMAT_SPIFFS_IF_FAILED true

#define LED_EMB 2

#define CIMA 13
#define DIREITA 12
#define BAIXO 14
#define ESQUERDA 27
#define ENTRAR 26
#define SAIR 25

const char ssid[] = "vena";
const char password[] = "veninhas";
// const char ssid[] = "NetVirtua92_2G";
// const char password[] = "14124804";
// const char ssid[] = "Acad-Test";

String ledState;

Reagentes *Lista;

AsyncWebServer server(80);

LiquidCrystal_I2C lcd(0x27,16,2);

void listDir(const char *dirname, uint8_t levels)
{
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = SPIFFS.open(dirname);
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
				//listDir((char*)file.path(), levels - 1);
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

void PrintWebA()
{
	SPIFFS.remove("/Print.html");
	File T = SPIFFS.open("/Print.html", FILE_APPEND);

	std::string a = R"====(
	<!DOCTYPE html>
		<html>
			<head>
				<title>Mostrando Reagentes</title>
  				<meta name="viewport" content="width=device-width, initial-scale=1">
  				<link rel="icon" href="data:,">
 				<link rel="stylesheet" type="text/css" href="style.css">
			</head>
			<body>
	)====";

	T.write((uint8_t *)a.c_str(), a.size());

	T.close();
}

void PrintWebD()
{
	File T = SPIFFS.open("/Print.html", FILE_APPEND);

	std::string a = R"====(			<p><a href="/"><button class="button">Voltar</button></a>
			</body>
		</html>
	)====";

	T.write((uint8_t *)a.c_str(), a.size());
	T.close();
}

void PrintMinDisplay(unsigned int Alerta)
{
	Lista->PrintMin(Alerta, 2);

	unsigned int Mostrar = 0;

	if (Lista->PosiveisDistancias.empty())
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Nao tem nenhum reagente baixo do minimo");
	}
	else
	{
		int LugarD1 = 0;
		int LugarD2 = 0;
		int LugarM = 0;
		unsigned int Passadas = 0;
		char a[17];

		bool iniciou1 = true;
		bool iniciou2 = false;
		bool Mudou1 = true;
		bool Mudou2 = true;

		while (!digitalRead(SAIR))
		{
			std::deque<std::string> mostrando = Lista->PrintMin(Lista->PosiveisDistancias[LugarM]);

			if (Passadas == 0)
			{
				if (!iniciou1)
				{
					if (LugarD1 + 16 <= mostrando[0].size())
					{
						lcd.setCursor(0, 0);

						memcpy(a, mostrando[0].c_str() + LugarD1, 16);
						a[16] = '\0';

						lcd.print(a);
					}
					else
					{
						lcd.setCursor(0, 0);
						lcd.print(mostrando[0].c_str());
						LugarD1 = 0;

						iniciou1 = true;
					}
				}
				else
				{
					iniciou1 = false;
					continue;
				}

				LugarD1++;
			}

			if (Passadas == 0 || Mudou2)
			{
				if (!iniciou2)
				{
					if (LugarD2 + 16 <= mostrando[1].size())
					{
						lcd.setCursor(0, 1);

						memcpy(a, mostrando[1].c_str() + LugarD2, 16);
						a[16] = '\0';
						lcd.print(a);
					}
					else
					{
						lcd.setCursor(0, 1);
						lcd.print(mostrando[1].c_str());
						LugarD2 = 0;

						iniciou2 = true;
					}
				}
				else
				{
					iniciou2 = false;
					continue;
				}

				LugarD2++;
				Mudou2 = false;
			}

			if (Passadas >= 15)
				Passadas = 0;
			else
				Passadas++;

			if (digitalRead(BAIXO) && (LugarM + 1 < Lista->PosiveisDistancias.size()))
			{
				LugarM++;
				LugarD1 = 0;
				LugarD2 = 0;

				lcd.clear();

				delay(200);
			}

			if (digitalRead(CIMA) && LugarM - 1 >= 0)
			{
				LugarM--;
				LugarD1 = 0;
				LugarD2 = 0;

				lcd.clear();
				delay(200);
			}

			delay(50);
		}
	}
}

void PrintValidadeDislpay(unsigned int Alerta)
{
	Lista->ChecarValidade();
	Lista->PrintValidade(Alerta, 2);

	unsigned int Mostrar = 0;

	if (Lista->PosiveisDistancias.empty())
	{
		Serial.println("vazio");

		while (!digitalRead(SAIR))
		{
			lcd.setCursor(0, 0);
			lcd.print("Nenhum reagent");
			lcd.setCursor(0, 1);
			lcd.print("e abaixo do minimo");
			delay(50);
		}
	}
	else
	{
		Serial.println("entrou else val");
		int LugarD1 = 0;
		int LugarD2 = 0;
		int LugarM = 0;
		int MDisplay2 = 1;
		unsigned int Passadas = 0;
		char a[17];

		bool iniciou1 = true;
		bool iniciou2 = false;

		while (!digitalRead(SAIR))
		{
			std::deque<std::string> mostrando = Lista->PrintValidade(Lista->PosiveisDistancias[LugarM]);

			if (Passadas == 0)
			{
				lcd.clear();

				if (!iniciou1)
				{
					if (LugarD1 + 16 <= mostrando[0].size())
					{
						lcd.setCursor(0, 0);

						memcpy(a, mostrando[0].c_str() + LugarD1, 16);
						a[16] = '\0';

						lcd.print(a);
					}
					else
					{
						lcd.setCursor(0, 0);
						lcd.print(mostrando[0].c_str());
						LugarD1 = 0;

						iniciou1 = true;
					}
				}
				else
				{
					iniciou1 = false;
					continue;
				}

				LugarD1++;
			}

			if (Passadas == 0)
			{
				if (!iniciou2)
				{
					if (LugarD2 + 16 <= mostrando[MDisplay2].size())
					{
						lcd.setCursor(0, 1);

						memcpy(a, mostrando[MDisplay2].c_str() + LugarD2, 16);
						a[16] = '\0';
						lcd.print(a);
					}
					else
					{
						lcd.setCursor(0, 1);
						lcd.print(mostrando[MDisplay2].c_str());
						LugarD2 = 0;

						iniciou2 = true;
					}
				}
				else
				{
					iniciou2 = false;
					continue;
				}

				LugarD2++;
			}

			if (Passadas >= 13)
				Passadas = 0;
			else
				Passadas++;

			if (digitalRead(BAIXO) && (LugarM + 1 < Lista->PosiveisDistancias.size()))
			{
				LugarM++;
				LugarD1 = 0;
				LugarD2 = 0;
				MDisplay2 = 1;

				delay(200);
			}

			if (digitalRead(CIMA) && LugarM - 1 >= 0)
			{
				LugarM--;
				LugarD1 = 0;
				LugarD2 = 0;
				MDisplay2 = 1;

				delay(200);
			}

			if (digitalRead(DIREITA) && ((MDisplay2 + 1) < (mostrando.size())))
			{
				LugarD2 = 0;
				MDisplay2++;

				delay(200);
			}

			if (digitalRead(ESQUERDA) && (MDisplay2 - 1) >= 1)
			{
				LugarD2 = 0;
				MDisplay2--;

				delay(200);
			}

			delay(50);
		}
	}
}

void PrintAlteradosDislpay(unsigned int Alerta)
{
	Lista->SalvarAlteracao();
	Serial.println("Salvou");
	Lista->PrintAlterados(2);
	Serial.println("Saiu da primeira leitura");

	unsigned int Mostrar = 0;

	Serial.println("Tamanho dentro d funcao");
	Serial.println(Lista->PosiveisDistancias.size());

	if (Lista->PosiveisDistancias.empty())
	{
		Serial.println("vazio");

		while (!digitalRead(SAIR))
		{
			lcd.setCursor(0, 0);
			lcd.print("Nenhum reagent");
			lcd.setCursor(0, 1);
			lcd.print("e abaixo do minimo");
			delay(50);
		}
	}
	else
	{
		Serial.println("entrou else val");
		int LugarD1 = 0;
		int LugarD2 = 0;
		int LugarM = 0;
		int MDisplay2 = 1;
		unsigned int Passadas = 0;
		char a[17];

		bool iniciou1 = true;
		bool iniciou2 = false;

		Serial.println(Lista->PosiveisDistancias.size());

		Serial.print("Tamanho alterados dentro do Display: ");
		Serial.println(Lista->Alterados.size());

		while (!digitalRead(SAIR))
		{
			Serial.print("Tamanho alterados dentro da func Display antes da : ");
			Serial.println(Lista->Alterados.size());
			std::deque<std::string> mostrando = Lista->PrintAlterados(Lista->PosiveisDistancias[LugarM], true);
			Serial.print("Tamanho alterados dentro da func Display depois da : ");
			Serial.println(Lista->Alterados.size());

			if (Passadas == 0)
			{
				Serial.print("Tamanho alterados dentro da func Display: ");
				Serial.println(Lista->Alterados.size());

				lcd.clear();
				delay(200);

				if (!iniciou1)
				{
					if (LugarD1 + 16 <= mostrando[0].size())
					{
						lcd.setCursor(0, 0);

						memcpy(a, mostrando[0].c_str() + LugarD1, 16);
						a[16] = '\0';

						lcd.print(a);
					}
					else
					{
						lcd.setCursor(0, 0);
						lcd.print(mostrando[0].c_str());
						LugarD1 = 0;

						iniciou1 = true;
					}
				}
				else
				{
					iniciou1 = false;
					continue;
				}

				if (mostrando.size() > 1)
				{
					if (!iniciou2)
					{
						if (LugarD2 + 16 <= mostrando[MDisplay2].size())
						{
							lcd.setCursor(0, 1);

							memcpy(a, mostrando[MDisplay2].c_str() + LugarD2, 16);
							a[16] = '\0';
							lcd.print(a);
						}
						else
						{
							lcd.setCursor(0, 1);
							lcd.print(mostrando[MDisplay2].c_str());
							LugarD2 = 0;

							iniciou2 = true;
						}
					}
					else
					{
						iniciou2 = false;
						continue;
					}

					LugarD2++;
				}

				LugarD1++;
			}

			if (Passadas >= 10)
				Passadas = 0;
			else
				Passadas++;

			if (digitalRead(BAIXO) && (LugarM + 1 < Lista->PosiveisDistancias.size()))
			{
				LugarM++;
				LugarD1 = 0;
				LugarD2 = 0;
				MDisplay2 = 1;

				delay(200);
			}

			if (digitalRead(CIMA) && LugarM - 1 >= 0)
			{
				LugarM--;
				LugarD1 = 0;
				LugarD2 = 0;
				MDisplay2 = 1;

				delay(200);
			}

			if (digitalRead(DIREITA) && ((MDisplay2 + 1) < (mostrando.size())))
			{
				LugarD2 = 0;
				MDisplay2++;

				delay(200);
			}

			if (digitalRead(ESQUERDA) && (MDisplay2 - 1) >= 1)
			{
				LugarD2 = 0;
				MDisplay2--;

				delay(200);
			}

			if (digitalRead(SAIR))
			{
				delay(50);
				return;
			}

			if (digitalRead(ENTRAR))
			{
				if (Lista->PosiveisDistancias[LugarM] == ((unsigned int)-1))
				{
					Serial.print	("Tamanho alterados indo pra salvar: ");
					Serial.println(Lista->Alterados.size());
					Lista->SalvarMudAut();

					Serial.println("Salvou");

					return;
				}
			}

			delay(50);
		}
	}
}

int DisplaySelectA()
{
	int saida = 0;
	int voltas = 0;

	while (true)
	{

		if (voltas == 0)
		{
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Escolha o Alerta");
			lcd.setCursor(0, 1);
			lcd.print(std::to_string(saida).c_str());
		}

		if (voltas == 15)
			voltas = 0;
		else
			voltas++;

		if (digitalRead(BAIXO))
		{
			saida--;
			delay(200);
		}
		else if (digitalRead(CIMA))
		{
			if (saida >= 0)
				saida++;
			delay(200);
		}
		else if (digitalRead(ENTRAR))
		{
			return saida;
			delay(200);
		}
		else if (digitalRead(SAIR))
		{
			return -1;
			delay(200);
		}

		delay(50);
	}
}

void setup()
{
	// Serial port for debugging purposes
	Serial.begin(115200);
	pinMode(LED_EMB, OUTPUT);

	pinMode(CIMA, INPUT_PULLDOWN);
	pinMode(DIREITA, INPUT_PULLDOWN);
	pinMode(BAIXO, INPUT_PULLDOWN);
	pinMode(ESQUERDA, INPUT_PULLDOWN);
	pinMode(ENTRAR, INPUT_PULLDOWN);
	pinMode(SAIR, INPUT_PULLDOWN);

	// Initialize SPIFFS
	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	Lista = new Reagentes("a");

	Serial.println("Pesos: ");
	Serial.print("Salvo: ");
	Serial.println(Lista->PesoSalvo);
	Serial.print("Atual: ");
	Serial.println(Lista->PesoAtual);
	// Connect to Wi-Fi
	WiFi.begin(ssid, password);

	Serial.print("Connecting to WiFi..");
	for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++)
	{
		delay(100);
		Serial.print(".");
	}
	Serial.println();

	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());

	// cria as paginas do site
	{
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/inicio.html"); 
	});

	server.on("/NomeEspI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/PesquisarNomeI.html"); 
	});

	server.on("/NomeEspP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");
		AsyncWebParameter* p = request->getParam("nome");

		Serial.println(p->value().c_str());
		
		PrintWebA();
		Lista->PrintEspecifico(std::string(p->value().c_str()), 1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/PesoEspI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/PesquisarPesoI.html"); 
	});

	server.on("/PesoEspP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");
		AsyncWebParameter* p = request->getParam("peso");

		Serial.println(p->value().c_str());
		
		PrintWebA();
		Lista->PrintEspecifico(p->value().toFloat(), 1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/ValidadeEspI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/PesquisarValidadeI.html"); 
	});

	server.on("/ValidadeEspI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/PesquisarValidadeI.html"); 
	});

	server.on("/ValidadeEspP", HTTP_GET, [](AsyncWebServerRequest *request)
			  { 
		Serial.println("redirecionou");
		AsyncWebParameter* p = request->getParam("mes");
		int mes = p->value().toInt();
		Serial.println(p->value().c_str());

		p = request->getParam("ano");
		int ano = p->value().toInt();

		Serial.println(p->value().c_str());
		
		PrintWebA();
		Lista->PrintEspecifico(mes, ano, 1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); });

	server.on("/VencidosI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/VencidosI.html"); 
	});

	server.on("/VencidosP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");

		AsyncWebParameter *p = request->getParam("alerta");
		Serial.println(p->value().c_str());

		PrintWebA();
		Lista->ChecarValidade();
		Lista->PrintValidade(p->value().toInt(), 1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/Infos", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");

		PrintWebA();
		Lista->PrintInfos(1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/PesoMinI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/PesoMinI.html"); 
	});

	server.on("/PesoMinP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");

		AsyncWebParameter *p = request->getParam("alerta");
		Serial.println(p->value().c_str());

		PrintWebA();
		Lista->ChecarValidade();
		Lista->PrintMin(p->value().toInt(), 1);
		Serial.println("opa");
		PrintWebD();
		Serial.println("opa");
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/Nomes", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");

		PrintWebA();
		Lista->PrintNomes(1);
		PrintWebD();
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/Alterados", HTTP_POST, [](AsyncWebServerRequest *request)
			  { 
		Lista->SalvarMudAut();

		request->send(200); });

	server.on("/Alterados", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("Alterados");

		std::string a = R"====(
		<!DOCTYPE html>
		<html>
			<head>
				<title>Mostrando Reagentes</title>
  				<meta name="viewport" charset = "utf-8" content="width=device-width, initial-scale=1">
				<script> 
				function Salvar()
				{
					var a = new XMLHttpRequest();
					a.open("POST", "/Alterados", true);											   // inicializa uma nova requisição, ou reinicializa uma requisição já existente.
					a.setRequestHeader("Content-Type", "application/x-www-form-urlencoded"); // define o valor do cabeçalho de uma requisição HTTP
					a.onreadystatechange = function()
					{
						if (this.readyState == 4 && this.status == 200)
						{
							// Typical action to be performed when the document is ready:
							alert("Requisição enviada com sucesso");
						}
						else if (this.readyState == 4 && this.status != 200)
						{
							alert("Requisição não foi enviada, pois houve alguma falha");
						}
					};
					a.send("salvar=1"); // envia uma requisição para o servidor.
				}
				</script>
				<link rel="icon" href="data:,">
 				<link rel="stylesheet" type="text/css" href="style.css">
			</head>
			<body>
			)====";

		SPIFFS.remove("/Print.html");
		File T = SPIFFS.open("/Print.html", FILE_APPEND);

		T.write((uint8_t *)a.c_str(), a.size());

		T.close();

		Lista->SalvarAlteracao();
		Lista->PrintAlterados(1);

		a ="<p><button onclick = \"Salvar()\"> Salvar as indicacoes</button>\n";

		T = SPIFFS.open("/Print.html", FILE_APPEND);

		if (!Lista->Alterados.empty())
		{
			a ="<p><button onclick = \"Salvar()\"> Salvar as indicacoes</button>\n";
			T.write((uint8_t *)a.c_str(), a.size());
		}
		else
		{
			a = "<p><button onclick = \"Salvar()\"> Confimar as mudancas - (não muda nada automaticamente)</button>\n";
			T.write((uint8_t *)a.c_str(), a.size());
		}

		T.close();
		PrintWebD();
		request->send(SPIFFS, "/Print.html"); 
	});

	server.on("/CriarRI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/CriarReagI.html"); 
	});

	server.on("/CriarRP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		Serial.println("redirecionou");

		AsyncWebParameter *p[8];
		p[0] = request->getParam("Nome");
		p[1] = request->getParam("PesoT");
		p[2] = request->getParam("PesoC");
		p[3] = request->getParam("PesoM");
		p[4] = request->getParam("Dia");
		p[5] = request->getParam("Mes");
		p[6] = request->getParam("Ano");
		p[7] = request->getParam("Alerta");

		Lista->CriarReagente(p[0]->value().c_str(), p[1]->value().toFloat(), p[2]->value().toFloat(), p[3]->value().toFloat(), p[4]->value().toInt(), p[5]->value().toInt(), p[6]->value().toInt(), p[7]->value().toInt());
		request->redirect("/"); 
	});

	server.on("/RemoverRI", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		request->send(SPIFFS, "/RemoverReagI.html"); 
	});

	server.on("/RemoverRP", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		if(request->hasParam("nome"))
		{
			AsyncWebParameter *p = request->getParam("nome");
			std::string Nome = p->value().c_str();
			Lista->EscolherReag(Nome);
			if (Lista->PosiveisDistancias.size() > 1)
			{
				PrintWebA();

				File T = SPIFFS.open("/Print.html", FILE_APPEND);
				std::string a;
				int b = 0;

				for (std::deque<unsigned int>::iterator i = Lista->PosiveisDistancias.begin(); i < Lista->PosiveisDistancias.end(); i++, b++)
				{
					T = SPIFFS.open("/Print.html", FILE_APPEND);
					a = "\n<h1 class=\"nome\"><b>" + std::to_string(b) + "</b></h1>";
					
					T.write((uint8_t *)a.c_str(), a.size());
					T.close();

					Lista->PrintInfo((*i) - 1, 1);
				}

				T = SPIFFS.open("/Print.html", FILE_APPEND);

				a = std::string(R"====(
					<form action="\RemoverRP" method="get">
						<p><input type="number" Min="0" Max=")====") +
					std::to_string(Lista->PosiveisDistancias.size() - 1) +
					R"====(				" name="posicao" id="posicao" class="button">
					<p><button type="submit" class="button">REMOVER</button>
					</form>)====";

				T.write((uint8_t *)a.c_str(), a.size());
				T.close();

				PrintWebD();

				request->send(SPIFFS, "/Print.html");
			}
			else if (Lista->PosiveisDistancias.size() == 1)
			{
				request->redirect("/RemoverRP?posicao=0");
			}
		}

		if (request->hasParam("posicao"))
		{
			AsyncWebParameter *p = request->getParam("posicao");
			Lista->RemoverReagente(Lista->PosiveisDistancias[p->value().toInt()]);
			request->redirect("/");
		}
	});

	server.on("/MudarP", HTTP_GET, [](AsyncWebServerRequest *request) 
	{
		if(request->hasParam("peso"))
		{
			AsyncWebParameter *p = request->getParam("peso");
			Lista->PesoAtual = p->value().toFloat();
			Lista->VerificarPeso();
			request->redirect("/");
		}
		else
		{
			Serial.println(Lista->PesoAtual);
			request->send(SPIFFS, "/MudarPeso.html");
		}
	});

	server.on("/MudarPR", HTTP_GET, [](AsyncWebServerRequest *request)
	{ 
		if(request->hasParam("nome"))
		{
			AsyncWebParameter *p = request->getParam("nome");
			std::string Nome = p->value().c_str();
			Lista->EscolherReag(Nome);

			p = request->getParam("peso");
			Nome = p->value().c_str();
			if (Lista->PosiveisDistancias.size() > 1)
			{
				PrintWebA();

				File T = SPIFFS.open("/Print.html", FILE_APPEND);
				std::string a;
				int b = 0;

				for (std::deque<unsigned int>::iterator i = Lista->PosiveisDistancias.begin(); i < Lista->PosiveisDistancias.end(); i++, b++)
				{
					T = SPIFFS.open("/Print.html", FILE_APPEND);
					a = "\n<h1 class=\"nome\"><b>" + std::to_string(b) + "</b></h1>";
					
					T.write((uint8_t *)a.c_str(), a.size());
					T.close();

					Lista->PrintInfo((*i) - 1, 1);
				}

				T = SPIFFS.open("/Print.html", FILE_APPEND);

				a = std::string(R"====(
					<form action="\MudarPR" method="get">
						<p><input type="number" min="0" max=")====") +
					std::to_string(Lista->PosiveisDistancias.size() - 1) + R"====(				" name="posicao" id="posicao" class="button"> Numero do Escolido
    					<p><input type="hidden" name="peso" id="peso" class="button" value=")====" +
					Nome + R"====("></p>
					<p><button type="submit" class="button">ESCOLHER</button>
					</form>)====";

				T.write((uint8_t *)a.c_str(), a.size());
				T.close();

				PrintWebD();

				request->send(SPIFFS, "/Print.html");
			}
			else if (Lista->PosiveisDistancias.size() == 1)
			{
				std::string a = "/MudarPR?posicao=0&peso=" + Nome;
				request->redirect(a.c_str());
			}
		} 
		
		if (request->hasParam("posicao"))
		{
			AsyncWebParameter *p = request->getParam("posicao");
			int Posi = p->value().toInt();

			p = request->getParam("peso");
			Lista->TrocarPeso((int)Lista->PosiveisDistancias[Posi], p->value().toFloat());
			request->redirect("/");
		}

		request->send(SPIFFS, "/MudarPesoReag.html");
	});

	server.on("/LimpH", HTTP_GET, [](AsyncWebServerRequest *request)
			  { 
		Lista->LimparHistorico();
		request->redirect("/"); });

	server.on("/Limpar", HTTP_GET, [](AsyncWebServerRequest *request)
			  { 
		Lista->Limpar();
		request->redirect("/"); });
	}
	
	// Start server
	server.begin();

	lcd.begin();
	lcd.backlight();
}

int LugarMain = 0;
bool MainM = true;
int voltasMain = 0;

void loop()
{
	if (MainM)
	{
		lcd.clear();

		Serial.println("mudou");

		if (LugarMain == 0)
		{
			lcd.print("Ver os Minimos");
			MainM = false;
		}

		if (LugarMain == 1 && MainM && voltasMain == 0)
		{
			lcd.print("Ver os Vencidos");
			MainM = false;
		}

		if (LugarMain == 2 && MainM && voltasMain == 0)
		{
			lcd.print("Ver os Alterados");
			MainM = false;
		}
	}

	if (digitalRead(ENTRAR))
	{
		MainM = true;

		delay(200);
		if (LugarMain == 0)
		{
			int a = DisplaySelectA();

			delay(200);

			if (a > -1)
				PrintMinDisplay(a);
		}
		else if (LugarMain == 1)
		{
			int a = DisplaySelectA();

			delay(200);

			if (a > -1)
				PrintValidadeDislpay(a);
		}
		else if (LugarMain == 2)
		{
			int a = DisplaySelectA();

			delay(200);

			if (a > -1)
				PrintAlteradosDislpay(a);
		}
	}

	if (digitalRead(BAIXO))
	{
		if (LugarMain >= 1)
		{
			LugarMain--;
			MainM = true;
		}

		delay(200);
	}

	if (digitalRead(CIMA))
	{
		if (LugarMain <= 2)
		{
			LugarMain++;
			MainM = true;
		}

		delay(200);
	}

	delay(50);
}