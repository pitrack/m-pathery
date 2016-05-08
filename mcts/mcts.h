#ifndef MCTS_HEADER_PETTER
#define MCTS_HEADER_PETTER
#define TREE true
#define ROOTS 4
#define PTRS 4
//
// Petter Strandmark 2013
// petter.strandmark@gmail.com
// Modified by Patrick Xia
// Monte Carlo Tree Search for finite games.
//
// Originally based on Python code at
// http://mcts.ai/code/python.html
//
// Uses the "root parallelization" technique [1].
//
// This game engine can play any game defined by a state like this:
/*

class GameState
{
public:
	typedef int Move;
	static const Move no_move = ...

	void do_move(Move move);
	template<typename RandomEngine>
	void do_random_move(*engine);
	bool has_moves() const;
	std::vector<Move> get_moves() const;

	// Returns a value in {0, 0.5, 1}.
	// This should not be an evaluation function, because it will only be
	// called for finished games. Return 0.5 to indicate a draw.
	double get_result(int current_player_to_move) const;

	int player_to_move;

	// ...
private:
	// ...
};

*/
//
// See the examples for more details. Given a suitable State, the
// following function (tries to) compute the best move for the
// player to move.
//

namespace MCTS
{
struct ComputeOptions
{
	int number_of_roots;
        int number_of_pointers;
	int max_iterations;
	double max_time;
	bool verbose;

	ComputeOptions() :
		number_of_roots(ROOTS),
                number_of_pointers(PTRS),
		max_iterations(10000),
		max_time(-1.0), // default is no time limit.
		verbose(false)
	{ }
};

template<typename State>
typename State::Move compute_move(const State root_state,
                                  const ComputeOptions options = ComputeOptions());
}
//
//
// [1] Chaslot, G. M. B., Winands, M. H., & van Den Herik, H. J. (2008).
//     Parallel monte-carlo tree search. In Computers and Games (pp. 
//     60-71). Springer Berlin Heidelberg.
//

#include <algorithm>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif


namespace MCTS
{
using std::cerr;
using std::endl;
using std::vector;
using std::size_t;

void check(bool expr, const char* message);
void assertion_failed(const char* expr, const char* file, int line);

#define attest(expr) if (!(expr)) { ::MCTS::assertion_failed(#expr, __FILE__, __LINE__); }
#ifndef NDEBUG
	#define dattest(expr) if (!(expr)) { ::MCTS::assertion_failed(#expr, __FILE__, __LINE__); }
#else
	#define dattest(expr) ((void)0)
#endif

//
// This class is used to build the game tree. The root is created by the users and
// the rest of the tree is created by add_node.
//
template<typename State>
class Node
{
public:
	typedef typename State::Move Move;

	Node(const State& state);
	~Node();

	bool has_untried_moves() const;
	//bool has_untried_neighbors() const;
	template<typename RandomEngine>
	Move get_untried_move(RandomEngine* engine) const;
	/* template<typename RandomEngine> */
	/* Move get_untried_neighbors(RandomEngine* engine) const; */
	Node* best_child() const;

	bool has_children() const
	{
		return ! children.empty();
	}

	Node* select_child_UCT();
	Node* add_child(const Move& move, const State& state);
	void update(double result);

	std::string to_string() const;
	std::string tree_to_string(int max_depth = 100, int indent = 0) const;

	const Move move;
	Node* const parent;
	const int player_to_move;
	
	//std::atomic<double> wins;
	//std::atomic<int> visits;
	double wins;
	int visits;
	std::mutex m;
	std::mutex back;

	std::pair<std::vector<Move>, std::vector<Move>> moves;
	std::vector<Move> neighbors;
	std::vector<Node*> children;
	

private:
	Node(const State& state, const Move& move, Node* parent);

	std::string indent_string(int indent) const;

	Node(const Node&);
	Node& operator = (const Node&);

	double UCT_score;
};


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


template<typename State>
Node<State>::Node(const State& state) :
	move(State::no_move),
	parent(nullptr),
	player_to_move(state.player_to_move),
	wins(0),
	visits(0),
	moves(state.get_moves()),
	UCT_score(0)
{ }

template<typename State>
Node<State>::Node(const State& state, const Move& move_, Node* parent_) :
	move(move_),
	parent(parent_),
	player_to_move(state.player_to_move),
	wins(0),
	visits(0),
	moves(state.get_moves()),
	UCT_score(0)
{ }

template<typename State>
Node<State>::~Node()
{
	for (auto child: children) {
		delete child;
	}
}

template<typename State>
bool Node<State>::has_untried_moves() const
{
  return ! (moves.first.empty() && moves.second.empty());
}

/* template<typename State> */
/* bool Node<State>::has_untried_neighbors() const */
/* { */
/* 	return ! neighbors.empty(); */
/* } */


template<typename State>
template<typename RandomEngine>
typename State::Move Node<State>::get_untried_move(RandomEngine* engine) const
{
        attest(! (moves.first.empty() && moves.second.empty()));
	if (! moves.first.empty()) {
	  std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.first.size() - 1);
	  return moves.first[moves_distribution(*engine)];
	}
	else {
	  std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.second.size() - 1);
	  return moves.second[moves_distribution(*engine)];
	}
}

/* template<typename State> */
/* template<typename RandomEngine> */
/* typename State::Move Node<State>::get_untried_neighbors(RandomEngine* engine) const */
/* { */
/* 	attest( ! moves.empty()); */
/* 	std::uniform_int_distribution<std::size_t> moves_distribution(0, neighbors.size() - 1); */
/* 	return neighbors[moves_distribution(*engine)]; */
/* } */

template<typename State>
Node<State>* Node<State>::best_child() const
{
        attest(moves.first.empty() && moves.second.empty());
	attest( ! children.empty() );

	return *std::max_element(children.begin(), children.end(),
		[](Node* a, Node* b) { return a->visits < b->visits; });;
}

template<typename State>
Node<State>* Node<State>::select_child_UCT()
{
	attest( ! children.empty() );
	for (auto child: children) {
         child->UCT_score = double(child->wins) / double(child->visits)
           + 2 * std::sqrt( std::log(double(this->visits)) / child->visits);
        }
	
	return *std::max_element(children.begin(), children.end(),
		[](Node* a, Node* b) { return a->UCT_score < b->UCT_score; });
}

template<typename State>
Node<State>* Node<State>::add_child(const Move& move, const State& state)
{
	auto node = new Node(state, move, this);
	children.push_back(node);
	attest( ! children.empty());

	if (! moves.first.empty() ) {
	  auto itr = moves.first.begin();
	  for (; itr != moves.first.end() && *itr != move; ++itr);
	  attest(itr != moves.first.end());
	  moves.first.erase(itr);
	}
	else {
	  auto itr = moves.second.begin();
	  for (; itr != moves.second.end() && *itr != move; ++itr);
	  attest(itr != moves.second.end());
	  moves.second.erase(itr);
	}

	return node;
}

template<typename State>
void Node<State>::update(double result)
{
	visits++;
	wins += result;
	//double my_wins = wins.load();
	//while ( ! wins.compare_exchange_strong(my_wins, my_wins + result));
}

template<typename State>
std::string Node<State>::to_string() const
{
	std::stringstream sout;
	sout << "["
	     << "P" << 3 - player_to_move << " "
	     << "M:" << "(" << move.first << "," << move.second << ")" << " "
	     << "W/V: " << wins << "/" << visits << " "
	     << "U: " << moves.first.size() + moves.second.size() << "]\n";
	return sout.str();
}

template<typename State>
std::string Node<State>::tree_to_string(int max_depth, int indent) const
{
	if (indent >= max_depth) {
		return "";
	}

	std::string s = indent_string(indent) + to_string();
	for (auto child: children) {
		s += child->tree_to_string(max_depth, indent + 1);
	}
	return s;
}

template<typename State>
std::string Node<State>::indent_string(int indent) const
{
	std::string s = "";
	for (int i = 1; i <= indent; ++i) {
		s += "| ";
	}
	return s;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

template<typename State>
  void compute_pointer(const std::shared_ptr<Node<State>> root,
                     const State root_state,
		     const ComputeOptions options,
		     std::mt19937_64::result_type initial_seed)
{
	std::mt19937_64 random_engine(initial_seed); 

       #ifdef USE_OPENMP
	double start_time = ::omp_get_wtime();
	double print_time = start_time;
	#endif

	for (int iter = 1; iter <= options.max_iterations || options.max_iterations < 0; ++iter) {
		auto node = root.get();
		State state = root_state.copy();

		// Select a path through the tree to a leaf node.
		if (TREE) {
                  node->m.lock(); 
                }

		while (!node->has_untried_moves() && node->has_children()) {
                     auto new_node = node->select_child_UCT();
		     if (TREE) {
                         new_node->m.lock(); 
			 node->m.unlock(); 
                     }
		     node = new_node;
		     state.do_move(node->move);
                }

		auto old_node = node;		
		// If we are not already at the final state, expand the
		// tree with a new node and move there.
		/* if (node->has_untried_neighbors()) { */
                /*     auto move = node->get_untried_neighbors(&random_engine); */
                /*     state.do_move(move); */
		/*     node = node->add_child(move, state); */
                /* } */
		if (node->has_untried_moves()) {
		  auto move = node->get_untried_move(&random_engine);
		  state.do_move(move);
		  node = node->add_child(move, state);
                }

		if (TREE) {
                    old_node->m.unlock();
                }
		
		// We now play randomly until the game ends.
		while (state.has_moves()) {
		  state.do_random_move(&random_engine);
		}


		//std::cerr << iter << endl; 
		// We have now reached a final state. Backpropagate the result
		// up the tree to the root node.
		if (TREE) {
		    node->back.lock(); 
                }
		while (node != nullptr) {
                        int result = state.get_result();
			if (result <= 0){
			  result = 0;
			}
			node->update(result);
			
			if (node->parent == nullptr) {
                            if (TREE) {
                                node->back.unlock();
			    }
			    break;
                        }
                        if (TREE) {
                            /* std::cerr << "W2p" << node->to_string() <<endl; */
                            node->parent->back.lock(); 
                            /* std::cerr << "U2p" << node->to_string() <<endl; */
			    node->back.unlock();
                        }
			node = node->parent;
                }

		//std::cerr << node->tree_to_string() << endl;
		
		#ifdef USE_OPENMP
		if (options.verbose || options.max_time >= 0) {
			double time = ::omp_get_wtime();
			if (options.verbose && (time - print_time >= 1.0 || iter == options.max_iterations)) {
				std::cerr << iter << " games played (" << double(iter) / (time - start_time) << " / second)." << endl;
				print_time = time;
			}

			if (time - start_time >= options.max_time) {
				break;
			}
		}
		#endif
	}

}

template<typename State>
std::shared_ptr<Node<State>>  compute_tree(const State root_state,
                                           const ComputeOptions options,
                                           std::mt19937_64::result_type initial_seed)
{
	attest(options.max_iterations >= 0 || options.max_time >= 0);
	if (options.max_time >= 0) {
		#ifndef USE_OPENMP
		throw std::runtime_error("ComputeOptions::max_time requires OpenMP.");
		#endif
	}

	attest(root_state.player_to_move == 1 || root_state.player_to_move == 2);
	auto root = std::shared_ptr<Node<State>>(new Node<State>(root_state));
	vector<std::future<void>> root_futures;	

	State state = root_state.copy();

	ComputeOptions job_options = options;
	job_options.verbose = false;

	for (int t = 0; t < options.number_of_pointers; ++t) {
	  auto func = [t, initial_seed, &root, &state, &job_options] () -> void
		{
                        return compute_pointer(root, state, job_options, 
					       2342123 * t + 731 * initial_seed + 15251); 
			//seed is arbitrary
		};
		root_futures.push_back(std::async(std::launch::async, func));
	}
	for (int t=  0; t < options.number_of_pointers; ++t) {
	  root_futures[t].get();
	}

	return root;
}

template<typename State>
typename State::Move compute_move(const State root_state,
                                  const ComputeOptions options)
{
	using namespace std;

	// Will support more players later.
	attest(root_state.player_to_move == 1 || root_state.player_to_move == 2);

	auto moves = root_state.get_moves();
	attest(moves.first.size() + moves.second.size() > 0);
	if (moves.first.size() == 1) {
	  return moves.first[0];
	}
	else if (moves.first.size() == 0 && moves.second.size() == 1) {
	  return moves.second[0];
	}

	#ifdef USE_OPENMP
	double start_time = ::omp_get_wtime();
	#endif

	// Start all jobs to compute trees.
	vector<future<shared_ptr<Node<State>>>> root_futures;
	ComputeOptions job_options = options;
	job_options.verbose = false;
	for (int t = 0; t < options.number_of_roots; ++t) {
		auto func = [t, &root_state, &job_options] () -> std::shared_ptr<Node<State>>
		{
                     return compute_tree(root_state.copy(), job_options, 12312 * t + 23);
		};

		root_futures.push_back(std::async(std::launch::async, func));
	}

	// Collect the results.
	vector<shared_ptr<Node<State>>> roots;
	for (int t = 0; t < options.number_of_roots; ++t) {
		roots.push_back(std::move(root_futures[t].get()));
	}

	// Merge the children of all root nodes.
	map<typename State::Move, int> visits;
	map<typename State::Move, double> wins;
	long long games_played = 0;
	for (int t = 0; t < options.number_of_roots; ++t) {
		auto root = roots[t].get();
		games_played += root->visits;
		for (auto child = root->children.cbegin(); child != root->children.cend(); ++child) {
			visits[(*child)->move] += (*child)->visits;
			wins[(*child)->move]   += (*child)->wins;
		}
	}

	// Find the node with the most visits.
	double best_score = -1;
	typename State::Move best_move = typename State::Move();
	for (auto itr: visits) {
		auto move = itr.first;
		double v = itr.second;
		double w = wins[move];
		// Expected success rate assuming a uniform prior (Beta(1, 1)).
		// https://en.wikipedia.org/wiki/Beta_distribution
		double expected_success_rate = (w + 1) / (v + 2);
		if (expected_success_rate > best_score) {
			best_move = move;
			best_score = expected_success_rate;
		}

		if (options.verbose) {
			cerr << "Move: " << "(" << itr.first.first << "," << itr.first.second << ")"
			     << " (" << setw(2) << right << v << " visits)"
			     << " (" << setw(2) << right << int(100 * w/v) / 100.0    << " points)" << endl;
		}
	}

	if (options.verbose) {
		auto best_wins = wins[best_move];
		auto best_visits = visits[best_move];
		cerr << "----" << endl;
		cerr << "Best: " << " (" << best_move.first << "," << best_move.second << ") "
		     << " (" << 100.0 * best_visits / double(games_played) << "% visits)"
		     << " (" << 1.0 * best_wins / best_visits << " points)" << endl;
	}

	#ifdef USE_OPENMP
	if (options.verbose) {
		double time = ::omp_get_wtime();
		std::cerr << games_played << " games played in " << double(time - start_time) << " s. " 
		          << "(" << double(games_played) / (time - start_time) << " / second, "
		          << options.number_of_roots << " parallel roots," << options.number_of_pointers                          << " pointers per root)." << endl;
	}
	#endif

	return best_move;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


void check(bool expr, const char* message)
{
	if (!expr) {
		throw std::invalid_argument(message);
	}
}

void assertion_failed(const char* expr, const char* file_cstr, int line)
{
	using namespace std;

	// Extract the file name only.
	string file(file_cstr);
	auto pos = file.find_last_of("/\\");
	if (pos == string::npos) {
		pos = 0;
	}
	file = file.substr(pos + 1);  // Returns empty string if pos + 1 == length.

	stringstream sout;
	sout << "Assertion failed: " << expr << " in " << file << ":" << line << ".";
	throw runtime_error(sout.str().c_str());
}

}

#endif
