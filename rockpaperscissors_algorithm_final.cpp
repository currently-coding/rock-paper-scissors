#include <iostream>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <map>

enum struct Result
{
    PLAYER_1_WINS,
    PLAYER_2_WINS,
    DRAW,
    UNDEFINED, // to represent initial value before game's started
};

enum struct Item
{
    rock,
    paper,
    scissors,
};

struct Score
{
    int player1;
    int player2;

    void final_result(int mode)
    {
        using namespace std;

        cout << "*** Final Score: ";

        short result = player1 - player2;

        if (result == 0)
        {
            cout << "DRAW";
        }
        else if (result > 0)
        {
            cout << "Player1 wins";
        }
        else if (result < 0)
        {
            if (mode == 1)
            {
                cout << "Computer wins";
            }
            else
            {
                cout << "Player2 wins";
            }
        }
        cout << " ***" << endl;
    }
};

struct Move
{
    Item player_1_move;
    Item player_2_move;

    int to_int(int player) // 1 = Rock, 2 = Paper, 3 = Scissors
    {
        switch (player)
        {
        case 1:
            switch (player_1_move)
            {
            case Item::rock:
                return 1;
            case Item::paper:
                return 2;
            case Item::scissors:
                return 3;
            default:
                break;
            }
        case 2:
            switch (player_2_move)
            {
            case Item::rock:
                return 1;
            case Item::paper:
                return 2;
            case Item::scissors:
                return 3;
            default:
                break;
            }
        default:
            throw std::runtime_error("Error in `move.to_int(int player)`. Player unknown(x != 1, x != 2)");
        }
    }
};

struct Game // stores everything related to one round
{
    Move move;

    Result res;

    Score score;

    int round_number;

    void output_result(Score *score)
    {
        switch (res)
        {
        case Result::DRAW:
            std::cout << "\nResult: Draw...\n\n";
            score->player1++;
            score->player2++;
            break;
        case Result::PLAYER_1_WINS:
            std::cout << "\nResult: Player 1 won\n\n";
            score->player1++;
            break;
        case Result::PLAYER_2_WINS:
            std::cout << "\nResult: Player 2 won\n\n";
            score->player2++;
            break;
        default:
            throw "\n\nError in function `main()` Unknown Game Result.";
        }
    }

    Result simulate_move() // calculate result
    {
        if (move.player_1_move == move.player_2_move)
        {
            return Result::DRAW;
        }
        switch (move.player_1_move)
        {
        case Item::rock:
            if (move.player_2_move == Item::scissors)
            {
                return Result::PLAYER_1_WINS;
            }
            else
            {
                return Result::PLAYER_2_WINS;
            }
            break;
        case Item::scissors:
            if (move.player_2_move == Item::paper)
            {
                return Result::PLAYER_1_WINS;
            }
            else
            {
                return Result::PLAYER_2_WINS;
            }
            break;
        case Item::paper:
            if (move.player_2_move == Item::rock)
            {
                return Result::PLAYER_1_WINS;
            }
            else
            {
                return Result::PLAYER_2_WINS;
            }
        default:
            throw std::runtime_error("Error in function `play_items(i1, i2)`: Result unknown.");
        }
    }
};

struct MoveProbability
{
    int move;          // 1 = Rock, 2 = Paper, 3 = Scissors
    float probability; // Probability within a group defined by the result and the computer move that led to it
};

struct Probabilities
{
    // maps result to map a of computer moves to vector of human moves(MoveProbability)
    std::map<Result, std::map<int, std::vector<MoveProbability>>> probabilities;

    // maps result to a map of a computer moves to a map of the move and its number of occurrences
    std::map<Result, std::map<int, std::map<int, int>>> values; // holds total number of values for each move in each category

    Probabilities() : probabilities{// ugly constructor
                                    {Result::PLAYER_1_WINS,
                                     {{1, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {2, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {3, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}}}},
                                    {Result::PLAYER_2_WINS,
                                     {{1, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {2, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {3, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}}}},
                                    {Result::DRAW,
                                     {{1, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {2, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}},
                                      {3, {{1, 0.33F}, {2, 0.33F}, {3, 0.33F}}}}}}
    {
        values = {
            {Result::PLAYER_1_WINS, {{1, {{1, 0}, {2, 0}, {3, 0}}}, {2, {{1, 0}, {2, 0}, {3, 0}}}, {3, {{1, 0}, {2, 0}, {3, 0}}}}},
            {Result::PLAYER_2_WINS, {{1, {{1, 0}, {2, 0}, {3, 0}}}, {2, {{1, 0}, {2, 0}, {3, 0}}}, {3, {{1, 0}, {2, 0}, {3, 0}}}}},
            {Result::DRAW, {{1, {{1, 0}, {2, 0}, {3, 0}}}, {2, {{1, 0}, {2, 0}, {3, 0}}}, {3, {{1, 0}, {2, 0}, {3, 0}}}}}};
    }

    void add(Game last_round, Game this_round)
    {
        int human_move = this_round.move.to_int(1);
        int computer_move = last_round.move.to_int(2);
        Result result = last_round.res;

        int result_number = 0;

        // incrementing values maps
        values[result][computer_move][human_move]++;

        this->update(human_move, computer_move, result);
    }
    void update(int human_move, int computer_move, Result result)
    {
        // Updates Probabilities using the values map

        // Human Move Probability calculation: How many moves in this category and this computer move are followed by this specific human move ? Divide that by the total number of moves within this category
        int group_total = 0;
        int human_move_total = 0;
        int human_plus_one_move_total = 0;
        int human_plus_two_move_total = 0;

        for (auto &entry : values[result][computer_move])
        {
            group_total += entry.second;

            if (entry.first == human_move)
            {
                human_move_total = entry.second;
            }
            else if (entry.first == (human_move + 1) % 3)
            {
                human_plus_one_move_total = entry.second;
            }
            else if (entry.first == (human_move + 2) % 3)
            {
                human_plus_two_move_total = entry.second;
            }
        }

        int offset_0 = human_move;
        int offset_1 = (human_move % 3) + 1;
        int offset_2 = ((human_move + 1) % 3) + 1;

        // Update probabilities based on group_total
        if (group_total > 0)
        {
            for (auto &move : probabilities[result][computer_move])
            {
                if (move.move == offset_0)
                {
                    move.probability = (float)human_move_total / group_total;
                }
                if (move.move == offset_1)
                {
                    move.probability = (float)human_plus_one_move_total / group_total;
                }
                if (move.move == offset_2)
                {
                    move.probability = (float)human_plus_two_move_total / group_total;
                }
            }
        }
    }
};

void printMoveProbability(const MoveProbability &moveProbability)
{
    std::cout << "Human Response: " << moveProbability.move << ", Probability: " << moveProbability.probability << std::endl;
}

void printProbabilities(const Probabilities &probabilities)
{
    for (const auto &resultEntry : probabilities.probabilities)
    {
        Result result = resultEntry.first;
        std::cout << "Result: ";
        switch (result)
        {
        case Result::PLAYER_1_WINS:
            std::cout << "Human Win";
            break;
        case Result::PLAYER_2_WINS:
            std::cout << "Computer win";
            break;
        case Result::DRAW:
            std::cout << "Draw";
            break;
        case Result::UNDEFINED:
            std::cout << "Undefined";
            break;
        }
        std::cout << std::endl;

        for (const auto &computerMoveEntry : resultEntry.second)
        {
            int computerMove = computerMoveEntry.first;
            std::cout << "Computer Move: " << computerMove << std::endl;

            for (const auto &moveProbability : computerMoveEntry.second)
            {
                printMoveProbability(moveProbability);
            }
        }
    }
}

Item beats(Item to_beat) // returns item that beats input Item
{
    switch (to_beat)
    {
    case Item::paper:
        return Item::scissors;
    case Item::rock:
        return Item::paper;
    case Item::scissors:
        return Item::rock;
    default:
        throw std::runtime_error("Unknown Item to convert");
    }
}

Item int_to_item(int num) // converts Item to int for calculations: 1=Rock,2=Paper,3=Scissors
{
    switch (num)
    {
    case 1:
        return Item::rock;
    case 2:
        return Item::paper;
    case 3:
        return Item::scissors;
    default:
        throw std::runtime_error("Error in function `int_to_item(num)`: Item unknown.");
    }
}

Item calculate_based_on_prior_moves(Game game, Probabilities &probabilities)
{
    using namespace std;

    // read through game data - store last computer move and last human move;
    int last_computer_move = game.move.to_int(2);
    int last_human_move = game.move.to_int(1);
    Result last_result = game.res;

    vector<MoveProbability> possible_moves = probabilities.probabilities[last_result][last_computer_move];

    float highest_probability = 0.0f;
    int next_human_move(0);

    for (auto &entry : possible_moves)
    {
        if (entry.probability >= highest_probability)
        {
            highest_probability = entry.probability;
            next_human_move = entry.move;
        }
    }

    Item next_human_move_item = int_to_item(next_human_move);
    std::cout << "Human will play: " << next_human_move << std::endl;

    return beats(next_human_move_item);
}

Item read_item(int player)
{
    using namespace std;

    int choice(0);

    cout << "\nPlayer " << player << ": Rock(1), Paper(2), Scissors(3) ... Shoot: ";

    cin >> choice;

    Item item = int_to_item(choice);

    return item;
}

Item random_item()
{
    int r = rand() % 3; // random value (0, 1 or 2)
    return int_to_item(++r);
}

int choose_gamemode()
{
    using namespace std;
    int mode(0);
    cout << "Welcome!\n\nPlease choose a gamemode:\n(1) Human vs Computer\n(2) Human vs Human\n\nChoice: ";
    cin >> mode;
    return mode;
}

int get_rounds()
{
    using namespace std;
    int num_rounds(0);
    cout << "How many rounds do you wish to play ? Rounds: ";
    cin >> num_rounds;
    return num_rounds;
}

void call_move(const Item item, const int player)
{
    using namespace std;
    if (player == 3)
    {
        cout << "Computer played ";
    }
    else
    {
        cout << "Player " << player << " played ";
    }
    switch (item)
    {
    case Item::paper:
        cout << "paper." << endl;
        break;
    case Item::rock:
        cout << "rock." << endl;
        break;
    case Item::scissors:
        cout << "scissors." << endl;
        break;
    default:
        cout << "<Unknown Item>" << endl;
        break;
    }
    cout << "\n"
         << endl;
}

int main()
{
    srand(time(NULL)); // initialize the random generator

    int mode = 0; // 1 = ComputerVsHuman, 2 = HumanVsHuman
    Score score = {0, 0};
    int algorithm = 0;
    std::vector<Score> score_data;
    Probabilities probabilities;
    std::vector<Game> game_data;
    int size_game_data = size(game_data);

    mode = choose_gamemode();

    if (mode < 1 || mode > 2)
    {
        throw std::runtime_error("Error in function `main()`: Unknown Mode.");
    }

    int num_rounds = get_rounds();

    Item i2;
    i2 = Item::paper; // against male players
    i2 = Item::rock;  // against female players

    for (int rounds_played(1); rounds_played <= num_rounds; rounds_played++)
    { // repeat for every round
        std::cout << "\n*** Round " << rounds_played << " ***" << std::endl;

        Item i1 = read_item(1);

        call_move(i2, 3); // Ausgabe
        call_move(i1, 1); // Ausgabe

        Game game{Move{i1, i2}, Result::UNDEFINED, score, rounds_played}; // initialisieren des Game Structs `game`

        game.res = game.simulate_move(); // Zug durchspielen und Ergebnis in `game.res(ult)` schreiben
        game.output_result(&score);      // pointer, da der score über das ganze Spiel hinweg gespeichert werden muss
        game.score = score;              // updaten von score

        game_data.push_back(game);        // speichern in vector um daten nachher analysieren zu können
        size_game_data = size(game_data); // um size() nur 1x aufzurufen, da `game_data` recht groß werden kann

        if (size_game_data >= 2)
        {
            // Letzten Computer Zug, letztes Ergebnis mit nächstem Mensch Zug verknüpfen
            probabilities.add(game_data[size_game_data - 2], game_data[size_game_data - 1]);
        }

        i2 = calculate_based_on_prior_moves(game, probabilities);

        std::cout << "\n\nData:" << std::endl;
        printProbabilities(probabilities); // for debug purposes
    }
    score.final_result(mode);
}