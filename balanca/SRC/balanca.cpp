#include <iostream>
#include <string>
#include <time.h>
#include "regulated-input.h"

class Reagente
{
public:
	std::string Nome;
	float PesoTotal, PesoContainer;
	tm* Validade;
	int Alerta;
	
	Reagente()
	{
		time_t rawtime;
  		time (&rawtime);
  		Validade = localtime (&rawtime);

		
		PesoTotal = 0;
		PesoContainer = 0;
	}
	
	void Print()
	{
		std::cout << "Nome: " << Nome << "\nPeso Total: " << PesoTotal << "g\tPeso do container: " << PesoContainer << "g\nValidade: " << Validade->tm_mday << ":" << Validade->tm_mon << ":" << Validade->tm_year + 1900 << "\nTipo de Alerta: " << Alerta << "\n";
	}
};


int main(int argc, char** argv) 
{
	Reagente a;
	std::string comando;
	
	std::cout << "insira o nome:\n> ";
	std::getline(std::cin, a.Nome);
	
	std::cout << "\ninsira o peso total(frasco e reagente):\n> ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.PesoTotal);
	
	std::cout << "\ninsira o peso do container(não necessario caso não queira coloque 0):\n> ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.PesoContainer);
	
	std::cout << "\ninsira a data de validade:\nDia: ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.Validade->tm_mday);
	
	std::cout << "\n\nMês: ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.Validade->tm_mon);
	
	std::cout << "\n\nAno: ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.Validade->tm_year);
	
	std::cout << "\ninsira o tipo de alerta:\n> ";
	std::geline(std::cin, comando);
	ValidadeInput(comando, a.Alerta);
	
	a.Print();	
}
