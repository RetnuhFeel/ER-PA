/* Preferential Attachment Random Graph Generation based on the Barabasi-Albert Model */
#include<iostream>
#include<cstdio>
#include<fstream>
#include<sstream>
#include<random>
#include<string>
#include<set>

using namespace std;
using namespace std::chrono;

const int BUFF_SIZE = 256;

set<int> uniformRSS(vector<int> list, int m0, int seed);

vector<string> generateGraph(int num_vertices, int m0, int seed);

void failBad(char *filename, ifstream &ifs);

void outputJSON(char *fn, int num_vertices, int num_edges, vector<string> &edges);

int main(int argc, char **argv){
  int n;
  int m0;
  double edge_density;
  vector<string> edges;
  duration<double> time_span;
  high_resolution_clock::time_point t1;
  high_resolution_clock::time_point t2;

  if(argc < 4){
    fprintf(stderr, "usage:    ./pa output-filename n m0\n");
    exit(1);
  }

  sscanf(argv[2], "%d", &n);

  sscanf(argv[3], "%d", &m0);


  t1 = high_resolution_clock::now();
  edges = generateGraph(n, m0, 0);
  t2 = high_resolution_clock::now();
  time_span = duration_cast<duration<double>>(t2 - t1);

  edge_density = edges.size() * 2.0;
  edge_density /= (n * (n - 1));

  cout << "Preferential Attachment edge density: " << edge_density << endl;
  cout << "Graph Generation time: " << time_span.count() << " seconds." << endl << endl;

  outputJSON(argv[1], n, (int) edges.size(), edges);

  return 0;
}

void failBad(char *filename, ofstream &ofs){
  fprintf(stderr, "%s failed or is a bad file\n", filename);
  ofs.close();
  exit(1);
}

/* Uniform Random Subset Selection of size m0 from a list of numbers */
set<int> uniformRSS(vector<int> list, int m0, int seed){
  set<int> rs;
  random_device r_d;
  mt19937 gen(r_d());
  uniform_int_distribution<> uid(0, list.size() - 1);

  while(rs.size() < m0){
    int choice = list.at(uid(gen));
    if(rs.find(choice) == rs.end()){ rs.insert(choice); }
  }
  return rs;
}

vector<string> generateGraph(int num_vertices, int m0, int seed){
  int i;
  int source;
  set<int> target_nodes;
  set<int>::iterator sit;
  /* repeated_nodes will list existing nodes -- once per the number of adjacent edges to it */
  vector<int> repeated_nodes;
  vector<string> edges;

  /* initialize m0 nodes to initially connect, 1 <= m0 < n */
  for(i = 0; i < m0; ++i){ target_nodes.insert(i); }

  source = m0;
  while(source < num_vertices){
    for(sit = target_nodes.begin(); sit != target_nodes.end(); ++sit){
      char buf[BUFF_SIZE];
      sprintf(buf, "%d    %d", source + 1, *sit + 1);
      string str = buf;
      edges.push_back(str);
    }
    for(sit = target_nodes.begin(); sit != target_nodes.end(); ++sit){
      repeated_nodes.push_back(*sit);
    }
    for(i = 0; i < m0; ++i){ repeated_nodes.push_back(source); }
    target_nodes = uniformRSS(repeated_nodes, m0, seed);
    ++source;
  }
  return edges;
}

void outputJSON(char *fn, int num_vertices, int num_edges, vector<string> &edges){
  istringstream iss;
  set<string>::iterator sit;
  int i;
  FILE *fout;

  fout = fopen(fn, "w");

  if(fout == NULL){
    fprintf(stderr, "FAIL/BAD Output File %s\n", fn);
    exit(1);
  }

  fprintf(fout, "{\n  \"nodes\":[\n");
  for(int i = 1; i <= num_vertices; ++i){
    fprintf(fout, "    {\n      \"id\":\"%d\"\n", i);
    if(i < num_vertices){ fprintf(fout, "    },\n"); }
    else{ fprintf(fout, "    }\n  ],");  }
  }
  fprintf(fout, "  \"edges\":[\n");
  for(i = 0; i < num_edges; ++i){
    int p1;
    int p2;
    iss.str(edges.at(i));
    iss >> p1;
    iss >> p2;
    fprintf(fout, "    {\n      \"source\":\"%d\",\n      \"target\":\"%d\"\n", p1, p2);
    if(i < num_edges - 1){ fprintf(fout, "    },\n");  }
    else{ fprintf(fout, "    }\n  ]\n}"); }
    iss.clear();
  }

  fclose(fout);
}

