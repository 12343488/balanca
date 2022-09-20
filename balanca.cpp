#include <iostream>
#include <string>
#include <time.h>


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
	
	std::cout << "insira o nome:\n> ";
	std::getline(std::cin, a.Nome);
	
	a.Print();	
}
