// Question and Answer arrays
const String q_array[] {"Question 1?", \
                "Question 2 is a two line long question?", \
                "Question 3 is way longer at three whole lines long?", \
                "Question 4 is so long that it actually needs to be cutoff a bit to be three lines?"};
#define NUMQ sizeof(q_array)/6
const String a_array[] {"one", \
                "two", \
                "three", \
                "four"};
                
// Match key inputs with electrode numbers
// Default: Number pad format
#define DELETE 0
#define PQRS 1
#define GHI 2
#define CHECK 3
#define SPACE 4
#define TUV 5
#define JKL 6
#define ABC 7
#define ENTER 8
#define WXYZ 9
#define MNO 10
#define DEF 11
