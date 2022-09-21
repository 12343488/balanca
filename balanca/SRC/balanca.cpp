#include <iostream>
#include <string>
#include <time.h>
#include "regulated-input.h"

class Reagente
{
public:
	std::string Nome;
	float PesoTotal, PesoContainer;
	tm Validade;
	int Alerta;
	
	Reagente() : Alerta(0), Nome(" "), PesoTotal(0), PesoContainer(0
)
	{
		time_t rawtime;
		time(&rawtime);
  		localtime_s (&Validade, &rawtime);

		
		PesoTotal = 0;
		PesoContainer = 0;
	}
	
	void Print()
	{
		std::cout << "\n\nNome: " << Nome << "\nPeso Total: " << PesoTotal << "g\t\tPeso do container: " << PesoContainer << "g\nValidade: " << Validade.tm_mday << ":" << Validade.tm_mon << ":" << Validade.tm_year + 1900 << "\nTipo de Alerta: " << Alerta << "\n";
	}
};


int main(int argc, char** argv) 
{
	Reagente a;
	std::string comando;
	float num;

	std::cout << "insira o nome:\n> ";
	std::getline(std::cin, a.Nome);
	
	std::cout << "\ninsira o peso total(frasco e reagente):\n> ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, a.PesoTotal);
			break;
		}
	}
	
	std::cout << "\ninsira o peso do container(não necessario caso não queira coloque 0):\n> ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, a.PesoContainer);
			break;
		}
	}
	
	std::cout << "\ninsira a data de validade:\nDia: ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, num);
			a.Validade.tm_mday = num;
			break;
		}
	}

	std::cout << "\n\nMes: ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, num);
			a.Validade.tm_mon = num;
			break;
		}
	}

	std::cout << "\n\nAno: ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, num);
			a.Validade.tm_year = num - 1900;
			break;
		}
	}

	std::cout << "\ninsira o tipo de alerta:\n> ";
	std::getline(std::cin, comando);
	while (true)
	{
		if (!ValidadeInput(comando))
		{
			std::cout << "\n> ";
			std::getline(std::cin, comando);
		}
		else
		{
			ValidadeInput(comando, num);
			a.Alerta = num;
			break;
		}
	}

	a.Print();	
}
