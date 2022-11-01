#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <deque>
#include <iomanip>
#include <utility>

#define NumeroAlertas 2

bool ValidadeInput(std::string input, float& result)
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
    std::string Nome;
    float PesoSalvo, PesoAtual;

    Reagentes(std::string Nome) : Nome(Nome), PesoAtual(0), PesoSalvo(0)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagH(ArqH.c_str(), std::ios::out | std::ios::app | std::ios::binary);


        if (Reag.is_open())
        {
            int fim;
            Reag.seekg(0, std::ios::end);
            fim = Reag.tellg();
            Reag.seekg(0, std::ios::beg);

            while (Reag.tellg() < fim)
            {
                char Marca;

                Reag.read(&Marca, sizeof(char));

                if (Marca == -1)
                {
                    unsigned int NomeSize;
                    Reag.read((char*)&NomeSize, sizeof(int));

                    Reag.seekg(NomeSize, std::ios::cur);

                    float Peso;

                    Reag.read((char*)&Peso, sizeof(float));

                    PesoSalvo += Peso;

                    Reag.seekg(sizeof(float) + (sizeof(int) * 4), std::ios::cur);
                }
            }

            ReagH.seekg(0, std::ios::end);
            int Fim = ReagH.tellg();

            if (Fim <= 1)
            {
                char Marca = -1;
                ReagH.write(&Marca, sizeof(char));
                ReagH.write((char*)&PesoSalvo, sizeof(float));
            }

            PesoAtual = PesoSalvo;
        }
        else
        {
            std::cout << "Essa lista ainda nao foi inicializada!!!\n";
        }
    }

    bool CriarReagente(std::string Nome, float Peso, float PesoCont, float PesoMin, int dia, int mes, int ano, int Alerta)
    {
        std::ofstream Reag(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary); //abre o arquivo			
        std::ofstream ReagMin(ArqMi.c_str(), std::ios::out | std::ios::app | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "problema ao abrir o arquvo!!\tLinha: 155\n";
            return false;
        }

        if (!ReagMin.is_open() || ReagMin.bad())
        {
            std::cout << "problema ao abrir o arquvo!!\tLinha: 210\n";
            return false;
        }

        char Marca = -1;
        Reag.write(&Marca, sizeof(char));
        ReagMin.write(&Marca, sizeof(char));

        unsigned int NomeSize = Nome.size() - 1;

        Reag.write((char*)&NomeSize, sizeof(unsigned int));
        Reag.write(Nome.c_str(), NomeSize);
        Reag.write((char*)&Peso, sizeof(float));
        Reag.write((char*)&PesoCont, sizeof(float));
        Reag.write((char*)&dia, sizeof(int));
        Reag.write((char*)&mes, sizeof(int));
        Reag.write((char*)&ano, sizeof(int));
        Reag.write((char*)&Alerta, sizeof(int));
        ReagMin.write((char*)&PesoMin, sizeof(float));
        PesoSalvo += Peso;

        Reag.close();
    }

    bool RemoverReagente(std::string Nome)
    {
        int LocalReag = EscolherReag(Nome);


        if (LocalReag > -1)
        {
            if (LocalReag > 0)
                GetAntes(Arq, LocalReag - 1);
            GetDepois(Arq, LocalReag - 1);

            WriteAntes(Arq);
            WriteDepois(Arq);

            std::fstream Reag(Arq, std::ios::in | std::ios::binary);
            std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);

            int Lugar = LocalReag;
            int LugarMin = 0;

            if (Lugar > -1)
            {
                while (Reag.tellg() < Lugar - 1)
                {
                    char Marca = 0;

                    Reag.read(&Marca, sizeof(char));

                    if (Marca != -1)
                        std::cout << "Problema ao ler o arquivo\tLinha: 441\n";

                    LugarMin++;
                    unsigned int NomeSize;
                    Reag.read((char*)&NomeSize, sizeof(unsigned int));
                    Reag.seekg(NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4), std::ios::cur);
                }

                LugarMin *= sizeof(char) + sizeof(float);

                if (LugarMin > 1)
                {
                    std::fstream ReagT(ArqA.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

                    if (ReagT.bad() || !ReagT.is_open())
                    {
                        std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
                        return false;
                    }

                    bool NewUsado = false;
                    char a = 'a';
                    char* TempA = &a;

                    if (LugarMin > 100)
                    {
                        TempA = new char[100];
                        NewUsado = true;
                    }

                    while (LugarMin - ReagMin.tellg() > 100)
                    {
                        ReagMin.read(TempA, 100);

                        ReagT.write(TempA, 100);
                    }

                    if (NewUsado)
                    {
                        delete[] TempA;
                    }

                    int distancia = LugarMin - ReagMin.tellg();

                    if (distancia)
                    {
                        TempA = new char[distancia];

                        ReagMin.read(TempA, distancia);

                        ReagT.write(TempA, distancia);

                        delete[] TempA;
                    }
                }

                ReagMin.seekg(sizeof(char) + sizeof(float), std::ios::cur);
                int LugarParado = ReagMin.tellg();
                ReagMin.seekg(0, std::ios::end);
                int FimReagMin = ReagMin.tellg();
                ReagMin.seekg(LugarParado, std::ios::beg);

                if (LugarParado < FimReagMin)
                {
                    std::fstream ReagT(ArqD.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

                    if (ReagT.bad() || !ReagT.is_open())
                    {
                        std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
                        return false;
                    }

                    bool NewUsado = false;
                    char a = 'a';
                    char* TempA = &a;

                    if (FimReagMin - LugarParado > 100)
                    {
                        TempA = new char[100];
                        NewUsado = true;
                    }

                    while (FimReagMin - ReagMin.tellg() > 100)
                    {
                        ReagMin.read(TempA, 100);

                        ReagT.write(TempA, 100);
                    }

                    if (NewUsado)
                    {
                        delete[] TempA;
                    }

                    int distancia = FimReagMin - ReagMin.tellg();

                    if (distancia)
                    {
                        TempA = new char[distancia];

                        ReagMin.read(TempA, distancia);

                        ReagT.write(TempA, distancia);

                        delete[] TempA;
                    }
                }

                ReagMin.close();

                ReagMin.open(ArqMi, std::ios::out | std::ios::trunc | std::ios::binary);

                ReagMin.close();

                if (LugarMin > 1)
                    WriteAntes(ArqMi);

                if (LugarParado < FimReagMin)
                    WriteDepois(ArqMi);
            }

            return true;
        }
        else
        {
            return false;
        }

        return false;
    }

    void ChecarValidade()
    {
        std::ifstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagV(ArqV.c_str(), std::ios::out | std::ios::in | std::ios::trunc | std::ios::binary);

        while (Reag.good())
        {
            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca != -1)
            {
                unsigned int LocalAtual = Reag.tellg();

                Reag.seekg(0, std::ios::end);
                unsigned int Fim = Reag.tellg();

                if (LocalAtual < Fim)//para não parar quando chegar ao fim do arquivo
                {
                    std::cout << "Problema ao ler o arquivo!!\tLinha: 452\n";
                }

                break;
            }

            unsigned int ComecoReag = Reag.tellg();

            unsigned int NomeSize;
            Reag.read((char*)&NomeSize, sizeof(unsigned int));

            Reag.seekg(NomeSize + (sizeof(float) * 2), std::ios::cur);

            int dia, mes, ano, Alerta;

            Reag.read((char*)&dia, sizeof(int));
            Reag.read((char*)&mes, sizeof(int));
            Reag.read((char*)&ano, sizeof(int));
            Reag.read((char*)&Alerta, sizeof(int));

            time_t RawTime;
            time(&RawTime);
            tm Tempo;
            localtime_s(&Tempo, &RawTime);

            if (ano == Tempo.tm_year + 1900)
            {
                if (mes == Tempo.tm_mon + 1)
                {
                    if (dia <= Tempo.tm_mday)
                    {
                        Marca = -1;
                        ReagV.write((char*)&Marca, sizeof(char));

                        ReagV.write((char*)&ComecoReag, sizeof(unsigned int));
                        ReagV.write((char*)&Alerta, sizeof(int));
                    }
                }
                else if (mes < Tempo.tm_mon + 1)
                {
                    Marca = -1;
                    ReagV.write((char*)&Marca, sizeof(char));

                    ReagV.write((char*)&ComecoReag, sizeof(unsigned int));
                    ReagV.write((char*)&Alerta, sizeof(int));
                }
            }
            else if (ano < Tempo.tm_year + 1900)
            {
                Marca = -1;
                ReagV.write((char*)&Marca, sizeof(char));

                ReagV.write((char*)&ComecoReag, sizeof(unsigned int));
                ReagV.write((char*)&Alerta, sizeof(int));
            }
        }
        Reag.close();
    }

    bool TrocarPeso(std::string Nome, float NovoPeso, float NovoPesoC)
    {
        if (NovoPeso >= NovoPesoC)
        {
            int LocalReag = EscolherReag(Nome);

            if (LocalReag > -1)
            {
                GetAntes(Arq, LocalReag - 1);

                //pega as informações excluido os pesos

                std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

                if (!Reag.is_open() || Reag.bad())
                {
                    std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
                    return false;
                }

                Reag.seekg(LocalReag - 1, std::ios::beg);

                char Marca = 0;

                Reag.read(&Marca, sizeof(char));

                if (Marca != -1)
                {
                    std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
                }

                unsigned int NomeSize;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char* Nome = new char[NomeSize];

                Reag.read(Nome, NomeSize);

                float PesoAntigo;

                Reag.read((char*)&PesoAntigo, sizeof(float));

                Reag.seekg((sizeof(float)), std::ios::cur);

                char* Outros = new char[sizeof(int) * 4];

                Reag.read(Outros, (sizeof(int) * 4));

                Reag.close();

                GetDepois(Arq, LocalReag - 1);

                WriteAntes(Arq);

                Reag.open(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);

                Reag.seekg(0, std::ios::end);

                Reag.write(&Marca, sizeof(char));
                Reag.write((char*)&NomeSize, sizeof(unsigned int));
                Reag.write(Nome, NomeSize);
                Reag.write((char*)&NovoPeso, sizeof(float));
                Reag.write((char*)&NovoPesoC, sizeof(float));
                Reag.write(Outros, sizeof(int) * 4);

                PesoSalvo += NovoPeso - PesoAntigo;

                delete[] Nome;
                delete[] Outros;

                Reag.close();

                WriteDepois(Arq);

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            std::cout << "informações invalidas!!\n";
        }
    }

    void TrocarPesoMin(std::string Nome, float NovoPesoMin)
    {
        std::fstream Reag(Arq, std::ios::in | std::ios::binary);
        std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);

        int Lugar = EscolherReag(Nome);
        int LugarMin = 0;

        if (Lugar > -1)
        {
            while (Reag.tellg() < Lugar - 1)
            {
                char Marca = 0;

                Reag.read(&Marca, sizeof(char));

                if (Marca != -1)
                    std::cout << "Problema ao ler o arquivo\tLinha: 441\n";

                LugarMin++;
                unsigned int NomeSize;
                Reag.read((char*)&NomeSize, sizeof(unsigned int));
                Reag.seekg(NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4), std::ios::cur);
            }

            LugarMin *= sizeof(char) + sizeof(float);

            if (LugarMin > 1)
            {
                std::fstream ReagT(ArqA.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

                if (ReagT.bad() || !ReagT.is_open())
                {
                    std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
                    return;
                }

                bool NewUsado = false;
                char a = 'a';
                char* TempA = &a;

                if (LugarMin > 100)
                {
                    TempA = new char[100];
                    NewUsado = true;
                }

                while (LugarMin - ReagMin.tellg() > 100)
                {
                    ReagMin.read(TempA, 100);

                    ReagT.write(TempA, 100);
                }

                if (NewUsado)
                {
                    delete[] TempA;
                }

                int distancia = LugarMin - ReagMin.tellg();

                if (distancia)
                {
                    TempA = new char[distancia];

                    ReagMin.read(TempA, distancia);

                    ReagT.write(TempA, distancia);

                    delete[] TempA;
                }
            }

            ReagMin.seekg(sizeof(char) + sizeof(float), std::ios::cur);
            int LugarParado = ReagMin.tellg();
            ReagMin.seekg(0, std::ios::end);
            int FimReagMin = ReagMin.tellg();
            ReagMin.seekg(LugarParado, std::ios::beg);

            if (LugarParado < FimReagMin)
            {
                std::fstream ReagT(ArqD.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

                if (ReagT.bad() || !ReagT.is_open())
                {
                    std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
                    return;
                }

                bool NewUsado = false;
                char a = 'a';
                char* TempA = &a;

                if (FimReagMin - LugarParado > 100)
                {
                    TempA = new char[100];
                    NewUsado = true;
                }

                while (FimReagMin - ReagMin.tellg() > 100)
                {
                    ReagMin.read(TempA, 100);

                    ReagT.write(TempA, 100);
                }

                if (NewUsado)
                {
                    delete[] TempA;
                }

                int distancia = FimReagMin - ReagMin.tellg();

                if (distancia)
                {
                    TempA = new char[distancia];

                    ReagMin.read(TempA, distancia);

                    ReagT.write(TempA, distancia);

                    delete[] TempA;
                }
            }

            ReagMin.close();

            ReagMin.open(ArqMi, std::ios::out | std::ios::trunc | std::ios::binary);

            ReagMin.close();

            if (LugarMin > 1)
                WriteAntes(ArqMi);

            ReagMin.open(ArqMi, std::ios::out | std::ios::app | std::ios::binary);

            char Marca = -1;
            ReagMin.write(&Marca, sizeof(char));
            ReagMin.write((char*)&NovoPesoMin, sizeof(float));

            ReagMin.close();

            if (LugarParado < FimReagMin)
                WriteDepois(ArqMi);
        }

        Reag.close();
    }

    bool TrocarNome(std::string Nome, std::string NovoNome)
    {
        int LocalReag = EscolherReag(Nome);

        if (LocalReag > -1)
        {
            GetAntes(Arq, LocalReag - 1);

            //pega as informações excluido os pesos

            std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

            if (!Reag.is_open() || Reag.bad())
            {
                std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
                return false;
            }

            Reag.seekg(LocalReag - 1, std::ios::beg);

            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca != -1)
            {
                std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
            }

            unsigned int NomeSize;

            Reag.read((char*)&NomeSize, sizeof(unsigned int));

            Reag.seekg(NomeSize, std::ios::cur);

            char* Outros = new char[(sizeof(float) * 2) + (sizeof(int) * 4)];

            Reag.read(Outros, ((sizeof(float) * 2) + (sizeof(int) * 4)));

            Reag.close();

            GetDepois(Arq, LocalReag - 1);

            WriteAntes(Arq);

            Reag.open(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);

            Reag.seekg(0, std::ios::end);

            NomeSize = NovoNome.size();

            Reag.write(&Marca, sizeof(char));
            Reag.write((char*)&NomeSize, sizeof(unsigned int));
            Reag.write(NovoNome.c_str(), NomeSize);
            Reag.write(Outros, ((sizeof(float) * 2) + (sizeof(int) * 4)));

            delete[] Outros;
            Reag.close();

            WriteDepois(Arq);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool TrocarValidade(std::string Nome, int NovoDia, int NovoMes, int NovoAno)
    {
        if ((NovoDia > 0 && NovoDia < 32) && (NovoMes > 0 && NovoMes < 13))
        {
            int LocalReag = EscolherReag(Nome);

            if (LocalReag > -1)
            {
                GetAntes(Arq, LocalReag - 1);

                //pega as informações excluido os pesos
                std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

                if (!Reag.is_open() || Reag.bad())
                {
                    std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
                    return false;
                }

                Reag.seekg(LocalReag - 1, std::ios::beg);

                char Marca = 0;

                Reag.read(&Marca, sizeof(char));

                if (Marca != -1)
                {
                    std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
                }

                unsigned int NomeSize;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char* Nome = new char[NomeSize];

                Reag.read(Nome, NomeSize);

                char* Outros = new char[(sizeof(float) * 2) + sizeof(int)];

                Reag.read(Outros, (sizeof(float) * 2));

                Reag.seekg(sizeof(int) * 3, std::ios::cur);

                Reag.read((Outros + (sizeof(float) * 2)), sizeof(int));

                Reag.close();

                GetDepois(Arq, LocalReag - 1);

                WriteAntes(Arq);

                Reag.open(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);

                Reag.seekg(0, std::ios::end);

                Reag.write(&Marca, sizeof(char));
                Reag.write((char*)&NomeSize, sizeof(unsigned int));
                Reag.write(Nome, NomeSize);
                Reag.write(Outros, sizeof(float) * 2);
                Reag.write((char*)&NovoDia, sizeof(int));
                Reag.write((char*)&NovoMes, sizeof(int));
                Reag.write((char*)&NovoAno, sizeof(int));
                Reag.write((Outros + (sizeof(float) * 2)), sizeof(int));

                delete[] Nome;
                delete[] Outros;

                Reag.close();

                WriteDepois(Arq);

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            std::cout << "informações invalidas!!\n";
        }
    }

    bool TrocarAlerta(std::string Nome, int NovoAlerta)
    {
        if (NovoAlerta <= NumeroAlertas && NovoAlerta >= 0)
        {
            int LocalReag = EscolherReag(Nome);

            if (LocalReag > -1)
            {
                GetAntes(Arq, LocalReag - 1);

                std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

                if (!Reag.is_open() || Reag.bad())
                {
                    std::cout << "Problema ao ler o arquivo!!\tLinha: 373\n";
                    return false;
                }

                Reag.seekg(LocalReag - 1, std::ios::beg);

                char Marca = 0;

                Reag.read(&Marca, sizeof(char));

                if (Marca != -1)
                {
                    std::cout << "problema ao ler o arquivo!!\tLinha: 376\n";
                }

                unsigned int NomeSize;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char* Nome = new char[NomeSize];

                Reag.read(Nome, NomeSize);

                char* Outros = new char[(sizeof(float) * 2) + (sizeof(int) * 3)];

                Reag.read(Outros, ((sizeof(float) * 2) + (sizeof(int) * 3)));

                Reag.close();

                GetDepois(Arq, LocalReag - 1);

                WriteAntes(Arq);

                Reag.open(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);

                Reag.seekg(0, std::ios::end);

                Reag.write(&Marca, sizeof(char));
                Reag.write((char*)&NomeSize, sizeof(unsigned int));
                Reag.write(Nome, NomeSize);
                Reag.write(Outros, (sizeof(float) * 2) + (sizeof(int) * 3));
                Reag.write((char*)&NovoAlerta, sizeof(int));

                delete[] Nome;
                delete[] Outros;

                Reag.close();

                WriteDepois(Arq);

                return true;
            }
            else
            {
                return false;
            }
        }
    }

    void VerificarPeso()
    {
        if (PesoSalvo != PesoAtual)
        {
            std::fstream ReagH(ArqH.c_str(), std::ios::out | std::ios::app | std::ios::binary);

            char Marca = -1;
            ReagH.write(&Marca, sizeof(char));
            ReagH.write((char*)&PesoAtual, sizeof(float));

            PesoSalvo = PesoAtual;
        }
    }

    void SalvarAlteração()
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagH(ArqH.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagA(ArqM.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 645\n";

        if (ReagH.bad() || !ReagH.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 648\n";

        if (ReagA.bad() || !ReagA.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 650\n";

        unsigned int ReagHEnd;

        ReagH.seekg(0, std::ios::end);

        ReagHEnd = ReagH.tellg();

        ReagH.seekg(0, std::ios::beg);

        float PesoAnt = -1;

        ReagA.close();

        while (ReagH.tellg() < ReagHEnd)
        {
            char Marca;
            ReagH.read(&Marca, sizeof(char));

            float a = 100;

            if (Marca != -1)
                std::cout << "Problemas ao ler o arquivo!!\tLinha: 657\n";

            if (PesoAnt > -1)
            {
                ReagA.open(ArqM, std::ios::out | std::ios::app | std::ios::in | std::ios::binary);
                if (ReagA.bad() || !ReagA.is_open())
                    std::cout << "Problema ao abrir o arquivo!!!\tLinha: 994\n";

                float Diferença, PesoVer;

                ReagH.read((char*)&PesoVer, sizeof(float));

                Diferença = PesoVer - PesoAnt;

                if (Diferença != 0)
                {
                    Reag.seekg(0, std::ios::end);
                    unsigned int ReagFim = Reag.tellg();
                    Reag.seekg(0, std::ios::beg);

                    ReagA.seekg(0, std::ios::end);
                    unsigned int ReagAFim = ReagA.tellg();

                    bool VariosCompativeis = false;
                    bool Compativel = false;

                    Marca = -1;
                    ReagA.write(&Marca, sizeof(char));

                    ReagA.write((char*)&Diferença, sizeof(float));

                    int AlertaM = 0;

                    //tudo só vai escrever se a diferença for negativa pois apnas escreve informaçoes relevantes para retiradas.
                    {
                        while (Reag.tellg() < ReagFim && Diferença < 0)
                        {
                            unsigned int InicioReag, NomeSize;
                            int Alerta;
                            float PesoReag;

                            Reag.read(&Marca, sizeof(char));

                            if (Marca != -1)
                                std::cout << "Problemas ao ler o arquivo!!\tLinha: 685\n";

                            InicioReag = Reag.tellg();

                            Reag.read((char*)&NomeSize, sizeof(unsigned int));

                            Reag.seekg(NomeSize, std::ios::cur);

                            Reag.read((char*)&PesoReag, sizeof(float));

                            Reag.seekg(sizeof(float) + (sizeof(int) * 3), std::ios::cur);

                            Reag.read((char*)&Alerta, sizeof(int));

                            if (Alerta > AlertaM)
                                AlertaM = Alerta;

                            if ((-Diferença >= PesoReag - DesvioAceitavel) && (-Diferença <= PesoReag + DesvioAceitavel))
                            {
                                if (VariosCompativeis)
                                {
                                    Marca = -2;

                                    ReagA.write(&Marca, sizeof(char));
                                }

                                ReagA.write((char*)&InicioReag, sizeof(unsigned int));

                                VariosCompativeis = true;
                                Compativel = true;
                            }
                        }

                        ReagA.close();

                        ReagA.open(ArqM, std::ios::out | std::ios::app | std::ios::in | std::ios::binary);

                        if (ReagA.bad() || !ReagA.is_open())
                            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 1070\n";

                        bool sinal = -1; //-1 inicializa, true - positivo, false = negativo
                        bool AchouA = false;
                        unsigned int InicioReag, ReagAParado;

                        ReagA.seekg(0, std::ios::beg);

                        std::cout << ReagA.tellg() << '\n';

                        while (ReagA.tellg() < ReagAFim && Diferença < 0)
                        {
                            int Alerta;
                            float PesoReag;

                            std::cout << ReagA.tellg() << '\n';

                            ReagA.read(&Marca, sizeof(char));

                            if (Marca != -1)
                                std::cout << "Problemas ao ler o arquivo!!\tLinha: 685\n";

                            if (Marca == -1)
                            {
                                AchouA = false;
                                ReagA.read((char*)&PesoReag, sizeof(float));

                                if (PesoReag < 0)
                                {
                                    ReagA.read((char*)&InicioReag, sizeof(int));

                                    sinal = false;
                                }

                                if (PesoReag > 0)
                                    sinal = true;

                                ReagA.read(&Marca, sizeof(char));

                                ReagA.seekg(-(long long)sizeof(char), std::ios::cur);

                                if (Marca != -1 && Marca != -2 && sinal == false)
                                {
                                    ReagA.read((char*)&Alerta, sizeof(int));
                                    
                                    if(AlertaM < Alerta)
                                        AlertaM = Alerta;
                                }

                                if (((-Diferença >= PesoReag - DesvioAceitavel) && (-Diferença <= PesoReag + DesvioAceitavel)) && sinal == true)
                                {
                                    ReagAParado = ReagA.tellg();

                                    if (VariosCompativeis)
                                    {
                                        Marca = -2;

                                        ReagA.write(&Marca, sizeof(char));
                                    }

                                    ReagA.write((char*)&InicioReag, sizeof(unsigned int));

                                    VariosCompativeis = true;
                                    AchouA = true;
                                    Compativel = true;

                                    ReagA.seekg(ReagAParado, std::ios::beg);
                                }
                            }
                            else if (Marca == -2 && AchouA)
                            {
                                ReagA.read((char*)&InicioReag, sizeof(int));

                                Marca = -2;
                                ReagA.write(&Marca, sizeof(char));

                                ReagA.write((char*)&InicioReag, sizeof(unsigned int));
                                VariosCompativeis = true;
                                Compativel = true;

                                ReagA.read(&Marca, sizeof(char));

                                if (Marca == -1 || Marca == -2)
                                    ReagA.seekg(-(long long)sizeof(char), std::ios::cur);
                                else if (AlertaM < Marca)
                                    AlertaM = Marca;
                            }
                        }

                        if (Compativel)
                            ReagA.write((char*)&AlertaM, sizeof(int));

                        ReagA.close();

                        if (!Compativel && Diferença < 0)
                        {
                            int Temp = -1;
                            ReagA.write((char*)&Temp, sizeof(int));
                        }
                    }

                    PesoAnt = PesoVer;
                }
            }
            else
            {
                float PesoVer;

                ReagH.read((char*)&PesoVer, sizeof(float));

                PesoAnt = PesoVer;
            }
        }

        Reag.close();
        ReagH.close();
        ReagA.close();
    }

    void ConfirmarAlteração()
    {
        std::fstream ReagH(ArqH, std::ios::in | std::ios::binary);
        std::fstream ReagHA(ArqHA, std::ios::out | std::ios::app | std::ios::binary);


        if (ReagH.bad() || !ReagH.is_open())
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 783\n";

        if (ReagHA.bad() || !ReagHA.is_open())
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 787\n";

        char a = 'a';
        char* TempA = &a;

        ReagH.seekg(0, std::ios::end);
        int Fim = ReagH.tellg();
        ReagH.seekg(0, std::ios::beg);

        if (Fim - ReagH.tellg() > 0)
        {
            bool NewUsado = false;

            if (Fim - ReagH.tellg() > 100)
            {
                TempA = new char[100];
                NewUsado = true;
            }

            while (Fim - ReagH.tellg() > 100)
            {
                ReagH.read(TempA, 100);

                ReagHA.write(TempA, 100);
            }

            if (NewUsado)
            {
                delete[] TempA;
            }

            int lugar = ReagH.tellg();

            if (Fim - ReagH.tellg())
            {
                TempA = new char[Fim - lugar];

                ReagH.read(TempA, Fim - lugar);

                ReagHA.write(TempA, Fim - lugar);

                delete[] TempA;
            }

            ReagH.close();
            ReagHA.close();

            ReagH.open(ArqH, std::ios::out | std::ios::trunc | std::ios::binary);

            a = - 1;

            ReagH.write(&a, sizeof(char));
            ReagH.write((char*)&PesoAtual, sizeof(float));

            ReagH.close();
        }
    }

    void LimparHistorico()
    {
        std::fstream ReagHA(ArqHA, std::ios::out | std::ios::trunc | std::ios::binary);

        ReagHA.close();
    }

    void PrintMin(int Alerta)
    {
        std::fstream Reag(Arq, std::ios::in | std::ios::binary);
        std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
            std::cout << "Problemas ao abrir o arquivo\tLinha: 860\n";


        if (ReagMin.bad() || !ReagMin.is_open())
            std::cout << "Problemas ao abrir o arquivo\tLinha: 860\n";

        int ReagFim;
        Reag.seekg(0, std::ios::end);
        ReagFim = Reag.tellg();
        Reag.seekg(0, std::ios::beg);

        while (Reag.tellg() < ReagFim)
        {
            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int NomeSize;
                float PesoR, PesoC;
                int AlertaR;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char* NomeReag = new char[NomeSize + 1];

                Reag.read(NomeReag, NomeSize);

                NomeReag[NomeSize] = '\0';

                Reag.read((char*)&PesoR, sizeof(float));
                Reag.read((char*)&PesoC, sizeof(float));

                Reag.seekg((sizeof(int) * 3), std::ios::cur);


                Reag.read((char*)&AlertaR, sizeof(int));

                if (AlertaR >= Alerta)
                {
                    ReagMin.read(&Marca, sizeof(char));

                    if (Marca != -1)
                        std::cout << "Problemas ao ler o arquivo!!\tLinha: 904\n";

                    float PesoMin;

                    ReagMin.read((char*)&PesoMin, sizeof(float));

                    if (PesoR - PesoC <= PesoMin)
                    {
                        std::cout << "\n\n-------------------------\nO Reagente " << NomeReag << " esta com " << PesoR - PesoC << " g sobrando e precisa de no minimo: " << PesoMin << " g\n";
                    }
                }
            }
        }
    }

    void PrintNomes()
    {
        std::ifstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problem ao abrir o arquivo!!\n";
            return;
        }

        std::cout << '\n';
        
        while (Reag.good())
        {
            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int NomeSize;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char* NomeReag = new char[NomeSize + 1];

                for (int NomeInd = 0; NomeInd < NomeSize; NomeInd++)
                {
                    Reag.read((char*)&NomeReag[NomeInd], sizeof(char));
                }

                NomeReag[NomeSize] = '\0';

                std::cout << NomeReag << '\n';

                delete[] NomeReag;

                Reag.seekg((sizeof(float) * 2) + (sizeof(int) * 4), std::ios::cur);
            }
        }
    } 

    void PrintInfos()
    {
        std::ifstream Reag(Arq.c_str(), std::ios::in | std::ios::binary); //abre o arquivo
        std::ifstream ReagMin(ArqMi.c_str(), std::ios::in | std::ios::binary); //abre o arquivo

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problem ao abrir o arquivo!!\tLinha: 1123\n";
            return;
        }
        
        if (!ReagMin.is_open() || ReagMin.bad())
        {
            std::cout << "Problem ao abrir o arquivo!!\tLinha: 1129\n";
            return;
        }

        std::cout << '\n';

        while (Reag.good())
        {
            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                ReagMin.read(&Marca, sizeof(char));

                if (Marca != -1)
                    std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1146\n";

                unsigned int NomeSize;

                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                char NomeReag;

                std::cout << '\n';

                for (int NomeInd = 0; NomeInd < NomeSize; NomeInd++)
                {
                    Reag.read(&NomeReag, sizeof(char));
                    std::cout << NomeReag;
                }

                std::cout << "\nPeso: ";


                float Pesos;
                Reag.read((char*)&Pesos, sizeof(float));
                std::cout << "Total: " << std::setw(14) << std::left << Pesos << "Container: ";

                Reag.read((char*)&Pesos, sizeof(float));
                std::cout << Pesos << "\nPeso Minimo: ";

                ReagMin.read((char*)&Pesos, sizeof(float));
                std::cout << Pesos << "\nValidade: ";

                int Inteiros;
                Reag.read((char*)&Inteiros, sizeof(float));
                std::cout << Inteiros << " / ";

                Reag.read((char*)&Inteiros, sizeof(float));
                std::cout << Inteiros << " / ";

                Reag.read((char*)&Inteiros, sizeof(float));
                std::cout << Inteiros << "\nAlerta: ";

                Reag.read((char*)&Inteiros, sizeof(float));
                std::cout << Inteiros << '\n';
            }
        }
    }

    void PrintValidade(int Tipo)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagV(ArqV.c_str(), std::ios::in | std::ios::binary);

        ReagV.seekg(0, std::ios::beg);

        while (ReagV.good())
        {
            char Marca = 0;
            ReagV.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int ComecoReag;
                int Alerta;
                ReagV.read((char*)&ComecoReag, sizeof(unsigned int));
                ReagV.read((char*)&Alerta, sizeof(int));

                if (Alerta >= Tipo)
                {
                    Reag.seekg(ComecoReag - 1, std::ios::beg);

                    Reag.read(&Marca, sizeof(char));

                    if (Marca == -1)
                    {
                        unsigned int NomeSize;
                        Reag.read((char*)&NomeSize, sizeof(unsigned int));

                        std::cout << "\nNome: ";

                        char NomeChar;

                        for (int i = 0; i < NomeSize; i++)
                        {
                            Reag.read(&NomeChar, sizeof(char));

                            std::cout << NomeChar;
                        }

                        float PesoT, PesoC, PesoR;

                        Reag.read((char*)&PesoT, sizeof(float));
                        Reag.read((char*)&PesoC, sizeof(float));

                        if (PesoC > 0)
                        {
                            PesoR = PesoT - PesoC;
                        }

                        std::cout << "\nPeso:";
                        if (PesoC > 0)
                        {
                            std::cout << "Peso total: " << PesoT << "\nPeso do Reagente: " << PesoR << '\n';
                        }
                        else
                        {
                            std::cout << "Peso total: " << PesoT << '\n';
                        }
                        int data;

                        Reag.read((char*)&data, sizeof(int));

                        std::cout << "Venceu dia: " << data << '/';

                        Reag.read((char*)&data, sizeof(int));

                        std::cout << data << '/';

                        Reag.read((char*)&data, sizeof(int));

                        std::cout << data << '\n';
                    }
                }
            }
        }
    }

    void PrintEspecifico(std::string Nome)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
        }

        bool tem = false;

        Reag.seekg(0, std::ios::end);
        unsigned int End = Reag.tellg(); //salva onde é o final do arquivo

        Reag.seekg(0, std::ios::beg);

        //passa pelo arquivo inteiro procurando os nomes
        while (Reag.tellg() < End)
        {
            char Marca = 0;
            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int NomeSize;
                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                if (Nome.size() == NomeSize)
                {
                    bool Achou = true;

                    char NomeSalvoChar;
                    int NomeInd = 0;

                    //procura o nome é vê se é igual ao nome sendo procurado
                    for (; NomeInd < NomeSize; NomeInd++)
                    {
                        Reag.read(&NomeSalvoChar, sizeof(char));

                        if (NomeSalvoChar != Nome.c_str()[NomeInd])
                        {
                            Achou = false;
                            break;
                        }
                    }

                    if (Achou == true)
                    {
                        Reag.seekg(-((int)NomeSize + (long)sizeof(int) + (long)sizeof(char)), std::ios::cur);

                        PrintInfo((int)Reag.tellg());

                        Reag.seekg(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 2), std::ios::cur);

                        tem = true;
                    }
                    else
                        Reag.seekg(((sizeof(float) * 2) + (sizeof(int) * 4)), std::ios::cur);
                }
                else
                    Reag.seekg((NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4)), std::ios::cur);
            }
        }

        if (!tem)
            std::cout << "\nO Reagente nao existe na lista!!\n";
    }

    void PrintEspecifico(float Peso)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
        }

        bool tem = false;

        Reag.seekg(0, std::ios::end);
        unsigned int End = Reag.tellg(); //salva onde é o final do arquivo

        Reag.seekg(0, std::ios::beg);

        //passa pelo arquivo inteiro procurando os nomes
        while (Reag.tellg() < End)
        {
            char Marca = 0;
            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int NomeSize;
                Reag.read((char*)&NomeSize, sizeof(unsigned int));
                Reag.seekg(NomeSize, std::ios::cur);
                float PesoReag;
                Reag.read((char*)&PesoReag, sizeof(float));

                if (PesoReag >= (Peso - DesvioAceitavel) && PesoReag <= (Peso + DesvioAceitavel))
                {
                    Reag.seekg(-((int)NomeSize + (long)sizeof(int) + (long)sizeof(char) + (long)sizeof(float)), std::ios::cur);

                    PrintInfo((int)Reag.tellg());

                    Reag.seekg(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 2), std::ios::cur);
                }
                else
                    Reag.seekg((sizeof(float) + (sizeof(int) * 4)), std::ios::cur);
            }
        }
    }

    void PrintEspecifico(int Mes, int Ano)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
        }

        bool tem = false;

        Reag.seekg(0, std::ios::end);
        unsigned int End = Reag.tellg(); //salva onde é o final do arquivo

        Reag.seekg(0, std::ios::beg);

        //passa pelo arquivo inteiro procurando os nomes
        while (Reag.tellg() < End)
        {
            char Marca = 0;
            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int NomeSize;
                Reag.read((char*)&NomeSize, sizeof(unsigned int));
                Reag.seekg(NomeSize + (sizeof(float) * 2) + sizeof(int), std::ios::cur);
                int MesR, AnoR;
                Reag.read((char*)&MesR, sizeof(int));
                Reag.read((char*)&AnoR, sizeof(int));

                if (MesR == Mes && AnoR == Ano)
                {
                    Reag.seekg(-((int)NomeSize + (long)(sizeof(int) * 4) + (long)sizeof(char) + (long)(sizeof(float) * 2)), std::ios::cur);

                    PrintInfo((int)Reag.tellg());

                    Reag.seekg(NomeSize + (sizeof(int) * 5) + sizeof(char) + (sizeof(float) * 2), std::ios::cur);
                }
                else
                    Reag.seekg((sizeof(int)), std::ios::cur);
            }
        }
    }

    void PrintAlterados()
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagA(ArqM.c_str(), std::ios::in | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 919\n";

        if (ReagA.bad() || !ReagA.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 922\n";

        ReagA.seekg(0, std::ios::end);
        unsigned int ReagAFim = ReagA.tellg();

        ReagA.seekg(0, std::ios::beg);

        float Diferença = 0;

        bool Combo = true;
        bool EstadoA = -1; //false Diminuiu, true aumentou

        int Alerta;

        std::deque<std::pair <int, float>> Alterados; //salva o index(depois da marca) e a diferença dos reagentes alterados enquanto o combo for true

        while (ReagA.tellg() < ReagAFim)
        {
            char Marca = 0;

            ReagA.read(&Marca, sizeof(char));

            if (Marca != -1 && Marca != -2)
                std::cout << "Problema ao ler o arquivo!!\tLinha: 931\n";

            bool MultComp = false;

            if (Marca == -2)
                MultComp = true;

            int Lugar, NomeSize;

            if (!MultComp)
                ReagA.read((char*)&Diferença, sizeof(float));

            if (Diferença < 0)
            {
                ReagA.read((char*)&Lugar, sizeof(int));
                ReagA.read((char*)&Alerta, sizeof(int));

                if (Lugar > -1)
                {
                    Reag.seekg(Lugar - 1, std::ios::beg);
                    Reag.read(&Marca, sizeof(char));

                    if (Marca != -1)
                        std::cout << "problemas ao ler o arquivo!!\tLinha: 940\n";

                    if (!EstadoA && EstadoA != -1)
                    {
                        Combo = false;
                        Alterados.clear();
                    }
                    EstadoA = false;

                    if (Combo)
                    {
                        Alterados.push_back(std::pair<int, float>(Lugar, -1));
                    }

                    Reag.read((char*)&NomeSize, sizeof(int));

                    char* Nome = new char[NomeSize + 1];

                    Reag.read(Nome, NomeSize);

                    Nome[NomeSize] = '\0';

                    if (!MultComp)
                    {
                        std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferença << " g\n" << "Possivel Reagente Aterado : " << Nome;
                    }
                    else
                    {
                        std::cout << " // " << Nome;

                        Combo = false;
                        Alterados.clear();
                    }

                    delete[] Nome;
                }
                else
                {
                    std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferença << " g\nNao tem nenhum Reagente compativel!!";

                    Combo = false;
                    Alterados.clear();

                }
            }
            else
            {
                std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferença << " g\nVerifique os Reagentes possivelmente alterados!!";
                if (EstadoA)
                {
                    Combo = false;
                    Alterados.clear();
                }
                EstadoA = true;

                if (Combo && !Alterados.empty())
                {
                    if (Alterados.back().second == -1)
                    {
                        Reag.seekg(Alterados.back().first, std::ios::beg);

                        Reag.seekg(Alterados.back().first - 1, std::ios::beg);

                        Reag.read(&Marca, sizeof(char));

                        if (Marca != -1)
                            std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1069\n";

                        int NomeSize;

                        Reag.read((char*)&NomeSize, sizeof(int));

                        Reag.seekg(NomeSize + sizeof(float), std::ios::cur);

                        float PesoC;

                        Reag.read((char*)&PesoC, sizeof(float));

                        if (Diferença >= PesoC)
                            Alterados.back().second = Diferença;
                        else
                            Combo = false;
                    }

                }
            }
        }
        std::cout << '\n';

        if (Combo && !Alterados.empty())
        {
            std::cout << "\n\nFoi possivel achar todos os reagentes alterados deseja salvar as mudancas?(verifique se as mudacas estao corretas antes de confirmar)\n(caso deseje seguir digite \"sim\")\n> ";
            std::string Comando;
            std::getline(std::cin, Comando);

            if (Comando != "Sim" && Comando != "sim")
                Combo = false;

            if (Combo)
            {
                char Marca;
                for (std::deque <std::pair<int, float>>::iterator i = Alterados.begin(); i < Alterados.end(); i++)
                {
                    Reag.seekg(i->first - 1, std::ios::beg);

                    Reag.read(&Marca, sizeof(char));

                    if (Marca != -1)
                        std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1069\n";

                    int NomeSize;

                    Reag.read((char*)&NomeSize, sizeof(int));

                    char* NomeReag = new char[NomeSize + 1];
                    Reag.read(NomeReag, NomeSize);
                    NomeReag[NomeSize] = '\0';


                    float PesoA, PesoC;

                    Reag.read((char*)&PesoA, sizeof(float));
                    Reag.read((char*)&PesoC, sizeof(float));

                    PesoSalvo -= PesoA - i->second;

                    TrocarPeso(NomeReag, i->second, PesoC);
                }
                ConfirmarAlteração();
            }
        }

        Reag.close();
        ReagA.close();
    }

    void PrintHistorico()
    {
        std::fstream ReagH(ArqH.c_str(), std::ios::in | std::ios::binary);
        std::fstream ReagHA(ArqHA.c_str(), std::ios::in | std::ios::binary);

        if (ReagH.bad() || !ReagH.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 1386\n";

        if (ReagHA.bad() || !ReagH.is_open())
            std::cout << "Problema ao abrir o arquivo!!!\tLinha: 1389\n";

        unsigned int ReagHEnd;

        ReagH.seekg(0, std::ios::end);

        ReagHEnd = ReagH.tellg();

        ReagH.seekg(0, std::ios::beg);

        float PesoAnt = -1;

        while (ReagH.tellg() < ReagHEnd)
        {
            char Marca;
            ReagH.read(&Marca, sizeof(char));

            if (Marca != -1)
                std::cout << "Problemas ao ler o arquivo!!\tLinha: 657\n";

            if (PesoAnt > -1)
            {
                float Diferença, PesoVer;

                ReagH.read((char*)&PesoVer, sizeof(float));

                Diferença = PesoVer - PesoAnt;

                if (Diferença != 0)
                {
                    if(Diferença < 0)
                        std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferença << " g";
                    else
                        std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferença << " g";

                    PesoAnt = PesoVer;
                }
            }
            else
            {
                float PesoVer;

                ReagH.read((char*)&PesoVer, sizeof(float));

                PesoAnt = PesoVer;
            }
        }

        ReagHA.seekg(0, std::ios::end);

        ReagHEnd = ReagHA.tellg();

        ReagHA.seekg(0, std::ios::beg);

        while (ReagHA.tellg() < ReagHEnd)
        {
            char Marca;
            ReagHA.read(&Marca, sizeof(char));

            if (Marca != -1)
                std::cout << "Problemas ao ler o arquivo!!\tLinha: 657\n";

            if (PesoAnt > -1)
            {
                float Diferença, PesoVer;

                ReagHA.read((char*)&PesoVer, sizeof(float));

                Diferença = PesoVer - PesoAnt;

                if (Diferença != 0)
                {
                    if (Diferença < 0)
                        std::cout << "\n\n-------------------------\nPESO DIMINUIU EM: " << Diferença << " g";
                    else
                        std::cout << "\n\n-------------------------\nPESO AUMENTOU EM: " << Diferença << " g";

                    PesoAnt = PesoVer;
                }
            }
            else
            {
                float PesoVer;

                ReagHA.read((char*)&PesoVer, sizeof(float));

                PesoAnt = PesoVer;
            }
        }

        ReagH.close();
        ReagHA.close();
    }

 private:
     std::string Arq  = "Reagentes/" + Nome + ".dat";
     std::string ArqV = "Reagentes/" + Nome + "_Vencido.dat";
     std::string ArqA = "Reagentes/" + Nome + "_Antes.dat";
     std::string ArqD = "Reagentes/" + Nome + "_Depois.dat";
     std::string ArqH = "Reagentes/" + Nome + "_Historico.dat";
     std::string ArqM = "Reagentes/" + Nome + "_Alterados.dat";//contem os possiveis reagentes que foram alterados quando o peso muda
     std::string ArqMi = "Reagentes/" + Nome + "_Minimo.dat";
     std::string ArqHA = "Reagentes/" + Nome + "_HistoricoAntigo.dat";

    const float DesvioAceitavel = 0; //para +/-

    unsigned int EscolherReag(std::string& Nome)
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 190\n";
            return -1;
        }

        std::deque<unsigned int> PosiveisDistancias; //salva depois das marcas

        bool tem = false;

        Reag.seekg(0, std::ios::end);
        unsigned int End = Reag.tellg(); //salva onde é o final do arquivo

        Reag.seekg(0, std::ios::beg);

        //passa pelo arquivo inteiro procurando os nomes
        while (Reag.good())
        {
            char Marca = 0;
            Reag.read(&Marca, sizeof(char));

            if (Marca == -1)
            {
                unsigned int ComecoReag = Reag.tellg();

                unsigned int NomeSize;
                Reag.read((char*)&NomeSize, sizeof(unsigned int));

                bool Achou = true;

                char NomeSalvoChar;
                int NomeInd = 0;

                //procura o nome é vê se é igual ao nome sendo procurado
                for (; NomeInd < NomeSize; NomeInd++)
                {
                    Reag.read(&NomeSalvoChar, sizeof(char));

                    if (NomeSalvoChar != Nome.c_str()[NomeInd])
                    {
                        Achou = false;
                        break;
                    }
                }

                if (Achou == true)
                {
                    Reag.seekg(ComecoReag - 1, std::ios::beg);

                    Marca = 0;
                    Reag.read(&Marca, sizeof(char));

                    if (Marca == -1)
                    {
                        PosiveisDistancias.push_back(ComecoReag); //coloca em possiveis distancias a distancia depois dad marca do Reagente achado
                        tem = true;
                    }
                }

                Reag.seekg(ComecoReag + (sizeof(unsigned int) + NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4)), std::ios::beg);
            }
        }

        Reag.close(); //chegou ao fim do arquivo, precisa abrir ele de novo para conseguir utiliza-lo

        if (!tem)
        {
            std::cout << "O reagente escolhido nao existe!!\tLinha 255\n";
            return -1;
        }

        Reag.open(Arq.c_str(), std::ios::in | std::ios::binary);

        if (!Reag.is_open() || Reag.bad())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 268\n";
            return -1;
        }

        if (PosiveisDistancias.size() > 1)
        {
            std::cout << "mais de um reagente corresponde a essas informacoes, qual deles apagar?\n";

            //passa por todos os reagentes com o mesmo nome do escolhido e mostra seus pesos, nomes e datas de validade
            for (int i = 0; i < PosiveisDistancias.size(); i++) 
            {
                Reag.seekg(PosiveisDistancias[i] - 1, std::ios::beg);

                char Marca = 0;
                Reag.read(&Marca, sizeof(char));

                if (Marca == -1)
                {
                    unsigned int NomeSize;
                    Reag.read((char*)&NomeSize, sizeof(unsigned int));

                    char NomeChar;

                    std::cout << "\nRegente " << i << "\nNome: ";

                    for (int NomeInd = 0; NomeInd < (int)NomeSize; NomeInd++)
                    {
                        Reag.read(&NomeChar, sizeof(char));

                        std::cout << NomeChar;
                    }

                    float Peso;
                    Reag.read((char*)&Peso, sizeof(float));

                    Reag.seekg(sizeof(float), std::ios::cur);

                    int Validade[3];
                    Reag.read((char*)&Validade, sizeof(int) * 3);

                    std::cout << "\nPeso: " << Peso << "\nValidade: " << Validade[0] << " / " << Validade[1] << " / " << Validade[2] << '\n';
                }
                else
                {
                    std::cout << "problema ao ler o arquivo!!\tLinha: 313\n";
                    Reag.close();
                    return -1;
                }
            }

            std::cout << "\nDigiteo numero do reagente:\n> ";

            float Escolido;
            bool valido = false;

            std::string comando;
            std::getline(std::cin, comando);

            while (!valido)
            {
                valido = ValidadeInput(comando, Escolido);
                if (!valido)
                {
                    std::cout << "INUT INVALIDO!!!  tente novamente\n> ";
                    std::getline(std::cin, comando);
                }
                else if (Escolido >= PosiveisDistancias.size() || Escolido < 0)
                {
                    std::cout << "Escolha nao e possivel!!! tente novamente\n";
                    valido = false;

                    std::cout << "> ";
                    std::getline(std::cin, comando);
                }
            }
            Reag.close();
            return PosiveisDistancias[Escolido];
        }
        else if (PosiveisDistancias.size() == 1)
        {
            Reag.close();
            return PosiveisDistancias[0];
        }
        else 
        {
            std::cout << "Escolha nao existe!!\n";
            Reag.close();
            return -1;
        }

        Reag.close();
    }

    void GetAntes(std::string Arq, int LocalReag)//Salva as informações Antes do reagente seleconado no Arquivo *NOME*_Antes.dat
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        std::fstream ReagT(ArqA.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 364";
            return;
        }

        if (ReagT.bad() || !ReagT.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 371";
            return;
        }

        if (LocalReag != -1)
        {
            char marca = ' ';

            Reag.seekg(LocalReag);

            Reag.read(&marca, sizeof(char));

            if (marca == -1)
            {
                char* TempA = &marca;

                Reag.seekg(0, std::ios::beg);

                if ((LocalReag - 1))
                {
                    bool NewUsado = false;

                    if (LocalReag > 100)
                    {
                        TempA = new char[100];
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
                        TempA = new char[LocalReag];

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

    void GetDepois(std::string Arq, int LocalReag)//Salva as informações depois do reagente seleconado no Arquivo *NOME*_Depois.dat
    {
        std::fstream Reag(Arq.c_str(), std::ios::in | std::ios::binary);

        std::fstream ReagT(ArqD.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 439";
            return;
        }

        if (ReagT.bad() || !ReagT.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 445";
            return;
        }

        if (LocalReag < 0)
            LocalReag = 0;
           
        Reag.seekg(LocalReag);

        char Marca;

        Reag.read(&Marca, sizeof(char));

        if (Marca == -1)
        {
            unsigned int NomeSize;

            Reag.read((char*)&NomeSize, sizeof(unsigned int));

            Reag.seekg(NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4), std::ios::cur);

            unsigned int ExcluidoD = Reag.tellg();

            Reag.seekg(0, std::ios::end);
            unsigned int EndReag = Reag.tellg();

            char* TempD = &Marca;
            unsigned int TamanhoD;

            if (ExcluidoD < EndReag)
            {
                Reag.seekg(ExcluidoD, std::ios::beg);

                Marca = 0;

                Reag.read(&Marca, sizeof(char));

                if (Marca != -1)
                {
                    std::cout << "problema ao ler o arquivo!!\tLinha: 382\n";
                    return;
                }

                Reag.seekg(ExcluidoD, std::ios::beg);

                TamanhoD = EndReag - ((ExcluidoD));

                bool NewUsado = false;

                for (; TamanhoD > 100; TamanhoD -= 100)
                {
                    TempD = new char[100];
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
                    TempD = new char[TamanhoD];
                    Reag.read(TempD, TamanhoD);

                    ReagT.write(TempD, TamanhoD);

                    delete[] TempD;
                }
            }
        }

        Reag.close();
        ReagT.close();
    }

    void WriteAntes(std::string Arq)//Limpa o Arquivo *NOME*.dat e escreve as informações do arquivo *NOME*_Antes.dat nele
    {
        std::fstream Reag(Arq.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

        std::fstream ReagT(ArqA.c_str(), std::ios::in | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 521";
            return;
        }

        if (ReagT.bad() || !ReagT.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 527";
            return;
        }

        unsigned int Tamanho = 0;

        char a = ' ';
        char* Temp = &a;

        ReagT.seekg(0, std::ios::end);

        Tamanho = ReagT.tellg();

        ReagT.seekg(0, std::ios::beg);

        bool NewUsado = false;

        if (Tamanho > 100)
        {
            Temp = new char[100];
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
            Temp = new char[Tamanho];

            ReagT.read(Temp, Tamanho);

            Reag.write(Temp, Tamanho);

            delete[] Temp;
        }

        Reag.close();
        ReagT.close();
    }

    void WriteDepois(std::string Arq)//Escreve as informações do arquivo *NOME*_Depois.dat ao fim do arquivo *NOME*.dat 
    {
        std::fstream Reag(Arq.c_str(), std::ios::out | std::ios::app | std::ios::binary);

        std::fstream ReagT(ArqD.c_str(), std::ios::in | std::ios::binary);

        if (Reag.bad() || !Reag.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 576";
            return;
        }

        if (ReagT.bad() || !ReagT.is_open())
        {
            std::cout << "Problema ao abrir o arquivo!!\tLinha: 582";
            return;
        }

        unsigned int Tamanho = 0;

        char a = ' ';
        char* Temp = &a;

        ReagT.seekg(0, std::ios::end);

        Tamanho = ReagT.tellg();

        ReagT.seekg(0, std::ios::beg);

        bool NewUsado = false;

        if (Tamanho > 100)
        {
            Temp = new char[100];
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
            Temp = new char[Tamanho];

            ReagT.read(Temp, Tamanho);

            Reag.write(Temp, Tamanho);

            delete[] Temp;
        }

        Reag.close();
        ReagT.close();
    }

    void PrintInfo(int Lugar)
    {
        std::fstream Reag(Arq, std::ios::in | std::ios::binary);
        std::fstream ReagMin(ArqMi, std::ios::in | std::ios::binary);

        Reag.seekg(Lugar, std::ios::beg);

        int LugarMin = 0;

        while (Reag.tellg() < Lugar - 1)
        {
            char Marca = 0;

            Reag.read(&Marca, sizeof(char));

            if (Marca != -1)
                std::cout << "Problema ao ler o arquivo\tLinha: 441\n";

            LugarMin++;
            unsigned int NomeSize;
            Reag.read((char*)&NomeSize, sizeof(unsigned int));
            Reag.seekg(NomeSize + (sizeof(float) * 2) + (sizeof(int) * 4), std::ios::cur);
        }

        LugarMin *= sizeof(char) + sizeof(float);

        char Marca;
        Reag.read(&Marca, sizeof(char));

        if (Marca != -1)
            std::cout << "Problemas ao ler o arquivo!!!\tLinha: 1667";

        ReagMin.read(&Marca, sizeof(char));

        float Peso, PesoC, PesoMin;
        int Dia, Mes, Ano, Alerta, NomeSize;

        Reag.read((char*)&NomeSize, sizeof(int));

        char* NomeReag = new char[NomeSize + 1];

        Reag.read(NomeReag, NomeSize);
        Reag.read((char*)&Peso, sizeof(float));
        Reag.read((char*)&PesoC, sizeof(float));
        Reag.read((char*)&Dia, sizeof(int));
        Reag.read((char*)&Mes, sizeof(int));
        Reag.read((char*)&Ano, sizeof(int));
        Reag.read((char*)&Alerta, sizeof(int));
        ReagMin.read((char*)&PesoMin, sizeof(float));

        NomeReag[NomeSize] = '\0';

        std::cout << '\n' << NomeReag << "\nTotal: " << std::setw(14) << std::left << Peso << "Container: " <<  PesoC << "\nPeso minimo: " << PesoMin << "\nValidade: " << Dia << " / " << Mes << " / " << Ano << "\nALerta : " << Alerta << '\n';

        delete[] NomeReag;
    }
};

void Loop()
{
    std::string Comando;

    std::cout << "Digite o nome da Lista que deseja ver!!\n> ";

    std::getline(std::cin, Comando);

    Reagentes Reag(Comando);

    Reag.PesoAtual = Reag.PesoSalvo;
    
    while (true)
    {
        Reag.VerificarPeso();
        //solução temporaria!!

        std::cout << "\n\n--------------------\n\nCaso necessite de ajuda escreva \"ajuda\"\n\n> ";

        std::getline(std::cin, Comando);

        if (Comando == "Ajuda" || Comando == "ajuda")
        {
            std::cout <<    "\n\nOs comandos sao:\n" <<
                            "Criar\n" <<
                            "Remover\n" <<
                            "Alterar Nome\n" <<
                            "Alterar Peso\n" <<
                            "Alterar Minimo\n" <<
                            "Alterar Validade\n" <<
                            "Alterar Alerta\n" <<
                            "Ver Nomes\n" <<
                            "Ver Tudo\n" <<
                            "Pesquisar Especifico (pesquisa apartir do Nome, Peso ou Validade)\n" <<
                            "Verificar Validade\n" <<
                            "Verificar Minimo\n" <<
                            "Ver Mudancas\n" <<
                            "Ver Historico\n" <<
                            "Confirmar Mudancas\n" <<
                            "Limpar Historico\n" <<
                            "Sair\n\n\n";
            
            std::cout << "Digite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Sair" || Comando == "sair")
        {
            std::cout << "\n\nCerteza que deseja fechar o programa? (S/N)\n> ";

            std::getline(std::cin, Comando);

            if (Comando == "S" || Comando == "s")
                break;
            else
                continue;
        }

        if (Comando == "Criar" || Comando == "criar")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome, Caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string Nome;
                int Dia, Mes, Ano, Alerta;
                float Peso, PesoC, PesoMin,Temp;
                Temp = 0;

                Nome = Comando + '\0';

                std::cout << "\nDigite o Peso:\n> ";
                std::getline(std::cin, Comando);

                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                ValidadeInput(Comando, Peso);

                std::cout << "\nDigite o Peso do container:\n> ";
                std::getline(std::cin, Comando);

                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, PesoC);

                        if (PesoC > Peso)
                            Comando = "a";
                        else
                            break;
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                ValidadeInput(Comando, PesoC);

                std::cout << "\nDigite o Peso minimo:\n> ";
                std::getline(std::cin, Comando);

                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                ValidadeInput(Comando, PesoMin);

                std::cout << "\nDigite a data de Validade(dia, mes e ano, apertando enter entre eles):\n> ";

                std::getline(std::cin, Comando);
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp > 0 && Temp < 32)
                            break;
                        else
                            Comando = "a";
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                Dia = Temp;

                std::cout << "> ";
                std::getline(std::cin, Comando);
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp > 0 && Temp < 13)
                            break;
                        else
                            Comando = "a";
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                Mes = Temp;

                std::cout << "> ";
                std::getline(std::cin, Comando);
                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                ValidadeInput(Comando, Temp);
                Ano = Temp;

                std::cout << "\nDigite o Alerta(a prioridade para alertar)\n> ";
                std::getline(std::cin, Comando);
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp > NumeroAlertas)
                            Comando == "a";
                        else
                            break;
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                Alerta = Temp;
                
                //SUPER TEMPORARIO APAGUE POR FAVOR HORA QUE FOR MANDAR
                Reag.PesoAtual += Peso;

                Reag.CriarReagente(Nome, Peso, PesoC, PesoMin, Dia, Mes, Ano, Alerta);
            }
            continue;
        }

        if (Comando == "Remover" || Comando == "remover")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente que deseja Remover, cso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);
                
                if (Comando == "Sair" || Comando == "sair")
                    break;

                Reag.RemoverReagente(Comando);
            }

            continue;
        }

        if (Comando == "Alterar Nome" || Comando == "Alterar nome" || Comando == "alterar nome")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente que deseja trocar Nome e Depois o Novo Nome, cso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string NomeAnt = Comando;

                std::cout << "\n> ";
                std::getline(std::cin, Comando);

                Reag.TrocarNome(NomeAnt, Comando);
            }

            continue;
        }

        if (Comando == "Alterar Peso" || Comando == "Alterar peso" || Comando == "alterar peso")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente que deseja trocar o Peso e Depois o Novo Peso e o Novo peso do container, caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string Nome = Comando;

                std::cout << "\n> ";
                std::getline(std::cin, Comando);

                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }

                float NovoPeso;
                ValidadeInput(Comando, NovoPeso);


                std::cout << "> ";
                std::getline(std::cin, Comando);
                float NovoPesoC;
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, NovoPesoC);

                        if (NovoPesoC <= NovoPeso)
                            break;                            
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);               
                }
                
                Reag.TrocarPeso(Nome, NovoPeso, NovoPesoC);
            }

            continue;
        }

        if (Comando == "Alterar Minimo" || Comando == "Alterar minimo" || Comando == "alterar minimo")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente o novo peso minimo, caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string Nome = Comando;

                std::cout << "\n> ";
                std::getline(std::cin, Comando);

                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }

                float NovoPeso;
                ValidadeInput(Comando, NovoPeso);

                Reag.TrocarPesoMin(Nome, NovoPeso);
            }

            continue;
        }

        if (Comando == "Alterar Validade" || Comando == "Alterar validade" || Comando == "alterar validade")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente que deseja trocar a validade, digite o dia, mes e ano apertando enter entre  eles, caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string NomeAnt = Comando;


                float Temp = 0;

                std::cout << "> ";
                std::getline(std::cin, Comando);
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp > 0 && Temp < 32)
                            break;
                        else
                            Comando = "a";
                    }

                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                int Dia = Temp;

                std::cout << "> ";
                std::getline(std::cin, Comando);
                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp > 0 && Temp < 13)
                            break;
                        else
                            Comando = "a";
                    }
                    std::cout << "\n\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "\n> ";
                    std::getline(std::cin, Comando);
                }
                int Mes = Temp;

                std::cout << "> ";
                std::getline(std::cin, Comando);
                while (!ValidadeInput(Comando))
                {
                    std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "> ";
                    std::getline(std::cin, Comando);
                }
                ValidadeInput(Comando, Temp);
                int Ano = Temp;

                Reag.TrocarValidade(NomeAnt, Dia, Mes, Ano);
            }
            continue;
        }

        if (Comando == "Alterar Alerta" || Comando == "Alterar alerta" || Comando == "alterar alerta")
        {
            while (true)
            {
                std::cout << "\n\nDigite o nome do reagente e o novo alertas, caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                std::string NomeAnt = Comando;

                std::cout << "\n> ";
                std::getline(std::cin, Comando);

                float Temp;

                while (true)
                {
                    if (ValidadeInput(Comando))
                    {
                        ValidadeInput(Comando, Temp);

                        if (Temp >= 0 && Temp < NumeroAlertas)
                            break;
                        else
                            Comando = "a";
                    }

                    std::cout << "\n\nINPUT INVALIDO!! Tente Novamente\n";

                    std::cout << "\n> ";
                    std::getline(std::cin, Comando);

                }

                int NovoAlerta = Temp;

                Reag.TrocarAlerta(NomeAnt, NovoAlerta);
            }
            continue;
        }

        if (Comando == "Ver Nomes" || Comando == "Ver nomes" || Comando == "ver nomes")
        {
            Reag.PrintNomes();
            
            std::cout << "\n\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Ver Tudo" || Comando == "Ver tudo" || Comando == "ver tudo")
        {
            Reag.PrintInfos();

            std::cout << "\n\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Pesquisar Especifico" || Comando == "Pesquisar especifico" || Comando == "pesquisar especifico")
        {
            while (true)
            {
                std::cout << "\n\nDeseja pesquisar pelo que? Nome, Peso, Validade. Caso queira sair digite \"Sair\":\n> ";
                std::getline(std::cin, Comando);

                if (Comando == "Sair" || Comando == "sair")
                    break;

                if (Comando == "Nome" || Comando == "nome")
                {
                    std::cout << "\nQual nome pesquisar?:\n> ";
                    std::getline(std::cin, Comando);

                    Reag.PrintEspecifico(Comando);

                    continue;
                }

                if (Comando == "Peso" || Comando == "peso")
                {
                    std::cout << "\nDigite o peso total(reagente mais container) pelo qual procurar:\n> ";

                    std::getline(std::cin, Comando);

                    while (!ValidadeInput(Comando))
                    {
                        std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                        std::cout << "> ";
                        std::getline(std::cin, Comando);
                    }

                    float Temp;

                    ValidadeInput(Comando, Temp);

                    Reag.PrintEspecifico(Temp);

                    continue;
                }

                if (Comando == "Validade" || Comando == "validade")
                {
                    std::cout << "\nDigite a validade que deseja pesquisar(mes e ano, aperte enter entre eles)\n> ";
                    std::getline(std::cin, Comando);

                    float Temp;

                    while (true)
                    {
                        if (ValidadeInput(Comando))
                        {
                            ValidadeInput(Comando, Temp);

                            if (Temp > 0 && Temp < 13)
                                break;
                            else
                                Comando = "a";
                        }

                        std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                        std::cout << "> ";
                        std::getline(std::cin, Comando);
                    }
                    int Mes = Temp;

                    std::cout << "> ";
                    std::getline(std::cin, Comando);

                    while (!ValidadeInput(Comando))
                    {
                        std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                        std::cout << "> ";
                        std::getline(std::cin, Comando);
                    }
                    ValidadeInput(Comando, Temp);
                    int Ano = Temp;

                    Reag.PrintEspecifico(Mes, Ano);

                    continue;
                }
            }
            continue;
        }

        if (Comando == "Verificar Validade" || Comando == "Verificar validade" || Comando == "verificar validade")
        {
            std::cout << "\n\nDigite o alerta minimo(0 para mostrar todos os reagentes vencido)\n> ";

            std::getline(std::cin, Comando);

            float Temp;

            while (true)
            {
                if (ValidadeInput(Comando))
                {
                    ValidadeInput(Comando, Temp);

                    if (Temp >= 0 && Temp < NumeroAlertas)
                        break;
                    else
                        Comando = "a";
                }

                std::cout << "\nINPUT INVALIDO!!\n";
                std::cout << "> ";
                std::getline(std::cin, Comando);
            }
            int Alerta = Temp;

            Reag.ChecarValidade();
            Reag.PrintValidade(Alerta);

            std::cout << "\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Verificar Minimo" || Comando == "Verificar minimo" || Comando == "verificar minimo")
        {
            std::cout << "\n\nDigite o alerta minimo(0 para mostrar todos os reagentes vencido)\n> ";

            std::getline(std::cin, Comando);

            float Temp;

            while (true)
            {
                if (ValidadeInput(Comando))
                {
                    ValidadeInput(Comando, Temp);

                    if (Temp >= 0 && Temp < NumeroAlertas)
                        break;
                    else
                        Comando = "a";
                }

                std::cout << "\nINPUT INVALIDO!!\n";
                std::cout << "> ";
                std::getline(std::cin, Comando);
            }
            int Alerta = Temp;

            Reag.PrintMin(Alerta);

            continue;
        }

        if (Comando == "Ver Mudancas" || Comando == "Ver mudancas" || Comando == "ver mudancas")
        {
            Reag.SalvarAlteração();
            Reag.PrintAlterados();
            
             std::cout << "\n\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Ver Historico" || Comando == "Ver historico" || Comando == "ver historico")
        {
            Reag.PrintHistorico();

            std::cout << "\n\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        if (Comando == "Confirmar Mudancas" || Comando == "Confirmar mudancas" || Comando == "confirmar mudancas")
        {
            std::cout << "\n\nTem certeza que deseja marcar como verificado? (Ainda sera possivel verificar as mudancas porem com menos detalhes e estarao todas juntas), (digite sim para confirmar)\n> ";
            std::getline(std::cin, Comando);

            if(Comando == "Sim" || Comando == "sim")
                Reag.ConfirmarAlteração();

            continue;
        }

        if (Comando == "Limpar Historico" || Comando == "Limpar historico" || Comando == "limpar historico")
        {
            Reag.LimparHistorico();

            continue;
        }

        if (Comando == "Mudar A" || Comando == "Mudar a" || Comando == "mudar a")
        {
            std::cout << "\n\nPeso Atual\n> ";
            std::getline(std::cin, Comando);

            while (!ValidadeInput(Comando))
            {
                std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                std::cout << "> ";
                std::getline(std::cin, Comando);
            }

            float NovoPeso;
            ValidadeInput(Comando, NovoPeso);

            Reag.PesoAtual = NovoPeso;

            continue;
        }

        if (Comando == "Mudar S" || Comando == "Mudar s" || Comando == "mudar s")
        {
            std::cout << "\n\nPeso Salvo\n> ";
            std::getline(std::cin, Comando);

            while (!ValidadeInput(Comando))
            {
                std::cout << "\nINPUT INVALIDO!! Tente Novamente\n";

                std::cout << "> ";
                std::getline(std::cin, Comando);
            }

            float NovoPeso;
            ValidadeInput(Comando, NovoPeso);

            Reag.PesoSalvo = NovoPeso;

            continue;
        }

        if (Comando == "Pesos" || Comando == "pesos")
        {
            std::cout << "\nSalvo: " << Reag.PesoSalvo << "\nPeso Medido(atual): " << Reag.PesoAtual << '\n';

            std::cout << "\n\nDigite qualquer coisa para sair:\n> ";
            std::getline(std::cin, Comando);

            continue;
        }

        std::cout << "\nCOMANDO NAO EXISTE!!!\n";
    }
}

int main()
{
    Loop();
}

