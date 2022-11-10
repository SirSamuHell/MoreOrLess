#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <ctime>
#include <tuple>
#include <random>
#include <sstream>

// Nombre d'essaie maximum
constexpr int MAX_TRYING = 10;
 
// Définition des choix possible 
enum class MainMenuChoice {PLAY = 1, SCORE = 2, QUIT = 3}; // Les 3 choix possible dans le menu principale tel que jouer/regarder le score/ et fermer l'application 
enum class LevelMenuChoice {VERY_LOW = 1, LOW = 2, MEDIUM = 3, HARD = 4}; // Définition la difficulter VERY_LOW (0-100) LOW (100-1000) MEDIUM (1000-10000) HARD (10000-100000)

// Variable global 
static std::fstream fScore {"score", std::ios::in | std::ios::out | std::ios::binary};
static std::fstream fLog{"logs.txt", std::ios::out | std::ios::app}; // Fichier de logs 

// Déclaration des protoypes de fonction
static void DisplayMainMenu();
static void DisplayLevelMenu();
static void DisplayLastScore();
static void CloseApplication();
static void WriteLog(const std::string& msg);
static int GenerateRandomNumber(const int min, const int max);
static std::tuple<bool, std::string> Check(const int proposition, int numberToFind);
static int ConsoleInputInteger(const std::string& msg, const int minInterval, const int maxInterval);
static std::tuple<int, int> MinMaxFromLevel(const LevelMenuChoice level);
static void RegisterScoreFromFile(int score);
static int GetScoreFromFile();

int main()
{
    while(true)
    {
        DisplayMainMenu();
        MainMenuChoice choiceMainMenu = static_cast<MainMenuChoice>(ConsoleInputInteger("Select >", 1, 3));
    
        if(choiceMainMenu ==  MainMenuChoice::PLAY)
        {
            DisplayLevelMenu();
            LevelMenuChoice choiceLevelMenu = static_cast<LevelMenuChoice>(ConsoleInputInteger("Select >", 1, 4));
            const std::tuple<int, int> interval = MinMaxFromLevel(choiceLevelMenu);
            const int min = std::get<0>(interval);
            const int max = std::get<1>(interval);
            int randomNumber = GenerateRandomNumber(min, max); 
            int remainingTry = MAX_TRYING;
            bool bGameOver = false;
 
            while(!bGameOver)
            {
                std::stringstream buffer;
                buffer << "Proposition " << remainingTry << "/" << MAX_TRYING << " > ";

                int proposition = ConsoleInputInteger(buffer.str(), min, max);
                std::tuple<bool, std::string> checkedProposition = Check(proposition, randomNumber);
                std::string information = std::get<1>(checkedProposition);
                bGameOver = std::get<0>(checkedProposition);

                if(bGameOver)
                {
                    std::cout << information << std::endl;
                    RegisterScoreFromFile((remainingTry * static_cast<int>(choiceLevelMenu)) + GetScoreFromFile());
                }
                else 
                {
                    std::cout << information << std::endl;
                    remainingTry--;

                    if(remainingTry==0)
                    {
                        std::cout << "You have used all your remaining trying point ! Game Over !" << std::endl;
                        bGameOver = true;
                    }
                }


            }

        }
        else if(choiceMainMenu == MainMenuChoice::SCORE)
        {
            DisplayLastScore();
        }
        else 
        {
            CloseApplication();
        }
    }
    return 0;
}

/**
 * AFFICHE LE MENU PRINCIPALE DU JEUX DANS LA CONSOLE 
*/
static void DisplayMainMenu()
{
    WriteLog("DisplayMainMenu is called.");

    std::cout << "================================" << std::endl;
    std::cout << "= WELCOME TO MORE OR LESS GAME =" << std::endl;
    std::cout << "================================" << std::endl;

    std::cout << "\t- Press (1) to play" << std::endl;
    std::cout << "\t- Press (2) to consult last score" << std::endl;
    std::cout << "\t- Press (3) to quit" << std::endl;
}
/**
 * AFFICHE LE MENU DES DIFFUCULTÉ
*/
static void DisplayLevelMenu()
{
    WriteLog("DisplayLevelMenu is called.");

    std::cout << "=========================" << std::endl;
    std::cout << "= SELECT THE DIFFICULTY =" << std::endl;
    std::cout << "=========================" << std::endl;

    std::cout << "\t- Press (1) for a number between 1 and 100" << std::endl;
    std::cout << "\t- Press (2) for a number between 100 and 1000" << std::endl;
    std::cout << "\t- Press (3) for a number between 1000 and 10000" << std::endl;
    std::cout << "\t- Press (4) for a number between 10000 and 100000" << std::endl;
}
/**
 * Affiche le dernier score connu du joueur
*/
static void DisplayLastScore()
{
    WriteLog("DisplayLastScore is called.");
    std::cout << "You're last score is: " << GetScoreFromFile() << std::endl;
}
/**
 * Ferme l'application proprement
*/
static void CloseApplication()
{
    std::cout << "Bey !" << std::endl;
    WriteLog("The application is terminated...");
    fScore.close();
    fLog.close();
    std::exit(EXIT_SUCCESS);
}
/**
 * Ecrit dans un fichier les logs si nécessaire...
*/
static void WriteLog(const std::string& msg)
{
    time_t timestamp = time(0);
    char szDate[255];
    strftime(szDate, 255, "%m/%d/%Y %H:%M", localtime(&timestamp));
    fLog << "["<< szDate << "] -> "<< msg << "\n";
}
/**
 * Fonction de saisi d'entrée utilisateur entre un interval donnée 
 * la fonction vérifie les erreurs de saisis et retourne le nombre saisi
 * dans l'interval demander...
*/
static int ConsoleInputInteger(const std::string& msg, const int minInterval, const int maxInterval)
{
    int data = 0;  
    bool bInputOk = false;

    while(!bInputOk)
    {
        bInputOk = true;
        std::cout << msg;
        std::cin >> data;
         
        if(!std::cin.good() || data < minInterval || data > maxInterval)
        {   
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "You need to input a number between " << minInterval << " and " << maxInterval << std::endl;
            WriteLog("Bad user input.");
            bInputOk = false;
        }
    }
    WriteLog("The input is correct.");
    return data;
}

static std::tuple<bool, std::string> Check(const int proposition, int numberToFind)
{
    if(proposition > numberToFind)
        return std::make_tuple<bool, std::string>(false, "It's less !");
    if(proposition < numberToFind)
        return std::make_tuple<bool, std::string>(false, "It's more !");
    return std::make_tuple<bool, std::string>(true, "You Win !");
}
/**
 * Génére un nombre aléatoire selon la difficulter en paramètre
*/
static int GenerateRandomNumber(const int min, const int max)
{
    std::random_device rd;
    return std::uniform_int_distribution<int>(min, max)(rd);
}
/**
 * Donne le nombre minimum et maximum dans un tuple en fonction du choix
 * fait dans le menu de difficulter.
*/
static std::tuple<int, int> MinMaxFromLevel(const LevelMenuChoice level)
{
    switch(level)
    {
        case LevelMenuChoice::VERY_LOW  :       return std::tuple<int, int>(1, 100);
        case LevelMenuChoice::LOW       :       return std::tuple<int, int>(100, 1000);
        case LevelMenuChoice::MEDIUM    :       return std::tuple<int, int>(1000, 10000);   
        case LevelMenuChoice::HARD      :       return std::tuple<int, int>(10000, 100000);
        default                         :       return std::tuple<int, int>(1, 100);
    }
}
/*
* Enregistre dans le fichier le score du joueur passez en paramètre
*/
static void RegisterScoreFromFile(int score)
{
    if(!fScore.is_open())
    {
        WriteLog("Impossible to open score file is open.");
        return;
    }
    WriteLog("Write in file the new score");
    fScore.seekg(std::ios_base::beg);
    fScore.write(reinterpret_cast<char*>(&score), sizeof(score));
}

/**
 * Récupère depuis le fichier le score du joueur 
*/
static int GetScoreFromFile()
{
    if(!fScore.is_open())
    {
        WriteLog("Impossible to open score file is open.");
        return 0;
    }
    WriteLog("The score file is open.");    

    int score = 0;
    fScore.seekg(std::ios_base::beg);
    fScore.read(reinterpret_cast<char*>(&score), sizeof(score));
 
    return score;
}
