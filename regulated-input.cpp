#include "regulated-input.h"

bool ValidadeInput(std::string input, float& result)
{

    bool negative = false;

    bool fraction = false;

    int FractionPosition = 0;

    result = 0;

    for (int i = 0; i < input.size(); i++)
    {
        float num = 0;

        if (input[i] != '0' && input[i] != '1' && input[i] != '2' && input[i] != '3' && input[i] != '4' && input[i] != '5' && input[i] != '6' && input[i] != '7' && input[i] != '8' && input[i] != '9' && !(input[i] == '-' && i == 0) && !((input[i] == '.' || input[i] == ',') && fraction == false))
        {
            std::cout << std::endl << std::endl << "INPUT INVALID" << std::endl;
            return false;
        }
        else
        {

            switch (input[i])
            {
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
                negative = true;
                break;

            case '.':
                fraction = true;

            case ',':
                fraction = true;

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
    }

    if (negative == true)
    {
        result *= -1;
    }

    return true;
}
