// Program: Tideman evaluates winner of an election. It accepts candidate names as CLI arguments

#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void quicksort(int low, int high);
int partition(int low, int high);
void swap(pair *a, pair *b);
void lock_pairs(void);
bool cycle(char chain[], int loser);
void print_winner(void);



int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // Ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}



// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        // Checking if vote is for a valid candidate
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}



// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    // Loop traverses through rows of preferences[][]
    for (int i = 0; i < candidate_count - 1; i++)
    {
        // Loop traverses through columns of preferences[][]
        for (int j = i + 1; j < candidate_count; j++)
        {
            int i_pos, j_pos;

            // Find position of i and j in ranks[]
            for (int k = 0; k < candidate_count; k++)
            {
                if (ranks[k] == i)
                {
                    i_pos = k;
                }
                if (ranks[k] == j)
                {
                    j_pos = k;
                }
            }
            // Compares positions and updates preferences[][]
            if (i_pos < j_pos)
            {
                ++preferences[i][j];
            }
            else
            {
                ++preferences[j][i];
            }
        }
    }
    return;
}



// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    int k = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Compares votes and decides winner and loser amongst pairs
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[k].winner = i;
                pairs[k].loser = j;
                ++k;
            }
            if (preferences[i][j] < preferences[j][i])
            {
                pairs[k].winner = j;
                pairs[k].loser = i;
                ++k;
            }
        }
    }

    pair_count = k;

    return;
}



// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
    quicksort(0, pair_count - 1);

    return;
}



// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    // Locking pairs
    for (int i = 0; i < pair_count; i++)
    {
        // chain stores a chain of candidates, each candidate is followed by another candidate it beat
        char chain[candidate_count];

        int winner = pairs[i].winner, loser = pairs[i].loser;

        sprintf(chain, "%d%d", winner, loser);
        // Lock if there is no cycle
        if (!cycle(chain, loser))
        {
            locked[winner][loser] = true;
        }
    }
    return;
}



// Print the winner of the election
void print_winner(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        bool lost = false;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                lost = true;
                break;
            }
        }
        if (!lost)
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
}

























void quicksort(int low, int high)
{
    if (low < high)
    {
        int pi = partition(low, high);
        quicksort(low, pi - 1);
        quicksort(pi + 1, high);
    }
}



// Partition pairs[]
int partition(int low, int high)
{
    int pivot = preferences[pairs[high].winner][pairs[high].loser] - preferences[pairs[high].loser][pairs[high].winner];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner] > pivot)
        {
            i++;
            swap(&pairs[i], &pairs[j]);
        }
    }
    swap(&pairs[i + 1], &pairs[high]);
    return i + 1;
}



// Swap pairs
void swap(pair *a, pair *b)
{
    pair temp = *a;
    *a = *b;
    *b = temp;
}



// Checks whether locking would create a cycle
bool cycle(char chain[], int loser)
{
    // Traversing through loser's list to see if he beat anyone that is locked
    for (int i = 0; i < candidate_count; i++)
    {
        if (i != loser)
        {
            // Checks if loser has a win
            if (locked[loser][i] == true)
            {
                // Storing the candidate i, who loser beat in a string ch[]
                char ch[2];
                sprintf(ch, "%d", i);

                int length = strlen(chain);

                // Traversing through chain[]
                for (int j = 0; j < length; j++)
                {
                    // Check if ch[0] already in chain[]
                    if (chain[j] == ch[0])
                    {
                        return true;
                    }
                    else
                    {
                        // Concatenate ch[] to chain[]
                        strcat(chain, ch);

                        // Check if ch[0] creates cycle
                        if (cycle(chain, i))
                        {
                            return true;
                        }
                        else
                        {
                            // Trimming chain[] to original length
                            int newLength = strlen(chain);
                            for (int k = length; k < newLength; k++)
                            {
                                chain[k] = '\0';
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
