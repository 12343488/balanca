#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <time.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <WiFiUdp.h>

#include "NTPClient.h"

#define NumeroAlertas 2

bool ValidadeInput(std::string input, float &result)
{
	bool negative = false;

	bool fraction = false;

	int FractionPosition = 0;

	result = 0;

	for (int i = 0; i < input.size(); i++)
	{
		float num = 0;

		switch (input[i])
		{
		default:
			return false;
			break;

		case '0':
			num = 0;
			break;

		case '1':
			num = 1;
			break;

		case '2':
			num = 2;
			break;

		case '3':
			num = 3;
			break;

		case '4':
			num = 4;
			break;

		case '5':
			num = 5;
			break;

		case '6':
			num = 6;
			break;

		case '7':
			num = 7;
			break;

		case '8':
			num = 8;
			break;

		case '9':
			num = 9;
			break;

		case '-':
			if (i == 0)
				negative = true;
			else
			{
				return false;
			}
			break;

		case '.':
			if (fraction == false)
			{
				fraction = true;
			}
			else
			{
				return false;
			}
			break;

		case ',':
			if (fraction == false)
			{
				fraction = true;
			}
			else
			{
				return false;
			}
			break;
		}

		if (!fraction)
		{
			result *= 10;

			result += num;
		}
		else
		{
			if (!(input[i] == '.' || input[i] == ','))
			{
				num /= pow(10, (i - FractionPosition));

				result += num;
			}
			else
			{
				FractionPosition = i;
			}
		}
	}

	if (negative == true)
	{
		result *= -1;
	}

	return true;
}

bool ValidadeInput(std::string input)
{
	float a;
	return ValidadeInput(input, a);
}

struct Reagentes
{
public:
	const float DesvioAceitavel = 0; // para +/-
	std::string Nome;
	float PesoSalvo, PesoAtual;

	struct ReagLP // Lugar e peso de um reagente
	{
		int Lugar;
		float Peso;

		ReagLP(int L, float P) : Lugar(L), Peso(P){};
	};

	std::deque<unsigned int> PosiveisDistancias;	// salva depois das marcas
	std::deque<ReagLP> Alterados;					// salva o index(depois da marca) e a Diferenca dos reagentes alterados enquanto o combo for true

	Reagentes(std::string Nome) : Nome(Nome), PesoAtual(0), PesoSalvo(0)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);
		File ReagH = SPIFFS.open(ArqH.c_str(), FILE_APPEND);

		if (Reag)
		{
			int fim;

			if (fim <= 1)
			{
				fim = Reag.size();

				uint8_t Marca;

				//Ver qual é o peso denrto da balanca sem checar o historico
				while (Reag.position() < fim)
				{

					Reag.read(&Marca, sizeof(char));

					if (Marca == 0xFF)
					{
						unsigned int NomeSize;
						Reag.read((uint8_t *)&NomeSize, sizeof(int));

						Reag.seek(NomeSize, SeekCur);

						float Peso;

						Reag.read((uint8_t *)&Peso, sizeof(float));

						PesoSalvo += Peso;

						Reag.seek((sizeof(float) * 2) + (sizeof(int) * 4), SeekCur);
					}
				}

				Marca = 0xFF;
				ReagH.write((uint8_t *)&Marca, sizeof(char));
				// Peso atual vai ser o peso no sensor
				PesoAtual = PesoSalvo;
				ReagH.write((uint8_t *)&PesoAtual, sizeof(float));
			}
			else
			{
				ReagH.seek(fim - (sizeof(float) + sizeof(uint8_t)));

				uint8_t marca;

				ReagH.read(&marca, sizeof(uint8_t));

				if (marca == 0xFF)
				{
					ReagH.read((uint8_t *)&PesoSalvo, sizeof(float));
				}
			}
		}
		else
		{
			// std::cout << "Essa lista ainda nao foi inicializada!!!\n";
			Serial.println("Essa lista ainda nao foi inicializada!!!");
		}
	}

	Reagentes() : Nome(""), PesoAtual(0), PesoSalvo(0)
	{			};

	void CriarReagente(std::string Nome, float Peso, float PesoCont, float PesoMin, int dia, int mes, int ano, int Alerta)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_APPEND);

		if (!Reag)
		{
			Serial.println("problema ao abrir o arquvo!!\tLinha: 155");
		}
		uint8_t Marca = 0XFF;
		Reag.write(&Marca, sizeof(char));

		unsigned int NomeSize = Nome.size();
		Reag.write((uint8_t *)&NomeSize, sizeof(unsigned int));
		Reag.write((uint8_t *)Nome.c_str(), NomeSize);
		Reag.write((uint8_t *)&Peso, sizeof(float));
		Reag.write((uint8_t *)&PesoCont, sizeof(float));
		Reag.write((uint8_t *)&PesoMin, sizeof(float));
		Reag.write((uint8_t *)&dia, sizeof(int));
		Reag.write((uint8_t *)&mes, sizeof(int));
		Reag.write((uint8_t *)&ano, sizeof(int));
		Reag.write((uint8_t *)&Alerta, sizeof(int));

		// temporario
		PesoSalvo += Peso;

		Reag.close();
	}

	bool RemoverReagente(int LocalReag)
	{
		Serial.println(LocalReag);

		if (LocalReag > -1)
		{
			if (LocalReag > 1)
				GetAntes(Arq, LocalReag - 1);
			GetDepois(Arq, LocalReag - 1);

			File Reag = SPIFFS.open(Arq.c_str());
			File ReagA = SPIFFS.open(ArqA.c_str());
			File ReagD = SPIFFS.open(ArqD.c_str());

			Serial.println("TAMANHOS");
			Serial.println(ReagA.size());
			Serial.println(ReagD.size());

			WriteAntes(Arq);
			WriteDepois(Arq);

			Serial.println(Reag.size());

			SPIFFS.remove(ArqA.c_str());
			SPIFFS.remove(ArqD.c_str());

			return true;
		}
		else
		{
			return false;
		}
	}

	bool TrocarPeso(int LocalReag, float NovoPeso)
	{
			if (LocalReag > -1)
			{
				GetAntes(Arq, LocalReag - 1);

				// pega as informa��es excluido os pesos

				File Reag = SPIFFS.open(Arq.c_str());

				if (!Reag)
				{
					std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
					return false;
				}

				Reag.seek(LocalReag - 1);

				uint8_t Marca = 0;

				Reag.read(&Marca, sizeof(char));

				if (Marca != 0xFF)
				{
					std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
				}

				unsigned int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				uint8_t NomeR[NomeSize];

				Reag.read(NomeR, NomeSize);

				float PesoAntigo;

				Reag.seek(sizeof(float), SeekCur);

				uint8_t Outros[(sizeof(float) * 2) + (sizeof(int) * 4)];

				Reag.read(Outros, (sizeof(int) * 4) + (sizeof(float) * 2));

				Reag.close();

				GetDepois(Arq, LocalReag - 1);

				WriteAntes(Arq);

				Reag.close();
				Reag = SPIFFS.open(Arq.c_str(), FILE_APPEND);

				Reag.write(&Marca, sizeof(char));
				Reag.write((uint8_t *)&NomeSize, sizeof(unsigned int));
				Reag.write(NomeR, NomeSize);
				Reag.write((uint8_t *)&NovoPeso, sizeof(float));
				Reag.write(Outros, sizeof(int) * 4 + (sizeof(float) * 2));

				PesoSalvo += NovoPeso - PesoAntigo;

				Reag.close();

				WriteDepois(Arq);

				SPIFFS.remove(ArqA.c_str());
				SPIFFS.remove(ArqD.c_str());

				return true;
			}
			else
			{
				return false;
			}
	}

	bool TrocarValidade(int LocalReag, int NovoDia, int NovoMes, int NovoAno)
	{
		if ((NovoDia > 0 && NovoDia < 32) && (NovoMes > 0 && NovoMes < 13))
		{
			if (LocalReag > -1)
			{
				GetAntes(Arq, LocalReag - 1);

				// pega as informa��es excluido os pesos
				File Reag = SPIFFS.open(Arq.c_str());

				if (!Reag)
				{
					std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
					return false;
				}

				Reag.seek(LocalReag - 1);

				uint8_t Marca = 0;

				Reag.read(&Marca, sizeof(char));

				if (Marca != 0xFF)
				{
					std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
				}

				unsigned int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				uint8_t Nome[NomeSize];

				Reag.read(Nome, NomeSize);

				uint8_t Outros[(sizeof(float) * 3) + sizeof(int)];

				Reag.read(Outros, (sizeof(float) * 2));

				Reag.seek(sizeof(int) * 3, SeekCur);

				Reag.read((Outros + (sizeof(float) * 3)), sizeof(int));

				Reag.close();

				GetDepois(Arq, LocalReag - 1);

				WriteAntes(Arq);

				Reag = SPIFFS.open(Arq.c_str(), FILE_APPEND);

				Reag.write(&Marca, sizeof(char));
				Reag.write((uint8_t *)&NomeSize, sizeof(unsigned int));
				Reag.write(Nome, NomeSize);
				Reag.write(Outros, sizeof(float) * 3);
				Reag.write((uint8_t *)&NovoDia, sizeof(int));
				Reag.write((uint8_t *)&NovoMes, sizeof(int));
				Reag.write((uint8_t *)&NovoAno, sizeof(int));
				Reag.write((Outros + (sizeof(float) * 3)), sizeof(int));

				Reag.close();

				WriteDepois(Arq);

				SPIFFS.remove(ArqA.c_str());
				SPIFFS.remove(ArqD.c_str());

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			std::cout << "informa��es invalidas!!\n";
		}
	}

	void ChecarValidade()
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);
		SPIFFS.remove(ArqV.c_str());
		File ReagV = SPIFFS.open(ArqV.c_str(), FILE_APPEND);

		unsigned int Fim = Reag.size();

		while (Reag.position() < Fim)
		{
			uint8_t Marca = 0;

			Reag.read(&Marca, sizeof(char));

			if (Marca != 0xFF)
				Serial.println("Problema o ler o arquivo!!!\tLinha: 447");

			unsigned int ComecoReag = Reag.position();

			unsigned int NomeSize;
			Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

			Reag.seek(NomeSize + (sizeof(float) * 3), SeekCur);

			int dia, mes, ano, Alerta;

			Reag.read((uint8_t *)&dia, sizeof(int));
			Reag.read((uint8_t *)&mes, sizeof(int));
			Reag.read((uint8_t *)&ano, sizeof(int));
			Reag.read((uint8_t *)&Alerta, sizeof(int));

			WiFiUDP ntpUDP;
			NTPClient ntp(ntpUDP);
			ntp.begin();

			ntp.setTimeOffset(-14400);
			ntp.forceUpdate();

			tm *Tempo;
			time_t segundos = ntp.getEpochTime();
			Tempo = localtime(&segundos);

			if (ano == Tempo->tm_year + 1900)
			{
				if (mes == Tempo->tm_mon + 1)
				{
					if (dia <= Tempo->tm_mday)
					{
						Marca = 0xFF;
						ReagV.write((uint8_t *)&Marca, sizeof(char));
						ReagV.write((uint8_t *)&ComecoReag, sizeof(unsigned int));
						ReagV.write((uint8_t *)&Alerta, sizeof(int));
					}
				}
				else if (mes < Tempo->tm_mon + 1)
				{
					Marca = 0xFF;
					ReagV.write((uint8_t *)&Marca, sizeof(char));
					ReagV.write((uint8_t *)&ComecoReag, sizeof(unsigned int));
					ReagV.write((uint8_t *)&Alerta, sizeof(int));
				}
			}
			else if (ano < Tempo->tm_year + 1900)
			{
				Marca = 0xFF;
				ReagV.write((uint8_t *)&Marca, sizeof(char));
				ReagV.write((uint8_t *)&ComecoReag, sizeof(unsigned int));
				ReagV.write((uint8_t *)&Alerta, sizeof(int));
			}
		}
		Reag.close();
		ReagV.close();
	}

	void VerificarPeso()
	{
		if (PesoSalvo != PesoAtual)
		{
			File ReagH = SPIFFS.open(ArqH.c_str(), FILE_APPEND);
			uint8_t Marca = 0xFF;
			ReagH.write(&Marca, sizeof(char));
			ReagH.write((uint8_t *)&PesoAtual, sizeof(float));

			PesoSalvo = PesoAtual;

			ReagH.close();
		}
	}

	void SalvarAlteracao()
	{
		File Reag = SPIFFS.open(Arq.c_str());
		File ReagH = SPIFFS.open(ArqH.c_str());
		SPIFFS.remove(ArqM.c_str());
		File ReagA = SPIFFS.open(ArqM.c_str(),  FILE_APPEND);

		if (!Reag)
			std::cout << "Problema ao abrir o arquivo!!!\tLinha: 645\n";

		if (!ReagH)
			std::cout << "Problema ao abrir o arquivo!!!\tLinha: 648\n";

		if (!ReagA)
			std::cout << "Problema ao abrir o arquivo!!!\tLinha: 650\n";

		unsigned int ReagHEnd = ReagH.size();

		float PesoAnt = -1;

		while (ReagH.position() < ReagHEnd)
		{
			uint8_t Marca;
			ReagH.read(&Marca, sizeof(char));

			float a = 100;

			if (Marca != 0xFF)
				std::cout << "Problemas ao ler o arquivo!!\tLinha: 988\n";

			if (PesoAnt > -1)
			{
				float Diferenca, PesoVer;

				ReagH.read((uint8_t *)&PesoVer, sizeof(float));

				Diferenca = PesoVer - PesoAnt;

				if (Diferenca != 0)
				{
					unsigned int ReagFim = Reag.size();

					unsigned int ReagAFim = ReagA.size();

					bool VariosCompativeis = false;
					bool Compativel = false;

					Marca = 0xFF;
					ReagA.write(&Marca, sizeof(char));

					ReagA.write((uint8_t *)&Diferenca, sizeof(float));

					int AlertaM = 0;

					// tudo s� vai escrever se a Diferenca for negativa pois apnas escreve informa�oes relevantes para retiradas.
					{
						//Lista Reag
						Reag.seek(0, SeekSet);

						while (Reag.position() < ReagFim && Diferenca < 0)
						{
							unsigned int InicioReag, NomeSize;
							int Alerta;
							float PesoReag;

							Reag.read(&Marca, sizeof(char));

							if (Marca != 0xFF)
							{
								std::cout << "Problemas ao ler o arquivo!!\tLinha: 685\n";
								delay(1000);
							}

							InicioReag = Reag.position();

							Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

							Reag.seek(NomeSize, SeekCur);

							Reag.read((uint8_t *)&PesoReag, sizeof(float));

							Reag.seek((sizeof(float) * 2) + (sizeof(int) * 3), SeekCur);

							Reag.read((uint8_t *)&Alerta, sizeof(int));

							if ((-Diferenca >= PesoReag - DesvioAceitavel) && (-Diferenca <= PesoReag + DesvioAceitavel))
							{
								if (VariosCompativeis)
								{
									Marca = -2;

									ReagA.write(&Marca, sizeof(char));
								}

								ReagA.write((uint8_t *)&InicioReag, sizeof(unsigned int));

								VariosCompativeis = true;
								Compativel = true;

								if (Alerta > AlertaM)
									AlertaM = Alerta;
							}
						}

						bool sinal = -1; //-1 inicializa, true - positivo, false = negativo
						bool AchouA = false;
						unsigned int InicioReag;
						unsigned int ReagAParado;
						ReagAParado = 0;

						ReagA.seek(0, SeekSet);

						//Lista Reags Mudados
						while ((ReagA.position() < ReagAFim) && (Diferenca < 0))
						{
							ReagA.close();
							ReagA = SPIFFS.open(ArqM.c_str(), FILE_READ);
							ReagA.seek(ReagAParado);

							int Alerta;
							float PesoReag;

							ReagA.read(&Marca, sizeof(char));

							if (Marca != 0xFF)
							{
								Serial.println("Problemas ao ler o arquivo!!\tLinha: 675\n");
								delay(1000);
							}

							if (Marca == 0xFF)
							{
							
								ReagA.read((uint8_t*)&PesoReag, sizeof(float));

								AchouA = false;

								if (PesoReag < 0)
								{
									ReagA.read((uint8_t *)&InicioReag, sizeof(int));

									sinal = false;
								}

								if (PesoReag > 0)
									sinal = true;

								ReagA.read(&Marca, sizeof(char));

								ReagA.seek(-(long long)sizeof(char), SeekCur);

								if (Marca != 0xFF && Marca != 0xFE && sinal == false)
								{
									ReagA.read((uint8_t *)&Alerta, sizeof(int));

									if (AlertaM < Alerta)
										AlertaM = Alerta;
								}

								ReagAParado = ReagA.position();

								if (((-Diferenca >= PesoReag - DesvioAceitavel) && (-Diferenca <= PesoReag + DesvioAceitavel)) && sinal == true)
								{
									ReagA.close();
									ReagA = SPIFFS.open(ArqM.c_str(), FILE_APPEND);

									if (VariosCompativeis)
									{
										Marca = 0xFE;

										ReagA.write(&Marca, sizeof(char));
									}

									ReagA.write((uint8_t *)&InicioReag, sizeof(unsigned int));

									ReagA.close();

									VariosCompativeis = true;
									AchouA = true;
									Compativel = true;
								}
							}
							else if (Marca == 0xFE && AchouA)
							{
								ReagA.read((uint8_t *)&InicioReag, sizeof(int));

								ReagA.read(&Marca, sizeof(char));
								if (Marca == 0xFF || Marca == 0xFE)
									ReagA.seek(-(long long)sizeof(char), SeekCur);
								else if (AlertaM < Marca)
									AlertaM = Marca;

								ReagAParado = ReagA.position();

								ReagA.close();
								ReagA = SPIFFS.open(ArqM.c_str(), FILE_APPEND);

								Marca = 0xFE;
								ReagA.write(&Marca, sizeof(char));

								ReagA.write((uint8_t *)&InicioReag, sizeof(unsigned int));
								VariosCompativeis = true;
								Compativel = true;

								ReagA.close();
								ReagA = SPIFFS.open(ArqM.c_str(), FILE_READ);

								ReagA.seek(ReagAParado);
							}
						}

						if (Compativel)
						{
							ReagA.close();
							ReagA = SPIFFS.open(ArqM.c_str(), FILE_APPEND);
							ReagA.write((uint8_t *)&AlertaM, sizeof(int));
						}

						if (!Compativel && Diferenca < 0)
						{
							ReagA.close();
							ReagA = SPIFFS.open(ArqM.c_str(), FILE_APPEND);

							int Temp = -1;
							ReagA.write((uint8_t *)&Temp, sizeof(int));

							ReagA.close();
							ReagA = SPIFFS.open(ArqM.c_str(), FILE_READ);
						}
					}

					PesoAnt = PesoVer;
				}
			}
			else
			{
				float PesoVer = 0;

				ReagH.read((uint8_t *)&PesoVer, sizeof(float));
				
				PesoAnt = PesoVer;
			}
		}

		Reag.close();
		ReagH.close();
		ReagA.close();
	}

	void ConfirmarAlteracao()
	{
		File ReagH = SPIFFS.open(ArqH.c_str());
		File ReagHA = SPIFFS.open(ArqHA.c_str(), FILE_APPEND);

		if (!ReagH)
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 783\n";

		if (!ReagHA)
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 787\n";

		uint8_t a = 'a';
		uint8_t *TempA = &a;

		int Fim = ReagH.size();

		if (Fim - ReagH.position() > 0)
		{
			bool NewUsado = false;

			if (Fim - ReagH.position() > 100)
			{
				TempA = new uint8_t[100];
				NewUsado = true;
			}

			while (Fim - ReagH.position() > 100)
			{
				ReagH.read(TempA, 100);

				ReagHA.write(TempA, 100);
			}

			if (NewUsado)
			{
				delete[] TempA;
			}

			int lugar = ReagH.position();

			if (Fim - ReagH.position())
			{
				TempA = new uint8_t[Fim - lugar];

				ReagH.read(TempA, Fim - lugar);

				ReagHA.write(TempA, Fim - lugar);

				delete[] TempA;
			}

			ReagH.close();
			ReagHA.close();

			ReagH = SPIFFS.open(ArqH.c_str(), FILE_WRITE);

			a = 0xFF;

			ReagH.write(&a, sizeof(char));
			ReagH.write((uint8_t *)&PesoAtual, sizeof(float));

			ReagH.close();
		}
	}

	void LimparHistorico()
	{
		SPIFFS.remove(ArqH.c_str());
	}

	void SalvarMudAut()
	{
		Serial.println("entrou na func");
		Serial.print("tamanho Alt; ");
		Serial.println(Alterados.size());

		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		delay(2000);

		if (!Alterados.empty())
		{
			Serial.println("não vazio");
			for (std::deque<ReagLP>::iterator i = Alterados.begin(); i < Alterados.end(); i++)
			{
				Serial.println("Entrou no loop");
				uint8_t Marca;
				Reag.seek(i->Lugar - 1);

				Reag.read(&Marca, sizeof(char));

				if (Marca != 0xFF)
					std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1462\n";

				int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(int));
				Reag.seek(NomeSize, SeekCur);

				float PesoA;

				Reag.read((uint8_t *)&PesoA, sizeof(float));

				PesoSalvo -= PesoA - i->Peso;

				Serial.print("Peso anterior: ");
				Serial.println(PesoA);
				Serial.print("Novo Peso: ");
				Serial.println(i->Peso);

				TrocarPeso(i->Lugar, i->Peso);
			}
		}

		ConfirmarAlteracao();

		Reag.close();
	}

	void Limpar()
	{
		SPIFFS.remove(Arq.c_str());
		SPIFFS.remove(ArqH.c_str());
	}

	void PrintMin(int Alerta, int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
			Serial.println("Problemas ao abrir o arquivo\tLinha: 860");

		int ReagFim = Reag.size();

		PosiveisDistancias.clear();

		while (Reag.position() < ReagFim)
		{
			uint8_t Marca = 0;

			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize, Lugar;
				float PesoR, PesoC, PesoM;
				int AlertaR;

				Lugar = Reag.position();

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				char NomeReag[NomeSize + 1];

				Reag.read((uint8_t *)NomeReag, NomeSize);

				NomeReag[NomeSize] = '\0';

				Reag.read((uint8_t *)&PesoR, sizeof(float));
				Reag.read((uint8_t *)&PesoC, sizeof(float));
				Reag.read((uint8_t *)&PesoM, sizeof(float));

				Reag.seek((uint32_t)(sizeof(int) * 3), SeekCur);

				Reag.read((uint8_t *)&AlertaR, sizeof(int));

				if (AlertaR >= Alerta)
				{
					if (PesoR - PesoC <= PesoM)
					{
						if(PrintT == 0)
						{
							Serial.print("\n\n-------------------------\nO Reagente ");
							Serial.print(NomeReag);
							Serial.print(" esta com ");
							Serial.print(PesoR - PesoC);
							Serial.print(" g sobrando e precisa de no minimo: ");
							Serial.print(PesoM);
							Serial.println(" g");
						}
						else if (PrintT == 1)
						{
							File T = SPIFFS.open("/Print.html", FILE_APPEND);
							std::string a;
							a = std::string("					\n<h1 class=\"nome\">O Reagente: <b>") + NomeReag + "</b></h1>" +
								"					\n<p class=\"texto\">esta com: " + std::to_string(PesoR - PesoC) + "g sobrando e precisa de no minimo: " + std::to_string(PesoM) + "g";

							T.write((uint8_t *)a.c_str(), a.size());
							T.close();
						}
						else if(PrintT == 2)
						{
							PosiveisDistancias.push_back(Lugar);
						}
					}
				}
			}
		}
	}
	
	std::deque<std::string> PrintMin(unsigned int Lugar)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
			Serial.println("Problemas ao abrir o arquivo\tLinha: 929");

		int ReagFim = Reag.size();

		Reag.seek(Lugar - 1);

		while (Reag.position() < ReagFim)
		{
			uint8_t Marca = 0;

			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize, Lugar;
				float PesoR, PesoC, PesoM;
				int AlertaR;

				Lugar = Reag.position();

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				char NomeReag[NomeSize + 1];

				Reag.read((uint8_t *)NomeReag, NomeSize);

				NomeReag[NomeSize] = '\0';

				Reag.read((uint8_t *)&PesoR, sizeof(float));
				Reag.read((uint8_t *)&PesoC, sizeof(float));
				Reag.read((uint8_t *)&PesoM, sizeof(float));

				Reag.seek((uint32_t)(sizeof(int) * 3), SeekCur);

				Reag.read((uint8_t *)&AlertaR, sizeof(int));

				std::deque<std::string> saida;
				std::string a;

				a = std::string("Reagente: ") + NomeReag;
				saida.push_back(a);

				a = std::string("tem: ") + std::to_string(PesoR - PesoC) + "g, Precisa de: " + std::to_string(PesoM) + "g.";
				saida.push_back(a);

				return saida;
			}
		}
	}

	void PrintNomes(int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
			Serial.println("Problemasao abrir o arquivo!!\tLinha: 1365");

		unsigned int Fim = Reag.size();

		while (Reag.position() < Fim)
		{
			uint8_t Marca = 0;

			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				char NomeReag[NomeSize + 1];

				Reag.read((uint8_t *)&NomeReag, NomeSize);

				NomeReag[NomeSize] = '\0';

				if(PrintT == 0)
					std::cout << NomeReag << '\n';
				else if (PrintT == 1)
				{
					File T = SPIFFS.open("/Print.html", FILE_APPEND);
					std::string a;
					a = std::string("					\n<h1 class=\"nome\"><b>") + NomeReag + "</b></h1>";

					T.write((uint8_t *)a.c_str(), a.size());
				}
					
				Reag.seek((sizeof(float) * 3) + (sizeof(int) * 4), SeekCur);
			}
		}
	}

	void PrintInfos(int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
		{
			Serial.println("Problema ao abrir o arquivo!!\tLinha: 1398");
			return;
		}

		unsigned int Fim = Reag.size();

		while (Reag.position() < Fim)
		{
			uint8_t Marca = 0;

			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				PrintInfo(Reag.position() - sizeof(int) - sizeof(char), PrintT);
				Reag.seek(NomeSize + (sizeof(int) * 4) + (sizeof(float) * 3), SeekCur);
			}
		}
	}

	void PrintValidade(int Tipo, int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);
		File ReagV = SPIFFS.open(ArqV.c_str(), FILE_READ);

		PosiveisDistancias.clear();

		unsigned int Fim = ReagV.size();
		Serial.println(ReagV.size());

		while (ReagV.position() < Fim)
		{
			uint8_t Marca = 0;
			ReagV.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int comeco = ReagV.position();
				unsigned int ComecoReag;
				int Alerta;
				ReagV.read((uint8_t *)&ComecoReag, sizeof(unsigned int));
				ReagV.read((uint8_t *)&Alerta, sizeof(int));

				if (Alerta >= Tipo)
				{
					Serial.println("Vencido");
					uint8_t Marca = 0;
					Reag.read(&Marca, sizeof(char));

					if (Marca == 0xFF)
					{
						unsigned int NomeSize;

						Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

						Reag.seek(ComecoReag - 1, SeekSet);

						if(PrintT == 2)
						{
							Serial.println("Salvando");
							PosiveisDistancias.push_back(comeco);
						}
						else
							PrintInfo(Reag.position(), PrintT);
							
						Reag.seek(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 3), SeekCur);
					}
				}
			}
			else
			{
				Serial.println("Problemas ao ler o arquivo!!!\tLinha: 1609");
			}
		}
	}

	std::deque<std::string> PrintValidade(unsigned int Lugar)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);
		File ReagV = SPIFFS.open(ArqV.c_str(), FILE_READ);

		unsigned int Fim = ReagV.size();
		Serial.println(ReagV.size());

		ReagV.seek(Lugar - 1);

		uint8_t Marca = 0;
		ReagV.read(&Marca, sizeof(char));

		std::deque<std::string> saida;

		if (Marca == 0xFF)
		{
			unsigned int ComecoReag;
			int Alerta;
			ReagV.read((uint8_t *)&ComecoReag, sizeof(unsigned int));
			ReagV.read((uint8_t *)&Alerta, sizeof(int));

			uint8_t Marca = 0;
			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;

				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				Reag.seek(ComecoReag - 1, SeekSet);

				PrintInfo(Reag.position(), saida);
				return saida;
			}
		}
		else
		{
			Serial.println("Problemas ao ler o arquivo!!!\tLinha: 1609");
		}
	}

	void PrintEspecifico(std::string NomeR, int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
		}

		bool tem = false;
		unsigned int End = Reag.size();

		while (Reag.position() < End)
		{
			uint8_t Marca = 0;
			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;
				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				if (NomeR.size() == NomeSize)
				{
					char NomeSalvo[NomeSize + 1];
					int NomeInd = 0;

					Reag.read((uint8_t *)NomeSalvo, NomeSize);
					NomeSalvo[NomeSize] = '\0';
					std::string NomeRS = NomeSalvo;

					if (NomeRS == NomeR)
					{
						Reag.seek(-((int)NomeSize + (long)sizeof(int) + (long)sizeof(char)), SeekCur);

						PrintInfo((int)Reag.position(), PrintT);

						Reag.seek(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 3), SeekCur);

						tem = true;
					}
					else
						Reag.seek(((sizeof(float) * 3) + (sizeof(int) * 4)), SeekCur);
				}
				else
				{
					Reag.seek((NomeSize + (sizeof(float) * 3) + (sizeof(int) * 4)), SeekCur);
				}
			}
		}

		if (!tem)
			// std::cout << "\nO Reagente nao existe na lista!!\n";
			Serial.println("O reagente não existe");
	}

	void PrintEspecifico(float Peso, int PrintT)
	{
		// std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());

		if (!Reag)
		{
			// std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
			Serial.println("Problemas ao abrir o arquivo!!!\tLinha: 190");
		}

		bool tem = false;

		unsigned int End = Reag.size();

		// passa pelo arquivo inteiro procurando os nomes
		while (Reag.position() < End)
		{
			uint8_t Marca = 0;
			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;
				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));
				Reag.seek(NomeSize, SeekCur);
				float PesoReag;
				Reag.read((uint8_t *)&PesoReag, sizeof(float));

				if (PesoReag >= (Peso - DesvioAceitavel) && PesoReag <= (Peso + DesvioAceitavel))
				{
					Reag.seek(-((int)NomeSize + (long)sizeof(int) + (long)sizeof(char) + (long)sizeof(float)), SeekCur);

					PrintInfo((int)Reag.position(), PrintT);

					Reag.seek(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 3), SeekCur);
				}
				else
					Reag.seek(((sizeof(float) * 2) + (sizeof(int) * 4)), SeekCur);
			}
		}
		Reag.close();
	}

	void PrintEspecifico(int Mes, int Ano, int PrintT)
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
		}

		bool tem = false;

		unsigned int End = Reag.size(); // salva onde � o final do arquivo

		// passa pelo arquivo inteiro procurando os nomes
		while (Reag.position() < End)
		{
			uint8_t Marca = 0;
			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				Serial.println("passou na marca");
				unsigned int NomeSize;
				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));
				Reag.seek(NomeSize + (sizeof(float) * 3) + sizeof(int), SeekCur);
				int MesR, AnoR;
				Reag.read((uint8_t *)&MesR, sizeof(int));
				Reag.read((uint8_t *)&AnoR, sizeof(int));

				if (MesR == Mes && AnoR == Ano)
				{
					Reag.seek(-((int)NomeSize + (long)(sizeof(int) * 4) + (long)sizeof(char) + (long)(sizeof(float) * 3)), SeekCur);

					PrintInfo((int)Reag.position(), PrintT);

					Reag.seek(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 3), SeekCur);
				}
				else
					Reag.seek((sizeof(int)), SeekCur);
			}
		}
	}

	void PrintAlterados(int PrintT)
	{
		//std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
		//std::fstream ReagA(ArqM.c_str(), std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());
		File ReagA = SPIFFS.open(ArqM.c_str(), FILE_READ);

		Serial.println("Entrou em leitura de alterados");
	
		if (!Reag)
			Serial.println("Problemas ao ler o arquivo!!!\tLinha 1185");

		if (!ReagA)
			Serial.println("Problemas ao ler o arquivo!!!\tLinha 1188");
		

		PosiveisDistancias.clear();
		Alterados.clear();

		unsigned int ReagAFim = ReagA.size();

		float Diferenca = 0;
		bool Combo = true;
		bool EstadoA = -1; // false Diminuiu, true aumentou
		int Alerta;

		if(ReagAFim == 0)
			Serial.println("Histoico vazio");

		while (ReagA.position() < ReagAFim)
		{
			unsigned int LugarReagA = ReagA.position();

			uint8_t Marca = 0;

			ReagA.read(&Marca, sizeof(char));

			if (Marca != 0xFF && Marca != 0xFE)
				Serial.println("Problemas ao ler o arquivo!!!\tLinha 1207");

			bool MultComp = false;

			if (Marca == 0xFE)
				MultComp = true;

			int Lugar, NomeSize;

			if (!MultComp)
				ReagA.read((uint8_t *)&Diferenca, sizeof(float));

			if (Diferenca < 0)
			{
				ReagA.read((uint8_t *)&Lugar, sizeof(int));

				bool leu = ReagA.read(&Marca, sizeof(char));

				if(leu)
					ReagA.seek(-(long long)sizeof(char), SeekCur);

				if (Marca != 0xFF && Marca != 0XFE)
				{
					ReagA.read((uint8_t *)&Alerta, sizeof(int));
				}

				if (Lugar > -1)
				{
					Reag.seek(Lugar - 1);
					Reag.read(&Marca, sizeof(char));

					if (Marca != 0xFF)
					{
						std::cout << "problemas ao ler o arquivo!!\tLinha: 940\n";
					}

					if (!EstadoA && EstadoA != -1)
					{
						Combo = false;
						Alterados.clear();
					}
					EstadoA = false;

					if (Combo)
					{
						Alterados.push_back(ReagLP(Lugar, -1));
					}

					Reag.read((uint8_t *)&NomeSize, sizeof(int));

					uint8_t Nome[NomeSize + 1];

					Reag.read(Nome, NomeSize);

					Nome[NomeSize] = '\0';

					if (!MultComp)
					{
						if(PrintT == 0)
							std::cout
								<< "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferenca << " g\n"
						 		<< "Possivel Reagente Aterado : " << Nome;

						else if (PrintT == 1)
						{
							File T = SPIFFS.open("/Print.html", FILE_APPEND);
							std::string a;
							a = std::string("					\n<p><b>") + "PESO DIMINUIU EM : " + std::to_string(Diferenca) + " g</b>\n" +
							"					<p>Possivel Reagente Aterado: " + (char *)Nome;

							T.write((uint8_t *)a.c_str(), a.size());
						}
						else if(PrintT == 2)
						{
							PosiveisDistancias.push_back(LugarReagA);
						}
					}
					else
					{
						if(PrintT == 0)
							std::cout << " // " << Nome;
						else if (PrintT == 1)
						{
							File T = SPIFFS.open("/Print.html", FILE_APPEND);
							std::string a;
							a = std::string(" // ") + (char *)Nome;

							T.write((uint8_t *)a.c_str(), a.size());
						}

						Combo = false;
						Alterados.clear();
					}
				}
				else
				{
					if(PrintT == 0)
						std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferenca << " g\nNao tem nenhum Reagente compativel!!";
					else if (PrintT == 1)
					{
						File T = SPIFFS.open("/Print.html", FILE_APPEND);
						std::string a;
						a = std::string("					\n<p><b>") + "PESO DIMINUIU EM : " + std::to_string(Diferenca) + " g</b>\n" +
							"					<p>Nao tem nenhum Reagente compativel!!";

						T.write((uint8_t *)a.c_str(), a.size());
					}
					else if (PrintT == 2)
					{
						PosiveisDistancias.push_back(LugarReagA);
					}

					Combo = false;
					Alterados.clear();
				}
			}
			else
			{
				if(PrintT == 0)
					std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferenca << " g\nVerifique os Reagentes possivelmente alterados!!";

				else if (PrintT == 1)
				{
					File T = SPIFFS.open("/Print.html", FILE_APPEND);
					std::string a;
					a = std::string("					\n<p><b>") + "PESO AUMENTOU EM: " + std::to_string(Diferenca) + " g</b>\n";

					T.write((uint8_t *)a.c_str(), a.size());
				}
				else if (PrintT == 2)
				{
					PosiveisDistancias.push_back(LugarReagA);
				}

				if (EstadoA)
				{
					Combo = false;
					Alterados.clear();
				}
				EstadoA = true;

				if (Combo && !Alterados.empty())
				{
					if (Alterados.back().Peso == -1)
					{
						Reag.seek(Alterados.back().Lugar - 1, SeekSet);
						Reag.read(&Marca, sizeof(char));

						if (Marca != 0xFF)
							std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1069\n";

						int NomeSize;

						Reag.read((uint8_t *)&NomeSize, sizeof(int));

						Reag.seek(NomeSize + sizeof(float), SeekCur);

						float PesoC;

						Reag.read((uint8_t *)&PesoC, sizeof(float));

						if (Diferenca >= PesoC)
							Alterados.back().Peso = Diferenca;
						else
						{
							Combo = false;
						}
					}
				}
			}
		}
		std::cout << '\n';

		Serial.print("Tamanho alterados dentro da func: ");
		Serial.println(Alterados.size());

		if(!Alterados.empty() && PrintT == 2)
		{
			PosiveisDistancias.push_back(((unsigned int)-1));
		}

		Reag.close();
		ReagA.close();
	}

	std::deque<std::string> PrintAlterados(int Lugar, bool aleatorio)
	{
		// std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
		// std::fstream ReagA(ArqM.c_str(), std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());
		File ReagA = SPIFFS.open(ArqM.c_str(), FILE_READ);

		if (!Reag)
			Serial.println("Problemas ao ler o arquivo!!!\tLinha 1185");

		if (!ReagA)
			Serial.println("Problemas ao ler o arquivo!!!\tLinha 1188");

		unsigned int ReagAFim = ReagA.size();

		float Diferenca = 0;
		bool Combo = true;
		bool EstadoA = -1; // false Diminuiu, true aumentou
		int Alerta;
		bool entrou = false;

		std::deque<std::string> saida;

		if (Lugar == ((unsigned int)-1))
		{
			std::string a = "Salvar as mudancas?";
			saida.push_back(a);
			return saida;
		}

		ReagA.seek(Lugar);

		while (ReagA.position() < ReagAFim)
		{
			uint8_t Marca = 0;

			bool leu = ReagA.read(&Marca, sizeof(char));

			if (leu)
			{
				if (Marca != 0xFF && Marca != 0xFE)
					Serial.println("Problemas ao ler o arquivo!!!\tLinha 1544");

				bool MultComp = false;

				if (Marca == 0xFE)
					MultComp = true;

				if ((Marca == 0xFF || ReagA.position() >= ReagAFim) && entrou)
				{
					Reag.close();
					ReagA.close();
					return saida;
				}

				int Lugar, NomeSize;

				if (!MultComp)
					ReagA.read((uint8_t *)&Diferenca, sizeof(float));

				if (Diferenca < 0)
				{
					ReagA.read((uint8_t *)&Lugar, sizeof(int));

					leu = ReagA.read(&Marca, sizeof(char));

					if (leu)
						ReagA.seek(-(long long)sizeof(char), SeekCur);

					if (Marca != 0xFF && Marca != 0XFE)
					{
						ReagA.read((uint8_t *)&Alerta, sizeof(int));
					}

					if (Lugar > -1)
					{
						Reag.seek(Lugar - 1);
						Reag.read(&Marca, sizeof(char));

						if (Marca != 0xFF)
						{
							std::cout << "problemas ao ler o arquivo!!\tLinha: 940\n";
						}

						Reag.read((uint8_t *)&NomeSize, sizeof(int));

						uint8_t Nome[NomeSize + 1];

						Reag.read(Nome, NomeSize);

						Nome[NomeSize] = '\0';

						if (!MultComp)
						{
							std::string a = "DIMINUIU EM: " + std::to_string(Diferenca);

							saida.push_back(a);

							saida.push_back((char *)Nome);
						}
						else
						{
							saida.push_back((char *)Nome);
						}
					}
					else
					{
						std::string a = "DIMINUIU EM: " + std::to_string(Diferenca);

						saida.push_back(a);

						a = "Nenhum reagente compativel";
						saida.push_back(a);
					}
				}
				else
				{
					std::string a = "AUMENTOU EM: " + std::to_string(Diferenca);

					saida.push_back(a);
				}
			}
			entrou = true;
		}

		return saida;
	}

	void PrintHistorico()
	{
		File ReagH = SPIFFS.open(ArqH.c_str());
		File ReagHA = SPIFFS.open(ArqHA.c_str());

		if (!ReagH)
			std::cout << "Problema ao abrir o arquivo!!!\tLinha: 1386\n";

		if (!ReagHA)
			std::cout << "Problema ao abrir o arquivo!!!\tLinha: 1389\n";

		unsigned int ReagHEnd;

		ReagHEnd = ReagH.size();

		float PesoAnt = -1;

		while (ReagH.position() < ReagHEnd)
		{
			uint8_t Marca;
			ReagH.read(&Marca, sizeof(char));

			if (Marca != 0xFF)
			{
				std::cout << "Problemas ao ler o arquivo!!\tLinha: 1907\n";
				break;
			}

			if (PesoAnt > -1)
			{
				float Diferenca, PesoVer;

				ReagH.read((uint8_t *)&PesoVer, sizeof(float));

				Diferenca = PesoVer - PesoAnt;

				if (Diferenca != 0)
				{
					if (Diferenca < 0)
						std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferenca << " g";
					else
						std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferenca << " g";

					PesoAnt = PesoVer;
				}
			}
			else
			{
				float PesoVer;

				ReagH.read((uint8_t *)&PesoVer, sizeof(float));

				PesoAnt = PesoVer;
			}
		}

		ReagHEnd = ReagHA.size();

		while (ReagHA.position() < ReagHEnd && ReagHA)
		{
			uint8_t Marca;
			ReagHA.read(&Marca, sizeof(char));

			if (Marca != 0xFF)
			{
				std::cout << "Problemas ao ler o arquivo!!\tLinha: 1951\n";
				break;
			}

			if (PesoAnt > -1)
			{
				float Diferenca, PesoVer;

				ReagHA.read((uint8_t *)&PesoVer, sizeof(float));

				Diferenca = PesoVer - PesoAnt;

				if (Diferenca != 0)
				{
					if (Diferenca < 0)
						std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferenca << " g";
					else
						std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferenca << " g";

					PesoAnt = PesoVer;
				}
			}
			else
			{
				float PesoVer;

				ReagHA.read((uint8_t *)&PesoVer, sizeof(float));

				PesoAnt = PesoVer;
			}
		}

		ReagH.close();
		ReagHA.close();
	}

	void EscolherReag(std::string &NomeR) //retorna o escolhido depois da marca
	{
		File Reag = SPIFFS.open(Arq.c_str(), FILE_READ);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
		}

		bool tem = false;

		unsigned int End = Reag.size();

		PosiveisDistancias.clear();
		
		while (Reag.position() < End)
		{
			uint8_t Marca = 0;
			Reag.read(&Marca, sizeof(char));

			if (Marca == 0xFF)
			{
				unsigned int NomeSize;
				Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

				if (NomeR.size() == NomeSize)
				{
					char NomeSalvo[NomeSize + 1];
					int NomeInd = 0;

					Reag.read((uint8_t *)NomeSalvo, NomeSize);
					NomeSalvo[NomeSize] = '\0';
					std::string NomeRS = NomeSalvo;

					if (NomeRS == NomeR)
					{
						Reag.seek(-(NomeSize + sizeof(int) + sizeof(uint8_t)), SeekCur);

						Marca = 0;
						Reag.read(&Marca, sizeof(char));

						if (Marca == 0xFF)
						{
							PosiveisDistancias.push_back(Reag.position()); // coloca em possiveis distancias a distancia depois dad marca do Reagente achado
							tem = true;
						}

						Reag.seek((sizeof(int) * 5) + NomeSize + (sizeof(float) * 4), SeekCur);
					}
					else
						Reag.seek(((sizeof(float) * 3) + (sizeof(int) * 4)), SeekCur);
				}
				else
				{
					Reag.seek((NomeSize + (sizeof(float) * 3) + (sizeof(int) * 4)), SeekCur);
				}
			}
		}

		Reag.close();
	}

	void PrintInfo(int Lugar, int PrintT)
	{
		// std::fstream Reag(Arq, std::ios::in | std::ios::binary);
		// std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());

		Reag.seek(Lugar, SeekSet);

		int LugarMin = 0;

		uint8_t Marca;
		Reag.read(&Marca, sizeof(char));

		if (Marca != 0xFF)
			std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1667\n";

		// ReagMin.read(&Marca, sizeof(char));

		float Peso, PesoC, PesoM;
		int Dia, Mes, Ano, Alerta, NomeSize;

		Reag.read((uint8_t *)&NomeSize, sizeof(int));

		char NomeReag[NomeSize + 1];

		Reag.read((uint8_t *)NomeReag, NomeSize);
		Reag.read((uint8_t *)&Peso, sizeof(float));
		Reag.read((uint8_t *)&PesoC, sizeof(float));
		Reag.read((uint8_t *)&PesoM, sizeof(float));
		Reag.read((uint8_t *)&Dia, sizeof(int));
		Reag.read((uint8_t *)&Mes, sizeof(int));
		Reag.read((uint8_t *)&Ano, sizeof(int));
		Reag.read((uint8_t *)&Alerta, sizeof(int));

		NomeReag[NomeSize] = '\0';

		// std::cout << '\n' << NomeReag << "\nTotal: " << std::setw(14) << std::left << Peso << "Container: " << PesoC << "\nPeso minimo: " << PesoM << "\nValidade: " << Dia << " / " << Mes << " / " << Ano << "\nALerta : " << Alerta << '\n';

		if (PrintT == 0)
		{
			Serial.print("Nome: ");
			Serial.println(NomeReag);
			Serial.println("Pesos");
			Serial.print("Total: ");
			Serial.print(Peso);
			Serial.print("\t\tContainer: ");
			Serial.println(PesoC);
			Serial.print("Peso Minimo: ");
			Serial.println(PesoM);
			Serial.print("Validade: ");
			Serial.print(Dia);
			Serial.print(" / ");
			Serial.print(Mes);
			Serial.print(" / ");
			Serial.println(Ano);
			Serial.print("Alerta: ");
			Serial.println(Alerta);
		}
		else if (PrintT == 1)
		{
			File T = SPIFFS.open("/Print.html", FILE_APPEND);
			std::string a;
			a = std::string("					\n<h1 class=\"nome\"><b>") + NomeReag + "</b></h1>" +
			"					\n<p class=\"texto\">Peso: " + std::to_string(Peso) + 
			"					\n<p class=\"texto\">Peso do Container: " + std::to_string(PesoC) +
			"					\n<p class=\"texto\">Peso do minimo: " + std::to_string(PesoM) +
			"					\n<p class=\"texto\">Validade: " + std::to_string(Dia) + " / " + std::to_string(Mes) + " / " + std::to_string(Ano) +
			"					\n<p class=\"texto\">Tipo de alerta: " + std::to_string(Alerta);

			T.write((uint8_t*)a.c_str(), a.size());
			T.close();
		}
	}

	void PrintInfo(int Lugar, std::deque<std::string>& saida)
	{
		// std::fstream Reag(Arq, std::ios::in | std::ios::binary);
		// std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());

		Reag.seek(Lugar, SeekSet);

		int LugarMin = 0;

		uint8_t Marca;
		Reag.read(&Marca, sizeof(char));

		if (Marca != 0xFF)
			std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1667\n";

		// ReagMin.read(&Marca, sizeof(char));

		float Peso, PesoC, PesoM;
		int Dia, Mes, Ano, Alerta, NomeSize;

		Reag.read((uint8_t *)&NomeSize, sizeof(int));

		char NomeReag[NomeSize + 1];

		Reag.read((uint8_t *)NomeReag, NomeSize);
		Reag.read((uint8_t *)&Peso, sizeof(float));
		Reag.read((uint8_t *)&PesoC, sizeof(float));
		Reag.read((uint8_t *)&PesoM, sizeof(float));
		Reag.read((uint8_t *)&Dia, sizeof(int));
		Reag.read((uint8_t *)&Mes, sizeof(int));
		Reag.read((uint8_t *)&Ano, sizeof(int));
		Reag.read((uint8_t *)&Alerta, sizeof(int));

		NomeReag[NomeSize] = '\0';

		std::string a;
		a = std::string("Nome: ") + NomeReag;
		saida.push_back(a);
		a = std::string("Peso T: ") + std::to_string(Peso);
		saida.push_back(a);
		a = std::string("Peso C: ") + std::to_string(PesoC);
		saida.push_back(a);
		a = std::string("Peso M: ") + std::to_string(PesoM);
		saida.push_back(a);
		a = std::string("Validade: ") + std::to_string(Dia) + "/" + std::to_string(Mes) + "/" + std::to_string(Ano);
		saida.push_back(a);
		a = std::string("Alerta: ") + std::to_string(Alerta);
		saida.push_back(a);
	}

private:
	std::string Arq = "/" + Nome + ".dat";
	std::string ArqV = "/" + Nome + "_Vencido.dat";
	std::string ArqA = "/" + Nome + "_Antes.dat";
	std::string ArqD = "/" + Nome + "_Depois.dat";
	std::string ArqH = "/" + Nome + "_Historico.dat";
	std::string ArqM = "/" + Nome + "_Alterados.dat"; // contem os possiveis reagentes que foram alterados quando o peso muda
	std::string ArqHA = "/" + Nome + "_HistoricoAntigo.dat";

	void GetAntes(std::string ArqS, int LocalReag) // Salva as informa��es Antes do reagente seleconado no Arquivo *NOME*_Antes.dat
	{
		//std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
		//std::fstream ReagT(ArqA.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		File Reag = SPIFFS.open(ArqS.c_str());
		SPIFFS.remove(ArqA.c_str());
		File ReagT = SPIFFS.open(ArqA.c_str(), FILE_APPEND);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 364";
			return;
		}

		if (!ReagT)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
			return;
		}

		if (LocalReag != 0)
		{
			uint8_t marca = ' ';

			Reag.seek(LocalReag);

			Reag.read(&marca, sizeof(char));

			if (marca == 0xFF)
			{
				uint8_t *TempA = &marca;

				Reag.seek(0, SeekSet);

				if ((LocalReag - 1))
				{
					bool NewUsado = false;

					if (LocalReag > 100)
					{
						TempA = new uint8_t[100];
						NewUsado = true;
					}

					for (; LocalReag > 100; LocalReag -= 100)
					{
						Reag.read(TempA, 100);

						ReagT.write(TempA, 100);
					}

					if (NewUsado)
					{
						delete[] TempA;
					}

					if (LocalReag)
					{
						TempA = new uint8_t[LocalReag];

						Reag.read(TempA, LocalReag);

						ReagT.write(TempA, LocalReag);

						delete[] TempA;
					}
				}
			}
			else
			{
				std::cout << "Problema ao ler o arquivo.\t Linha: 490\n";
			}
		}

		Reag.close();
		ReagT.close();
	}

	void GetDepois(std::string Arq, int LocalReag) // Salva as informa��es depois do reagente seleconado no Arquivo *NOME*_Depois.dat
	{
		//std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
		//std::fstream ReagT(ArqD.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str());
		SPIFFS.remove(ArqD.c_str());
		File ReagT = SPIFFS.open(ArqD.c_str(), FILE_APPEND);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 439";
			return;
		}

		if (!ReagT)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 445";
			return;
		}

		if (LocalReag < 0)
			LocalReag = 0;

		Reag.seek(LocalReag);

		uint8_t Marca;

		Reag.read(&Marca, sizeof(char));

		if (Marca == 0xFF)
		{
			unsigned int NomeSize;

			Reag.read((uint8_t *)&NomeSize, sizeof(unsigned int));

			Reag.seek(NomeSize + (sizeof(float) * 3) + (sizeof(int) * 4), SeekCur);

			unsigned int EndReag = Reag.size();

			uint8_t *TempD = &Marca;
			unsigned int TamanhoD;

			if (Reag.position() < EndReag)
			{
				Marca = 0;

				Reag.read(&Marca, sizeof(char));

				if (Marca != 0XFF)
				{
					std::cout << "problema ao ler o arquivo!!\tLinha: 382\n";
					Serial.println("problemas ao ler o arquivo!!!\tLinha 1809");
					return;
				}

				Reag.seek(-1, SeekCur);

				TamanhoD = EndReag - (Reag.position());

				bool NewUsado = false;

				for (; TamanhoD > 100; TamanhoD -= 100)
				{
					TempD = new uint8_t[100];
					NewUsado = true;
					Reag.read(TempD, 100);

					ReagT.write(TempD, 100);
				}

				if (NewUsado)
				{
					delete[] TempD;
				}

				if (TamanhoD > 0)
				{
					TempD = new uint8_t[TamanhoD];
					Reag.read(TempD, TamanhoD);

					ReagT.write(TempD, TamanhoD);

					delete[] TempD;
				}
			}
		}

		Reag.close();
		ReagT.close();
	}

	void WriteAntes(std::string Arq) // Limpa o Arquivo *NOME*.dat e escreve as informa��es do arquivo *NOME*_Antes.dat nele
	{
		//std::fstream Reag(Arq.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		//std::fstream ReagT(ArqA.c_str(), std::ios::in | std::ios::binary);
		SPIFFS.remove(Arq.c_str());
		File Reag = SPIFFS.open(Arq.c_str(), FILE_APPEND);
		File ReagT = SPIFFS.open(ArqA.c_str(), FILE_READ);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 521";
			return;
		}

		if (!ReagT)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 527";
			return;
		}

		unsigned int Tamanho = 0;

		uint8_t a = ' ';
		uint8_t *Temp = &a;
		Tamanho = ReagT.size();
		bool NewUsado = false;

		if (Tamanho > 100)
		{
			Temp = new uint8_t[100];
			NewUsado = true;
		}

		for (; Tamanho > 100; Tamanho -= 100)
		{
			ReagT.read(Temp, 100);

			Reag.write(Temp, 100);
		}

		if (NewUsado)
		{
			delete[] Temp;
		}

		if (Tamanho > 0)
		{
			Temp = new uint8_t[Tamanho];

			ReagT.read(Temp, Tamanho);

			Reag.write(Temp, Tamanho);

			delete[] Temp;
		}

		Reag.close();
		ReagT.close();
	}

	void WriteDepois(std::string Arq) // Escreve as informa��es do arquivo *NOME*_Depois.dat ao fim do arquivo *NOME*.dat
	{
		//std::fstream Reag(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);
		//std::fstream ReagT(ArqD.c_str(), std::ios::in | std::ios::binary);
		File Reag = SPIFFS.open(Arq.c_str(), FILE_APPEND);
		File ReagT = SPIFFS.open(ArqD.c_str(), FILE_READ);

		if (!Reag)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 576";
			return;
		}

		if (!ReagT)
		{
			std::cout << "Problema ao abrir o arquivo!!\tLinha: 582";
			return;
		}

		unsigned int Tamanho = ReagT.size();
		uint8_t a = ' ';
		uint8_t *Temp = &a;
		bool NewUsado = false;

		if (Tamanho > 100)
		{
			Temp = new uint8_t[100];
			NewUsado = true;
		}

		for (; Tamanho > 100; Tamanho -= 100)
		{
			ReagT.read(Temp, 100);

			Reag.write(Temp, 100);
		}

		if (NewUsado)
		{
			delete[] Temp;
		}

		if (Tamanho > 0)
		{
			Temp = new uint8_t[Tamanho];

			ReagT.read(Temp, Tamanho);

			Reag.write(Temp, Tamanho);

			delete[] Temp;
		}

		Reag.close();
		ReagT.close();
	}
};