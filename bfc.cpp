/* BFC : a brainfuck interpreter in C++ */

#include <vector>
#include <stack>
#include <iostream>

#include <cstdio>
#include <cstring>

typedef unsigned int uint;

/// Prints a character for display (correctly escapes unprintables)
std::string print_char(const char& c)
{
    // Printable characters get no special treatment
    if (std::isprint(c)) {
        return std::string(1,c);
    }

    // Escaped characters
    switch (c){
        case 0: return "\\0";
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        default:  // Characters with no meanings get printed in hex
                   char buffer[5]; // Room for "\x00"
                   snprintf(buffer, 5, "\\x%02x",c);
                   return std::string(buffer);
    }
}

/// The main class: holds the state of a program
class State {

    public:
        State( uint size = 30000 ): 
            array(size,0),
            data(0)
            {}

        // States can be copied
        State(const State& other) = default;
        State(State&& other) = default;
        State& operator= (const State& other) = default;

        // Brainfuck basic operations

        /// Moves the data pointer to the next cell to the right
        void next_cell() {
            if (data < array.size() +1) {
                data++;
            }
            else {
                // throw exception
            }
        }

        /// Moves the data pointer to the next cell to the left
        void prev_cell() {
            if (data > 0 ){ 
                --data;
            }
            else {
                // throw exception
            }
        }

        /// Increase the value at the current cell
        void incr_data() {
            ++array[data];
        }

        /// Decrease the value at the current cell
        void decr_data() {
            --array[data];
        }

        /// Prints the value at the current cell
        void output() {
            std::putchar(array[data]);
        }
        /// Writes input at the current cell
        void input() {
            array[data]=std::getchar();
        }


        /// Prints the debug state of the program
        void dbg(const char* prog, const char* ip) {
            std::cout<<prog<<std::endl; // Print the program string
            for (uint i = 0; i < ip - prog; ++i) { std::cout<<" ";} 
            std::cout<<"^"<<std::endl;// Shows the current instruction pointer

            // Print the current cell index and its value as an int and a char
            std::cout<<"[:"<<data<<"] "<<(int)array[data]<<" '"<<print_char(array[data])<<"'"<<std::endl;
            std::cout<<"LS: "<<loop_stack.size(); // Print the loop stack size
            if (loop_stack.size() > 0) {          // and the top value if any
                std::cout<<" ("<<loop_stack.top()<<")";
            }
            std::cout<<std::endl;
        }

        // Implement the jumping of a loop block
        void jump( const char* prog, const char*& ip) {
            uint loop_counter = 0;
            ++ip; // Move to instruction next to the opening bracked
            while( true ) { // TODO :check end of prog not reached.
                
               // std::cout<<"JMP"<<std::endl;
                //dbg(prog,ip);
                //getchar();
                char instr = *ip;
                ++ip;
                switch(instr)
                {
                    case'[': ++loop_counter; ; break; // Each new loop we cross increments the loop counter
                    case']':
                             if (loop_counter == 0) { // If we reached the end of our loop
                                 return;
                             }
                             else { // this was not our loop
                                 --loop_counter;
                             }
                             break;
                    default: break; // Ignore all other instructions (or comment)
                }
            }
        }

        /// Executes one instruction of the given program at the given instruction pointer
        void exec( const char* prog, const char*& ip ){
            char instr = *ip;

            //dbg(prog, ip);
            //getchar();

            // If we are in "jump" mode, we just skip to the end of the current loop
            // (jump mode should be removed)
            switch (instr)
            {  
                // Basic instructions
                case '>': next_cell(); ++ip; break;
                case '<': prev_cell(); ++ip; break;
                case '+': incr_data(); ++ip; break;
                case '-': decr_data(); ++ip; break;
                case '.': output();    ++ip; break;
                case ',': input();     ++ip; break;
                // Control flow logic
                case '[': {
                              if (array[data]) { // Start of a loop
                                  loop_stack.push(ip - prog);
                                  ++ip;
                              }
                              else { // Conditionnal jump
                                  jump(prog,ip);
                              }
                              break;
                          }
                case']': {
                             if (array[data]){ // Loop back 
                                 uint offset = loop_stack.top();
                                 ip = prog + offset+1;
                             }
                             else { // Exit loop
                                 loop_stack.pop();
                                 ++ip;
                             }
                             break;
                         }

                default: // All other characters are comments
                         break;

            }
        }

    private:
        // Memory stuff
        std::vector<char> array;  // Memory array
        uint data;                // Data pointer

        // Loop state
        std::stack<uint> loop_stack; // List of loop offsets in program strings
};


int main(int argc, char** argv)
{
    State state;

    const char* helloworld="++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";

    const char* prog = argc > 1  ? argv[1] : helloworld;;
    const uint len = std::strlen(prog);
    const char* ip = prog;
    do {
        state.exec(prog, ip);
    } while( ip != prog + len);

    return 0;
}





