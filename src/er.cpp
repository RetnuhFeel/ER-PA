/* G(n, p) Erdos-Reyni Random Graph Generation -- iteratively build a pseudo-random    *
 * Graph with n nodes from the pool of all possible edges with p independent probability *
 * of inclusion */
#include<iostream>
#include<cstdio>
#include<fstream>
#include<sstream>
#include<random>
#include<string>
#include<set>
#include<chrono>

using namespace std;
using namespace std::chrono;

const int BUFF_SIZE = 256;

void generateGraph(set<string> &edge_combo, vector<string> &all_edges, double p);

void failBad(char *filename, ifstream &ifs);

void generateAllEdges(vector<string> &all_edges, int num_vertices);

void outputJSON(char *fn, int num_vertices, int num_edges, set<string> &edges);

int main(int argc, char **argv){
  int n;
  double p;
  double edge_density;
  vector<string> a_e;
  set<string> e_c;
  duration<double> time_span;
  high_resolution_clock::time_point t1;
  high_resolution_clock::time_point t2;

  if(argc < 4){
    fprintf(stderr, "usage:    ./pr output-filename n p\n");
    exit(1);
  }

  sscanf(argv[2], "%d", &n);

  sscanf(argv[3], "%lf", &p);

  generateAllEdges(a_e, n);

  t1 = high_resolution_clock::now();
  generateGraph(e_c, a_e, p);
  t2 = high_resolution_clock::now();
  time_span = duration_cast<duration<double>>(t2 - t1);

  edge_density = e_c.size() * 2.0;
  edge_density /= (n * (n - 1));

  cout << "Erdos-Renyi G(n, p) edge density: " << edge_density << endl;
  cout << "Graph Generation time: " << time_span.count() << " seconds." << endl << endl;

  outputJSON(argv[1], n, (int) e_c.size(), e_c);

  return 0;
}

void failBad(char *filename, ofstream &ofs){
  fprintf(stderr, "%s failed or is a bad file\n", filename);
  ofs.close();
  exit(1);
}

void generateAllEdges(vector<string> &all_edges, int num_vertices){
  char buff[BUFF_SIZE];
  string nu_edge;

  for(int i = 1; i < num_vertices; ++i){
    for(int j = i + 1; j <= num_vertices; ++j){
      sprintf(buff, "%d %d", i, j);
      nu_edge = buff;
      all_edges.push_back(nu_edge);
    }
  }
}

void generateGraph(set<string> &edge_combo, vector<string> &all_edges, double p){
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> u_d(0.0, 1.01);

  for(int i = 0; i < all_edges.size(); ++i){
    double rv;
    rv = u_d(gen);
    if(rv <= p){ edge_combo.insert(all_edges.at(i)); }
  }
}

void outputJSON(char *fn, int num_vertices, int num_edges, set<string> &edges){
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
  i = 0;
  for(sit = edges.begin(); sit != edges.end(); ++sit){
    int p1;
    int p2;
    iss.str(*sit);
    iss >> p1;
    iss >> p2;
    fprintf(fout, "    {\n      \"source\":\"%d\",\n      \"target\":\"%d\"\n", p1, p2);
    if(i < num_edges - 1){ fprintf(fout, "    },\n");  }
    else{ fprintf(fout, "    }\n  ]\n}"); }
    iss.clear();
    ++i;
  }

  fclose(fout);
}
